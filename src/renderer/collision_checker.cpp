#include "collision_checker.hpp"

#include <math.h>
#include <float.h>

using namespace std ;

bool OBB::overlaps1Way(const OBB& other) const
{
    for (int a = 0; a < 2; ++a) {

        double t = other.corner[0].dot(axis[a]);

        // Find the extent of box 2 on axis a
        double tMin = t;
        double tMax = t;

        for (int c = 1; c < 4; ++c) {
            t = other.corner[c].dot(axis[a]);

            if (t < tMin) {
                tMin = t;
            } else if (t > tMax) {
                tMax = t;
            }
        }

        // We have to subtract off the origin

        // See if [tMin, tMax] intersects [0, 1]
        if ((tMin > 1 + origin[a]) || (tMax < origin[a])) {
            // There was no intersection along this dimension;
            // the boxes cannot possibly overlap.
            return false;
        }
    }

    // There was no dimension along which there is no intersection.
    // Therefore the boxes overlap.
    return true;
}

void OBB::computeAxes()
{
    axis[0] = corner[1] - corner[0];
    axis[1] = corner[3] - corner[0];

    // Make the length of each axis 1/edge length so we know any
    // dot product must be less than 1 to fall within the edge.

    for (int a = 0; a < 2; ++a) {
        axis[a] /= axis[a].squaredLength();
        origin[a] = corner[0].dot(axis[a]);
    }
}

OBB::OBB(double cx, double cy, double angle, double w, double h)
{
    Vector2 center(cx, cy) ;
    Vector2 X( w*cos(angle)/2, w*sin(angle)/2);
    Vector2 Y(-h*sin(angle)/2, h*cos(angle)/2);

    corner[0] = center - X - Y;
    corner[1] = center + X - Y;
    corner[2] = center + X + Y;
    corner[3] = center - X + Y;

    computeAxes();

    minx = DBL_MAX ; miny = DBL_MAX ;
    maxx = -DBL_MAX ; maxy = -DBL_MAX ;

    for(int i=0 ; i<4 ; i++)
    {
        minx = std::min(minx, corner[i].x) ;
        miny = std::min(miny, corner[i].y) ;
        maxx = std::max(maxx, corner[i].x) ;
        maxy = std::max(maxy, corner[i].y) ;
    }
}

bool OBB::overlaps(const OBB& other) const {
    if ( maxx < other.minx ) return false ;
    if ( maxy < other.miny ) return false ;
    if ( minx > other.maxx ) return false ;
    if ( miny > other.maxy ) return false ;

    return overlaps1Way(other) && other.overlaps1Way(*this);
}


bool CollisionChecker::addLabelBox(double cx, double cy, double angle, double w, double h, int32_t id, int32_t item)
{
    OBB box(cx, cy, angle, w, h) ;

    for(int i=0 ; i<labels_.size() ; i++ )
    {
        const Label &l = labels_[i] ;

        for(int j=0 ; j<l.boxes_.size() ; j++ ) {
            if ( l.uid_ == id && l.item_ == item ) continue ;
            if ( box.overlaps(l.boxes_[j])) return false ;
        }
    }

    Label label ;
    label.boxes_.push_back(box) ;
    label.uid_ = id ;
    label.item_ = item ;
    labels_.push_back(label) ;

    return true ;
}

bool CollisionChecker::addLabelBox(const std::vector<OBB> &boxes, int32_t id, int32_t item)
{
    for(int i=0 ; i<labels_.size() ; i++ )
    {
        const Label &l = labels_[i] ;

        for(int j=0 ; j<boxes.size() ; j++ )
                for(int k=0 ; k<l.boxes_.size() ; k++ )
            if ( l.uid_ != id && l.item_ != item && boxes[j].overlaps(l.boxes_[k])) return false ;
    }

    Label label ;
    label.boxes_ = boxes ;
    label.uid_ = id ;
    label.item_ = item ;
    labels_.push_back(label) ;

    return true ;
}
