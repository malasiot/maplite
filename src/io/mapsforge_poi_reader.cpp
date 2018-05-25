#include "mapsforge_poi_reader.hpp"

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

using namespace std ;

static void sqlite3_regexp(sqlite3_context* context, int argc, sqlite3_value** argv)
{
    if ( argc != 2 ) {
        sqlite3_result_int(context, 0);
        return;
    }

    string pattern(reinterpret_cast<const char*>(sqlite3_value_text(argv[0])));
    string subject(reinterpret_cast<const char*>(sqlite3_value_text(argv[1])));

    auto cache = static_cast<std::map<std::string, boost::regex> *>(sqlite3_user_data(context));

    boost::regex rx ;
    try {
        rx.assign(pattern) ;

        bool found = boost::regex_match(pattern, rx) ;

        sqlite3_result_int(context, int(found));
    }
    catch (boost::bad_expression &e ) {
        stringstream error ;
        error << "matches() invalid regexp: " << e.what() ;
        sqlite3_result_error(context, error.str().c_str(), -1);
        sqlite3_result_error_code(context, SQLITE_ERROR);
    }
}

static Dictionary serialize_key_val(const std::string &data) {
    Dictionary res ;
    vector<string> tokens ;
    boost::split(tokens, data, boost::is_any_of("\r"), boost::token_compress_on );

    for ( const string &token: tokens ) {
        vector<string> key_val ;
        boost::split(key_val, token, boost::is_any_of("="), boost::token_compress_on );

        if ( key_val.size() != 2 ) continue ;
        res.add(boost::trim_copy(key_val[0]), boost::trim_copy(key_val[1])) ;
    }

    return res ;
}

static void sqlite3_tag(sqlite3_context* ctx, int argc, sqlite3_value** argv) {

    if ( ( argc != 2 && argc != 3 ) ||
         sqlite3_value_type(argv[0]) != SQLITE_TEXT ||
         sqlite3_value_type(argv[1]) != SQLITE_TEXT
         ) {
        sqlite3_result_error(ctx, "tag(): invalid arguments", -1);
        sqlite3_result_error_code(ctx, SQLITE_MISMATCH);
        return ;
    }

    string data(reinterpret_cast<const char*>(sqlite3_value_text(argv[0])));
    string key(reinterpret_cast<const char*>(sqlite3_value_text(argv[1])));
    string default_val ;

    if ( argc == 3 )
        default_val = reinterpret_cast<const char*>(sqlite3_value_text(argv[2]));

    Dictionary dict = serialize_key_val(data) ;

    string val = dict.get(key, default_val) ;

    sqlite3_result_text(ctx, val.c_str(), -1, SQLITE_TRANSIENT) ;
}


void POIReader::open(const string &file_path) {
    db_.open(file_path, SQLITE_OPEN_READONLY) ;

    int rc;

    rc = sqlite3_create_function_v2(
                db_.handle(),
                "tag",                           // zFunctionName
                -1,                                   // nArg
                SQLITE_UTF8 | SQLITE_DETERMINISTIC,  // eTextRep
                nullptr,                             // pApp
                sqlite3_tag,                             // xFunc
                nullptr,                             // xStep
                nullptr,                             // xFinal
                nullptr                              // xDestroy
                );

    rc = sqlite3_create_function_v2(
                db_.handle(),
                "matches",                           // zFunctionName
                1,                                   // nArg
                SQLITE_UTF8 | SQLITE_DETERMINISTIC,  // eTextRep
                nullptr,                             // pApp
                sqlite3_regexp,                             // xFunc
                nullptr,                             // xStep
                nullptr,                             // xFinal
                nullptr                              // xDestroy
                );


    categories_.loadFromPOIFile(db_) ;
}

POIData::Collection POIReader::query(const POICategoryFilter &filter, const BBox &bbox, const std::string &pattern, uint max_results)
{
    stringstream strm ;
    strm << "SELECT poi_index.id, poi_index.minLat, poi_index.minLon, poi_data.data, poi_category_map.category, tag(poi_data.data, 'name') FROM poi_index ";
    strm << "JOIN poi_category_map ON poi_index.id = poi_category_map.id " ;
    strm << "JOIN poi_data ON poi_index.id = poi_data.id " ;
    strm << "WHERE poi_index.minLat <= ? AND poi_index.minLon <= ? AND poi_index.minLat >= ? AND poi_index.minLon >= ? ";
    if ( !pattern.empty() ) strm << "AND tag(poi_data.data, 'name') LIKE ? ";

    auto whitelist = filter.getCategories() ;

    if ( !whitelist.empty() ) {
        strm << " AND poi_category_map.category IN (" ;

        bool is_first = true ;
        for( const auto &c: whitelist ) {
            if ( !is_first ) strm << ',' ;
            strm << c->ID() ;
            is_first = false ;
        }
        strm << ")" ;
    }

    strm << " LIMIT ?" ;

//    cout << strm.str() << endl ;

    SQLite::Query q(db_, strm.str()) ;
    q.bind(bbox.maxy_) ;  q.bind(bbox.maxx_) ;  q.bind(bbox.miny_) ;  q.bind(bbox.minx_) ;
    if ( !pattern.empty() )
        q.bind(pattern) ;
    q.bind(max_results) ;

    for ( const SQLite::Row &r: q.exec()) {
        cout << r[3].as<string>() <<' ' << r[4].as<string>() << ' ' << r[5].as<string>() << endl ;
    }

}

void POICategoryFilter::addCategory(const POICategory::Ptr &category) {
    assert ( category ) ;

    white_list_.insert(category) ;
}

POICategory::Collection POICategoryFilter::getCategories() const {
    set<POICategory::Ptr> nodes ;

    for( const auto &w: white_list_ ) {
        auto desc = w->descendants() ;
        nodes.insert(w) ;
        for( const auto &d: desc ) {
            nodes.insert(d) ;
        }
    }

    return {nodes.begin(), nodes.end()} ;
}

