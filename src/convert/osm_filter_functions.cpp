#include <string>

#include <stdarg.h>

#include "osm_rule_parser.hpp"
#include "parse_context.hpp"

#include <iomanip>
#include <boost/format.hpp>

using namespace std ;

namespace OSM {

namespace Filter {

/*
    Copyright 2001, 2002 Georges Menie (www.menie.org)
    stdarg version contributed by Christian Ettinger

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#define PAD_RIGHT 1
#define PAD_ZERO 2

static int prints(string &out, const string &in, int width, int pad)
{
    register int pc = 0, padchar = ' ';
    const char *istr = in.c_str() ;

    if (width > 0) {
        register int len = 0;
        register const char *ptr;
        for (ptr = istr; *ptr; ++ptr) ++len;
        if (len >= width) width = 0;
        else width -= len;
        if (pad & PAD_ZERO) padchar = '0';
    }
    if (!(pad & PAD_RIGHT)) {
        for ( ; width > 0; --width) {
            out += padchar ;
            ++pc;
        }
    }
    for ( ; *istr ; ++istr) {
        out += *istr ;
        ++pc;
    }
    for ( ; width > 0; --width) {
        out += padchar ;
        ++pc;
    }

    return pc;
}

/* the following should be enough for 32 bit int */
#define PRINT_BUF_LEN 12

static int printi(string &out, int i, int b, int sg, int width, int pad, int letbase)
{
    char print_buf[PRINT_BUF_LEN];
    register char *s;
    register int t, neg = 0, pc = 0;
    register unsigned int u = i;

    if (i == 0) {
        print_buf[0] = '0';
        print_buf[1] = '\0';
        return prints (out, print_buf, width, pad);
    }

    if (sg && b == 10 && i < 0) {
        neg = 1;
        u = -i;
    }

    s = print_buf + PRINT_BUF_LEN-1;
    *s = '\0';

    while (u) {
        t = u % b;
        if( t >= 10 )
            t += letbase - '0' - 10;
        *--s = t + '0';
        u /= b;
    }

    if (neg) {
        if( width && (pad & PAD_ZERO) ) {
            out += '-' ;
            ++pc;
            --width;
        }
        else {
            *--s = '-';
        }
    }

    return pc + prints (out, s, width, pad);
}

static int printd(string &out, double v, int width, int prec, int pad)
{
    ostringstream strm ;

    strm << std::setiosflags (std::ios::showbase | std::ios::uppercase) << std::setfill('0') << std::setprecision(prec) << setw(width) << v ;
    out += strm.str() ;
    return out.length() ;
}

string format(const char *format, vector<Literal> &args) {

    register int width, pad;
    register int pc = 0;
    register int ac = 0 ;
    char scr[2];
    string out ;

    for (; *format != 0; ++format) {
        if ( ac == args.size() ) break ;
        if (*format == '%') {
            ++format;
            width = pad = 0;
            if (*format == '\0') break;
            if (*format == '%') goto out;
            if (*format == '-') {
                ++format;
                pad = PAD_RIGHT;
            }
            while (*format == '0') {
                ++format;
                pad |= PAD_ZERO;
            }
            for ( ; *format >= '0' && *format <= '9'; ++format) {
                width *= 10;
                width += *format - '0';
            }
            if( *format == 's' ) {
                string s = args[ac++].toString() ;
                pc += prints (out, s, width, pad);
                continue;
            }
            if( *format == 'd' ) {
                pc += printi(out, args[ac++].toNumber(), 10, 1, width, pad, 'a');
                continue;
            }
            if( *format == 'x' ) {
                pc += printi(out, args[ac++].toNumber(), 16, 1, width, pad, 'a');
                continue;
            }
            if( *format == 'X' ) {
                pc += printi(out, args[ac++].toNumber(), 16, 1, width, pad, 'A');
                continue;
            }
            if( *format == 'u' ) {
                pc += printi(out, args[ac++].toNumber(), 10, 0, width, pad, 'a');
                continue;
            }
            if( *format == 'c' ) {
                /* char are converted to int then pushed on the stack */
                string s = args[ac++].toString() ;
                scr[0] = s[0] ;
                scr[1] = '\0' ;
                pc += prints (out, scr, width, pad);
                continue;
            }
            if ( *format == 'f' ) {
                pc += printd(out, args[ac++].toNumber(), width, 2, pad);
                continue;
            }
        }
        else {
out:
            out += *format ;
            ++pc;
        }
    }

    return out ;

    // i = va_arg(argp, int);
}


Literal Function::eval(Context &ctx)
{
    if ( name_ == "format" )
    {
        if ( args_.empty() ) return Literal() ;
        if ( args_.size() < 2 ) return Literal() ;
        string frmt_str = args_[0]->eval(ctx).toString() ;

        vector<Literal> vals ;
        for(uint i=1 ; i<args_.size() ; i++)
            vals.push_back(args_[i]->eval(ctx)) ;
        return Literal(format(frmt_str.c_str(), vals), false) ;
    }
    else if ( name_ == "type" ) {
        if ( ctx.type() == Context::Node ) return "node" ;
        else if ( ctx.type() == Context::Way ) return "way" ;
    }
    else if ( name_ == "is_poi" ) {
        return ( ctx.type() == Context::Node )  ;
    }
    else if ( name_ == "is_way" ) {
        return ctx.type() == Context::Way ;
    }
/*    else if ( name_ == "lname" ) {
        if ( ctx.has_tag("name") ) return Literal(ctx.value("name"), false) ;

        string lang = "en" ;
        if ( args_.size() > 0 ) {
            lang = args_[0]->eval(ctx).toString() ;
        }
        string tag = "name:" +  lang ;

        if ( ctx.has_tag(tag) )
            return Literal(ctx.value(tag), false) ;

        return Literal() ;
    }
    */
    else if ( name_ == "attach_tags_from_hiking_route") {

        if ( ctx.type() == Context::Way ) {
            Way &w = ctx.doc_->ways_[ctx.fid_] ;
            string network, ref, symbol ;

            for( uint ridx: w.relations_ )
            {
                const Relation &r = ctx.doc_->relations_[ridx] ;
                if ( r.tags_.get("type") != "route") continue ;
                if ( r.tags_.get("route") != "hiking" ) continue ;

                string rnet =  r.tags_.get("network") ;
                if ( rnet.empty() ) rnet = "lwn" ;
                string rref = r.tags_.get("ref") ;
                string rsymbol = r.tags_.get("osmc:symbol") ;

                if ( network == "iwn" ) continue ;
                else if ( network == "nwn" && ( rnet == "rwn" || rnet == "lwn") ) continue ;
                else if ( network == "rwn" && rnet == "lwn" ) continue ;
                else if ( network == "lwn" ) continue ;
                else {
                    network = rnet ;
                    ref = rref ;
                    symbol = "osmc:" + rsymbol ;
                }
            }

            ctx.tw_->actions_.emplace_back("hknetwork", network, 0, 255, true) ;
            if ( !ref.empty() ) ctx.tw_->actions_.emplace_back("ref", ref, 0, 255, true) ;
            if ( !symbol.empty() ) ctx.tw_->actions_.emplace_back("symbol", symbol, 0, 255, true) ;
        }


        return Literal() ;

    }
    else {
        vector<Literal> vals ;
        for(uint i=0 ; i<args_.size() ; i++)
            vals.push_back(args_[i]->eval(ctx)) ;
        return lua_->call(name_, vals) ;
    }

}

}
}
