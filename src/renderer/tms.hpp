#ifndef __GEOM_HELPERS_H__
#define __GEOM_HELPERS_H__

#include <cstdint>
#include <vector>


namespace tms {

// Resolution (meters/pixel) for given zoom level (measured at Equator)
double resolution(uint32_t zoom) ;

// Converts given lat/lon in WGS84 Datum to XY in Spherical Mercator EPSG:900913
void latlonToMeters(double lat, double lon, double &mx, double &my)  ;

// Converts XY point from Spherical Mercator EPSG:900913 to lat/lon in WGS84 Datum
void metersToLatLon(double mx, double my, double &lat, double &lon) ;

//Converts pixel coordinates in given zoom level of pyramid to EPSG:900913
void pixelsToMeters(double px, double py, uint32_t zoom, double &mx, double &my) ;

//Converts EPSG:900913 to pyramid pixel coordinates in given zoom level
void metersToPixels(double mx, double my, uint32_t zoom, double &px, double &py) ;

//Returns a tile covering region in given pixel coordinates
void pixelsToTile(double px, double py, uint32_t &tx, uint32_t &ty) ;

void tileToPixels(uint32_t tx, uint32_t ty, double &px, double &py) ;

// Returns tile for given mercator coordinates
void metersToTile(double mx, double my, uint32_t zoom, int32_t &tx, int32_t &ty) ;

// Returns bounds of the given tile in EPSG:900913 coordinates
void tileBounds(uint32_t tx, uint32_t ty, uint32_t zoom, double &minx, double &miny, double &maxx, double &maxy, uint32_t buffer=0) ;

// Returns bounds of the given tile in latutude/longitude using WGS84 datum
void tileLatLonBounds(uint32_t tx, uint32_t ty, uint32_t zoom, double &minLat, double &minLon, double &maxLat, double &maxLon) ;

void tilesWithinBounds(double minLat, double minLon, double maxLat, double maxLon, uint32_t zoom,
                       int32_t &tx_min, int32_t &ty_min, int32_t &tx_max, int32_t &ty_max) ;
}




















#endif
