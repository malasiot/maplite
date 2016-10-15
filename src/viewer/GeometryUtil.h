#ifndef __GEOMETRY_UTIL_H__
#define __GEOMETRY_UTIL_H__

#include "Renderer.h"

#include <vector>


// samples a geometry collection (linestrings and polygon outlines) and samples points and their angle along them

void sampleLinearGeometry(std::vector<double> &samples, // output samples
                    const gaiaGeomCollPtr geom_,  // geometry collection
                    const cairo_matrix_t &cmm,    // map to device transformation
                    float gap_ = 0.0,             // gap between consecutive samples (if 0.0 then mid line sampling takes place)
                    float initialGap_ = 0.0,      // initial gap
                    const std::vector<double> &offsets = std::vector<double>(1, 0), // offsets for multi-point sampling
                    bool fix_orientation = true // change direction so labels are upright
                ) ;

// samples a point from each geometry type. a) point geometries: as is, b) line geometries: mid-point, c) polygon geometries: centroid

void samplePointGeometry(std::vector<double> &samples, // output samples
                         const gaiaGeomCollPtr geom_) ;  // geometry collection

gaiaGeomCollPtr offsetGeometry(
                    const gaiaGeomCollPtr geom_,  // geometry collection
                    double offset // offset in user coordinates
                ) ;

#endif
