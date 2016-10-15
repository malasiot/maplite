#include <string>
#include <sstream>

using namespace std ;

string makeQuery(const std::string &tableName, const std::string &geomColumn, const std::string &condition, int target_srid, double minx, double miny, double maxx, double maxy, int srid )
{
    stringstream sql ;

    sql.precision(16) ;

    sql << "SELECT ST_Transform(" << geomColumn << ',' << target_srid << ")  FROM " << tableName << " WHERE " << condition ;

    if ( !condition.empty() ) sql << " AND " ;

    sql << "ROWID IN ( SELECT ROWID FROM SpatialIndex WHERE f_table_name='" << tableName << "'AND search_frame = ST_Envelope(ST_GeomFromText('LINESTRING(" ;
    sql << minx << ' ' << miny << ',' << maxx << ' ' << maxy << ")'," << srid << ")))" ;

    return sql.str() ;
}
