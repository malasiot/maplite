#include "MapFile.h"

#include <boost/algorithm/string.hpp>
#include <iomanip>

using namespace std ;

// SELECT * from poi_geom ,poi_text WHERE poi_geom.gid = poi_text.docid AND poi_text.content MATCH 'Ανοι*' ;

bool MapFile::queryPOIs(const string &layerName, vector<POI> &pois,  const string &condition,  const string &match, BBox *bbox,
               double dist_threshold,  double lon, double lat, int prj_srid  )
{
    SQLite::Session session(db_) ;
    SQLite::Connection &con = session.handle() ;

    try {
        string sql = "SELECT *" ;

        if ( dist_threshold > 0)
        {
            stringstream strm ;
            strm << "POINT(" << setprecision(12) << lon << ' ' << lat << ")" ;
            sql += ",Distance(Transform(ST_GeomFromText('" ;
            sql += strm.str() ;
            sql += "',4326),@prj), Transform(poi_geom.geom, @prj)) AS distance" ;
        }

        sql += " FROM " + layerName +  " AS poi_geom, " + layerName + "_text AS poi_text WHERE poi_geom.gid = poi_text.docid AND poi_text.content MATCH @key" ;

        if ( !condition.empty() )
            sql += " AND (" + condition + ")" ;

        if ( bbox )
            sql += " AND gid IN ( SELECT ROWID FROM SpatialIndex WHERE f_table_name='poi_geom' AND search_frame = ST_Transform(BuildMBR(@b1, @b2, @b3, @b4, @b5),4326))" ;

        if ( dist_threshold > 0 )
            sql += " AND distance < @dis ORDER BY distance" ;

        SQLite::Query query(con, sql) ;

        if ( dist_threshold > 0 )
            query.bindp("@dis", dist_threshold).bindp("@prj", prj_srid) ;

        if ( bbox )
            query.bindp("@b1", bbox->minx).bindp("@b2", bbox->miny).bindp("@b3", bbox->maxx).bindp("@b4", bbox->maxy).bindp("@b5", bbox->srid) ;

        query.bindp("@key", match) ;

        SQLite::QueryResult res = query.exec() ;

        while ( res )
        {
            POI poi ;

            int buf_size ;
            const char *data = res.getBlob("geom", buf_size) ;

            gaiaGeomCollPtr geom = gaiaFromSpatiaLiteBlobWkb ((const unsigned char *)data, buf_size);

            poi.lon_ = geom->FirstPoint->X ;
            poi.lat_ = geom->FirstPoint->Y ;

            for(int i=0 ; i<res.columns() ; i++ )
            {
                const char *columnName = res.columnName(i) ;

                if ( strcmp(columnName, "distance") == 0 )
                    poi.distance_ = res.get<double>(i) ;
                else if ( strcmp(columnName, "content") == 0 ) ;
                else if ( strcmp(columnName, "gid") == 0 ) ;
                else if ( strcmp(columnName, "geom") == 0 ) ;
                else
                    poi.tags_.add(columnName, res.get<string>(i)) ;
            }

            pois.push_back(poi) ;

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


