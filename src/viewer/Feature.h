#ifndef __FEATURE_H__
#define __FEATURE_H__

#include <map>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include <sqlite3.h>
#include <spatialite.h>
#include <spatialite/gaiageo.h>

#include "Value.h"

using std::string ;

class AttributeDescriptor {
public:
    enum DataType { Integer, Real, Text, DictionaryIndex } ;

    AttributeDescriptor(const string name_, DataType type_): name(name_), type(type_) {}

    std::string name ;
    DataType type ;
};

class AttributeCollection {
public:

    void add(const string &name, AttributeDescriptor::DataType type)
    {
        fields.push_back(AttributeDescriptor(name, type)) ;
        index[name] = fields.size() - 1 ;
    }

    string name(int idx) const {
        assert(idx >= 0 && idx<fields.size()) ;
        return fields[idx].name ;
    }

    int attributeIndex(const string &key) const {
        std::map<string, int>::const_iterator it = index.find(key) ;
        if ( it == index.end() ) return -1 ;
        else return it->second ;
    }

    std::vector<AttributeDescriptor> fields ;

private:
    std::map<std::string, int> index ;

};

typedef boost::shared_ptr<AttributeCollection> AttributeCollectionPtr ;

class Feature {
public:


    Value attribute(const std::string &key) const {
        int idx = desc->attributeIndex(key) ;
        if ( idx < 0 ) return Value() ;
        else return attributes[idx] ;
    }

    void add(const Value &v) {
        attributes.push_back(v) ;
    }

    void setGeometry(gaiaGeomCollPtr geom_) {
        geom = geom_ ;
    }

    Feature(const AttributeCollectionPtr &desc_): desc(desc_), geom(0) {
    }



    ~Feature() {
        if ( geom ) gaiaFreeGeomColl(geom) ;
    }

public:

    std::vector<Value> attributes ;
    gaiaGeomCollPtr geom ;

    boost::shared_ptr<AttributeCollection> desc ;

private:

    friend class Renderer ;
    // this is a special case for rasters
    Feature(): geom(0) { }

};
typedef boost::shared_ptr<Feature> FeaturePtr ;

class FeatureCollection {



public:

    std::vector<FeaturePtr> features ;

};


#endif
