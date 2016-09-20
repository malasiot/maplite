#ifndef __VECTOR_TILE_WRITTER_H__
#define __VECTOR_TILE_WRITTER_H__

#include "dictionary.hpp"

#include <vector>
#include <string>
#include <map>

#include <sqlite3.h>
#include <spatialite.h>

#include "geom_helpers.hpp"

struct EncodedVertex {
    int32_t x_, y_ ;
} ;

typedef std::vector<EncodedVertex> LinearRing ;

class VectorTileWriter {
public:

    VectorTileWriter(uint32_t tx, uint32_t ty, uint32_t tz, uint32_t te = 4096):
        te_(te), tz_(tz), tx_(tx), ty_(ty) {
        compute_extents() ;
    }

    uint32_t x() const { return tx_ ; }
    uint32_t y() const { return ty_ ; }
    uint32_t z() const { return tz_ ; }
    uint32_t extent() const { return te_ ; }
    BBox box(uint32_t buffer = 128) const {
        BBox res ;
        tms::tileBounds(tx_, ty_, tz_, res.minx_, res.miny_, res.maxx_, res.maxy_, buffer) ;
        res.srid_ = 3857 ;
        return res ;
   }

    void beginLayer(const std::string &name) ;
    void endLayer() ;
    void encodeFeatures(const gaiaGeomCollPtr &geom, const Dictionary &attr);

    std::string toString(bool compress = true) { return std::string() ; }

private:

 /*   vector_tile::Tile tile_msg_ ;
    vector_tile::Tile_Layer *current_layer_ ;
    vector_tile::Tile_Feature *current_feature_ ;
*/
    double tile_origin_x_, tile_origin_y_ ;
    uint32_t te_, tx_, ty_, tz_ ;
    BBox box_ ;

    std::map<std::string, uint32_t> key_map_, value_map_ ;

private:

    void compute_extents() {
        tms::tileBounds(tx_, ty_, tz_, box_.minx_, box_.miny_, box_.maxx_, box_.maxy_) ;
        box_.srid_ = 3857 ;
    }

    void tile_coords(double x, double y, int &tx, int &ty) {
        tx = te_ * ( x - box_.minx_ )/( box_.maxx_ - box_.minx_ ) ;
        ty = te_ * ( box_.maxy_ - y )/( box_.maxy_ - box_.miny_ ) ;
    }

    void addPoint(int x, int y) ;
    void addGeomCmd(uint32_t cmd, uint32_t count=1) ;
    void encodePointGeometry(const gaiaGeomCollPtr &geom, const Dictionary &attr) ;
    void encodeLineGeometry(const gaiaGeomCollPtr &geom, const Dictionary &attr) ;
    void encodePolygonGeometry(const gaiaGeomCollPtr &geom, const Dictionary &attr) ;
    void setFeatureAttributes(const Dictionary &attr) ;
    void encodeRing(const LinearRing &r, bool close) ;
};











#endif
