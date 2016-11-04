#include "lua_context.hpp"
#include "parse_context.hpp"

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


static int attach_tags(lua_State* L)
{
    OSM::Filter::Context **ctx_proxy = (OSM::Filter::Context **) lua_touserdata(L, lua_upvalueindex(1));

    OSM::Filter::Context *ctx = *ctx_proxy ;
    TagWriteList *tw = ctx->tw_ ;

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

bool LuaContext::loadScript(const std::string &script) {
    if ( luaL_loadstring(state_, script.c_str()) || lua_pcall(state_, 0, 0, 0) ) {
        error("error parsing Lua script: ") ;
        return false ;
    }

    return true ;
}

void LuaContext::addGlobalVariable(const char *name, const Dictionary &dict)
{
    // set global variable
    lua_createtable(state_, 0, dict.count());

    DictionaryIterator it(dict) ;
    while ( it ) {
        lua_pushstring(state_, it.value().c_str());
        lua_setfield(state_, -2, it.key().c_str());
        ++it ;
    }
    lua_setglobal(state_, name) ;
}

OSM::Filter::Literal LuaContext::call(const string &fname, const std::vector<OSM::Filter::Literal> &args)
{
    using namespace OSM::Filter ;

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

void LuaContext::setupContext(const OSM::Filter::Context &ctx)
{
    addGlobalVariable("_tags_", ctx.tags_) ;
}
