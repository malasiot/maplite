#include "tms.hpp"

#include <cmath>

using namespace std ;
namespace tms {

constexpr double tile_size = 256 ;
constexpr double gm_initial_resolution = 2 * M_PI * 6378137 / tile_size ;
constexpr double gm_origin_shift = 2 * M_PI * 6378137 / 2.0 ;

// Resolution (meters/pixel) for given zoom level (measured at Equator)
double resolution(uint32_t zoom) {
    return gm_initial_resolution / pow(2, zoom) ;
}
// Converts given lat/lon in WGS84 Datum to XY in Spherical Mercator EPSG:900913
void latlonToMeters(double lat, double lon, double &mx, double &my) {
    mx = lon * gm_origin_shift / 180.0 ;
    my = log( tan((90 + lat) * M_PI / 360.0 )) / (M_PI / 180.0) ;

    my = my * gm_origin_shift / 180.0 ;
}

// Converts XY point from Spherical Mercator EPSG:900913 to lat/lon in WGS84 Datum
void metersToLatLon(double mx, double my, double &lat, double &lon) {
    lon = (mx / gm_origin_shift) * 180.0 ;
    lat = (my / gm_origin_shift) * 180.0 ;

    lat = 180 / M_PI * (2 * atan( exp( lat * M_PI / 180.0)) - M_PI / 2.0) ;
}

//Converts pixel coordinates in given zoom level of pyramid to EPSG:900913
void pixelsToMeters(double px, double py, uint32_t zoom, double &mx, double &my) {
    double res = resolution( zoom ) ;
    mx = px * res - gm_origin_shift ;
    my = py * res - gm_origin_shift ;
}

//Converts EPSG:900913 to pyramid pixel coordinates in given zoom level
void metersToPixels(double mx, double my, uint32_t zoom, double &px, double &py) {
    double res = resolution( zoom ) ;
    px = (mx + gm_origin_shift) / res ;
    py = (my + gm_origin_shift) / res ;
}

//Returns a tile covering region in given pixel coordinates
void pixelsToTile(double px, double py, int32_t &tx, int32_t &ty) {
    tx = int( ceil( px / float(tile_size) ) - 1 ) ;
    ty = int( ceil( py / float(tile_size) ) - 1 ) ;
}

void tileToPixels(uint32_t tx, uint32_t ty, double &px, double &py) {
    px = tx * tile_size ;
    py = ty * tile_size ;
}

// Returns tile for given mercator coordinates
void metersToTile(double mx, double my, uint32_t zoom, int32_t &tx, int32_t &ty) {
    double px, py ;
    metersToPixels(mx, my, zoom, px, py) ;
    pixelsToTile(px, py, tx, ty) ;
}
// Returns bounds of the given tile in EPSG:900913 coordinates

void tileBounds(uint32_t tx, uint32_t ty, uint32_t zoom, double &minx, double &miny, double &maxx, double &maxy, uint32_t buffer) {
    pixelsToMeters( tx*tile_size - buffer, ty*tile_size - buffer, zoom, minx, miny ) ;
    pixelsToMeters( (tx+1)*tile_size + buffer, (ty+1)*tile_size + buffer, zoom, maxx, maxy ) ;
}

// Returns bounds of the given tile in latutude/longitude using WGS84 datum
void tileLatLonBounds(uint32_t tx, uint32_t ty, uint32_t zoom, double &minLat, double &minLon, double &maxLat, double &maxLon) {

    double minx, miny, maxx, maxy ;
    tileBounds(tx, ty, zoom, minx, miny, maxx, maxy) ;
    metersToLatLon(minx, miny, minLat, minLon) ;
    metersToLatLon(maxx, maxy, maxLat, maxLon) ;
}

void tilesWithinBounds(double minLat, double minLon, double maxLat, double maxLon, uint32_t zoom,
                       int32_t &tx_min, int32_t &ty_min, int32_t &tx_max, int32_t &ty_max)  {
    double minx, miny, maxx, maxy ;

    latlonToMeters(minLat, minLon, minx, miny) ;
    latlonToMeters(maxLat, maxLon, maxx, maxy) ;

    metersToTile(minx, miny, zoom, tx_min, ty_min) ;
    metersToTile(maxx, maxy, zoom, tx_max, ty_max) ;
}


}
