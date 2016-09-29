#include "map_config.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <iomanip>
#include <fstream>

#include "rapidjson/istreamwrapper.h"
#include "rapidjson/document.h"
#include "rapidjson/error/error.h"
#include "rapidjson/error/en.h"

#include "geometry.hpp"
#include "tms.hpp"

using namespace std ;

bool MapConfig::parse(const string &fileName)
{
    ifstream strm(fileName.c_str()) ;

    rapidjson::IStreamWrapper is(strm);

    rapidjson::Document jsdoc ;

    rapidjson::ParseResult res = jsdoc.ParseStream(is) ;

    if ( !res )  {
        fprintf(stderr, "JSON parse error: %s (%u)",
              rapidjson::GetParseError_En(res.Code()), (unsigned int)res.Offset());

        return false ;
    }

    if ( jsdoc.HasMember("layers") && jsdoc["layers"].IsObject() ) {

        const rapidjson::Value &layers = jsdoc["layers"] ;

        for ( rapidjson::Value::ConstMemberIterator iter = layers.MemberBegin(); iter != layers.MemberEnd(); ++iter ) {

            Layer layer ;

            layer.name_ = iter->name.GetString() ;

            const rapidjson::Value &layer_info = iter->value ;

            if ( layer_info.HasMember("levels") ) {
                const rapidjson::Value &levels = layer_info["levels"] ;

                if ( !levels.IsNull() ) {
                    if ( levels.IsArray() ) {
                        for( uint i=0 ; i<levels.Size() ; i++ ) {
                            const rapidjson::Value &level = levels[i] ;

                            ZoomInterval zi ;
                            if ( level.HasMember("min_zoom") ) zi.min_zoom_ = level["min_zoom"].GetInt() ;
                            if ( level.HasMember("max_zoom" ) )zi.max_zoom_ = level["max_zoom"].GetInt() ;
                            if ( level.HasMember("simplify_threshold") ) zi.simplify_threshold_ = level["simplify_threshold"].GetDouble() ;

                            layer.zr_.intervals_.push_back(zi) ;
                        }
                    }
                    else {
                        ZoomInterval zi ;
                        if ( levels.HasMember("min_zoom") ) zi.min_zoom_ = levels["min_zoom"].GetInt() ;
                        if ( levels.HasMember("max_zoom") ) zi.max_zoom_ = levels["max_zoom"].GetInt() ;
                        if ( levels.HasMember("simplify_threshold") ) zi.simplify_threshold_ = levels["simplify_threshold"].GetDouble() ;

                        layer.zr_.intervals_.push_back(zi) ;
                    }
                }
            }

            layers_.push_back(layer) ;
        }
    }

    if ( jsdoc.HasMember("name") ) name_ = jsdoc["name"].GetString() ;
    if ( jsdoc.HasMember("description") ) description_ = jsdoc["description"].GetString() ;
    if ( jsdoc.HasMember("attribution") ) attribution_ = jsdoc["attribution"].GetString() ;
    if ( jsdoc.HasMember("min_zoom") ) minz_ = jsdoc["min_zoom"].GetInt() ;
    if ( jsdoc.HasMember("max_zoom") ) maxz_ = jsdoc["max_zoom"].GetInt() ;
    if ( jsdoc.HasMember("bbox") && jsdoc["bbox"].IsArray() && jsdoc["bbox"].Size() == 4 ) {
        bbox_.minx_ = jsdoc["bbox"][0].GetDouble() ;
        bbox_.miny_ = jsdoc["bbox"][1].GetDouble() ;
        bbox_.maxx_ = jsdoc["bbox"][2].GetDouble() ;
        bbox_.maxy_ = jsdoc["bbox"][3].GetDouble() ;
        has_bbox_ = true ;
        tms::latlonToMeters(bbox_.minx_, bbox_.miny_, bbox_.minx_, bbox_.miny_) ;
        tms::latlonToMeters(bbox_.maxx_, bbox_.maxy_, bbox_.maxx_, bbox_.maxy_) ;
    }
    else has_bbox_= false ;

    return true ;
}

