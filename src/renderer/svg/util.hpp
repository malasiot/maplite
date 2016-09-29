#ifndef __SVG_PRIVATE_H__
#define __SVG_PRIVATE_H__

#include <boost/shared_ptr.hpp>
#include <vector>
#include <stdexcept>
#include <string.h>

void eatwhite(const char *&p) ;

void eatwhite_comma(const char *&p) ;

void css_parse_number_list(const char *&p, char terminator, std::vector<double> &numbers) ;

bool parseFloatingPoint(const std::string &str, double &val) ;

bool parse_coordinate_list(const std::string &p, std::vector<float> &args) ;
bool parse_coordinate_list(const std::string &p, float &arg1, float &arg2) ;
bool parse_coordinate_list(const std::string &p, float &arg1, float &arg2, float &arg3, float &arg4) ;
bool parse_coordinate_list(const std::string &p, float &arg1, float &arg2, float &arg3, float &arg4, float &arg5, float &arg6) ;
bool parse_coordinate_list(const std::string &p, float &arg1, float &arg2, float &arg3, float &arg4, float &arg5, float &arg6, float &arg7) ;

bool parse_css_color(const std::string &str, unsigned int &clr) ;

std::string parseUri(const std::string &str) ;

#endif
