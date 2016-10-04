#include "vector_tile_writer.hpp"

#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/io/gzip_stream.h>
#include <iostream>

#if 0
using namespace std ;
void VectorTileWriter::beginLayer(const std::string &name)
{
    current_layer_ = tile_msg_.add_layers() ;
    current_layer_->set_name(name) ;
    current_layer_->set_version(2) ;

    key_map_.clear() ;
    value_map_.clear() ;
}

void VectorTileWriter::endLayer()
{
    assert(current_layer_) ;

    // write key/value tables

    vector<pair<string, uint32_t>> keys(key_map_.begin(), key_map_.end()),
            values(value_map_.begin(), value_map_.end()) ;

    auto comparator = [&] (const pair<string, uint32_t> &a, const pair<string, uint32_t> &b) {
        return a.second < b.second ;
    } ;

    // sort by value i.e. by index
    std::sort(keys.begin(), keys.end(), comparator) ;
    std::sort(values.begin(), values.end(), comparator) ;

    for( auto key: keys ) {
        current_layer_->add_keys(key.first) ;
    }

    for( auto value: values ) {
        vector_tile::Tile_Value *tv = current_layer_->add_values() ;
        tv->set_string_value(value.first);
    }

    current_layer_ = nullptr ;

}


void VectorTileWriter::encodeFeatures(const gaiaGeomCollPtr &geom, const Dictionary &attr) {

    assert(current_layer_) ;

    encodePointGeometry(geom, attr) ;
    encodeLineGeometry(geom, attr) ;
    encodePolygonGeometry(geom, attr) ;

    current_feature_ = nullptr ;
}

string VectorTileWriter::toString(bool compress)
{
    string res ;
    if ( !compress ) {
        tile_msg_.SerializeToString(&res) ;
        return res ;
    }
    else {
        ::google::protobuf::io::StringOutputStream compressedStream(&res);
        ::google::protobuf::io::GzipOutputStream compressingStream(&compressedStream);
        tile_msg_.SerializeToZeroCopyStream(&compressingStream);
        return res ;
    }
}

void VectorTileWriter::encodePointGeometry(const gaiaGeomCollPtr &geom, const Dictionary &attr) {

    assert(current_layer_) ;

    if ( geom->FirstPoint == 0 ) return ;

    current_feature_ = current_layer_->add_features() ;
    current_feature_->set_type(vector_tile::Tile_GeomType_POINT) ;

    int cx, cy ;

    uint32_t count = 0 ;

    for( gaiaPointPtr p = geom->FirstPoint ; p != NULL ; p = p->Next )
        count ++ ;

    addGeomCmd(1, count) ;

    for( gaiaPointPtr p = geom->FirstPoint ; p != NULL ; p = p->Next ) {
        tile_coords(geom->FirstPoint->X, geom->FirstPoint->Y, cx, cy) ;
        addPoint(cx, cy) ;
    }

    setFeatureAttributes(attr) ;
}

void VectorTileWriter::addPoint(int x, int y) {
    assert(current_feature_) ;
    current_feature_->add_geometry((x << 1) ^ (x  >> 31));
    current_feature_->add_geometry((y << 1) ^ (y  >> 31));
}

void VectorTileWriter::addGeomCmd(uint32_t cmd, uint32_t count) {
    assert(current_feature_) ;
    current_feature_->add_geometry((cmd & 0x7) | (count << 3));
}


void VectorTileWriter::encodeLineGeometry(const gaiaGeomCollPtr &geom, const Dictionary &attr) {

    assert(current_layer_) ;

    if ( geom->FirstLinestring == 0 ) return ;

    current_feature_ = current_layer_->add_features() ;
    current_feature_->set_type(vector_tile::Tile_GeomType_LINESTRING) ;

    int start_x = 0, start_y = 0;
    int cx, cy ;

    for( gaiaLinestringPtr pl = geom->FirstLinestring ; pl != NULL ; pl = pl->Next ) {

        LinearRing ls ;

        tile_coords(pl->Coords[0], pl->Coords[1], cx, cy) ;
        ls.push_back(EncodedVertex{cx - start_x, cy - start_y}) ;
        start_x = cx ; start_y = cy ;

        for(uint32_t count = 2 ; count < 2 * pl->Points ; count += 2) {

            tile_coords(pl->Coords[count], pl->Coords[count+1], cx, cy) ;

            int dx = cx - start_x, dy = cy - start_y ;

            if ( dx == 0 && dy == 0 ) continue ;

            ls.push_back(EncodedVertex{cx - start_x, cy - start_y}) ;

            start_x = cx ; start_y = cy ;
        }

        encodeRing(ls, false) ;
    }

    setFeatureAttributes(attr) ;

}

void VectorTileWriter::encodePolygonGeometry(const gaiaGeomCollPtr &geom, const Dictionary &attr) {

    assert(current_layer_) ;

    if ( geom->FirstPolygon == 0 ) return ;

    current_feature_ = current_layer_->add_features() ;
    current_feature_->set_type(vector_tile::Tile_GeomType_POLYGON) ;

    int start_x = 0, start_y = 0;
    int cx, cy ;

    for( gaiaPolygonPtr pl = geom->FirstPolygon ; pl != NULL ; pl = pl->Next ) {

        // exterior

        LinearRing exr ;

        gaiaRingPtr ex = pl->Exterior ;

        int n = ex->Points ;

        tile_coords(ex->Coords[0], ex->Coords[1], cx, cy) ;
        exr.push_back(EncodedVertex{cx - start_x, cy - start_y}) ;
        start_x = cx ; start_y = cy ;

        for(uint32_t count = 2 ; count < 2 * n ; count += 2) {

            tile_coords(ex->Coords[count], ex->Coords[count+1], cx, cy) ;

            int dx = cx - start_x, dy = cy - start_y ;

            if ( dx == 0 && dy == 0 ) continue ;

            exr.push_back(EncodedVertex{dx, dy}) ;

            start_x = cx ; start_y = cy ;
        }

        encodeRing(exr, true) ;

        for( uint i = 0 ; i < pl->NumInteriors ; i++ ) {

            gaiaRingPtr intr = &(pl->Interiors[i]) ;
            uint n = intr->Points ;

            LinearRing ir ;

            tile_coords(intr->Coords[0], intr->Coords[1], cx, cy) ;
            ir.push_back(EncodedVertex{cx - start_x, cy - start_y}) ;
            start_x = cx ; start_y = cy ;

            for(uint32_t count = 2 ; count <2 *n ; count += 2) {
                int cx, cy ;
                tile_coords(intr->Coords[count], intr->Coords[count+1], cx, cy) ;

                int dx = cx - start_x, dy = cy - start_y ;

                if ( dx == 0 && dy == 0 ) continue ;

                ir.push_back(EncodedVertex{dx, dy}) ;

                start_x = cx ; start_y = cy ;
            }

            encodeRing(ir, true) ;
        }
    }

    setFeatureAttributes(attr) ;
}

void VectorTileWriter::setFeatureAttributes(const Dictionary &attr)
{
    assert(current_feature_) ;

    // iterate over keys
    DictionaryIterator kit(attr) ;

    while ( kit ) {
        auto ikey = key_map_.find(kit.key()) ;
        uint32_t count ;
        if ( ikey == key_map_.end() ) {
            count = key_map_.size() ;
            key_map_.insert(make_pair(kit.key(), count)) ;
        }
        else count = ikey->second ;

        current_feature_->add_tags(count) ;

        auto ival = value_map_.find(kit.value()) ;

        if ( ival == value_map_.end() ) {
            count = value_map_.size() ;
            value_map_.insert(make_pair(kit.value(), count)) ;
        }
        else count = ival->second ;

        current_feature_->add_tags(count) ;

        ++kit ;
    }
}

void VectorTileWriter::encodeRing(const LinearRing &r, bool close)
{
    int n = r.size() ;

    if ( n == 0 ) return ;

    // moveto first point
    addGeomCmd(1) ;
    addPoint(r[0].x_, r[0].y_) ;

    // lineto (remaining points)

    addGeomCmd(2, n-1) ;

    for(uint32_t count = 1 ; count < n ; count ++)
        addPoint(r[count].x_, r[count].y_) ;

    if ( close )
        addGeomCmd(7) ; // close path
}
#endif
