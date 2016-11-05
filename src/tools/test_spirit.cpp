extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "luapp/lua.hpp"
#include <iostream>
#include <fstream>
#include <map>

using namespace std ;

using namespace lua;

Retval makeSequence(Context& c){   // create a sequence of natural numbers
    const unsigned int amount = c.args.at(0);  // First argument of function is sequence size
    Valset rv(c);
    for(auto i = 1u; i <= amount; ++i)
        rv.push_back(i);
    return c.ret(rv);
}


int main(int argc, char *argv[]) {

    lua::State state ;

    state.runFile("/home/malasiot/tmp/config.lua") ;


    std::map<string, string> tags ;
    tags["highway"] = "secondary" ;
    tags["surface"] = "asphalt" ;
}
