#ifndef __COLLISION_CHECKER_H__
#define __COLLISION_CHECKER_H__

#include <vector>

struct Vector2 {

    Vector2(): x(0), y(0) {}
    Vector2(double x_, double y_): x(x_), y(y_) {}

    friend Vector2 operator - ( const Vector2 &op1, const Vector2 &op2 ) {
        return Vector2(op1.x - op2.x, op1.y - op2.y) ;
    }

    friend Vector2 operator + ( const Vector2 &op1, const Vector2 &op2 ) {
        return Vector2(op1.x + op2.x, op1.y + op2.y) ;
    }

    Vector2 &operator /= (double s) {
        x /= s ; y /= s ;
        return *this ;
    }

    double squaredLength() const {
        return x * x + y * y ;
    }

    double dot(const Vector2 &other) const {
        return x * other.x + y * other.y ;
    }

    double x, y ;

};

class OBB {
public:
    OBB(double cx_, double cy_, double angle_, double width_, double height_) ;

    bool overlaps(const OBB& other) const ;

private:
    void computeAxes() ;
    bool overlaps1Way(const OBB& other) const ;

    Vector2         corner[4];
    Vector2         axis[2];
    double          origin[2];
    double minx, miny, maxx, maxy ;
};

class CollisionChecker {

public:

    CollisionChecker() {}

    // check collision between given OBB and stored labels

    bool addLabelBox(double cx, double cy, double angle, double width, double height) ;
    bool addLabelBox(const std::vector<OBB> &boxes) ;

private:

    struct Label {
        std::vector<OBB> boxes ;
    };

    std::vector<Label> labels ;

};





#endif
