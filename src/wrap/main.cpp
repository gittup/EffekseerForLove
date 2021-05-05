#include <stdio.h>
extern "C" {
#include "lua.h"
}

#include "wrap_EffectManager.h"
#include "wrap_Effect.h"
#include "wrap_EffectHandle.h"
#include <string>

lua_State *L;

int w_newEffectManager(lua_State *L)
{
	EffectManager *manager = nullptr;
	LUA_TRYWRAP(manager = new EffectManager(););
	EffectManager **dat = (EffectManager**)lua_newuserdata(L, sizeof(EffectManager*));
	*dat = manager;
	luaL_getmetatable(L, "EffectManager");
	lua_setmetatable(L, -2);
	return 1;
}

int w_newEffect(lua_State *L)
{
	EffectManager *manager = *(EffectManager**)luaL_checkudata(L, 1, "EffectManager");
	Effect *effect = nullptr;
	std::string filename = luaL_checkstring(L, 2);
	LUA_TRYWRAP(effect = new Effect(manager, filename););
	Effect **dat = (Effect**)lua_newuserdata(L, sizeof(Effect*));
	*dat = effect;
	luaL_getmetatable(L, "Effect");
	lua_setmetatable(L, -2);
	return 1;
}

extern "C" int luaopen_effekseer(lua_State *newL)
{
	L = newL;
	if(luaopen_effectmanager(L) < 0)
		return 0;
	if(luaopen_effect(L) < 0)
		return 0;
	if(luaopen_effecthandle(L) < 0)
		return 0;
	printf("OHIA: Lua state: %p\n", L);

	lua_createtable(L, 0, 0);
	lua_pushstring(L, "newEffectManager");
	lua_pushcfunction(L, w_newEffectManager);
	lua_rawset(L, -3);
	lua_pushstring(L, "newEffect");
	lua_pushcfunction(L, w_newEffect);
	lua_rawset(L, -3);
	return 1;
}
