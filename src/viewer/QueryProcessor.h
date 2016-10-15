#ifndef __QUERY_PROCESSOR_H__
#define __QUERY_PROCESSOR_H__

#include "MapFile.h"

#include <string>

using std::string ;

// Helper class used to make spatial queries

class QueryProcessor
{
public:

    QueryProcessor() {}

    // get and cache info for a layer

    bool getLayerInfo(const MapFile &file,
                      const string &layerName,
                      LayerInfo &) ;

    // perform spatial query

    bool queryFeatures(SQLite::ConnectionPtr &, // database connection
                       FeatureCollection &col,  // resulting set of features
                       const LayerInfo &info,   // layer information
                       int target_srid,         // target srid
                       const string &condition, // SQL WHERE clause
                       const BBox &box);        // bbox for the query

private:

    string makeBBoxQuery(const string &tableName, const AttributeCollection &desc, const string &geomColumn, int target_srid, const string &condition, const BBox &box);

private:



};

















#endif
