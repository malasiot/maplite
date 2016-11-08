#include "osm_sqlite_accessor.hpp"

#include <iostream>
#include <sstream>
#include <cstdint>

using namespace std ;

enum
{
    O32_LITTLE_ENDIAN = 0x03020100ul,
    O32_BIG_ENDIAN = 0x00010203ul,
    O32_PDP_ENDIAN = 0x01000302ul
};

static const union { unsigned char bytes[4]; uint32_t value; } o32_host_order =
{ { 0, 1, 2, 3 } };

static const bool platform_is_little_endian = ( o32_host_order.value == 0x03020100ul ) ;

static void byte_swap_32(uint32_t &data)
{
    union u {uint32_t v; uint8_t c[4];};
    u un, vn;
    un.v = data ;
    vn.c[0]=un.c[3];
    vn.c[1]=un.c[2];
    vn.c[2]=un.c[1];
    vn.c[3]=un.c[0];
    data = vn.v ;
}

static void byte_swap_64(uint64_t &data)
{
    union u {uint64_t v; uint8_t c[8];};
    u un, vn;
    un.v = data ;
    vn.c[0]=un.c[7];
    vn.c[1]=un.c[6];
    vn.c[2]=un.c[5];
    vn.c[3]=un.c[4];
    vn.c[4]=un.c[3];
    vn.c[5]=un.c[2];
    vn.c[6]=un.c[1];
    vn.c[7]=un.c[0];
    data = vn.v ;
}

static void byte_swap_16(uint16_t &nValue)
{
    nValue = ((( nValue>> 8)) | (nValue << 8));
}

static void blob_write_64(ostringstream &strm, uint64_t data) {
    if ( platform_is_little_endian )  byte_swap_64(data) ;
    strm.write((char *)&data, 8) ;
}

static void blob_write_32(ostringstream &strm, uint32_t data) {
    if ( platform_is_little_endian )  byte_swap_32(data) ;
    strm.write((char *)&data, 4) ;
}

static void blob_write_16(ostringstream &strm, uint16_t data) {
    if ( platform_is_little_endian )  byte_swap_16(data) ;
    strm.write((char *)&data, 2) ;
}

static void blob_write_string(ostringstream &strm, const string &str) {
    blob_write_16(strm, str.length()) ;
    strm.write((char *)&str[0], str.length()) ;
}

static uint64_t blob_read_64(istringstream &strm) {
    uint64_t data ;
    strm.read((char *)&data, 8) ;
    if ( platform_is_little_endian )  byte_swap_64(data) ;
    return data ;
}

static uint32_t blob_read_32(istringstream &strm) {
    uint32_t data ;
    strm.read((char *)&data, 4) ;
    if ( platform_is_little_endian )  byte_swap_32(data) ;
    return data ;
}

static uint16_t blob_read_16(istringstream &strm) {
    uint16_t data ;
    strm.read((char *)&data, 2) ;
    if ( platform_is_little_endian )  byte_swap_16(data) ;
    return data ;
}

static string blob_read_string(istringstream &strm) {
    string str ;
    uint16_t len = blob_read_16(strm) ;
    str.resize(len) ;
    strm.read((char *)&str[0], len) ;
    return str ;
}

static std::string encode_ids(const std::vector<osm_id_t> &ids) {
    ostringstream strm(ios::binary) ;

    blob_write_32(strm, ids.size()) ;

    for( osm_id_t id: ids )
        blob_write_64(strm, id) ;

    return strm.str() ;
}

static std::string encode_strings(const std::vector<string> &l) {
    ostringstream strm(ios::binary) ;

    blob_write_32(strm, l.size()) ;

    for( const string &s: l )
        blob_write_string(strm, s) ;

    return strm.str() ;
}

static std::string encode_tags(const Dictionary &tags) {
    ostringstream strm(ios::binary) ;

    blob_write_16(strm, tags.count()) ;

    DictionaryIterator it(tags)  ;

    while ( it ) {
        blob_write_string(strm, it.key()) ;
        blob_write_string(strm, it.value()) ;
        ++it ;
    }

    return strm.str() ;
}

static void decode_ids(const std::string &blob, std::vector<osm_id_t> &ids) {
    istringstream strm(blob, ios::binary) ;

    uint32_t len = blob_read_32(strm) ;

    for( uint32_t i=0 ; i<len ; i++ ) {
        uint64_t id = blob_read_64(strm) ;
        ids.push_back(id) ;
    }
}

static void decode_strings(const std::string &blob, std::vector<string> &l) {
    istringstream strm(blob, ios::binary) ;

    uint32_t len = blob_read_32(strm) ;

    for( uint32_t i=0 ; i<len ; i++ ) {
        string s = blob_read_string(strm) ;
        l.push_back(s) ;
    }
}

static void decode_tags(const string &blob, Dictionary &tags) {
    istringstream strm(blob, ios::binary) ;

    uint16_t len = blob_read_16(strm) ;

    for( uint32_t i=0 ; i<len ; i++ ) {
        string key= blob_read_string(strm) ;
        string val= blob_read_string(strm) ;
        tags.add(key, val) ;
    }
}

namespace OSM {

void SQLiteAccessor::createTables() {

    try {
        con_.exec("DROP TABLE IF EXISTS nodes;DROP TABLE IF EXISTS ways;DROP TABLE IF EXISTS relations;DROP TABLE IF EXISTS membership") ;
        con_.exec("CREATE TABLE nodes (osm_id INTEGER PRIMARY KEY, lat REAL, lon REAL, tags BLOB)") ;
        con_.exec("CREATE TABLE ways (osm_id INTEGER PRIMARY KEY, node_list BLOB, tags BLOB)") ;
        con_.exec("CREATE TABLE relations (osm_id INTEGER PRIMARY KEY, node_list BLOB, node_roles BLOB, way_list BLOB, way_roles BLOB, relation_list BLOB, relation_roles BLOB, tags BLOB)") ;
        con_.exec("CREATE TABLE memberships (osm_id INTEGER PRIMARY KEY, parent INTEGER)") ;
    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
    }
}

bool SQLiteAccessor::readWay(osm_id_t id, Way &way)
{
    SQLite::Query q(con_, "SELECT * FROM ways WHERE osm_id=? LIMIT 1");
    q.bind(1, (long long)id) ;
    SQLite::QueryResult res = q.exec() ;

    if ( res ) {

        way.id_ = res.get<long long>(0) ;
        int bs ;
        const char *blob = res.getBlob(1, bs) ;
        decode_ids(string(blob, blob+bs), way.nodes_) ;
        blob = res.getBlob(2, bs) ;
        decode_tags(string(blob, blob+bs), way.tags_) ;
    }

}

void SQLiteAccessor::readWays(const std::vector<osm_id_t> ids, std::vector<Way> &ways)
{
    /*
    string sql =

    SQLite::Query q(con_, "SELECT * FROM ways WHERE osm_id IN  LIMIT 1");
    q.bind(1, (long long)id) ;
    SQLite::QueryResult res = q.exec() ;

    if ( res ) {

        way.id_ = res.get<long long>(0) ;
        int bs ;
        const char *blob = res.getBlob(1, bs) ;
        decode_ids(string(blob, blob+bs), way.nodes_) ;
        blob = res.getBlob(2, bs) ;
        decode_tags(string(blob, blob+bs), way.tags_) ;
    }
*/
}

void SQLiteAccessor::readWayNodes(osm_id_t wid, std::vector<Node> &nodes)
{
    SQLite::Query q(con_, "SELECT * FROM nodes WHERE osm_id IN ( SELECT osm_id FROM memberships WHERE parent=? );");
    q.bind(1, (long long)wid) ;

    SQLite::QueryResult res = q.exec() ;

    while ( res ) {

        Node node ;
        node.id_ = res.get<long long>(0) ;
        node.lat_ = res.get<double>(1) ;
        node.lon_ = res.get<double>(2) ;

        int bs ;
        const char *blob = res.getBlob(3, bs) ;
        decode_tags(string(blob, blob+bs), node.tags_) ;

        nodes.push_back(node) ;

        res.next() ;
    }

}

void SQLiteAccessor::readParentRelations(osm_id_t id, std::vector<Relation> &parents)
{

}

void SQLiteAccessor::forAllNodes(std::function<void (const Node &)> cf)
{
    SQLite::Query q(con_, "SELECT * FROM nodes;");
    SQLite::QueryResult res = q.exec() ;

    while ( res ) {

        Node node ;
        node.id_ = res.get<long long>(0) ;
        node.lat_ = res.get<double>(1) ;
        node.lon_ = res.get<double>(2) ;

        int bs ;
        const char *blob = res.getBlob(3, bs) ;
        decode_tags(string(blob, blob+bs), node.tags_) ;

        cf(node) ;

        res.next() ;
    }

}

void SQLiteAccessor::forAllWays(std::function<void (const Way &)> cf)
{
    SQLite::Query q(con_, "SELECT * FROM ways;");
    SQLite::QueryResult res = q.exec() ;

    while ( res ) {
        Way way ;
        way.id_ = res.get<long long>(0) ;

        int bs ;
        const char *blob = res.getBlob(1, bs) ;
        decode_ids(string(blob, blob+bs), way.nodes_) ;
        blob = res.getBlob(2, bs) ;
        decode_tags(string(blob, blob+bs), way.tags_) ;

        cf(way) ;

        res.next() ;
    }

}

void SQLiteAccessor::forAllRelations(std::function<void (const Relation &)> cf)
{
    SQLite::Query q(con_, "SELECT * FROM relations;");
    SQLite::QueryResult res = q.exec() ;

    while ( res ) {
        Relation rel ;
        rel.id_ = res.get<long long>(0) ;

        int bs ;
        const char *blob = res.getBlob(1, bs) ;
        decode_ids(string(blob, blob+bs), rel.nodes_) ;

        blob = res.getBlob(2, bs) ;
        decode_strings(string(blob, blob+bs), rel.nodes_role_) ;

        blob = res.getBlob(3, bs) ;
        decode_ids(string(blob, blob+bs), rel.ways_) ;

        blob = res.getBlob(4, bs) ;
        decode_strings(string(blob, blob+bs), rel.ways_role_) ;

        blob = res.getBlob(5, bs) ;
        decode_ids(string(blob, blob+bs), rel.children_) ;

        blob = res.getBlob(6, bs) ;
        decode_strings(string(blob, blob+bs), rel.children_role_) ;

        blob = res.getBlob(7, bs) ;
        decode_tags(string(blob, blob+bs), rel.tags_) ;

        cf(rel) ;

        res.next() ;
    }


}


void SQLiteAccessor::writeNode(const Node &n)
{
    try {

        SQLite::Command cmd(con_, "INSERT INTO nodes (osm_id, lat, lon, tags) VALUES (?, ?, ?, ?)");

        cmd.bind(1, (long long)n.id_) ;
        cmd.bind(2, n.lat_) ;
        cmd.bind(3, n.lon_) ;
        string tags = encode_tags(n.tags_) ;
        cmd.bind(4, tags.c_str(), tags.size()) ;
        cmd.exec() ;
    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
    }
}



void SQLiteAccessor::writeWay(const Way &way)
{
    try {

        SQLite::Command cmd(con_, "INSERT INTO ways (osm_id, node_list, tags) VALUES (?, ?, ?)") ;

        cmd.bind(1, (long long)way.id_) ;
        string nblob = encode_ids(way.nodes_) ;
        cmd.bind(2, nblob.c_str(), nblob.size()) ;
        string tags = encode_tags(way.tags_) ;
        cmd.bind(3, tags.c_str(), tags.size()) ;
        cmd.exec() ;

        SQLite::Command mcmd(con_, "INSERT INTO memberships (osm_id, parent) VALUES (?, ?)") ;

        for( osm_id_t id: way.nodes_ ) {
            mcmd.bind(1, (long long)id) ;
            mcmd.bind(2, (long long)way.id_) ;
            mcmd.exec() ;
            mcmd.clear() ;
        }

    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
    }
}

void SQLiteAccessor::writeRelation(const Relation &r)
{
    try {
        SQLite::Command cmd(con_, "INSERT INTO relations (osm_id, node_list, node_roles, way_list, way_roles, relation_list, relation_roles, tags) VALUES (?, ?, ?, ?, ?, ?, ?, ?)") ;

        cmd.bind(1, (long long)r.id_) ;
        string nblob = encode_ids(r.nodes_) ;
        cmd.bind(2, nblob.c_str(), nblob.size()) ;
        string nrblob = encode_strings(r.nodes_role_) ;
        cmd.bind(3, nrblob.c_str(), nrblob.size()) ;
        string wblob = encode_ids(r.ways_) ;
        cmd.bind(4, wblob.c_str(), wblob.size()) ;
        string wrblob = encode_strings(r.ways_role_) ;
        cmd.bind(5, wrblob.c_str(), wrblob.size()) ;
        string rblob = encode_ids(r.children_) ;
        cmd.bind(6, rblob.c_str(), rblob.size()) ;
        string rrblob = encode_strings(r.children_role_) ;
        cmd.bind(7, rrblob.c_str(), rrblob.size()) ;
        string tags = encode_tags(r.tags_) ;
        cmd.bind(8, tags.c_str(), tags.size()) ;
        cmd.exec() ;
    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
    }

}

void SQLiteAccessor::beginWrite()
{
    con_.exec("BEGIN TRANSACTION") ;
}

void SQLiteAccessor::endWrite()
{
    con_.exec("COMMIT TRANSACTION") ;
}

}
