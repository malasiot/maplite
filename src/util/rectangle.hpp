#ifndef __RECTANGLE_HPP__
#define __RECTANGLE_HPP__

#include <cstdint>

class Rect
{
public:

    int32_t x_, y_ ;
    uint32_t width_, height_ ;

public:

    Rect(int32_t left=0, int32_t top=0, uint32_t width=0, uint32_t height=0);

    Rect intersection(const Rect&) const;	// intersection
    Rect runion(const Rect&) const;	// union

    Rect &translate(int32_t ox, int32_t oy);			// translate
    Rect translated(int32_t ox, int32_t oy) const ;

    bool contains(int32_t x, int32_t y) const;
    bool contains(const Rect &) const;

    void include(int32_t x, int32_t y) ;

    bool intersects(const Rect &) const;

    Rect &move(int32_t x, int32_t y);
    Rect moved(int32_t x, int32_t y) const;

    friend std::ostream & operator << (std::ostream &strm, const Rect &r) ;
};

inline Rect::Rect(int32_t left, int32_t top, uint32_t width, uint32_t height):
    x_(left), y_(top), width_(width), height_(height){
}

inline Rect Rect::intersection(const Rect& r) const
{
    int32_t tlx = std::max(x_, r.x_) ;
    int32_t tly = std::max(y_, r.y_) ;
    int32_t brx = std::min(x_ + width_, r.x_ + r.width_) ;
    int32_t bry = std::min(y_ + height_, r.y_ + r.height_) ;

    return Rect(tlx, tly, std::max(0, brx - tlx), std::max(0, bry - tly)) ;
}


inline Rect Rect::runion(const Rect& r) const
{
    int32_t tlx = std::min(x_, r.x_) ;
    int32_t tly = std::min(y_, r.y_) ;
    int32_t brx = std::max(x_ + width_, r.x_ + r.width_) ;
    int32_t bry = std::max(y_ + height_, r.y_ + r.height_) ;

    return Rect(tlx, tly, std::max(0, brx - tlx), std::max(0, bry - tly)) ;
}

inline Rect & Rect::translate(int32_t ox, int32_t oy) {
    x_ += ox ; y_ += oy ;
    return *this ;
}

inline Rect Rect::translated(int32_t ox, int32_t oy) const {
    return Rect(x_ + ox, y_ + oy, width_, height_) ;
}


inline bool Rect::contains(int32_t px, int32_t py) const {
    return (x_ <= px && y_ <= py ) && ( px < x_ + (int32_t)width_ && py < y_ + (int32_t)height_);
}

inline bool Rect::contains(const Rect& r) const
{
    return ( contains(r.x_, r.y_) && contains(r.x_ + (int32_t)r.width_ -1, r.y_ + (int32_t)r.height_ - 1) );
}

inline bool Rect::intersects(const Rect& r) const
{
    return ( std::max(x_, r.x_) < std::min(x_ + (int32_t)width_ - 1, r.x_ + (int32_t)r.width_ - 1) &&
             std::max(y_, r.y_) < std::min(y_ + (int32_t)height_ - 1, r.y_ + (int32_t)r.height_ - 1) )  ;
}

inline void Rect::include(int32_t px, int32_t py)
{
    x_ = std::min(x_, px) ; y_ = std::min(y_, py) ;
    int32_t brx = std::max(x_ + (int32_t)width_, px) ; int32_t bry = std::max(y_ + (int32_t)height_ - 1, py) ;
    width_ = brx - x_ ; height_ = bry - y_ ;
}

inline Rect &Rect::move(int32_t x, int32_t y) {
    x_ = x ; y_ = y ;
    return *this ;
}

inline Rect Rect::moved(int32_t x, int32_t y) const {
    return Rect(x, y, width_, height_) ;
    return *this ;
}

inline std::ostream & operator << (std::ostream &strm, const Rect &r) {
    return strm << "{ " << r.x_ << ',' << r.y_ << " " << r.width_ << ',' << r.height_ << " }" ;
}













#endif
