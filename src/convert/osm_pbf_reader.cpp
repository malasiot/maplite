#include <osm_document.hpp>

#include <osm/fileformat.pb.h>
#include <osm/osmformat.pb.h>

#include <fstream>
#ifndef _WIN32
#include <arpa/inet.h>
#endif
#include <zlib.h>

using namespace std ;

#define MAX_BLOCK_HEADER_SIZE 64*1024
#define MAX_BLOB_SIZE 32*1024*1024
#define NANO_DEGREE .000000001

namespace OSM {

using PBF::BlockHeader ;
using PBF::Blob ;
using PBF::PrimitiveGroup ;
using PBF::StringTable ;
using PBF::HeaderBlock;
using PBF::PrimitiveBlock ;
using PBF::DenseNodes ;

static uint32_t get_length(ifstream &input)
{
    char buf[4];

    if ( !input.read(buf, sizeof(buf) ) ) return 0;

    return ntohl(*((size_t *)buf));
}

static bool read_header(BlockHeader &header_msg, ifstream &input)
{
    size_t length = get_length(input);

    if ( length == 0 ) return false ;

    std::unique_ptr<char []> buf(new char [length]) ;

    if ( !buf || !input.read(buf.get(), length) ) return false ;

    return header_msg.ParseFromArray(buf.get(), length) ;
}

static bool read_blob(Blob &blob_msg, ifstream &input, int32_t length)
{
    std::unique_ptr<char []> buf(new char [length]) ;

    if ( !buf || !input.read(buf.get(), length) ) return false ;

    return blob_msg.ParseFromArray(buf.get(), length) ;
}

static bool uncompress_blob(PBF::Blob &bmsg, char *ubuf)
{
    if ( bmsg.has_raw() ) {
        memcpy(ubuf, bmsg.raw().data(), bmsg.raw().size());
    }
    else if ( bmsg.has_zlib_data() ) {

        int ret;
        z_stream strm;
        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        strm.avail_in = bmsg.zlib_data().size();
        strm.next_in = (unsigned char *)bmsg.zlib_data().data();
        strm.avail_out = bmsg.raw_size();
        strm.next_out = (unsigned char *)ubuf;

        ret = inflateInit(&strm);

        if (ret != Z_OK) {
            delete [] ubuf ;
            return false ;
        }

        ret = inflate(&strm, Z_NO_FLUSH);

        (void)inflateEnd(&strm);

        if (ret != Z_STREAM_END) return false ;

    }
    else return false ;

    return true ;
}

static string make_id(int64_t id)
{
    stringstream s ;
    s << id ;
    return s.str() ;
}

static bool process_osm_data_nodes(vector<Node> &nodes, map<int64_t, uint64_t> &nodeMap, const PrimitiveGroup &group, const StringTable &string_table, double lat_offset, double lon_offset, double granularity)
{

    for ( unsigned node_id = 0; node_id < group.nodes_size() ; node_id++ )
    {
        const PBF::Node &node = group.nodes(node_id) ;

        nodes.push_back(Node()) ;
        Node &n = nodes.back() ;

        n.id_ = make_id(node.id()) ;

        n.lat_ = lat_offset + (node.lat() * granularity);
        n.lon_ = lon_offset + (node.lon() * granularity);

        nodeMap.insert(make_pair(node.id(), nodes.size()-1)) ;

        for ( unsigned key_id = 0; key_id < node.keys_size() ; key_id++ )
        {
            uint32_t key_idx = node.keys(key_id) ;
            uint32_t val_idx = node.vals(key_id) ;

            string key = string_table.s(key_idx) ;
            string val = string_table.s(val_idx) ;

            n.tags_.add(key, val) ;
        }
    }

    return true ;

}

static bool process_osm_data_dense_nodes(vector<Node> &nodes, map<int64_t, uint64_t> &nodeMap, const PrimitiveGroup &group, const StringTable &string_table, double lat_offset, double lon_offset, double granularity)
{
    if ( !group.has_dense() ) return true ;

    unsigned l = 0;
    int64_t deltaid = 0;
    long int deltalat = 0;
    long int deltalon = 0;

    const DenseNodes &dense = group.dense() ;

    for ( unsigned node_id = 0; node_id < dense.id_size() ; node_id++ )
    {
        nodes.push_back(Node()) ;
        Node &n = nodes.back() ;

        deltaid += dense.id(node_id) ;
        deltalat += dense.lat(node_id);
        deltalon += dense.lon(node_id) ;

        n.id_ = make_id(deltaid) ;

        nodeMap.insert(make_pair(deltaid, nodes.size()-1)) ;

        if ( l < dense.keys_vals_size() )
        {
            while (dense.keys_vals(l) != 0 && l < dense.keys_vals_size() )
            {
                uint32_t key_idx = dense.keys_vals(l) ;
                uint32_t val_idx = dense.keys_vals(l+1) ;

                string key = string_table.s(key_idx) ;
                string val = string_table.s(val_idx) ;

                n.tags_.add(key, val) ;

                l += 2;
            }
            l++ ;
        }

        n.lat_ = lat_offset + (deltalat * granularity);
        n.lon_ = lon_offset + (deltalon * granularity);

    }

    return true ;

}


static bool process_osm_data_ways(vector<Way> &ways, map<int64_t, uint64_t> &wayMap, vector< vector<int64_t> > &wayNodeMap, const PrimitiveGroup &group, const StringTable &string_table)
{
    for ( unsigned way_id = 0; way_id < group.ways_size() ; way_id++ )
    {
        int64_t deltaref = 0 ;

        const PBF::Way &way = group.ways(way_id) ;

        ways.push_back(Way()) ;
        Way &w = ways.back() ;

        w.id_ = make_id(way.id()) ;

        wayMap.insert(make_pair(way.id(), ways.size()-1)) ;

        for ( unsigned key_id = 0; key_id < way.keys_size() ; key_id++ )
        {
            uint32_t key_idx = way.keys(key_id) ;
            uint32_t val_idx = way.vals(key_id) ;

            string key = string_table.s(key_idx) ;
            string val = string_table.s(val_idx) ;

            w.tags_.add(key, val) ;
        }

        wayNodeMap.push_back( vector<int64_t>() ) ;
        vector<int64_t> &node_refs = wayNodeMap.back() ;

        for ( unsigned ref_id = 0; ref_id < way.refs_size() ; ref_id++ )
        {
            int64_t delta = way.refs(ref_id) ;
            deltaref += delta ;
            node_refs.push_back(deltaref) ;
        }
    }

    return true ;

}


static bool process_osm_data_relations(vector<Relation> &relations, map<int64_t, uint64_t> &relMap,
                                       vector< vector<int64_t> > &relNodeMap, vector< vector<int64_t> > &relWayMap, vector< vector<int64_t> > &relRelMap,
                                       vector< vector<string> > &roleNodeMap, vector< vector<string> > &roleWayMap, vector< vector<string> > &roleRelMap,
                                       const PrimitiveGroup &group, const StringTable &string_table)
{
    for ( unsigned rel_id = 0; rel_id < group.relations_size() ; rel_id++ )
    {
        const PBF::Relation &relation = group.relations(rel_id) ;

        relations.push_back(Relation()) ;
        Relation &r = relations.back() ;

        r.id_ = make_id(relation.id()) ;

        relMap.insert(make_pair(relation.id(), relations.size()-1)) ;

        for ( unsigned key_id = 0; key_id < relation.keys_size() ; key_id++ )
        {
            uint32_t key_idx = relation.keys(key_id) ;
            uint32_t val_idx = relation.vals(key_id) ;

            string key = string_table.s(key_idx) ;
            string val = string_table.s(val_idx) ;

            r.tags_.add(key, val) ;
        }

        relNodeMap.push_back( vector<int64_t>() ) ;
        vector<int64_t> &node_refs = relNodeMap.back() ;

        relWayMap.push_back( vector<int64_t>() ) ;
        vector<int64_t> &way_refs = relWayMap.back() ;

        relRelMap.push_back( vector<int64_t>() ) ;
        vector<int64_t> &rel_refs = relRelMap.back() ;

        roleNodeMap.push_back( vector<string>() ) ;
        vector<string> &node_roles = roleNodeMap.back() ;

        roleWayMap.push_back( vector<string>() ) ;
        vector<string> &way_roles = roleWayMap.back() ;

        roleRelMap.push_back( vector<string>() ) ;
        vector<string> &rel_roles = roleRelMap.back() ;

        uint64_t deltaref = 0 ;

        for( unsigned member_id = 0 ; member_id < relation.memids_size() ; member_id++ )
        {
            deltaref += relation.memids(member_id) ;

            int32_t role_id = relation.roles_sid(member_id) ;
            string role = string_table.s(role_id) ;

            switch (relation.types(member_id) ) {
                case PBF::Relation::NODE:
                    node_refs.push_back(deltaref) ;
                    node_roles.push_back(role) ;
                    break ;
                case PBF::Relation::WAY:
                    way_refs.push_back(deltaref) ;
                    way_roles.push_back(role) ;
                    break ;
                case PBF::Relation::RELATION:
                    rel_refs.push_back(deltaref) ;
                    rel_roles.push_back(role) ;
                    break ;
            }
        }


    }

    return true ;

}

bool Document::readPBF(const string &fileName)
{
    map<int64_t, uint64_t> nodeMap, wayMap, relMap ;
    vector< vector<int64_t> > wayNodeMap, relNodeMap, relWayMap, relRelMap ;
    vector< vector<string> > roleNodeMap, roleWayMap, roleRelMap ;

    BlockHeader header_msg;
    Blob blob_msg ;

    ifstream input ;
    input.open(fileName.c_str(), ios::in | ios::binary) ;

    if ( !input ) return false ;

    while ( input.good() )
    {
        // reader header and blob data

        if ( !read_header(header_msg, input) ) break ;

        if ( !read_blob(blob_msg, input, header_msg.datasize()) )
           return false ;

       // uncompress data

       size_t bsize = blob_msg.raw_size() ;

       std::unique_ptr<char []> bbuf(new char [bsize]) ;

       if ( !bbuf || !uncompress_blob(blob_msg, bbuf.get()) ) return false ;

       // process data

       if ( header_msg.type() == "OSMHeader" )
       {
           // Ignore header contents for now

           HeaderBlock hb_msg ;

           if ( !hb_msg.ParseFromArray(bbuf.get(), bsize) ) return false ;

       }
       else if ( header_msg.type() == "OSMData" )
       {
           PrimitiveBlock pb_msg ;

           if ( !pb_msg.ParseFromArray(bbuf.get(), bsize) ) return false ;

           double lat_offset = NANO_DEGREE * pb_msg.lat_offset();
           double lon_offset = NANO_DEGREE * pb_msg.lon_offset();
           double granularity = NANO_DEGREE * pb_msg.granularity();

           for ( int j = 0; j < pb_msg.primitivegroup_size(); j++ )
           {
                const PrimitiveGroup &group = pb_msg.primitivegroup(j) ;
                const StringTable &string_table = pb_msg.stringtable() ;


                if ( !process_osm_data_nodes(nodes_, nodeMap, group, string_table, lat_offset, lon_offset, granularity) ) return false ;
                if ( !process_osm_data_dense_nodes(nodes_, nodeMap, group, string_table, lat_offset, lon_offset, granularity) ) return false ;
                if ( !process_osm_data_ways(ways_, wayMap, wayNodeMap, group, string_table) ) return false ;
                if ( !process_osm_data_relations(relations_, relMap, relNodeMap, relWayMap, relRelMap,
                                                 roleNodeMap, roleWayMap, roleRelMap,
                                                 group, string_table) ) return false ;
           }
       }

     } ;


    // establish feature dependencies

    for(int i=0 ; i<ways_.size() ; i++ )
    {
        Way &way = ways_[i] ;

        vector<int64_t> &node_refs = wayNodeMap[i] ;

        for(int j=0 ; j<node_refs.size() ; j++ )
        {

            int idx = nodeMap[node_refs[j]] ;
            way.nodes_.push_back(idx) ;

            nodes_[idx].ways_.push_back(i) ;
        }

    }

    for(int i=0 ; i<relations_.size() ; i++ )
    {
        Relation &relation = relations_[i] ;

        vector<int64_t> &node_refs = relNodeMap[i] ;
        vector<string> &node_roles = roleNodeMap[i] ;

        for(int j=0 ; j<node_refs.size() ; j++ )
        {
            auto it = nodeMap.find(node_refs[j]) ;

            if ( it != nodeMap.end() )
            {
                int idx = (*it).second ;
                relation.nodes_.push_back(idx) ;
                relation.nodes_role_.push_back(node_roles[j]) ;

                nodes_[idx].relations_.push_back(i) ;
            }
        }

    }

    for(int i=0 ; i<relations_.size() ; i++ )
    {
        Relation &relation = relations_[i] ;

        vector<int64_t> &way_refs = relWayMap[i] ;
        vector<string> &way_roles = roleWayMap[i] ;

        for(int j=0 ; j<way_refs.size() ; j++ )
        {
            auto it = wayMap.find(way_refs[j]) ;

            if ( it != wayMap.end() )
            {

                int idx = (*it).second ;
                relation.ways_.push_back(idx) ;
                relation.ways_role_.push_back(way_roles[j]) ;

                ways_[idx].relations_.push_back(i) ;
            }
        }

    }

    for(int i=0 ; i<relations_.size() ; i++ )
    {
        Relation &relation = relations_[i] ;

        vector<int64_t> &rel_refs = relRelMap[i] ;
        vector<string> &rel_roles = roleRelMap[i] ;

        for(int j=0 ; j<rel_refs.size() ; j++ )
        {
            auto it = relMap.find(rel_refs[j]) ;

            if ( it != relMap.end() )
            {
                int idx = relMap[rel_refs[j]] ;
                relation.children_.push_back(idx) ;
                relation.children_role_.push_back(rel_roles[j]) ;

                relations_[idx].parents_.push_back(i) ;
            }
        }

    }

     return true ;


}





}
