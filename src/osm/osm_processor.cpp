#include "map_file.hpp"

using namespace std ;

static bool processSetTagActions(const OSM::Filter::Rule *r, OSM::Filter::Context &ctx, OSM::Feature *node)
{
   OSM::Filter::Command *action = r->actions_ ;

   bool cont = false ;

   while ( action )
   {
       if ( action->cmd_ ==  OSM::Filter::Command::Add )
       {
           node->tags_.add(action->tag_, action->expression_->eval(ctx).toString()) ;
       }
       else if ( action->cmd_ == OSM::Filter::Command::Set )
       {
           if ( node->tags_.contains(action->tag_) )
               node->tags_[action->tag_] = action->expression_->eval(ctx).toString() ;
           else
               node->tags_.add(action->tag_, action->expression_->eval(ctx).toString()) ;
       }
       else if ( action->cmd_ == OSM::Filter::Command::Continue )
       {
           cont = true ;
       }
       else if ( action->cmd_ == OSM::Filter::Command::Delete )
       {
           node->tags_.remove(action->tag_) ;
       }

       action = action->next_ ;

   }

   return cont ;

}

static bool processStoreActions(const OSM::Filter::Rule *r, OSM::Filter::Context &ctx, OSM::Feature *node, vector<Action> &actions)
{
   OSM::Filter::Command *action = r->actions_ ;

   bool cont = false ;

   while ( action )
   {
       if ( action->cmd_ == OSM::Filter::Command::Continue )
       {
           cont = true ;
       }
       else if ( action->cmd_ == OSM::Filter::Command::Store )
       {
           Action act ;

           act.key_ = action->tag_ ;
           act.val_ = action->expression_->eval(ctx) ;

           actions.push_back(act) ;
       }

       action = action->next_ ;

   }

   return cont ;

}

extern string escape_tag(const string &) ;

void bindActions(const vector<Action> &actions, SQLite::Command &cmd)
{
   string kvl ;

   for( int i=0 ; i<actions.size() ; i++ )
   {
       const Action &act = actions[i] ;

       string val = act.val_.toString() ;
       string key = act.key_ ;

       if ( !val.empty() ) kvl += escape_tag(key) + '@' + escape_tag(val) + ';' ;
   }

   if ( kvl.empty() ) cmd.bind(2, SQLite::Nil) ;
   else cmd.bind(2, kvl) ;
}

bool MapFile::addOSMLayerPoints(OSM::Document &doc, const OSM::Filter::LayerDefinition *layer,
                      const vector<NodeRuleMap > &node_idxs)
{
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

   return true ;
}


bool MapFile::addOSMLayerLines(OSM::Document &doc, const OSM::Filter::LayerDefinition *layer,
                     const vector<NodeRuleMap> &way_idxs,
                     vector<OSM::Way> &chunk_list,
                     const vector<NodeRuleMap > &rule_map
                     )
{

   SQLite::Database &db = handle() ;

   SQLite::Session session(&db) ;
   SQLite::Connection &con = session.handle() ;

   unsigned char *blob;
   int blob_size;

   if ( layer->type_ != "lines" ) return false ;

   SQLite::Transaction trans(con) ;

   string geoCmd = "CompressGeometry(Transform(?," + layer->srid_ + "))" ;
   SQLite::Command cmd(con, insertFeatureSQL(layer->name_, geoCmd)) ;

   for(int i=0 ; i<way_idxs.size() ; i++ )
   {
       vector<Action> actions ;

       const NodeRuleMap &nr = way_idxs[i] ;

       int node_idx = nr.node_idx_ ;
       OSM::Way &way = doc.ways_[node_idx] ;

       OSM::Filter::Context ctx(&way) ;

       for(int j=0 ; j<nr.matched_rules_.size() ; j++ ) {
           const OSM::Filter::Rule *r = nr.matched_rules_[j] ;
           if ( ! processStoreActions(r, ctx, &way, actions) ) break ;
       }

       cmd.clear() ;

       bindActions(actions, cmd) ;

       gaiaGeomCollPtr geo_line = gaiaAllocGeomColl();
       geo_line->Srid = 4326;

       gaiaLinestringPtr ls = gaiaAddLinestringToGeomColl (geo_line, way.nodes_.size());

       for(int j=0 ; j<way.nodes_.size() ; j++)
       {
           const OSM::Node &node = doc.nodes_[way.nodes_[j]] ;

           gaiaSetPoint (ls->Coords, j, node.lon_, node.lat_);
       }

       gaiaToSpatiaLiteBlobWkb (geo_line, &blob, &blob_size);

       gaiaFreeGeomColl (geo_line);

       cmd.bind(1, blob, blob_size) ;

       cmd.exec() ;

       free(blob) ;

   }

   for( int i=0 ; i<rule_map.size() ; i++ )
   {
       vector<Action> actions ;

       const NodeRuleMap &nr = rule_map[i] ;

       OSM::Way &way = chunk_list[i] ;

       OSM::Filter::Context ctx(&way) ;

       for(int j=0 ; j<nr.matched_rules_.size() ; j++ ) {
           const OSM::Filter::Rule *r = nr.matched_rules_[j] ;
           if ( ! processStoreActions(r, ctx, &way, actions) ) break ;
       }

       cmd.clear() ;

       bindActions(actions, cmd) ;

       gaiaGeomCollPtr geo_line = gaiaAllocGeomColl();
       geo_line->Srid = 4326;

       gaiaLinestringPtr ls = gaiaAddLinestringToGeomColl (geo_line, way.nodes_.size());

       for(int j=0 ; j<way.nodes_.size() ; j++)
       {
           const OSM::Node &node = doc.nodes_[way.nodes_[j]] ;

           gaiaSetPoint (ls->Coords, j, node.lon_, node.lat_);
       }

       gaiaToSpatiaLiteBlobWkb (geo_line, &blob, &blob_size);

       gaiaFreeGeomColl (geo_line);

       cmd.bind(1, blob, blob_size) ;

       cmd.exec() ;

       free(blob) ;

   }

   trans.commit() ;

   return true ;
}

bool MapFile::addOSMLayerPolygons(const OSM::Document &doc, const OSM::Filter::LayerDefinition *layer,
                        vector<OSM::Polygon> &polygons, const vector<NodeRuleMap > &poly_idxs)
{
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

   return true ;
}


bool MapFile::processOsmFiles(const vector<string> &osmFiles, const ImportConfig &cfg)
{
    // read files from memory and write to spatialite database

    for(int i=0 ; i<osmFiles.size() ; i++ ) {

        OSM::Document doc ;

        cout << "Reading file: " << osmFiles[i] << endl ;

        if ( !doc.read(osmFiles[i]) )
        {
            cerr << "Error reading from " << osmFiles[i] << endl ;
            continue ;
        }

        for( OSM::Filter::LayerDefinition *layer = cfg.layers_ ;
             layer ; layer = layer->next_ )
        {

            std::vector<NodeRuleMap> passFilterNodes, passFilterWays, passFilterPoly, passFilterRel ;

            if ( layer->type_ == "points" )
            {
                for(int k=0 ; k<doc.nodes_.size() ; k++ )
                {
                    auto node = doc.nodes_[k] ;

                    OSM::Filter::Context ctx(&node) ;

                    NodeRuleMap nr ;

                    nr.node_idx_ = k ;

                    for( OSM::Filter::Rule *r = layer->rules_ ; r ; r = r->next_ )
                    {
                        if ( r->node_ && !r->node_->eval(ctx).toBoolean() ) continue ;
                        processSetTagActions(r, ctx, &node) ;
                        nr.matched_rules_.push_back(r) ;
                    }

                    if ( !nr.matched_rules_.empty() ) passFilterNodes.push_back(nr) ;

                }

                addOSMLayerPoints(doc, layer, passFilterNodes) ;
            }
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
    }

    return true ;

}
