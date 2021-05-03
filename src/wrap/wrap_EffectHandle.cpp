#include "wrap_EffectHandle.h"

int w_EffectHandle_exists(lua_State *L)
{
	EffectHandle *handle = *(EffectHandle**)luaL_checkudata(L, 1, "EffectHandle");
	lua_pushboolean(L, handle->exists());
	return 1;
}

int w_EffectHandle_draw(lua_State *L)
{
	EffectHandle *handle = *(EffectHandle**)luaL_checkudata(L, 1, "EffectHandle");
	handle->draw();
	return 1;
}

int w_EffectHandle_gc(lua_State *L)
{
	EffectHandle **x = (EffectHandle**)luaL_checkudata(L, 1, "EffectHandle");
	delete *x;
	*x = nullptr;
	return 0;
}

int w_EffectHandle_tostring(lua_State *L)
{
	EffectHandle *manager = *(EffectHandle**)luaL_checkudata(L, 1, "EffectHandle");
	lua_pushfstring(L, "EffectHandle: %p", manager);
	return 1;
}

const luaL_Reg w_EffectHandle_functions[] = {
	{ "exists", w_EffectHandle_exists },
	{ "draw", w_EffectHandle_draw },
};

extern "C" int luaopen_effecthandle(lua_State *L)
{
	luaL_newmetatable(L, "EffectHandle");
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, w_EffectHandle_gc);
	lua_rawset(L, -3);
	lua_pushstring(L, "__tostring");
	lua_pushcfunction(L, w_EffectHandle_tostring);
	lua_rawset(L, -3);
	lua_pushstring(L, "__index");
	lua_createtable(L, 0, 0);
	for (auto& x: w_EffectHandle_functions) {
		lua_pushstring(L, x.name);
		lua_pushcfunction(L, x.func);
		lua_rawset(L, -3);
	}
	lua_rawset(L, -3);
	lua_pop(L, 1);
	return 1;
}
