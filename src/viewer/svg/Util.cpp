#include "Util.h"

#include "ParseUtil.h"

#include <math.h>
#include <string.h>
#include <errno.h>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>


using namespace std;

namespace svg {


void eatwhite(const char *&p)
{
  while (*p != 0 && *p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' ) ++p ;
}

void eatwhite_comma(const char *&p)
{
  while (*p != 0 && *p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == ',' ) ++p ;
}


void css_parse_number_list(const char *&p, char terminator, vector<double> &numbers)
{
    if ( !p ) return ;

    while ( *p && *p != terminator )
  {
      const char *q = 0 ;
      eatwhite_comma(p) ;
      double d = strtod(p, (char **)&q) ;

    numbers.push_back(d) ;
      eatwhite_comma(q) ;
    p = q ;
    }

    if ( *p == terminator ) ++p ;

}


bool parseFloatingPoint(const std::string &str, double &val)
{
    val = strtod (str.c_str(), NULL);

    if ((val == -HUGE_VAL || val == HUGE_VAL) && (ERANGE == errno)) return false ;
    else return true ;
}


bool parse_coordinate_list(const std::string &p, vector<float> &args)
{
    vector<string> strs;
    boost::algorithm::split(strs, p, boost::is_any_of(" ,"), boost::token_compress_on);

    for(int i=0 ; i<strs.size() ; i++ )
    {
        if ( strs[i].empty() ) continue ;

        double val ;
        if ( !parseFloatingPoint(strs[i], val)) return false ;
        else args.push_back((float)val) ;
    }

    return true ;
}

bool parse_coordinate(const std::string &p, float &arg)
{
    double val ;
    if ( !parseFloatingPoint(p, val)) return false ;
    else arg = (float)val ;

}


bool parse_coordinate_list(const std::string &p, float &arg1, float &arg2)
{
    vector<float> args ;
    if ( !parse_coordinate_list(p, args) || args.size() != 2 ) return false ;
    else {
        arg1 = args[0] ; arg2 = args[1] ;
        return true ;
    }
}

bool parse_coordinate_list(const std::string &p, float &arg1, float &arg2, float &arg3, float &arg4)
{
    vector<float> args ;
    if ( !parse_coordinate_list(p, args) || args.size() != 4 ) return false ;
    else {
        arg1 = args[0] ; arg2 = args[1] ; arg3 = args[2] ; arg4 = args[3] ;
        return true ;
    }
}


bool parse_coordinate_list(const string &p, float &arg1, float &arg2, float &arg3, float &arg4, float &arg5, float &arg6)
{
    vector<float> args ;
    if ( !parse_coordinate_list(p, args) || args.size() != 6 ) return false ;
    else {
        arg1 = args[0] ; arg2 = args[1] ; arg3 = args[2] ; arg4 = args[3] ;
        arg5 = args[4] ; arg6 = args[5] ;
        return true ;
    }
}

bool parse_coordinate_list(const std::string &p, float &arg1, float &arg2, float &arg3, float &arg4,
                                                                    float &arg5, float &arg6, float &arg7)
{
    vector<float> args ;
    if ( !parse_coordinate_list(p, args) || args.size() != 7 ) return false ;
    else {
        arg1 = args[0] ; arg2 = args[1] ; arg3 = args[2] ; arg4 = args[3] ;
        arg5 = args[4] ; arg6 = args[5] ; arg7 = args[6] ;
        return true ;
    }
}

string parseUri(const string &str)
{
    boost::smatch what ;

    static boost::regex rx_url("[\\s]*url[\\s]*\\([\\s]*([^)]+)[\\s]*\\)[\\s]*")
            ;
    if ( boost::regex_match(str, what, rx_url) )
          return what.str(1) ;
    else return string() ;

}

Transform Transform::inverse(const Transform &src)
{
    Transform dst ;

    double r_det;

    r_det = 1.0 / (src.m[0] * src.m[3] - src.m[1] * src.m[2]);
    dst.m[0] = src.m[3] * r_det;
    dst.m[1] = -src.m[1] * r_det;
    dst.m[2] = -src.m[2] * r_det;
    dst.m[3] = src.m[0] * r_det;
    dst.m[4] = -src.m[4] * dst.m[0] - src.m[5] * dst.m[2];
    dst.m[5] = -src.m[4] * dst.m[1] - src.m[5] * dst.m[3];

    return dst ;
}

Transform Transform::flip(const Transform &src, bool horz, bool vert)
{
    Transform dst ;

    dst.m[0] = horz ? -src.m[0] : src.m[0];
    dst.m[1] = horz ? -src.m[1] : src.m[1];
    dst.m[2] = vert ? -src.m[2] : src.m[2];
    dst.m[3] = vert ? -src.m[3] : src.m[3];
    dst.m[4] = horz ? -src.m[4] : src.m[4];
    dst.m[5] = vert ? -src.m[5] : src.m[5];

    return dst ;
}

#define EPSILON 1e-6

Transform Transform::multiply(const Transform &src1, const Transform &src2)
{
    Transform dst ;

    double d0, d1, d2, d3, d4, d5;

    d0 = src1.m[0] * src2.m[0] + src1.m[1] * src2.m[2];
    d1 = src1.m[0] * src2.m[1] + src1.m[1] * src2.m[3];
    d2 = src1.m[2] * src2.m[0] + src1.m[3] * src2.m[2];
    d3 = src1.m[2] * src2.m[1] + src1.m[3] * src2.m[3];
    d4 = src1.m[4] * src2.m[0] + src1.m[5] * src2.m[2] + src2.m[4];
    d5 = src1.m[4] * src2.m[1] + src1.m[5] * src2.m[3] + src2.m[5];
    dst.m[0] = d0;
    dst.m[1] = d1;
    dst.m[2] = d2;
    dst.m[3] = d3;
    dst.m[4] = d4;
    dst.m[5] = d5;

    return dst ;
}

Transform Transform::identity()
{
    Transform dst ;

    dst.m[0] = 1;  dst.m[1] = 0;  dst.m[2] = 0;
    dst.m[3] = 1;  dst.m[4] = 0;  dst.m[5] = 0;

    return dst ;
}

Transform Transform::scaling(double sx, double sy)
{
    Transform dst ;

    dst.m[0] = sx;  dst.m[1] = 0;  dst.m[2] = 0;
    dst.m[3] = sy;  dst.m[4] = 0;  dst.m[5] = 0;

    return dst ;
}

Transform Transform::rotation(double theta)
{
    Transform dst ;

    double s, c;

    s = sin (theta * M_PI / 180.0);
    c = cos (theta * M_PI / 180.0);

    dst.m[0] = c;    dst.m[1] = s;    dst.m[2] = -s;
    dst.m[3] = c;    dst.m[4] = 0;    dst.m[5] = 0;

    return dst ;
}

Transform Transform::shearing(double theta)
{
    Transform dst ;
    double t;

    t = tan (theta * M_PI / 180.0);
    dst.m[0] = 1;    dst.m[1] = 0;    dst.m[2] = t;
    dst.m[3] = 1;    dst.m[4] = 0;    dst.m[5] = 0;

    return dst ;
}

Transform Transform::translation(double tx, double ty)
{
    Transform dst ;

    dst.m[0] = 1;  dst.m[1] = 0;  dst.m[2] = 0;
    dst.m[3] = 1;  dst.m[4] = tx;  dst.m[5] = ty;

    return dst ;
}

double Transform::expansion(const Transform &src)
{
    return sqrt (fabs (src.m[0] * src.m[3] - src.m[1] * src.m[2]));
}

bool Transform::isRectilinear(const Transform &src)
{
    return ((fabs (src.m[1]) < EPSILON && fabs (src.m[2]) < EPSILON) ||
            (fabs (src.m[0]) < EPSILON && fabs (src.m[3]) < EPSILON));
}

bool Transform::isEqual(const Transform &matrix1, const Transform &matrix2)
{
    return (fabs (matrix1.m[0] - matrix2.m[0]) < EPSILON &&
            fabs (matrix1.m[1] - matrix2.m[1]) < EPSILON &&
            fabs (matrix1.m[2] - matrix2.m[2]) < EPSILON &&
            fabs (matrix1.m[3] - matrix2.m[3]) < EPSILON &&
            fabs (matrix1.m[4] - matrix2.m[4]) < EPSILON &&
                        fabs (matrix1.m[5] - matrix2.m[5]) < EPSILON);
}

Transform::Transform()
{
    m[0] = 1;  m[1] = 0;  m[2] = 0;
    m[3] = 1;  m[4] = 0;  m[5] = 0;
}

Transform::Transform(double s0, double s1, double s2, double s3, double s4, double s5)
{
    m[0] = s0 ; m[1] = s1 ; m[2] = s2 ;
    m[3] = s3 ; m[4] = s4 ; m[5] = s5 ;
}

bool Transform::fromString(const std::string &str)
{
    Transform &trs = *this ;

    const char *p = str.c_str() ;
    eatwhite(p) ;

    while ( *p )
    {
        if ( strncmp(p, "matrix", 6) == 0 )
        {
            vector<double> nums ;
            p += 6 ;

            while ( *p != 0 && *p != '(' ) ++p ;
            if ( *p != '(' ) return false ;
            ++p ;

            css_parse_number_list(p,  ')', nums) ;

            if ( nums.size() >= 6 )
            {
                Transform d(nums[0], nums[1], nums[2], nums[3], nums[4], nums[5]) ;
                trs = Transform::multiply(d, trs) ;
            }

        }
        else if ( strncmp(p, "translate", 9) == 0 )
        {
            p += 9 ;
            vector<double> nums ;

            while ( *p != 0 && *p != '(' ) ++p ;
            if ( *p != '(' ) return false ;
            ++p ;

            css_parse_number_list(p, ')', nums) ;

            if ( nums.size() >= 2 )
            {
                Transform d = Transform::translation(nums[0], nums[1]) ;
                trs = Transform::multiply(d, trs) ;
            }
            else if ( nums.size() == 1 )
            {
                Transform d = Transform::translation(nums[0], 0) ;
                trs = Transform::multiply(d, trs) ;
            }

        }
        else if ( strncmp(p, "rotate", 6) == 0 )
        {
            p += 6 ;
            vector<double> nums ;

            while ( *p != 0 && *p != '(' ) ++p ;
            if ( *p != '(' ) return false ;
            ++p ;

            css_parse_number_list(p,  ')', nums) ;

            if ( nums.size() == 1 )
            {
                Transform d = Transform::rotation(nums[0]) ;
                trs = Transform::multiply(d, trs) ;
            }
            else if ( nums.size() == 3 )
            {
                //?
                Transform tc =  Transform::translation(-nums[1], -nums[2]) ;
                Transform r =   Transform::rotation(nums[0]) ;
                Transform tmc = Transform::translation(nums[1], nums[2]) ;
                trs = Transform::multiply(trs, Transform::multiply(tc, Transform::multiply(r, tmc))) ;
            }

        }
        else if ( strncmp(p, "scale", 5) == 0 )
        {
            p += 5 ;

            vector<double> nums ;

            while ( *p != 0 && *p != '(' ) ++p ;
            if ( *p != '(' ) return false ;
            ++p ;

            css_parse_number_list(p,  ')', nums) ;

            if ( nums.size() == 1 )
            {
                Transform d = Transform::scaling(nums[0], nums[0]) ;
                trs = Transform::multiply(d, trs) ;
            }
            else if ( nums.size() >= 2 )
            {
                Transform d = Transform::scaling(nums[0], nums[1]) ;
                trs = Transform::multiply(d, trs) ;
            }
        }
        else if ( strncmp(p, "skewX", 5) == 0 )
        {
            p += 5 ;

            vector<double> nums ;

            while ( *p != 0 && *p != '(' ) ++p ;
            if ( *p != '(' ) return false ;
            ++p ;

            css_parse_number_list(p, ')', nums) ;

            if ( nums.size() >= 1 )
            {
                Transform d = Transform::shearing(nums[0]) ;
                trs = Transform::multiply(d, trs) ;
            }

        }
        else if ( strncmp(p, "skewY", 5) == 0 )
        {
            p += 5 ;

            vector<double> nums ;

            while ( *p != 0 && *p != '(' ) ++p ;
            if ( *p != '(' ) return false ;
            ++p ;

            css_parse_number_list(p, ')', nums) ;

            if ( nums.size() >= 1 )
            {
                Transform d = Transform::shearing(nums[0]) ;
                d.m[1] = d.m[2] ;
                d.m[2] = 0 ;
                trs = Transform::multiply(d, trs) ;
            }
        }

        eatwhite_comma(p) ;
    }

    return true ;

}


bool Length::fromString(const std::string &str, bool scale)
{
    static boost::regex rx("(?:([+-]?[0-9]+)|([+-]?[0-9]*\\.[0-9]+(?:[Ee][+-]?[0-9]+)?))(em|ex|px|in|cm|mm|pt|pc|\\%)?", boost::regex::icase) ;

    boost::smatch what ;
    float length ;

    if ( boost::regex_match(str, what, rx) )
    {
        double val ;

        string num = ( what.str(1).empty() ) ? what[2] : what[1] ;

        if ( !parseFloatingPoint(num, val) )
            return false ;
        else {
            valueInSpecifiedUnits = val ;
            scaleToViewport = scale ;

            string unit = what[3] ;

            if ( boost::iequals(unit, "em" ) )
                unitType = EMSLengthType ;
            else if ( boost::iequals(unit, "ex") )
                unitType = EXSLengthType;
            else if ( boost::iequals(unit, "px") )
                unitType = PXLengthType;
            else if ( boost::iequals(unit, "in") )
                unitType = INLengthType;
            else if ( boost::iequals(unit, "cm") )
                unitType = CMLengthType;
            else if ( boost::iequals(unit, "mm") )
                unitType = MMLengthType;
            else if ( boost::iequals(unit, "pt") )
                unitType = PTLengthType;
            else if ( boost::iequals(unit, "pc") )
                unitType = PCLengthType;
            else if ( unit == "%" ) {
                unitType = PercentageLengthType ;
                valueInSpecifiedUnits /= 100.0 ;
            }
            else
                unitType = NumberLengthType;
        }
    }
    else return false ;

    return true ;
}

bool Length::parseList(const string &str, std::vector<Length> &ls)
{
    typedef boost::tokenizer<boost::char_separator<char> >  tokenizer;
    boost::char_separator<char> sep(" ,");
    tokenizer tokens(str, sep);

    for (tokenizer::iterator tok_iter = tokens.begin();  tok_iter != tokens.end(); ++tok_iter)
    {
        Length l ;

        string token = *tok_iter  ;

        if ( !l.fromString(token) ) return false ;
        else ls.push_back(l) ;
    }

    return true ;
}

bool ViewBox::fromString(const std::string &str)
{
    return parse_coordinate_list(str, x, y, w, h) ;
}

bool PreserveAspectRatio::fromString(const std::string &str)
{
    boost::regex rx("(?:(defer)[\\s]+)?([a-zA-Z]+)(?:[\\s]+(meet|slice))?") ;

    viewBoxAlign = NoViewBoxAlign ;
    viewBoxPolicy = MeetViewBoxPolicy ;

    boost::smatch what ;

    if ( !boost::regex_match(str, what, rx) ) return false ;

    deferAspectRatio = false ;

    if ( what.str(1) == "defer" )
        deferAspectRatio = true ;

    string align = what.str(2) ;

    if ( align == "none" )
        viewBoxAlign = NoViewBoxAlign ;
    else if ( align == "xMinYMin" )
        viewBoxAlign = XMinYMin ;
    else if ( align == "xMidYMin" )
        viewBoxAlign = XMidYMin ;
    else if ( align == "xMaxYMin" )
        viewBoxAlign = XMaxYMin ;
    else if ( align == "xMinYMid" )
        viewBoxAlign = XMinYMid ;
    else if ( align == "xMidYMid" )
        viewBoxAlign = XMidYMid ;
    else if ( align == "xMaxYMid" )
        viewBoxAlign = XMaxYMid ;
    else if ( align == "xMinYMax" )
        viewBoxAlign = XMinYMax ;
    else if ( align == "xMidYMax" )
        viewBoxAlign = XMidYMax ;
    else if ( align == "xMaxYMax" )
        viewBoxAlign = XMaxYMax ;

    if ( what.str(3) == "meet" )
        viewBoxPolicy = MeetViewBoxPolicy ;
    else if ( what.str(3) == "slice" )
        viewBoxPolicy = SliceViewBoxPolicy ;

    return true ;
}


void PreserveAspectRatio::constrainViewBox(double width_, double height_, ViewBox &orig)
{
    double origx = orig.x ;
    double origy = orig.y ;
    double origw = orig.w ;
    double origh = orig.h ;

    double neww, newh;

    if ( viewBoxPolicy == MeetViewBoxPolicy )
    {
        neww = width_ ;
        newh = height_ ;

        if ( height_ * origw > width_ * origh )
            newh = origh * width_ / origw ;
        else
            neww = origw * height_ / origh;
    }
    else
    {
        neww = width_ ;
        newh = height_ ;

        if ( height_ * origw < width_ * origh )
            newh = origh * width_ / origw ;
        else
            neww = origw * height_ / origh;
    }

    if ( viewBoxAlign == XMinYMin  || viewBoxAlign == XMinYMid  || viewBoxAlign == XMinYMax  ) ;
    else if ( viewBoxAlign == XMidYMin  ||	viewBoxAlign == XMidYMid  || viewBoxAlign == XMidYMax  )
        origx -= (neww - width_) / 2 ;
    else
        origx -= neww - width_ ;

    if ( viewBoxAlign == XMinYMin || viewBoxAlign == XMidYMin || viewBoxAlign == XMaxYMin ) ;
    else if ( viewBoxAlign == XMinYMid || viewBoxAlign == XMidYMid || viewBoxAlign == XMaxYMid )
        origy -= (newh - height_) / 2;
    else
        origy -= newh - height_ ;

    origw = neww ;
    origh = newh ;

    orig.x = origx ;
    orig.y = origy ;
    orig.w = origw ;
    orig.h = origh ;
}

Transform PreserveAspectRatio::getViewBoxTransform(double sw, double sh, double vwidth, double vheight, double vx, double vy)
{
    Transform trs ;

    if ( vwidth != 0.0 && vheight != 0.0 )
    {
        double vboxx = vx ;
        double vboxy = vy ;
        double vboxw = vwidth ;
        double vboxh = vheight ;

        double ofx = 0, ofy = 0 ;
        double aspScaleX = 1.0 ;
        double aspScaleY = 1.0 ;

        if ( viewBoxAlign != NoViewBoxAlign )
        {
            ViewBox vbox ;

            vbox.x = vboxx ;
            vbox.y = vboxy ;
            vbox.w = vboxw ;
            vbox.h = vboxh ;

            constrainViewBox(sw, sh, vbox) ;

            ofx = vbox.x ;
            ofy = vbox.y ;


            aspScaleX = vbox.w/vwidth ;
            aspScaleY = vbox.h/vheight ;
        }
        else {
            aspScaleX = sw/vboxw ;
            aspScaleY = sh/vboxh ;
        }

        trs = Transform::translation(-vx, -vy) ;
        trs = Transform::multiply(trs, Transform::scaling(aspScaleX, aspScaleY)) ;
        trs = Transform::multiply(trs, Transform::translation(ofx, ofy)) ;
    }

    return trs ;
}


static void
svg_path_arc_segment (double ctx[6],
                       double xc, double yc,
                       double th0, double th1, double rx, double ry, double x_axis_rotation)
{
    double sin_th, cos_th;
    double a00, a01, a10, a11;
    double x1, y1, x2, y2, x3, y3;
    double t;
    double th_half;

    sin_th = sin (x_axis_rotation * (M_PI / 180.0));
    cos_th = cos (x_axis_rotation * (M_PI / 180.0));
    /* inverse transform compared with rsvg_path_arc */
    a00 = cos_th * rx;
    a01 = -sin_th * ry;
    a10 = sin_th * rx;
    a11 = cos_th * ry;

    th_half = 0.5 * (th1 - th0);
    t = (8.0 / 3.0) * sin (th_half * 0.5) * sin (th_half * 0.5) / sin (th_half);
    x1 = xc + cos (th0) - t * sin (th0);
    y1 = yc + sin (th0) + t * cos (th0);
    x3 = xc + cos (th1);
    y3 = yc + sin (th1);
    x2 = x3 + t * sin (th1);
    y2 = y3 - t * cos (th1);

        ctx[0] = a00 * x1 + a01 * y1 ;
        ctx[1] = a10 * x1 + a11 * y1 ;
        ctx[2] = a00 * x2 + a01 * y2 ;
        ctx[3] = a10 * x2 + a11 * y2 ;
        ctx[4] = a00 * x3 + a01 * y3 ;
        ctx[5] = a10 * x3 + a11 * y3;
}


int nPath = 0 ;
bool PathData::fromString(const std::string &str)
{
    Command previousCmd  ;

    bool insideGroup = false ;

    bool isFirst = true ;

    float arg1, arg2, arg3, arg4, arg5, arg6, arg7 ;
    float cx , cy  ;
    float rx, ry ;


    boost::sregex_iterator it(str.begin(), str.end(), boost::regex("(?:([mMsShHvVlLcCQqQtTaA]+)([^mMsShHvVlLcCqQtTaAzZ]+))|([zZ])[\\s]*")) ;
    boost::sregex_iterator end ;

    while ( it != end )
    {

        string args ;
        char cmd ;

        if ( it->str(3).empty() )
        {
            cmd = it->str(1).at(0) ;
            args = it->str(2) ;
        }
        else
            cmd = 'z' ;

        if ( cmd == 'M' || cmd == 'm' )
        {
            bool isRel = ( cmd == 'm') ;

            insideGroup = true ;

            vector<float> argList ;
            if ( !parse_coordinate_list(args, argList) ) return false ;

            if ( !isRel || isFirst )
            {
                cx = argList[0] ;
                cy = argList[1] ;
            }
            else {
                cx += argList[0] ;
                cy += argList[1] ;
            }

            elements.push_back(PathData::Element(MoveToCmd, cx, cy)) ;
            isFirst = false ;

            for(int i=2 ; i<argList.size() ; i += 2)
            {
                arg1 = argList[i] ;
                arg2 = argList[i+1] ;

                if ( isRel )
                    elements.push_back(PathData::Element(LineToCmd, cx = arg1 + cx, cy = arg2 + cy)) ;
                else
                    elements.push_back(PathData::Element(LineToCmd, cx = arg1, cy = arg2)) ;
            }

            previousCmd = MoveToCmd ;
        }
        else if ( cmd == 'z' )
        {
            insideGroup = false ;

            elements.push_back(PathData::Element(ClosePathCmd)) ;

            previousCmd = ClosePathCmd ;
        }
        else if ( cmd == 'l' || cmd == 'L' )
        {
            bool isRel = ( cmd == 'l' ) ;

            vector<float> argList ;
            if ( !parse_coordinate_list(args, argList) ) return false ;

            for(int i=0 ; i<argList.size() ; i += 2)
            {
                arg1 = argList[i] ;
                arg2 = argList[i+1] ;

                if ( isRel )
                    elements.push_back(PathData::Element(LineToCmd, cx = arg1 + cx, cy = arg2 + cy)) ;
                else
                    elements.push_back(PathData::Element(LineToCmd, cx = arg1, cy = arg2)) ;
            }

            previousCmd = LineToCmd ;
        }
        else if ( cmd == 'h' || cmd == 'H' )
        {
            bool isRel = (cmd == 'h') ;

            vector<float> argList ;
            if ( !parse_coordinate_list(args, argList) ) return false ;

            for(int i=0 ; i<argList.size() ; i += 2)
            {
                arg1 = argList[i] ;
                arg2 = argList[i+1] ;

                if ( isRel )
                    elements.push_back(PathData::Element(LineToCmd, cx = arg1 + cx, cy)) ;
                else
                    elements.push_back(PathData::Element(LineToCmd, cx = arg1, cy)) ;
            }
            previousCmd = LineToCmd ;
        }
        else if ( cmd == 'v' || cmd == 'V' )
        {
            bool isRel = (cmd == 'v') ;

            vector<float> argList ;
            if ( !parse_coordinate_list(args, argList) ) return false ;

            for(int i=0 ; i<argList.size() ; i++)
            {
                arg1 = argList[i] ;

                if ( isRel )
                    elements.push_back(PathData::Element(LineToCmd, cx, cy = arg1 + cy)) ;
                else
                    elements.push_back(PathData::Element(LineToCmd, cx, cy = arg1)) ;
            }
            previousCmd = LineToCmd ;
        }
        else if ( cmd == 'c' || cmd == 'C' )
        {
            bool isRel = (cmd == 'c') ;

            vector<float> argList ;
            if ( !parse_coordinate_list(args, argList) ) return false ;

            for(int i=0 ; i<argList.size() ; i+=6)
            {
                arg1 = argList[i] ;
                arg2 = argList[i+1] ;
                arg3 = argList[i+2] ;
                arg4 = argList[i+3] ;
                arg5 = argList[i+4] ;
                arg6 = argList[i+5] ;

                if ( isRel )
                {
                    elements.push_back(PathData::Element(CurveToCmd,
                        cx + arg1, cy + arg2, rx = cx + arg3, ry = cy + arg4, arg5 + cx, arg6 + cy)) ;
                        cx += arg5 ;
                        cy += arg6 ;

                }
                else
                    elements.push_back(PathData::Element(CurveToCmd,
                        arg1, arg2, rx = arg3, ry = arg4, cx = arg5, cy = arg6)) ;
            }
            previousCmd = CurveToCmd ;
        }
        else if ( cmd == 's' || cmd == 'S' )
        {
            bool isRel = (cmd == 's') ;

            vector<float> argList ;
            if ( !parse_coordinate_list(args, argList) ) return false ;

            for(int i=0 ; i<argList.size() ; i+=4)
            {
                arg3 = argList[i] ;
                arg4 = argList[i+1] ;
                arg5 = argList[i+2] ;
                arg6 = argList[i+3] ;

                if ( elements.empty() ) return false ;

                Element &preCmd = elements.back() ;

                if ( previousCmd == CurveToCmd )
                {
                    arg1 = 2 * cx - rx ;
                    arg2 = 2 * cy - ry ;
                }
                else
                {
                    arg1 = cx ; arg2 = cy ;
                }
                if ( isRel ) { arg1 -= cx ; arg2 -= cy ; }

                if ( isRel )
                {
                    elements.push_back(PathData::Element(CurveToCmd,
                        cx + arg1, cy + arg2, rx = cx + arg3, ry = cy + arg4, arg5 + cx, arg6 + cy)) ;
                        cx += arg5 ;
                        cy += arg6 ;
                }
                else
                    elements.push_back(PathData::Element(CurveToCmd,
                        arg1, arg2, rx = arg3, ry = arg4, cx = arg5, cy = arg6)) ;
            }
            previousCmd = CurveToCmd ;
        }
        else if ( cmd == 'Q' || cmd == 'q' )
        {
            bool isRel = (cmd == 'q') ;

            vector<float> argList ;
            if ( !parse_coordinate_list(args, argList) ) return false ;

            for(int i=0 ; i<argList.size() ; i+=4)
            {
                arg1 = argList[i] ;
                arg2 = argList[i+1] ;
                arg3 = argList[i+2] ;
                arg4 = argList[i+3] ;

                if ( isRel )
                {
                    arg1 += cx ; arg3 += cx ;
                    arg2 += cy ; arg4 += cy ;
                }

                rx = arg1 ; ry = arg2 ;

                /* raise quadratic bezier to cubic */
                double x1 = (cx + 2 * arg1) * (1.0 / 3.0);
                double y1 = (cy + 2 * arg2) * (1.0 / 3.0);
                double x3 = arg3 ;
                double y3 = arg4 ;
                double x2 = (x3 + 2 * arg1) * (1.0 / 3.0);
                double y2 = (y3 + 2 * arg2) * (1.0 / 3.0);

                elements.push_back(PathData::Element(CurveToCmd, x1, y1, x2, y2, x3, y3)) ;

                cx = arg3 ;
                cy = arg4 ;
            }
            previousCmd = QuadCurveToCmd ;
        }
        else if ( cmd == 'T' || cmd == 't' )
        {
            bool isRel = (cmd == 't') ;

            vector<float> argList ;
            if ( !parse_coordinate_list(args, argList) ) return false ;

            for(int i=0 ; i<argList.size() ; i+=2)
            {
                arg3 = argList[i] ;
                arg4 = argList[i+1] ;

                if ( elements.empty() ) return false ;

                Element &preCmd = elements.back() ;

                if ( previousCmd == QuadCurveToCmd )
                {
                    arg1 = 2 * cx - rx ;
                    arg2 = 2 * cy - ry ;
                }
                else
                {
                    arg1 = cx ; arg2 = cy ;
                }

                if ( isRel )
                {
                    arg3 += cx ; arg4 += cy ;
                }

                /* raise quadratic bezier to cubic */
                double x1 = (cx + 2 * arg1) * (1.0 / 3.0);
                double y1 = (cy + 2 * arg2) * (1.0 / 3.0);
                double x3 = arg3 ;
                double y3 = arg4 ;
                double x2 = (x3 + 2 * arg1) * (1.0 / 3.0);
                double y2 = (y3 + 2 * arg2) * (1.0 / 3.0);

                elements.push_back(PathData::Element(CurveToCmd, x1, y1, x2, y2, x3, y3)) ;

                cx = arg3 ;
                cy = arg4 ;
            }
            previousCmd = QuadCurveToCmd ;
        }
        else if ( cmd == 'A' || cmd == 'a' )
        {
            bool isRel = (cmd == 'a') ;

            vector<float> argList ;
            if ( !parse_coordinate_list(args, argList) ) return false ;

            for(int k=0 ; k<argList.size() ; k+=7)
            {
                arg1 = argList[k] ;
                arg2 = argList[k+1] ;
                arg3 = argList[k+2] ;
                arg4 = argList[k+3] ;
                arg5 = argList[k+4] ;
                arg6 = argList[k+5] ;
                arg7 = argList[k+6] ;

                if ( isRel )
                {
                    arg1 += cx ; arg2 += cy ;
                    arg6 += cx ; arg7 += cy ;
                }

                /**
                * rsvg_path_arc: Add an RSVG arc to the path context.
                * @ctx: Path context.
                * @rx: Radius in x direction (before rotation).
                * @ry: Radius in y direction (before rotation).
                * @x_axis_rotation: Rotation angle for axes.
                * @large_arc_flag: 0 for arc length <= 180, 1 for arc >= 180.
                * @sweep: 0 for "negative angle", 1 for "positive angle".
                * @x: New x coordinate.
                * @y: New y coordinate.
                *
                **/

                double rx = arg1 ;
                double ry = arg2 ;
                double x_axis_rotation = arg3 ;
                int large_arc_flag = (arg4 == 0.0 ) ? 0 : 1 ;
                int sweep_flag = (arg5 == 0.0 ) ? 0 : 1 ;
                double x = arg6 ;
                double y = arg7 ;

                double sin_th, cos_th;
                double a00, a01, a10, a11;
                double x0, y0, x1, y1, xc, yc;
                double d, sfactor, sfactor_sq;
                double th0, th1, th_arc;
                int i, n_segs;

                /* Check that neither radius is zero, since its isn't either
                    geometrically or mathematically meaningful and will
                    cause divide by zero and subsequent NaNs.  We should
                    really do some ranged check ie -0.001 < x < 000.1 rather
                    can just a straight check again zero.
                */
                if ((rx == 0.0) || (ry == 0.0))
                    continue ;

                sin_th = sin (x_axis_rotation * (M_PI / 180.0));
                cos_th = cos (x_axis_rotation * (M_PI / 180.0));
                a00 = cos_th / rx;
                a01 = sin_th / rx;
                a10 = -sin_th / ry;
                a11 = cos_th / ry;
                x0 = a00 * cx + a01 * cy;
                y0 = a10 * cx + a11 * cy;
                x1 = a00 * x + a01 * y;
                y1 = a10 * x + a11 * y;
                /* (x0, y0) is current point in transformed coordinate space.
                   (x1, y1) is new point in transformed coordinate space.

                   The arc fits a unit-radius circle in this space.
                */
                d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
                sfactor_sq = 1.0 / d - 0.25;
                if (sfactor_sq < 0) sfactor_sq = 0;
                sfactor = sqrt (sfactor_sq);
                if (sweep_flag == large_arc_flag)  sfactor = -sfactor;
                xc = 0.5 * (x0 + x1) - sfactor * (y1 - y0);
                yc = 0.5 * (y0 + y1) + sfactor * (x1 - x0);
                /* (xc, yc) is center of the circle. */

                th0 = atan2 (y0 - yc, x0 - xc);
                th1 = atan2 (y1 - yc, x1 - xc);

                th_arc = th1 - th0;
                if (th_arc < 0 && sweep_flag) th_arc += 2 * M_PI;
                else if (th_arc > 0 && !sweep_flag) th_arc -= 2 * M_PI;

                n_segs = ceil (fabs (th_arc / (M_PI * 0.5 + 0.001)));

                for (i = 0; i < n_segs; i++)
                {
                    double ccc[6] ;
                    svg_path_arc_segment (ccc, xc, yc,
                               th0 + i * th_arc / n_segs,
                               th0 + (i + 1) * th_arc / n_segs, rx, ry, x_axis_rotation);

                    elements.push_back(PathData::Element(CurveToCmd,
                        ccc[0], ccc[1], ccc[2], ccc[3], ccc[4], ccc[5])) ;
                }

                cx = x ;
                cy = y ;
            }
            previousCmd = EllipticArcToCmd ;
        }

        ++it ;
    }



    return true ;
}


bool PointList::fromString(const std::string &str)
{
    if ( !parse_coordinate_list(str, points) ) return false ;
    if ( points.size() %2 ) return false ;
    return true ;
}

/////////////////////////////////////////////////////////////////////////

unsigned int Style::parseOpacity(const std::string &str)
{
   char *end_ptr;
   double opacity;

   opacity = strtod (str.c_str(), &end_ptr);

   if (end_ptr && end_ptr[0] == '%')
        opacity *= 0.01;

   return (unsigned int) floor (opacity * 255. + 0.5);
}


Style::Style()
{
    fillPaintType = SolidColorPaint ;
    fillPaint.clr = 0 ;
    fillOpacity = 0xff ;

    fillRule = NonZeroFillRule ;
    strokePaintType = NoPaint ;

    strokeWidth = Length(Length::NumberLengthType, 1.0) ;
    lineCap = ButtLineCap ;
    lineJoin = MiterLineJoin ;
    miterLimit = 4 ;
    dashOffset = Length(Length::NumberLengthType, 0.0) ;
    strokeOpacity = 0xff ;
    opacity = 0xff ;
    solidStroke = true ;
    textRenderingQuality = AutoTextQuality ;
    shapeRenderingQuality = AutoShapeQuality ;
    overflow = false ;
 //   fontFamily = "Arial" ;
    fontSize = Length(Length::PXLengthType, 10.0) ;
}

void Style::resetNonInheritable()
{

}


void Style::parsePaint(const std::string &val, bool fill)
{
    if ( val == "none" )
    {
        if ( fill )
            fillPaintType = Style::NoPaint ;
        else
            strokePaintType = Style::NoPaint ;
    }
    else if ( val == "currentColor" )
    {
        if ( fill )
            fillPaintType = Style::CurrentColorPaint ;
        else
        strokePaintType = Style::CurrentColorPaint ;
    }
    else if ( val == "inherit") ;
    else if ( boost::algorithm::starts_with(val, "url") )
    {
        string id = parseUri(val) ;

        if ( !id.empty() )
        {
            if ( fill )
            {
                fillPaint.paintServerId = strdup(id.c_str()) ;
                fillPaintType = Style::PaintServerPaint ;
            }
            else
            {
                strokePaint.paintServerId = strdup(id.c_str()) ;
                strokePaintType = Style::PaintServerPaint ;
            }
        }
    }
    else
    {
        unsigned int clr ;

        if ( parse_css_color(val, clr) )
        {
            if ( fill )
            {
                fillPaint.clr = clr ;
                fillPaintType = Style::SolidColorPaint ;
            }
            else
            {
                strokePaint.clr = clr ;
                strokePaintType = Style::SolidColorPaint ;
            }

        }
    }

}


bool Style::hasFlag(Flag f) const
{
    for( int i=0 ; i<flags.size() ; i++ )
        if ( flags[i] == f ) return true ;

    return false ;
}

}
