#include "lua_context.hpp"
#include "tag_filter_context.hpp"
#include "osm_document.hpp"

#include <iostream>
#include <sstream>

using namespace std ;

LuaContext::LuaContext()
{
    state_ = luaL_newstate() ;

    if ( state_ == nullptr )
        throw std::bad_alloc();

    luaL_openlibs(state_);

    lua_atpanic(state_, [](lua_State* L) -> int {
        const std::string str = lua_tostring(L, -1);
        throw LuaException(str) ;
        lua_pop(L, 1);
    });
}


LuaContext::~LuaContext() {
    lua_close(state_) ;
}

void LuaContext::error(const std::string &msg) {
    ostringstream strm ;

    strm << msg << lua_tostring(state_, -1) ;

    throw LuaException(strm.str()) ;
}

static void stackDump (lua_State *L) {
    int i;
    int top = lua_gettop(L);
    for (i = 1; i <= top; i++) {  /* repeat for each level */
        int t = lua_type(L, i);
        switch (t) {

        case LUA_TSTRING:  /* strings */
            cout << lua_tostring(L, i) ;
            break;

        case LUA_TBOOLEAN:  /* booleans */
            cout << lua_toboolean(L, i) ? "true" : "false" ;
            break;

        case LUA_TNUMBER:  /* numbers */
            cout << lua_tonumber(L, i) ;
            break;

        default:  /* other values */
            cout <<  lua_typename(L, t);
            break;

        }
        cout << "  ";  /* put a separator */
    }
    cout << endl;  /* end the listing */
}


static int add_tags(lua_State* L)
{
    TagFilterContext **ctx_proxy = (TagFilterContext **) lua_touserdata(L, lua_upvalueindex(1));
    TagFilterContext *ctx = *ctx_proxy ;

    if ( lua_istable(L, 1) ) {
        lua_pushnil(L);  /* first key */

        while ( lua_next(L, 1) ) {
            string key = lua_tostring(L, -2) ;
            string val = lua_tostring(L, -1) ;
            ctx->tags_[key] = val ;
            lua_pop(L, 1);
        }
    }
    else {
        lua_pushstring(L, "add_tags expects table with key/value pairs") ;
        lua_error(L) ;
    }

    return 0 ;
}

static int set_tags(lua_State* L)
{
    TagFilterContext **ctx_proxy = (TagFilterContext **) lua_touserdata(L, lua_upvalueindex(1));
    TagFilterContext *ctx = *ctx_proxy ;

    if ( lua_istable(L, 1) ) {
        lua_pushnil(L);  /* first key */

        while ( lua_next(L, 1) ) {
            string key = lua_tostring(L, -2) ;
            string val = lua_tostring(L, -1) ;
            if ( ctx->tags_.contains(key) )
                ctx->tags_[key] = val ;
            lua_pop(L, 1);
        }
    }
    else {
        lua_pushstring(L, "set_tags expects table with key/value pairs") ;
        lua_error(L) ;
    }

    return 0 ;
}

static int attach_tags(lua_State* L)
{
    TagFilterContext **ctx_proxy = (TagFilterContext **) lua_touserdata(L, lua_upvalueindex(1));

    TagFilterContext *ctx = *ctx_proxy ;
    TagWriteList *tw = &ctx->tw_ ;

    if ( !tw ) return 0 ;

    if ( lua_istable(L, 1) ) {
        size_t len = lua_rawlen(L, 1) ;
        for(int i=1 ; i<=len ; i++) {
            lua_rawgeti (L, 1, i) ;
            string key = lua_tostring(L, -1) ;
            if ( ctx->tags_.contains(key) )
                tw->actions_.emplace_back(key, ctx->tags_.get(key), 0, 255, true) ;
        }
    }
    else {
        lua_pushstring(L, "attach_tags expects array of tags") ;
        lua_error(L) ;
    }

    return 0 ;
}

static int delete_tags(lua_State* L)
{
    TagFilterContext **ctx_proxy = (TagFilterContext **) lua_touserdata(L, lua_upvalueindex(1));

    TagFilterContext *ctx = *ctx_proxy ;

    if ( lua_istable(L, 1) ) {
        size_t len = lua_rawlen(L, 1) ;
        for(int i=1 ; i<=len ; i++) {
            lua_rawgeti (L, 1, i) ;
            string key = lua_tostring(L, -1) ;
            ctx->tags_.remove(key);
        }
    }
    else {
        lua_pushstring(L, "delete_tags expects array of tags") ;
        lua_error(L) ;
    }

    return 0 ;
}

static int write(lua_State* L)
{
    TagFilterContext **ctx_proxy = (TagFilterContext **) lua_touserdata(L, lua_upvalueindex(1));
    TagFilterContext *ctx = *ctx_proxy ;
    TagWriteList *tw = &ctx->tw_ ;

    int n = lua_gettop(L);

    if ( n < 3 ) {
        lua_pushstring(L, "Wrong number of arguments in function `write`") ;
        lua_error(L) ;
        return 0 ;
    }

    int is_num ;
    uint8_t zmin = lua_tointegerx(L, 1, &is_num) ;
    if ( !is_num ) {
        lua_pushstring(L, "Invalid argument zmin in function `write`. Integer expected.") ;
        lua_error(L) ;
        return 0 ;
    }

    uint8_t zmax = lua_tointegerx(L, 2, &is_num) ;
    if ( !is_num ) {
        lua_pushstring(L, "Invalid argument zmax in function `write`. Integer expected.") ;
        lua_error(L) ;
        return 0  ;
    }

    string tag = lua_tostring(L, 3) ;

    string val ;
    if ( n == 4 )
        val = lua_tostring(L, 4) ;
    else
        val = ctx->tags_.get(tag) ;

    tw->actions_.emplace_back(tag, val, zmin, zmax, false) ;

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

static int is_relation(lua_State* L)
{
    TagFilterContext **ctx_proxy = (TagFilterContext **) lua_touserdata(L, lua_upvalueindex(1));
    TagFilterContext *ctx = *ctx_proxy ;

    bool is_rel = (ctx->type_ == TagFilterContext::Relation) ;
    lua_pushboolean(L, is_rel) ;

    return 1 ;
}

bool LuaContext::loadScript(const std::string &script) {

    if ( luaL_loadstring(state_, script.c_str()) || lua_pcall(state_, 0, 0, 0) ) {
        error("error parsing Lua script: ") ;
        return false ;
    }

    lua_newtable(state_) ;

    lua_pushlightuserdata(state_, &cproxy_);
    lua_pushcclosure(state_, &attach_tags, 1);
    lua_setfield(state_, -2, "attach_tags");

    lua_pushlightuserdata(state_, &cproxy_);
    lua_pushcclosure(state_, &add_tags, 1);
    lua_setfield(state_, -2, "add_tags");

    lua_pushlightuserdata(state_, &cproxy_);
    lua_pushcclosure(state_, &set_tags, 1);
    lua_setfield(state_, -2, "set_tags");

    lua_pushlightuserdata(state_, &cproxy_);
    lua_pushcclosure(state_, &delete_tags, 1);
    lua_setfield(state_, -2, "delete_tags");

    lua_pushlightuserdata(state_, &cproxy_);
    lua_pushcclosure(state_, &write, 1);
    lua_setfield(state_, -2, "write");

    lua_pushlightuserdata(state_, &cproxy_);
    lua_pushcclosure(state_, &is_node, 1);
    lua_setfield(state_, -2, "is_node");

    lua_pushlightuserdata(state_, &cproxy_);
    lua_pushcclosure(state_, &is_way, 1);
    lua_setfield(state_, -2, "is_way");

    lua_pushlightuserdata(state_, &cproxy_);
    lua_pushcclosure(state_, &is_relation, 1);
    lua_setfield(state_, -2, "is_relation");

    lua_setglobal(state_, "osm") ;

    return true ;
}

void LuaContext::addDictionary(const Dictionary &dict)
{
    // set global variable
    lua_newtable(state_);

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

        error("Fatal error during call to function (" + fname + "): ") ;
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

    // we fill in the tags table and relations table that all live within the osm table

    lua_getglobal(state_, "osm") ;
    lua_pushstring(state_, "tags") ;
    addDictionary(ctx.tags_) ;
    lua_settable(state_, -3) ;

    using namespace tag_filter ;


    if ( ctx.type_ == TagFilterContext::Way && ctx.doc_ ) {

        OSM::Way way ;
        if ( ctx.doc_->readWay(ctx.id_, way) ) {
            lua_pushstring(state_, "relations") ;
            vector<OSM::Relation> relations ;
            ctx.doc_->readParentRelations(ctx.id_, relations) ;

            lua_newtable(state_) ;

            uint count = 1 ;

            for( OSM::Relation &rel: relations ) {
                lua_pushinteger(state_, count++) ;
                addDictionary(rel.tags_) ;
                lua_settable(state_, -3);
            }
            lua_settable(state_, -3) ;
        }
    }


    lua_pop(state_, -1) ;
}
