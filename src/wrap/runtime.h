extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#define LUA_TRYWRAP(expr) {try { expr } catch(std::exception &x) { lua_settop(L, 0); luaL_error(L, x.what()); }}

extern lua_State *L;
