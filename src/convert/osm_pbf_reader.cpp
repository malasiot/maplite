#include "osm_pbf_reader.hpp"

#include <iostream>

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

static uint32_t get_length(istream &input)
{
    char buf[4];

    if ( !input.read(buf, sizeof(buf) ) ) return 0;

    return ntohl(*((size_t *)buf));
}

static bool read_header(BlockHeader &header_msg, istream &input)
{
    size_t length = get_length(input);

    if ( length == 0 ) return false ;

    std::unique_ptr<char []> buf(new char [length]) ;

    if ( !buf || !input.read(buf.get(), length) ) return false ;

    return header_msg.ParseFromArray(buf.get(), length) ;
}

static bool read_blob(Blob &blob_msg, istream &input, int32_t length)
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


bool PBFReader::process_osm_data_nodes(Storage &doc, const PrimitiveGroup &group, const StringTable &string_table, double lat_offset, double lon_offset, double granularity)
{
    for ( unsigned node_id = 0; node_id < group.nodes_size() ; node_id++ )
    {
        const PBF::Node &node = group.nodes(node_id) ;

        Node n ;
        n.id_ = node.id() ;

        n.lat_ = lat_offset + (node.lat() * granularity);
        n.lon_ = lon_offset + (node.lon() * granularity);

        for ( unsigned key_id = 0; key_id < node.keys_size() ; key_id++ )
        {
            uint32_t key_idx = node.keys(key_id) ;
            uint32_t val_idx = node.vals(key_id) ;

            string key = string_table.s(key_idx) ;
            string val = string_table.s(val_idx) ;

            n.tags_.add(key, val) ;
        }

        doc.writeNode(n) ;
    }

    return true ;

}

bool PBFReader::process_osm_data_dense_nodes(Storage &doc, const PrimitiveGroup &group, const StringTable &string_table, double lat_offset, double lon_offset, double granularity)
{
    if ( !group.has_dense() ) return true ;

    unsigned l = 0;
    int64_t deltaid = 0;
    long int deltalat = 0;
    long int deltalon = 0;

    const DenseNodes &dense = group.dense() ;

    for ( unsigned node_id = 0; node_id < dense.id_size() ; node_id++ )
    {
        Node n ;

        deltaid += dense.id(node_id) ;
        deltalat += dense.lat(node_id);
        deltalon += dense.lon(node_id) ;

        n.id_ = deltaid ;

        if ( n.id_ < 0 ) std::cout << n.id_ << std::endl ;

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

        doc.writeNode(n);

    }

    return true ;

}


bool PBFReader::process_osm_data_ways(Storage &doc, const PrimitiveGroup &group, const StringTable &string_table)
{
    for ( unsigned way_id = 0; way_id < group.ways_size() ; way_id++ )
    {
        int64_t deltaref = 0 ;

        const PBF::Way &way = group.ways(way_id) ;

        Way w ;

        w.id_ = way.id() ;

        for ( unsigned key_id = 0; key_id < way.keys_size() ; key_id++ )
        {
            uint32_t key_idx = way.keys(key_id) ;
            uint32_t val_idx = way.vals(key_id) ;

            string key = string_table.s(key_idx) ;
            string val = string_table.s(val_idx) ;

            w.tags_.add(key, val) ;
        }

        for ( unsigned ref_id = 0; ref_id < way.refs_size() ; ref_id++ )
        {
            int64_t delta = way.refs(ref_id) ;
            deltaref += delta ;
            w.nodes_.push_back(deltaref) ;
        }

        doc.writeWay(w) ;
    }


    return true ;

}


bool PBFReader::process_osm_data_relations(Storage &doc, const PrimitiveGroup &group, const StringTable &string_table)
{
    for ( unsigned rel_id = 0; rel_id < group.relations_size() ; rel_id++ )
    {
        const PBF::Relation &relation = group.relations(rel_id) ;

        Relation r ;

        r.id_ = relation.id() ;

        for ( unsigned key_id = 0; key_id < relation.keys_size() ; key_id++ )
        {
            uint32_t key_idx = relation.keys(key_id) ;
            uint32_t val_idx = relation.vals(key_id) ;

            string key = string_table.s(key_idx) ;
            string val = string_table.s(val_idx) ;

            r.tags_.add(key, val) ;
        }

        int64_t deltaref = 0 ;

        for( unsigned member_id = 0 ; member_id < relation.memids_size() ; member_id++ )
        {
            deltaref += relation.memids(member_id) ;

            int64_t role_id = relation.roles_sid(member_id) ;
            string role = string_table.s(role_id) ;

            switch (relation.types(member_id) ) {
            case PBF::Relation::NODE:
                r.nodes_.push_back(deltaref) ;
                r.nodes_role_.push_back(role) ;
                break ;
            case PBF::Relation::WAY:
                r.ways_.push_back(deltaref) ;
                r.ways_role_.push_back(role) ;
                break ;
            case PBF::Relation::RELATION:
                r.children_.push_back(deltaref) ;
                r.children_role_.push_back(role) ;
                break ;
            }
        }

        doc.writeRelation(r);

    }

    return true ;

}

bool PBFReader::read(istream &input, Storage &doc)
{
    BlockHeader header_msg;
    Blob blob_msg ;

    if ( !input ) return false ;

    doc.writeBegin() ;

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

                if ( !process_osm_data_nodes(doc, group, string_table, lat_offset, lon_offset, granularity) ) return false ;
                if ( !process_osm_data_dense_nodes(doc, group, string_table, lat_offset, lon_offset, granularity) ) return false ;
                if ( !process_osm_data_ways(doc, group, string_table) ) return false ;
                if ( !process_osm_data_relations(doc, group, string_table) ) return false ;
            }
        }

    }

    doc.writeEnd() ;

    return true ;


}

}
