#include "map_file.hpp"
#include <spatialite.h>

using namespace std ;

static bool processSetTagActions(const OSM::Filter::Rule *r, OSM::Filter::Context &ctx, OSM::Feature *node)
{
    /*
   OSM::Filter::CommandPtr *action = r->actions_ ;

   bool cont = false ;

   while ( action )
   {
       if ( action->type() ==  OSM::Filter::Command::Add )
       {
           OSM::Filter::SimpleCommand *ac = dynamic_cast<OSM::Filter::SimpleCommand *>(action) ;
           node->tags_.add(ac->tag_, ac->expression_->eval(ctx).toString()) ;
       }
       else if ( action->type() == OSM::Filter::Command::Set )
       {
           OSM::Filter::SimpleCommand *ac = dynamic_cast<OSM::Filter::SimpleCommand *>(action) ;

           if ( node->tags_.contains(ac->tag_) )
               node->tags_[ac->tag_] = ac->expression_->eval(ctx).toString() ;
           else
               node->tags_.add(ac->tag_, ac->expression_->eval(ctx).toString()) ;
       }
       else if ( action->type() == OSM::Filter::Command::Continue )
       {
           cont = true ;
       }
       else if ( action->type() == OSM::Filter::Command::Delete )
       {
           OSM::Filter::SimpleCommand *ac = dynamic_cast<OSM::Filter::SimpleCommand *>(action) ;
           node->tags_.remove(ac->tag_) ;
       }

       action = action->next_ ;

   }

   return cont ;
   */

}

static bool processStoreActions(const OSM::Filter::Rule *r, OSM::Filter::Context &ctx, OSM::Feature *node, vector<TagWriteAction> &actions)
{
    /*
   OSM::Filter::Command *action = r->actions_ ;

   bool cont = false ;

   while ( action )
   {
       if ( action->type() == OSM::Filter::Command::Continue )
       {
           cont = true ;
       }
       else if ( action->type() == OSM::Filter::Command::Store )
       {
           OSM::Filter::SimpleCommand *ac = dynamic_cast<OSM::Filter::SimpleCommand *>(action) ;
           Action act ;

           act.key_ = ac->tag_ ;
           act.val_ = ac->expression_->eval(ctx) ;

           actions.push_back(act) ;
       }

       action = action->next_ ;

   }

   return cont ;
*/
}

extern string escape_tag(const string &) ;

void bindActions(const TagWriteList &actions, SQLite::Command &cmd)
{
    string kvl ;

    for( int i=0 ; i<actions.actions_.size() ; i++ )
    {
        const TagWriteAction &act = actions.actions_[i] ;

        string val = act.val_ ;
        string key = act.key_ ;

        if ( !val.empty() ) kvl += escape_tag(key) + '@' + escape_tag(val) + ';' ;
    }

    if ( kvl.empty() ) cmd.bind(2, SQLite::Nil) ;
    else cmd.bind(2, kvl) ;
}

bool MapFile::addOSMLayerPoints(OSM::Document &doc, const OSM::Filter::LayerDefinition *layer,
                                const vector<NodeRuleMap > &node_idxs)
{
    /*
   SQLite::Database &db = handle() ;

   SQLite::Session session(&db) ;
   SQLite::Connection &con = session.handle() ;

   unsigned char *blob;
   int blob_size;

   if ( layer->type_ != "points" ) return false ;

   SQLite::Transaction trans(con) ;

   string geoCmd = "Transform(?," + layer->srid_ + ")" ;
   SQLite::Command cmd(con, insertFeatureSQL(layer->name_, geoCmd)) ;

   for(int i=0 ; i<node_idxs.size() ; i++ )
   {
       vector<Action> actions ;

       const NodeRuleMap &nr = node_idxs[i] ;

       int node_idx = nr.node_idx_ ;
       OSM::Node &node = doc.nodes_[node_idx] ;

       OSM::Filter::Context ctx(&node) ;

       for(int j=0 ; j<nr.matched_rules_.size() ; j++ )
       {
           const OSM::Filter::Rule *r = nr.matched_rules_[j] ;

           if ( ! processStoreActions(r, ctx, &node, actions) ) break ;
       }

       cmd.clear() ;

       bindActions(actions, cmd) ;

       gaiaGeomCollPtr geo_pt = gaiaAllocGeomColl();

       geo_pt->Srid = 4326;

       gaiaAddPointToGeomColl (geo_pt, node.lon_, node.lat_);

       gaiaToSpatiaLiteBlobWkb (geo_pt, &blob, &blob_size);

       gaiaFreeGeomColl (geo_pt);

       cmd.bind(1, blob, blob_size) ;

       cmd.exec() ;
       cmd.clear() ;
       free(blob);

   }

   trans.commit() ;
*/
    return true ;
}


bool MapFile::add_line_geometry(SQLite::Connection &con, OSM::Document &doc, const OSM::Way &way)
{
    try {
        unsigned char *blob;
        int blob_size;

        string geoCmd = "CompressGeometry(?)" ;
        SQLite::Command cmd(con, insertFeatureSQL("lines", geoCmd)) ;

        gaiaGeomCollPtr geo_line = gaiaAllocGeomColl();
        geo_line->Srid = 4326;

        gaiaLinestringPtr ls = gaiaAddLinestringToGeomColl (geo_line, way.nodes_.size());

        for(int j=0 ; j<way.nodes_.size() ; j++)
        {
            const OSM::Node &node = doc.nodes_[way.nodes_[j]] ;

            gaiaSetPoint (ls->Coords, j, node.lon_, node.lat_);
        }

        gaiaToSpatiaLiteBlobWkb (geo_line, &blob, &blob_size);

        cmd.bind(1, blob, blob_size) ;
        cmd.bind(2, way.id_) ;

        cmd.exec() ;


        gaiaFreeGeomColl (geo_line);
        free(blob) ;

    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
        return false ;
    }

    return true ;
}

bool MapFile::add_poi_geometry(SQLite::Connection &con, const OSM::Node &poi)
{
    try {
        unsigned char *blob;
        int blob_size;

        SQLite::Command cmd(con, insertFeatureSQL("pois")) ;

        gaiaGeomCollPtr geo_pt = gaiaAllocGeomColl();

        geo_pt->Srid = 4326;

        gaiaAddPointToGeomColl (geo_pt, poi.lon_, poi.lat_);

        gaiaToSpatiaLiteBlobWkb (geo_pt, &blob, &blob_size);

        cmd.bind(1, blob, blob_size) ;
        cmd.bind(2, poi.id_) ;

        cmd.exec() ;

        gaiaFreeGeomColl (geo_pt);
        free(blob) ;

    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
        return false ;
    }

}

bool MapFile::add_tags(SQLite::Connection &con, const TagWriteList &tags, const string &id)
{
    SQLite::Command cmd(con, "INSERT INTO kv (key, val, osm_id, zoom_min, zoom_max) VALUES (?, ?, ?, ?, ?)") ;

    try {
        for( const TagWriteAction &kv: tags.actions_) {

            cmd.bind(1, kv.key_) ;
            cmd.bind(2, kv.val_) ;
            cmd.bind(3, id) ;
            cmd.bind(4, kv.zoom_min_) ;
            cmd.bind(5, kv.zoom_max_) ;

            cmd.exec() ;
            cmd.clear() ;
        }
    }
    catch ( SQLite::Exception &e ) {
        cerr << e.what() << endl ;
    }

}

bool MapFile::addOSMLayerPolygons(const OSM::Document &doc, const OSM::Filter::LayerDefinition *layer,
                                  vector<OSM::Polygon> &polygons, const vector<NodeRuleMap > &poly_idxs)
{
    /*
   SQLite::Database &db = handle() ;

   SQLite::Session session(&db) ;
   SQLite::Connection &con = session.handle() ;

   unsigned char *blob;
   int blob_size;

   if ( layer->type_ != "polygons" ) return false ;

   SQLite::Transaction trans(con) ;

   string geoCmd = "CompressGeometry(Transform(ST_BuildArea(?)," + layer->srid_ + "))" ;
   SQLite::Command cmd(con, insertFeatureSQL(layer->name_,  geoCmd)) ;

   for( int i=0 ; i< poly_idxs.size() ; i++ )
   {
       vector<Action> actions ;

       const NodeRuleMap &nr = poly_idxs[i] ;

       int poly_idx = nr.node_idx_ ;
       OSM::Polygon &poly = polygons[poly_idx] ;

       OSM::Filter::Context ctx(&poly) ;

       for(int j=0 ; j<nr.matched_rules_.size() ; j++ ) {
           const OSM::Filter::Rule *r =nr.matched_rules_[j] ;
           if ( ! processStoreActions(r, ctx, &poly, actions) ) break ;
       }

       cmd.clear() ;

       bindActions(actions, cmd) ;

       gaiaGeomCollPtr geo_poly = gaiaAllocGeomColl();
       geo_poly->Srid = 4326;

       for(int j=0 ; j<poly.rings_.size() ; j++)
       {
           auto ring = poly.rings_[j] ;
           gaiaLinestringPtr gpoly = gaiaAddLinestringToGeomColl(geo_poly,ring.nodes_.size());

           for(int k=0 ; k<ring.nodes_.size() ; k++)
           {
               auto node = doc.nodes_[ring.nodes_[k]] ;

               gaiaSetPoint (gpoly->Coords, k, node.lon_, node.lat_);
           }
       }

       gaiaToSpatiaLiteBlobWkb (geo_poly, &blob, &blob_size);

       gaiaFreeGeomColl (geo_poly);

       cmd.bind(1, blob, blob_size) ;

       cmd.exec() ;

       free(blob) ;
   }

   trans.commit() ;
*/
    return true ;
}

using namespace OSM::Filter ;

bool MapFile::processOsmFiles(const vector<string> &osmFiles, const FilterConfig &cfg)
{
    // read files from memory and write to spatialite database

    SQLite::Database &db = handle() ;

    SQLite::Session session(&db) ;
    SQLite::Connection &con = session.handle() ;

    for(int i=0 ; i<osmFiles.size() ; i++ ) {

        OSM::Document doc ;

        cout << "Reading file: " << osmFiles[i] << endl ;

        if ( !doc.read(osmFiles[i]) )
        {
            cerr << "Error reading from " << osmFiles[i] << endl ;
            continue ;
        }

        std::vector<NodeRuleMap> passFilterNodes, passFilterWays, passFilterPoly, passFilterRel ;

        SQLite::Transaction trans(con) ;

        for(int k=0 ; k<doc.nodes_.size() ; k++ )
        {
            auto node = doc.nodes_[k] ;

            if ( node.tags_.empty() ) continue ;

            OSM::Filter::Context ctx(node) ;

            NodeRuleMap nr ;

            nr.node_idx_ = k ;

            TagWriteList tags ;
            bool cont = false;

            for( const RulePtr &rule: cfg.rules_ )
            {

                if ( match_rule( rule, ctx, tags, cont) ) {
                    if ( !cont ) break ;
                }
            }

            if ( tags.actions_.empty() ) continue ;

            add_poi_geometry(con, node) ;
            add_tags(con, tags, node.id_) ;
        }



        for(int k=0 ; k<doc.ways_.size() ; k++ )
        {
            auto way = doc.ways_[k] ;

            if ( way.tags_.empty() ) continue ;

            // deal with closed ways

            if ( way.nodes_.front() == way.nodes_.back() )
            {
                if ( way.tags_.get("area") == "yes" ) continue ;
                if ( !way.tags_.contains("highway") && !way.tags_.contains("barrier") && !way.tags_.contains("contour") ) continue ;
            }

            // match feature with filter rules

            OSM::Filter::Context ctx(way) ;

            TagWriteList tags ;
            bool cont = false;

            for( const RulePtr &rule: cfg.rules_ )
            {
                if ( match_rule( rule, ctx, tags, cont) ) {
                    if ( !cont ) break ;
                }
            }

            if ( tags.actions_.empty() ) continue ;

            add_line_geometry(con, doc, way) ;
            add_tags(con, tags, way.id_) ;
        }

        // relations of type route, merge ways into chunks

        for(int k=0 ; k<doc.relations_.size() ; k++ )
        {
            auto relation = doc.relations_[k] ;

            if ( relation.tags_.get("type") != "route" ) continue ;

            OSM::Filter::Context ctx(relation) ;

            TagWriteList tags ;
            bool cont = false;

            for( const RulePtr &rule: cfg.rules_ )
            {
                if ( match_rule( rule, ctx, tags, cont) ) {
                    if ( !cont ) break ;
                }
            }

            if ( tags.actions_.empty() ) continue ;

            vector<OSM::Way> chunks ;
            if ( !OSM::Document::makeWaysFromRelation(doc, relation, chunks) ) continue ;


        }

        trans.commit() ;

        /*
            else if ( layer->type_ == "lines" )
            {
                for(int k=0 ; k<doc.ways_.size() ; k++ )
                {
                    auto way = doc.ways_[k] ;

                    OSM::Filter::Context ctx(&way) ;

                    NodeRuleMap nr ;

                    nr.node_idx_ = k ;

                    for( const OSM::Filter::Rule *r = layer->rules_ ; r ; r = r->next_ )
                    {
                        if ( r->node_ && !r->node_->eval(ctx).toBoolean() ) continue ;
                        processSetTagActions(r, ctx, &way) ;
                        nr.matched_rules_.push_back(r) ;
                    }

                    // deal with closed ways

                    if ( way.nodes_.front() == way.nodes_.back() )
                    {
                        if ( way.tags_.get("area") == "yes" ) continue ;
                        if ( !way.tags_.contains("highway") && !way.tags_.contains("barrier") && !way.tags_.contains("contour") ) continue ;
                    }

                    if ( !nr.matched_rules_.empty() ) passFilterWays.push_back(nr) ;
                }

                // relations of type route, merge ways into chunks

                vector<OSM::Way> chunk_list ;

                for(int k=0 ; k<doc.relations_.size() ; k++ )
                {
                    auto relation = doc.relations_[k] ;

                    if ( relation.tags_.get("type") != "route" ) continue ;

                    OSM::Filter::Context ctx(&relation) ;

                    vector<const OSM::Filter::Rule *> matched ;

                    for( const OSM::Filter::Rule *r = layer->rules_ ; r ; r = r->next_ )
                    {
                        if ( r->node_ && !r->node_->eval(ctx).toBoolean() ) continue ;
                        processSetTagActions(r, ctx, &relation) ;
                        matched.push_back(r) ;
                    }

                    if ( matched.empty() ) continue ;

                    vector<OSM::Way> chunks ;
                    if ( !OSM::Document::makeWaysFromRelation(doc, relation, chunks) ) continue ;

                    for(int c=0 ; c<chunks.size() ; c++)
                    {
                        NodeRuleMap nr ;

                        nr.node_idx_ = chunk_list.size() ;
                        nr.matched_rules_ = matched ;

                        chunk_list.push_back(chunks[i]) ;
                        passFilterRel.push_back(nr) ;
                    }

                }

                addOSMLayerLines(doc, layer, passFilterWays, chunk_list, passFilterRel) ;
            }
            else if ( layer->type_ == "polygons" )
            {
                vector<OSM::Polygon> polygons ;

                // first look for multi-polygon relations

                for(int k=0 ; k<doc.relations_.size() ; k++ )
                {
                    OSM::Relation &relation = doc.relations_[k] ;

                    string rel_type = relation.tags_.get("type") ;
                    if (  rel_type != "multipolygon" && rel_type != "boundary" ) continue ;

                    OSM::Filter::Context ctx(&relation) ;

                    vector<const OSM::Filter::Rule *> matched ;

                    for( const OSM::Filter::Rule *r = layer->rules_ ; r ; r = r->next_ )
                    {
                        if ( r->node_ && !r->node_->eval(ctx).toBoolean() ) continue ;
                        processSetTagActions(r, ctx, &relation) ;
                        matched.push_back(r) ;
                    }

                    if ( matched.empty() ) continue ;

                    OSM::Polygon polygon ;
                    if ( !OSM::Document::makePolygonsFromRelation(doc, relation, polygon) ) continue ;

                    NodeRuleMap nr ;

                    nr.node_idx_ = polygons.size() ;
                    nr.matched_rules_ = matched ;
                    polygons.push_back(polygon) ;

                    passFilterPoly.push_back(nr) ;

                }

                // check simple polygons

                for(int k=0 ; k<doc.ways_.size() ; k++ )
                {
                    auto way = doc.ways_[k] ;

                    if ( way.nodes_.front() != way.nodes_.back() ) continue ;
                    if ( way.tags_.get("area") == "no" ) continue ;
                    if ( way.tags_.contains("highway") ) continue ;
                    if ( way.tags_.contains("barrier") ) continue ;

                    OSM::Filter::Context ctx(&way) ;

                    NodeRuleMap nr ;

                    nr.node_idx_ = polygons.size()  ;

                    for( const OSM::Filter::Rule *r = layer->rules_ ; r ; r = r->next_ )
                    {
                        if ( r->node_ && !r->node_->eval(ctx).toBoolean() ) continue ;
                        processSetTagActions(r, ctx, &way) ;
                        nr.matched_rules_.push_back(r) ;
                    }

                    if ( nr.matched_rules_.empty() ) continue ;

                    OSM::Polygon poly ;

                    OSM::Ring ring ;
                    ring.nodes_.insert(ring.nodes_.end(), way.nodes_.begin(), way.nodes_.end()) ;
                    poly.rings_.push_back(ring) ;
                    poly.tags_ = way.tags_ ;
                    polygons.push_back(poly) ;
                    passFilterPoly.push_back(nr) ;

                }

                addOSMLayerPolygons(doc, layer, polygons, passFilterPoly) ;
            }



        }
        */
    }

    return true ;

}

bool MapFile::match_rule(const RulePtr &rule, Context &ctx, TagWriteList &tw, bool &cont)
{
    cont = false ;

    if ( !rule->condition_ || rule->condition_->eval(ctx).toBoolean() ) {

        bool rcont = true ;

        for ( const CommandPtr &cmd: rule->commands_->commands_ ) {
            if ( cmd->type() == Command::Conditional && rcont ) {
                RuleCommand *rc = dynamic_cast<RuleCommand *>(cmd.get());
                bool c = false ;
                if ( match_rule(rc->rule_, ctx, tw, c) ) {
                    if ( !c ) rcont = false ;
                }
            }
            else if ( cmd->type() == Command::Set ) {
                SimpleCommand *rc = dynamic_cast<SimpleCommand *>(cmd.get());

                if ( ctx.has_tag(rc->tag_ ) )
                    ctx.tags_[rc->tag_] = rc->val_->eval(ctx).toString() ;
                else
                    ctx.tags_.add(rc->tag_, rc->val_->eval(ctx).toString()) ;
            }
            else if ( cmd->type() == Command::Add ) {
                SimpleCommand *rc = dynamic_cast<SimpleCommand *>(cmd.get());
                ctx.tags_.add(rc->tag_, rc->val_->eval(ctx).toString()) ;
            }
            else if ( cmd->type() == Command::Continue ) {
                cont = true ;
            }
            else if ( cmd->type() == Command::Delete ) {
                SimpleCommand *rc = dynamic_cast<SimpleCommand *>(cmd.get());
                ctx.tags_.remove(rc->tag_) ;
            }
            else if ( cmd->type() == Command::WriteAll ) {
                WriteAllCommand *rc = dynamic_cast<WriteAllCommand *>(cmd.get());
                ZoomRange zr = rc->zoom_range_ ;
                DictionaryIterator it(ctx.tags_) ;
                while ( it ) {
                    tw.actions_.emplace_back(it.key(), it.value(), zr.min_zoom_, zr.max_zoom_) ;
                    ++it ;
                }
            }
            else if ( cmd->type() == Command::Write ) {
                WriteCommand *rc = dynamic_cast<WriteCommand *>(cmd.get());
                ZoomRange zr = rc->zoom_range_ ;
                for( const string &tag: rc->tags_ ) {
                    if ( ctx.has_tag(tag) )
                        tw.actions_.emplace_back(tag, ctx.value(tag), zr.min_zoom_, zr.max_zoom_) ;
                }
            }
            else if ( cmd->type() == Command::Exclude ) {
                ExcludeCommand *rc = dynamic_cast<ExcludeCommand *>(cmd.get());

                set<string> exclude(rc->tags_.begin(), rc->tags_.end()) ;

                ZoomRange zr = rc->zoom_range_ ;
                DictionaryIterator it(ctx.tags_) ;
                while ( it ) {
                    if ( exclude.count(it.key()) == 0 )
                        tw.actions_.emplace_back(it.key(), it.value(), zr.min_zoom_, zr.max_zoom_) ;
                    ++it ;
                }
            }
        }
        return true ;
    }
    else return false ;
}
