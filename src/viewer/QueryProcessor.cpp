#include "MapFile.h"

#include <spatialite/gaiaexif.h>
#include <stdio.h>

#include <png.h>
#include <pngconf.h>
#include <cairo.h>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <float.h>

using namespace std ;


string MapFile::makeBBoxQuery(const std::string &tableName, const AttributeCollection &attrs, const std::string &geomColumn, int t_srid,
                              const std::string &condition, const BBox &bbox, bool z_order)
{
    stringstream sql ;

    sql.precision(16) ;

    sql << "SELECT " ;

    const vector<AttributeDescriptor> &fields = attrs.fields ;

    int counter = 1 ;

    for( int i=0 ; i<fields.size() ; i++ )
    {
        const AttributeDescriptor &data = fields[i] ;

        if ( data.type == AttributeDescriptor::DictionaryIndex )
            sql << "__dictionary" << counter++ << ".key AS " << data.name << "_," ;
        else
            sql << data.name << " AS " << data.name << "_," ;
    }

    sql << " ST_Transform(" << geomColumn << "," << t_srid << ") AS _geom_ FROM " << tableName << " AS __table__";

    counter = 1 ;
    for( int i=0 ; i<fields.size() ; i++ )
    {
        const AttributeDescriptor &data = fields[i] ;

        if ( data.type == AttributeDescriptor::DictionaryIndex )
        {
            sql << " LEFT JOIN __dictionary AS __dictionary" << counter << " ON __dictionary" << counter << ".id = " << data.name ;
            counter ++ ;
        }
    }


    sql << " WHERE " << condition ;

    if ( !condition.empty() ) sql << " AND " ;

    sql << "__table__.ROWID IN ( SELECT ROWID FROM SpatialIndex WHERE f_table_name='" << tableName << "' AND search_frame = ST_Transform(BuildMBR(" ;
    sql << bbox.minx << ',' << bbox.miny << ',' << bbox.maxx << ',' << bbox.maxy << "," << bbox.srid << "),4326))" ;

    if ( z_order ) sql << " ORDER BY z_order ASC" ;
    return sql.str() ;
}

bool MapFile::queryFeatures( FeatureCollection &col, const LayerInfo &info, int target_srid, const string &condition, const BBox &box)
{
    SQLite::Session session(db_) ;
    SQLite::Connection &con = session.handle() ;

    bool z_order = false ;

    for( int i=0 ; i< info.attrs->fields.size() ; i++ )
    {
        if ( info.attrs->fields[i].name == "z_order") {
            z_order = true ;
            break ;
        }
    }

    string sql = makeBBoxQuery(info.name, *(info.attrs), info.geomColumn, target_srid, condition, box, z_order) ;

 //   sql = "SELECT gid,type,state,notes, ST_Transform(geom,3857) AS _geom_ FROM tracks WHERE (\"type\"='primary') AND ROWID IN ( SELECT ROWID FROM SpatialIndex WHERE f_table_name='tracks' AND search_frame = ST_Transform(BuildMBR(2617203.848484434,4935997.538543543,2636771.727725439,4955565.417784547,3857),4326))" ;

    try {

        SQLite::Query q(con, sql) ;

        SQLite::QueryResult res = q.exec() ;

        while ( res )
        {
            FeaturePtr f(new Feature(info.attrs)) ;

            int buf_size ;
            const char *data = res.getBlob("_geom_", buf_size) ;

            gaiaGeomCollPtr geom = gaiaFromSpatiaLiteBlobWkb ((const unsigned char *)data, buf_size);

            const vector<AttributeDescriptor> &fields = info.attrs->fields ;

            for( int i=0 ; i<fields.size() ; i++ )
            {
                const AttributeDescriptor &data = fields[i] ;

                if ( data.type == AttributeDescriptor::Integer )
                    f->add(Value(res.get<int>(i))) ;
                else if ( data.type == AttributeDescriptor::Real )
                    f->add(Value(res.get<double>(i))) ;
                else if ( data.type == AttributeDescriptor::Text )
                    f->add(Value(res.get<string>(i))) ;
                else if ( data.type == AttributeDescriptor::DictionaryIndex )
                    f->add(Value(res.get<string>(i))) ;

            }

            f->setGeometry(geom) ;
            col.features.push_back(f) ;

            res.next() ;
        }

        return true ;
    }
    catch ( SQLite::Exception &e )
    {
        cout << e.what() << endl ;
        return false ;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

const double earth_circ = 20037508.342789244 ;

struct RasterTile
{
    double minx, miny, maxx, maxy ;
    int level ;
    int id ;
    char *data ;
    int sz ;
    char *image ;
    int stride ;
    double px, py ;
    int width, height ;

    ~RasterTile() { if ( image ) delete [] image ; }
};

typedef boost::shared_ptr<RasterTile> RasterTilePtr ;


struct PNGReadContext {
    char *data_ptr ;
    unsigned int consumed ;
    unsigned int sz ;
};

static void read_png_stream(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead)
{
   if ( png_ptr->io_ptr == NULL ) return;

   PNGReadContext & ctx= *(PNGReadContext *)png_ptr->io_ptr;

   if ( byteCountToRead > ctx.sz - ctx.consumed ) return ;

   memcpy(outBytes, ctx.data_ptr, byteCountToRead) ;
   ctx.consumed += byteCountToRead ;
   ctx.data_ptr += byteCountToRead ;

}


static bool decode_png_image(char *img, unsigned int stride, char *src, int src_sz)
{
    if ( !png_check_sig ( (png_bytep)src, 8 ) ) return false ;

    png_structp png_ptr = NULL;
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if( png_ptr == NULL ) return false ;

    png_infop info_ptr = NULL;
    info_ptr = png_create_info_struct(png_ptr);

    if( info_ptr == NULL )
    {
       png_destroy_read_struct(&png_ptr, NULL, NULL);
       return false;
    }

    PNGReadContext ctx ;
    ctx.data_ptr = src ;
    ctx.sz = src_sz ;
    ctx.consumed = 0 ;

    png_set_read_fn(png_ptr, &ctx, read_png_stream);

    //png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr) ;

    png_uint_32 width = 0;
    png_uint_32 height = 0;

    int bitDepth = 0;
    int colorType = -1;

    png_uint_32 retval = png_get_IHDR(png_ptr, info_ptr,  &width, &height, &bitDepth, &colorType,
       NULL, NULL, NULL);

    if ( retval != 1 ) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return false ;
    }

    const png_uint_32 bytesPerRow = png_get_rowbytes(png_ptr, info_ptr);
    char* rowData = new char[bytesPerRow];

    // read single row at a time

    unsigned int rowOffset = 0 ;

    // TODO:
    // endianess

    for(unsigned int rowIdx = 0; rowIdx < height; ++rowIdx)
    {
        png_read_row(png_ptr, (png_bytep)rowData, NULL);

        char *dst = img + rowOffset ;

        unsigned int byteIndex = 0;

        if ( colorType == PNG_COLOR_TYPE_RGBA )
        {
            for(unsigned int colIdx = 0; colIdx < width; ++colIdx)
            {
                const char red   = rowData[byteIndex++];
                const char green = rowData[byteIndex++];
                const char blue  = rowData[byteIndex++];
                const char alpha = rowData[byteIndex++];

                *dst++ = alpha ;
                *dst++ = red ;
                *dst++ = green ;
                *dst++ = blue ;
            }

        }
        else if ( colorType == PNG_COLOR_TYPE_RGB )
        {
            for(unsigned int colIdx = 0; colIdx < width; ++colIdx)
            {
                const char red   = rowData[byteIndex++];
                const char green = rowData[byteIndex++];
                const char blue  = rowData[byteIndex++];


                *dst++ = red ;
                *dst++ = green ;
                *dst++ = blue ;
                *dst++ = 255 ;
            }
        }
        else if ( colorType == PNG_COLOR_TYPE_GRAY )
        {
            for(unsigned int colIdx = 0; colIdx < width; ++colIdx)
            {
                const char gray = rowData[byteIndex++];

                *dst++ = gray ;
                *dst++ = gray ;
                *dst++ = gray ;
                *dst++ = 255 ;
            }
        }
        else if ( colorType == PNG_COLOR_TYPE_GRAY_ALPHA )
        {
            for(unsigned int colIdx = 0; colIdx < width; ++colIdx)
            {
                const char gray = rowData[byteIndex++];
                const char alpha = rowData[byteIndex++] ;

                *dst++ = gray ;
                *dst++ = gray ;
                *dst++ = gray ;
                *dst++ = alpha ;
            }
        }

        rowOffset += stride ;

    }

    delete [] rowData;

    return true ;


}

static void paste_image(char *img, unsigned int offset, unsigned int dst_stride, RasterTile &tile)
{
    int w = tile.width, h = tile.height ;
    int src_stride = tile.stride ;

    char *src_ptr = tile.image  ;
    char *dst_ptr = img + offset ;

    for(int i=0 ; i<h ; i++)
    {
        memcpy(dst_ptr, src_ptr, 4*w) ;

        src_ptr += src_stride ;
        dst_ptr += dst_stride ;
    }

}



static bool checkSrid(SQLite::Connection &con, const string &layerName, int srid)
{
    string sql = "SELECT srid ";

    sql += "FROM geometry_columns ";
    sql += "WHERE f_table_name LIKE";
    sql += " \"" + layerName + "_metadata\" ";
    sql += "AND f_geometry_column LIKE 'geometry' AND srid = " ;
    sql += str(boost::format("%d") % srid) ;

    SQLite::Query q(con, sql) ;
    SQLite::QueryResult res = q.exec() ;

    return res ;
}

static bool findRasterliteTiles(SQLite::Connection &con, const std::string &layerName, double scale,
                                int &level, double &pixel_size_x, double &pixel_size_y, int &tile_count)
{
    try {
        string sql = "SELECT pixel_x_size, pixel_y_size, tile_count FROM raster_pyramids" ;
        sql += " WHERE table_prefix LIKE ?";
        sql += " ORDER BY pixel_x_size DESC";

        SQLite::Query q(con, sql) ;
        q.bind(1, layerName) ;

        SQLite::QueryResult res = q.exec() ;

        level = -1 ;
        double min_dist = DBL_MAX;
        double dist;
        pixel_size_x = DBL_MAX;
        pixel_size_y = DBL_MAX;
        tile_count = -1;

        int nlevels = 0 ;

        while ( res )
        {
            double px = res.get<double>(0) ;
            double py = res.get<double>(1) ;
            int n_tiles = res.get<int>(2) ;

            double dist = fabs (scale - px);

            if ( dist < min_dist )
            {
                min_dist = dist;
                pixel_size_x = px ;
                pixel_size_y = py ;
                tile_count = n_tiles ;
                level = nlevels ;
            }

            nlevels++ ;

            res.next() ;
        }

        return tile_count > 0  ;
    }
    catch (SQLite::Exception &e)
    {
        return false ;
    }

}

static bool fetchRasterliteTiles(SQLite::Connection &con, TileCache &cache, const string &layerName, bool useRTree, int level, double pixel_size_x, double pixel_size_y,
                                 const BBox &box,
                                 vector<RasterTilePtr> &tiles)
{
    string sql ;
    if ( useRTree )
    {
        sql = "SELECT m.geometry, r.id, m.tile_id, m.width, m.height FROM \"" ;
        sql += layerName ;
        sql += "_metadata\" AS m, \"";
        sql += layerName ;
        sql += "_rasters\" AS r WHERE m.ROWID IN (SELECT pkid ";
        sql += "FROM \"idx_";
        sql += layerName ;
        sql += "_metadata_geometry\" ";
        sql += "WHERE  xmax > ? AND AND ymax > ? AND xmin < ? AND ymin < ? ) ";
        sql += "AND m.pixel_x_size = ? AND m.pixel_y_size = ? AND r.id = m.id";
    }
    else
    {
        sql = "SELECT m.geometry, r.id, m.tile_id, m.width, m.height FROM \"" ;
        sql += layerName ;
        sql += "_metadata\" AS m, \"" ;
        sql += layerName ;
        sql += "_rasters\" AS r ";
        sql += "WHERE MbrIntersects(m.geometry, BuildMbr(?, ?, ?, ?)) ";
        sql += "AND m.pixel_x_size = ? AND m.pixel_y_size = ? AND r.id = m.id";
    }

    // first query tile id's intersecting the bounding box

    try {

        SQLite::Query q(con, sql) ;
        q.bind(1, box.minx) ;
        q.bind(2, box.miny) ;
        q.bind(3, box.maxx) ;
        q.bind(4, box.maxy) ;
        q.bind(5, pixel_size_x);
        q.bind(6, pixel_size_y);

        SQLite::QueryResult res = q.exec() ;

        while ( res )
        {
            int raster_id = res.get<int>(1) ;

            RasterTilePtr tile ;


            // if it is in the cache just point there

            if ( cache.exists(raster_id) )
                tile = cache.get(raster_id) ;
            else
            {
                int blob_size ;
                const char *blob = res.getBlob(0, blob_size) ;

                gaiaGeomCollPtr geom =
                    gaiaFromSpatiaLiteBlobWkb ((const unsigned char *)blob, blob_size);

                int tile_id = res.get<int>(2) ;
                int tile_width = res.get<int>(3) ;
                int tile_height = res.get<int>(4) ;

                unsigned int tile_stride = ((( tile_width * 4 ) + 63) & ~63)  ;

                // fill in geometry information for this tile

                tile.reset(new RasterTile) ;

                tile->minx = geom->MinX ;
                tile->miny = geom->MinY ;
                tile->maxx = geom->MaxX ;
                tile->maxy = geom->MaxY ;
                tile->level = level ;
                tile->px = pixel_size_x ;
                tile->py = pixel_size_y ;
                tile->id = raster_id ;
                tile->width = tile_width ;
                tile->height = tile_height ;
                tile->data = 0 ;
                tile->image = 0 ;
                tile->stride = tile_stride ;

                cache.put(raster_id, tile, tile_stride * tile_height ) ;

                gaiaFreeGeomColl(geom);

            }

            tiles.push_back(tile) ;

            res.next() ;
        }

        // fetch tiles not in cache

        sql = "SELECT raster FROM \"" ;
        sql += layerName ;
        sql += "_rasters\" WHERE id = ?";

        SQLite::Query qraster(con, sql) ;

        for( int i=0 ; i<tiles.size() ; i++ )
        {
            RasterTile &tile = *tiles[i] ;

            // tile has been already read and decoded
            if ( tile.image ) continue ;

            qraster.bind(1, tile.id) ;
            SQLite::QueryResult res = qraster.exec() ;

            int blob_size ;
            const char *blob = res.getBlob(0, blob_size) ;

            blob = res.getBlob(0, blob_size) ;
            int type = gaiaGuessBlobType ((const unsigned char *)blob, blob_size);
            if (type != GAIA_PNG_BLOB) continue ;

            // store intermediate buffer to be decoded later on

            tile.data = new char [blob_size] ;
            memcpy(tile.data, blob, blob_size) ;
            tile.sz = blob_size ;

            qraster.clear() ;
        }

        return true ;
    }
    catch (SQLite::Exception &e)
    {

        return false ;
    }

}

// combine tiles into a single raster image

static void buildRasterImage(RasterData &data, vector<RasterTilePtr> &tiles)
{
    // find bounds

    double minx = DBL_MAX, miny = DBL_MAX, maxx = -DBL_MAX, maxy =- DBL_MAX ;

    for(int i=0 ; i<tiles.size() ; i++ )
    {
        const RasterTile &tile = *tiles[i] ;

        minx = std::min(minx, tile.minx) ;
        maxx = std::max(maxx, tile.maxx) ;
        miny = std::min(miny, tile.miny) ;
        maxy = std::max(maxy, tile.maxy) ;
    }

    // Order tile in row columns. We do not use projected coordinates directly to avoid round off errors that may cause artifacts on tile boundaries

    typedef map<int, RasterTilePtr> RowMap ;
    typedef map<int, RowMap> RowColMap ;

    RowColMap row_col ;

    for(int i=0 ; i<tiles.size() ; i++)
    {
        const RasterTile &tile = *tiles[i] ;

        int x = (tile.minx - minx)/tile.px ;
        int y = (tile.miny - miny)/tile.py ;

        map<int, RasterTilePtr> &row = row_col[-y] ;
        row.insert(make_pair(x, tiles[i])) ;
    }

    // calculate image dimensions

    unsigned int width = 0, height = 0 ;

    for( RowColMap::const_iterator it = row_col.begin() ; it != row_col.end() ; ++it )
          height += (*it).second.begin()->second->height ;

    const RowMap &row = (*row_col.begin()).second ;

    for( RowMap::const_iterator it = row.begin() ; it != row.end() ; ++it )
          width += (*it).second->width ;

    // we decode the image into RGBA buffer

    unsigned int stride = ((( width * 4 ) + 63) & ~63)  ;

    char *img = new char [stride * height] ;

    int y=0 ;

    for( RowColMap::const_iterator r = row_col.begin() ; r != row_col.end() ; ++r )
    {
        int x = 0 ;

        const RowMap &row = (*r).second ;

        for( RowMap::const_iterator c = row.begin() ; c != row.end() ; ++c )
        {
            const RasterTilePtr &rp = (*c).second ;

            // TODO: Fill with color ?

            if ( !rp->image && rp->data ) // decode the image if not already cached
            {
                char *tile_img ;

                unsigned int tile_stride = ((( rp->width * 4 ) + 63) & ~63)  ;
                tile_img = new char [tile_stride * rp->height] ;

                decode_png_image(tile_img, tile_stride, rp->data, rp->sz) ;

                rp->image = tile_img ;
                rp->stride = tile_stride ;
                delete [] rp->data ;

            }

            if ( rp->image )
                paste_image(img, y * stride + 4*x, stride, *rp) ;

            x += rp->width ;
        }

        y += row.begin()->second->height ;
    }

    // Fill in the raster data structure

    data.minx = minx ;
    data.miny = miny ;
    data.maxx = maxx ;
    data.maxy = maxy ;
    data.image = img ;
    data.width = width ;
    data.height = height ;
    data.stride = stride ;

/*
    cairo_surface_t *surf = cairo_image_surface_create_for_data((unsigned char *)img, CAIRO_FORMAT_ARGB32, width, height, stride) ;
    cairo_surface_write_to_png(surf, "/tmp/oo.png") ;
    */

}

bool MapFile::queryRasterlite(RasterData &data, const LayerInfo &info, int target_srid, double scale, const BBox &box)
{
    SQLite::Session session(db_) ;
    SQLite::Connection &con = session.handle() ;

    const string &layerName = info.name ;

    int tile_count, level ;
    double pixel_size_x, pixel_size_y ;

    if ( !findRasterliteTiles(con, layerName, scale, level, pixel_size_x, pixel_size_y, tile_count) ) return false ;

    if ( !checkSrid(con, layerName, target_srid) ) return false ;

    data.px = pixel_size_x ;
    data.py = pixel_size_y ;

    vector<RasterTilePtr> tiles ;

    if ( !fetchRasterliteTiles(con, tileCache, layerName, (tile_count > 500), level, pixel_size_x, pixel_size_y, box, tiles) ) return false ;

    buildRasterImage(data, tiles) ;

    return true ;

}

static bool fetchMercatorTiles(const string &tilePath, TileCache &cache, int level, double pixel_size, const BBox &box,  vector<RasterTilePtr> &tiles)
{
    int tx_min = ceil((box.minx + earth_circ)/256/pixel_size - 1);
    int ty_min = ceil((box.miny + earth_circ)/256/pixel_size - 1);

    int tx_max = ceil((box.maxx + earth_circ)/256/pixel_size - 1);
    int ty_max = ceil((box.maxy + earth_circ)/256/pixel_size - 1);

 //   ty_min = pow(2, level-1) - ty_min ;


    for(int x=tx_min ; x<=tx_max ; x++)
        for(int y=ty_min ; y<=ty_max ; y++)
        {

            string imgPath = tilePath + str(boost::format("/%d/%d/%d.png") % level % x % y) ;

            RasterTilePtr tile(new RasterTile) ;

            tile->width = 256 ;
            tile->height = 256 ;
            tile->level = level ;
            tile->minx = x * pixel_size * 256 - earth_circ ;
            tile->miny = y * pixel_size * 256 - earth_circ ;
            tile->maxx = (x + 1)*pixel_size * 256 - earth_circ ;
            tile->maxy = (y + 1)*pixel_size * 256 - earth_circ ;
            tile->px = pixel_size ;
            tile->py = pixel_size ;
            tile->data = 0 ;
            tile->sz = 0 ;
            tile->image = 0 ;

            if ( !boost::filesystem::exists(imgPath) ) continue ;

            unsigned int sz = boost::filesystem::file_size(imgPath) ;

            tile->data = new char [sz] ;
            tile->sz = sz ;

            ifstream strm(imgPath.c_str(), ios::binary) ;
            strm.read(tile->data, sz) ;
            if ( strm.gcount() != sz ) {
                delete [] tile->data ;
                tile->data = 0 ;
            }

            tiles.push_back(tile) ;
        }

    return tiles.size() > 0 ;

}


bool MapFile::queryTiles(RasterData &data, const LayerInfo &info, int target_srid, double scale, const BBox &box)
{
    const string &layerName = info.name ;
    string tilePath = "/home/malasiot/tmp/tiles/" ;

    unsigned int q = 1 ;
    unsigned int level ;
    double min_dist = DBL_MAX ;
    double pixel_size ;

    for(int z = 0 ; z<25 ; z++)
    {
        double res = earth_circ * 2 / 256 / q ;
        /*
        double dist = fabs(res - scale) ;
        if ( dist < min_dist )
        {
            min_dist = dist ;
            level = z ;
            pixel_size = res ;
        }
        */
        if ( scale > res ) {
            level = z -1 ;
            pixel_size = 2*res ;
            break ;
        }
        q <<= 1 ;
    }

    data.px = pixel_size ;
    data.py = pixel_size ;

    vector<RasterTilePtr> tiles ;

    if ( !fetchMercatorTiles(tilePath, tileCache, level, pixel_size, box, tiles) ) return false ;

    buildRasterImage(data, tiles) ;

    return true ;

}
