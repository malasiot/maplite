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
#include "tag_filter_rule.hpp"

class TagFilterContext ;

class LuaContext {
public:
    LuaContext() ;
    ~LuaContext() ;

   bool loadScript(const std::string &script) ;

   tag_filter::Literal call(const std::string &fname, const std::vector<tag_filter::Literal> &args ) ;
   void setupContext(TagFilterContext &ctx) ;


   std::string lastError() const { return error_str_ ; }

private:
   void error(const std::string &) ;
   void addDictionary(const Dictionary &dict) ;

private:


   TagFilterContext *cproxy_ ;
   lua_State* state_ ;
   std::string error_str_ ;
};


#endif
