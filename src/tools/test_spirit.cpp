extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#include <iostream>
#include <fstream>
#include <map>

using namespace std ;

class ContextBinding
{
public:


    std::map<std::string, std::string> tags_ ;
};

static int attach_tags(lua_State* L)
{
    ContextBinding *ctx = (ContextBinding *) lua_touserdata(L, lua_upvalueindex(1));

    cout << ctx->tags_["highway"] << endl ;
    int argCount = lua_gettop(L);

    for(int i = 1; i <= argCount; i++)
    {

        switch(lua_type(L, i))
        {
            case LUA_TSTRING:
                cout << lua_tostring(L, i) << endl ;
                break;
            case LUA_TNUMBER:
                cout << lua_tonumber(L, i) << endl ;
                break;
            case LUA_TBOOLEAN:
                cout << boolalpha << lua_toboolean(L, i) << endl ;
                break;
            case LUA_TNIL:
                cout << "#Nil#";
                break;
            default:
                cout << "#Unknown type '" << lua_type(L, i) << "'#";
                break;
        }
    }

    cout << endl;
}

int main(int argc, char *argv[]) {


   lua_State* L = luaL_newstate() ;
   luaL_openlibs(L);

    std::map<string, string> tags ;
    tags["highway"] = "secondary" ;
    tags["surface"] = "asphalt" ;


    // set global variable
    lua_createtable(L, 0, tags.size());

    for( auto &kv: tags) {
       //table[kv.first] = kv.second ;
        lua_pushstring(L, kv.second.c_str());
        lua_setfield(L, -2, kv.first.c_str());  /* 3rd element from the stack top */
    }
    lua_setglobal(L, "_tags_") ;

    ContextBinding ctx1 ;

    ContextBinding *ctx = &ctx1 ;
    lua_pushlightuserdata(L, ctx);
    lua_pushcclosure(L, &attach_tags, 1);
    lua_setglobal(L, "attach") ;


    if (luaL_loadfile(L, "/home/malasiot/tmp/config.lua") )
        cerr << "cannot run configuration file: " << lua_tostring(L, -1) << endl ;

    lua_pcall(L, 0, 0, 0) ;

    /* push functions and arguments */
         lua_getglobal(L, "test");  /* function to be called */

         /* do the call (2 arguments, 1 result) */
         if (lua_pcall(L, 0, 1, 0) != 0)
                 cerr << "cannot call: " << lua_tostring(L, -1) << endl ;

    /* retrieve result */
         if (!lua_isstring(L, -1))
           cerr << "function `f' must return a number" << endl ;
         string z = lua_tostring(L, -1);
         lua_pop(L, 1);  /* pop returned value */


         ctx1.tags_["highway"] = "test" ;


    lua_getglobal(L, "test");  /* function to be called */

    if (lua_pcall(L, 0, 0, 0) != 0)

            cerr << "cannot call: " << lua_tostring(L, -1) << endl ;
}
