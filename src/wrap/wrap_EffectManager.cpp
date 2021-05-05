#include "EffectManager.h"
#include "Effect.h"
#include "runtime.h"

int w_EffectManager_gc(lua_State *L)
{
	EffectManager **x = (EffectManager**)luaL_checkudata(L, 1, "EffectManager");
	delete *x;
	*x = nullptr;
	return 0;
}

int w_EffectManager_tostring(lua_State *L)
{
	EffectManager *manager = *(EffectManager**)luaL_checkudata(L, 1, "EffectManager");
	lua_pushfstring(L, "EffectManager: %p", manager);
	return 1;
}

int w_EffectManager_play(lua_State *L)
{
	EffectManager *manager = *(EffectManager**)luaL_checkudata(L, 1, "EffectManager");
	Effect *effect = *(Effect**)luaL_checkudata(L, 2, "Effect");
	::Effekseer::Handle handle = 0;
	LUA_TRYWRAP(handle = manager->play(effect););
	lua_pushinteger(L, handle);
	return 1;
}

int w_EffectManager_stop(lua_State *L)
{
	EffectManager *manager = *(EffectManager**)luaL_checkudata(L, 1, "EffectManager");
	::Effekseer::Handle handle = luaL_checkinteger(L, 2);
	LUA_TRYWRAP(manager->stop(handle););
	return 0;
}

int w_EffectManager_stopAll(lua_State *L)
{
	EffectManager *manager = *(EffectManager**)luaL_checkudata(L, 1, "EffectManager");
	LUA_TRYWRAP(manager->stopAll(););
	return 0;
}

int w_EffectManager_exists(lua_State *L)
{
	EffectManager *manager = *(EffectManager**)luaL_checkudata(L, 1, "EffectManager");
	::Effekseer::Handle handle = luaL_checkinteger(L, 2);
	bool exists = false;
	LUA_TRYWRAP(exists = manager->getManager()->Exists(handle););
	lua_pushboolean(L, exists);
	return 1;
}

int w_EffectManager_setLocation(lua_State *L)
{
	EffectManager *manager = *(EffectManager**)luaL_checkudata(L, 1, "EffectManager");
	::Effekseer::Handle handle = luaL_checkinteger(L, 2);
	float x = luaL_checknumber(L, 3);
	float y = luaL_checknumber(L, 4);
	float z = luaL_checknumber(L, 5);
	LUA_TRYWRAP(manager->setLocation(handle, x, y, z););
	return 0;
}

int w_EffectManager_update(lua_State *L)
{
	EffectManager *manager = *(EffectManager**)luaL_checkudata(L, 1, "EffectManager");
	float dt = luaL_checknumber(L, 2);
	LUA_TRYWRAP(manager->update(dt););
	return 0;
}

int w_EffectManager_draw(lua_State *L)
{
	EffectManager *manager = *(EffectManager**)luaL_checkudata(L, 1, "EffectManager");
	LUA_TRYWRAP(manager->draw(););
	return 0;
}

const luaL_Reg w_EffectManager_functions[] = {
	{ "play", w_EffectManager_play },
	{ "stop", w_EffectManager_stop },
	{ "stopAll", w_EffectManager_stopAll },
	{ "exists", w_EffectManager_exists },
	{ "setLocation", w_EffectManager_setLocation },
	{ "update", w_EffectManager_update },
	{ "draw", w_EffectManager_draw },
};

extern "C" int luaopen_effectmanager(lua_State *L)
{
	luaL_newmetatable(L, "EffectManager");
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, w_EffectManager_gc);
	lua_rawset(L, -3);
	lua_pushstring(L, "__tostring");
	lua_pushcfunction(L, w_EffectManager_tostring);
	lua_rawset(L, -3);
	lua_pushstring(L, "__index");
	lua_createtable(L, 0, 0);
	for (auto& x: w_EffectManager_functions) {
		lua_pushstring(L, x.name);
		lua_pushcfunction(L, x.func);
		lua_rawset(L, -3);
	}
	lua_rawset(L, -3);
	lua_pop(L, 1);
	return 1;
}
