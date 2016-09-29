#ifndef __PARSE_UTIL_H__
#define __PARSE_UTIL_H__

#include <string>
#include <vector>

bool parse_css_color(const std::string &src, unsigned int &packed_color) ;
bool parse_number_list(const std::string &src, std::vector<double> &v) ;

bool parseNumber(const std::string &str, double &val) ;


#endif
