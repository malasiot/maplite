#include "lua_context.hpp"
#include "tag_filter_context.hpp"

#include <sstream>

using namespace std ;

LuaContext::LuaContext()
{
    state_ = luaL_newstate() ;
    luaL_openlibs(state_);
}


LuaContext::~LuaContext() {
    lua_close(state_) ;
}

void LuaContext::error(const std::string &msg) {
    ostringstream strm ;

    strm << msg << lua_tostring(state_, -1) ;

    error_str_ = strm.str() ;
}

static int add_tag(lua_State* L)
{
    TagFilterContext **ctx_proxy = (TagFilterContext **) lua_touserdata(L, lua_upvalueindex(1));
    TagFilterContext *ctx = *ctx_proxy ;

    string val ;
    string key = lua_tostring(L, 1) ;
    if ( lua_isnil(L, 2) ) return 0 ;
    val = lua_tostring(L, 2) ;

    ctx->tags_[key] = val ;

    return 0 ;
}

static int attach_tags(lua_State* L)
{
    TagFilterContext **ctx_proxy = (TagFilterContext **) lua_touserdata(L, lua_upvalueindex(1));

    TagFilterContext *ctx = *ctx_proxy ;
    TagWriteList *tw = &ctx->tw_ ;

    if ( !tw ) return 0 ;

    int argCount = lua_gettop(L);

    for(int i = 1; i <= argCount; i++)
    {
        if ( lua_isstring(L, i) ) {
            string tag = lua_tostring(L, i) ;
            DictionaryIterator it(ctx->tags_) ;
            while ( it ) {
              if ( it.key() == tag )
              tw->actions_.emplace_back(it.key(), it.value(), 0, 255, true) ;
              ++it ;
            }

            break;
        }
    }

    return 0 ;
}

static int is_way(lua_State* L)
{
    TagFilterContext **ctx_proxy = (TagFilterContext **) lua_touserdata(L, lua_upvalueindex(1));

    TagFilterContext *ctx = *ctx_proxy ;

    bool is_way = (ctx->type_ == TagFilterContext::Way) ;
    lua_pushboolean(L, is_way) ;

    return 1 ;
}

static int is_node(lua_State* L)
{
    TagFilterContext **ctx_proxy = (TagFilterContext **) lua_touserdata(L, lua_upvalueindex(1));

    TagFilterContext *ctx = *ctx_proxy ;

    bool is_node = (ctx->type_ == TagFilterContext::Node) ;
    lua_pushboolean(L, is_node) ;

    return 1 ;
}


bool LuaContext::loadScript(const std::string &script) {
    if ( luaL_loadstring(state_, script.c_str()) || lua_pcall(state_, 0, 0, 0) ) {
        error("error parsing Lua script: ") ;
        return false ;
    }

    lua_pushlightuserdata(state_, &cproxy_);
    lua_pushcclosure(state_, &attach_tags, 1);
    lua_setglobal(state_, "attach_tags") ;

    lua_pushlightuserdata(state_, &cproxy_);
    lua_pushcclosure(state_, &add_tag, 1);
    lua_setglobal(state_, "add_tag") ;

    lua_pushlightuserdata(state_, &cproxy_);
    lua_pushcclosure(state_, &is_node, 1);
    lua_setglobal(state_, "is_node") ;

    lua_pushlightuserdata(state_, &cproxy_);
    lua_pushcclosure(state_, &is_way, 1);
    lua_setglobal(state_, "is_way") ;

    return true ;
}

void LuaContext::addDictionary(const Dictionary &dict)
{
    // set global variable
    lua_createtable(state_, 0, dict.count());

    DictionaryIterator it(dict) ;
    while ( it ) {
        lua_pushstring(state_, it.value().c_str());
        lua_setfield(state_, -2, it.key().c_str());
        ++it ;
    }

}

tag_filter::Literal LuaContext::call(const string &fname, const std::vector<tag_filter::Literal> &args)
{
    using namespace tag_filter ;

    /* push functions and arguments */
    lua_getglobal(state_, fname.c_str());  /* function to be called */

    for( const Literal &l: args) {
        if ( l.type_ == Literal::Boolean )
            lua_pushboolean(state_, (int)l.boolean_val_) ;
        else if ( l.type_ == Literal::String )
            lua_pushstring(state_, l.string_val_.c_str()) ;
        else if ( l.type_ == Literal::Number )
            lua_pushnumber(state_, l.number_val_);
        else if ( l.type_ == Literal::Null )
            lua_pushnil(state_);
    }

    /* do the call */
    if (lua_pcall(state_, args.size(), 1, 0) != 0) {
       error("in function call") ;
       cerr << error_str_ << endl ;
       return Literal() ;
    }

    /* retrieve result */

    if ( lua_isstring(state_, -1) ) {
         string z = lua_tostring(state_, -1);
         lua_pop(state_, 1);
         return Literal(z) ;
    }
    else if ( lua_isboolean(state_, -1) ) {
        bool res = lua_toboolean(state_, -1) ;
        lua_pop(state_, -1) ;
        return Literal(res) ;
    }
    else if ( lua_isnumber(state_, -1) ) {
        double res = lua_tonumber(state_, -1) ;
        lua_pop(state_, -1) ;
        return Literal(res) ;
    }
    else {
        lua_pop(state_, -1) ;
        return Literal();
    }
}

void LuaContext::setupContext(TagFilterContext &ctx)
{
    cproxy_ = &ctx ;
    addDictionary(ctx.tags_) ;
    lua_setglobal(state_, "_tags_") ;

    using namespace tag_filter ;

    if ( ctx.type_ == TagFilterContext::Way ) {
        OSM::Way &way = ctx.doc_->ways_[ctx.fid_] ;
        lua_newtable(state_) ;
        uint count = 1 ;

        for( uint idx: way.relations_) {
            lua_pushinteger(state_, count++) ;
            OSM::Relation &rel = ctx.doc_->relations_[idx] ;
            addDictionary(rel.tags_) ;
        }
        lua_setglobal(state_, "_relations_") ;
    }
}
