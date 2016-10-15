#include <mapnik/map.hpp>
#include <mapnik/layer.hpp>
#include <mapnik/rule.hpp>
#include <mapnik/feature_type_style.hpp>
#include <mapnik/graphics.hpp>
#include <mapnik/datasource_cache.hpp>
#include <mapnik/font_engine_freetype.hpp>
#include <mapnik/agg_renderer.hpp>

#include <mapnik/color_factory.hpp>
#include <mapnik/image_util.hpp>
#include <mapnik/config_error.hpp>
#include <mapnik/projection.hpp>
#include <mapnik/proj_transform.hpp>
#include <mapnik/box2d.hpp>
#include <mapnik/filter_factory.hpp>


//#if defined(HAVE_CAIRO)
// cairo
#include <mapnik/cairo_renderer.hpp>
#include <cairomm/surface.h>
//#endif

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>

#include <iostream>

#include "MapFile.h"

using namespace mapnik ;
using namespace std;

bool renderMapImage(const MapFile &file, const std::string &outFile, float dpi, float scale_denominator)
{
    double bbox_[4] ;
    file.getBoundingBox(bbox_) ;

    double minlat = bbox_[0] ;
    double minlon = bbox_[1] ;
    double maxlat = bbox_[2] ;
    double maxlon = bbox_[3] ;

    try {

        datasource_cache::instance().register_datasources("/home/malasiot/local/lib/mapnik/input/");
        freetype_engine::register_fonts("/usr/share/fonts/", true);

        projection merc("+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +no_defs") ;
        projection longlat("+init=epsg:4326") ;
    //    mapnik::projection greek_grid("+init=epsg:2100") ;

        box2d<double> bbox(minlat, minlon, maxlat, maxlon) ;
        proj_transform trans(longlat, merc)	;

        trans.forward(bbox) ;

        double mapw = bbox[2] - bbox[0] ;
        double maph = bbox[3] - bbox[1] ;

        int imgx = int(mapw * dpi  / scale_denominator / 0.0254) ;
        int imgy = int(maph * dpi / scale_denominator / 0.0254 ) ;

        Map map(imgx, imgy);

        map.set_background(parse_color("cadetblue"));

        map.set_srs(merc.expanded()) ;

        map.zoom_to_box(bbox);
        map.set_buffer_size(128);

        // Provinces (polyline)
        feature_type_style style;

        stroke lines_stk (color(0,0,0),10.0);


        rule lines_rule;
        lines_rule.append(line_symbolizer(lines_stk));
        style.add_rule(lines_rule);

        map.insert_style("roads",style);

        // Layers

        string extStr = str(boost::format("%f %f %f %f") % bbox_[0] % bbox_[1] % bbox_[2] % bbox_[3]) ;
        {
            parameters p;
            p["type"] = "sqlite";
            p["file"] = file.getFileName() ;
            p["table"] = "highways" ;
            p["wkb_format"] = "spatialite" ;
            p["geometry_field"]="geom";
            p["extent"] = extStr;
            p["key_field"] = "gid" ;

            layer lyr("roads");
            lyr.set_datasource(datasource_cache::instance().create(p));
            lyr.add_style("roads");
            map.addLayer(lyr);

            ;
        }



/*


#if defined(HAVE_CAIRO)
        Cairo::RefPtr<Cairo::Surface> surf = Cairo::PdfSurface::create("cali.pdf", 1000, 1000);

        mapnik::cairo_renderer<Cairo::Surface> ren(map, surf);
#else
*/
        mapnik::image_32 buf(map.width(), map.height());
        mapnik::agg_renderer<mapnik::image_32> ren(map, buf);
        ren.apply();

        mapnik::save_to_file<image_data_32>(buf.data(), outFile);

    }

    catch ( const mapnik::config_error & ex )
    {
             std::cerr << "### Configuration error: " << ex.what();
             return false ;
    }
    catch ( const std::exception & ex )
    {
             std::cerr << "### std::exception: " << ex.what();
             return false;
    }
    catch ( ... )
     {
             std::cerr << "### Unknown exception." << std::endl;
             return false;
     }
     return true;


}
