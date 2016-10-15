#ifndef __PROJECTION_H__
#define __PROJECTION_H__

#include <string>

using std::string ;

class Projection {

public:

    Projection (): proj_(0), proj_ctx_(0), is_geographic_(false) {}
    ~Projection();

    bool init(const string &pj_params_);

    bool isGeographic() const { return is_geographic_ ; }

    void forward(double x, double y, double &px, double &py) const;
    void forward(double &x, double &y) const;

    void inverse(double x, double y, double &px, double &py) const;
    void inverse(double &x, double &y) const;

private:

    void * proj_;
    void * proj_ctx_;
    bool is_geographic_ ;
};



#endif
