#ifndef __LUA_CONTEXT_HPP__
#define __LUA_CONTEXT_HPP__

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#include <string>

#include "dictionary.hpp"
#include "osm_filter_rule.hpp"


class LuaContext {
public:
    LuaContext() ;
    ~LuaContext() ;

   bool loadScript(const std::string &script) ;
   void addGlobalVariable(const char *name, const Dictionary &dict) ;
   OSM::Filter::Literal call(const std::string &fname, const std::vector<OSM::Filter::Literal> &args ) ;
   void setupContext(const OSM::Filter::Context &ctx) ;


   std::string lastError() const { return error_str_ ; }

private:
   void error(const std::string &) ;

private:

   static int attach_tags(lua_State *) ;

   OSM::Filter::Context *cproxy_ ;
   lua_State* state_ ;
   std::string error_str_ ;
};


#endif
