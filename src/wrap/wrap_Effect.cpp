#include "wrap_Effect.h"
#include "Effekseer.h"

int w_Effect_gc(lua_State *L)
{
	::Effekseer::Effect **x = (::Effekseer::Effect**)luaL_checkudata(L, 1, "Effect");
	(*x)->Release();
	*x = nullptr;
	return 0;
}

int w_Effect_tostring(lua_State *L)
{
	::Effekseer::Effect *manager = *(::Effekseer::Effect**)luaL_checkudata(L, 1, "Effect");
	lua_pushfstring(L, "Effect: %p", manager);
	return 1;
}

const luaL_Reg w_Effect_functions[] = {
};

extern "C" int luaopen_effect(lua_State *L)
{
	luaL_newmetatable(L, "Effect");
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, w_Effect_gc);
	lua_rawset(L, -3);
	lua_pushstring(L, "__tostring");
	lua_pushcfunction(L, w_Effect_tostring);
	lua_rawset(L, -3);
	lua_pushstring(L, "__index");
	lua_createtable(L, 0, 0);
	for (auto& x: w_Effect_functions) {
		lua_pushstring(L, x.name);
		lua_pushcfunction(L, x.func);
		lua_rawset(L, -3);
	}
	lua_rawset(L, -3);
	lua_pop(L, 1);
	return 1;
}
