/**
 * Copyright (c) 2021 Mike Shal
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/

#include "EffectManager.h"
#include "runtime.h"
#include <string>

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

int w_EffectManager_newEffect(lua_State *L)
{
	EffectManager *manager = *(EffectManager**)luaL_checkudata(L, 1, "EffectManager");
	::Effekseer::EffectRef effect = nullptr;
	std::string filename = luaL_checkstring(L, 2);

	// Default to a 10.0 magnification. This seems to work pretty well for
	// most effects on 1080p
	float magnification = luaL_optnumber(L, 3, 10.0);

	EFK_CHAR filename16[256];
	::Effekseer::ConvertUtf8ToUtf16(filename16, 256, filename.c_str());
	LUA_TRYWRAP(effect = Effekseer::Effect::Create(manager->getManager(), filename16, magnification););
	if(effect == NULL) {
		luaL_error(L, "Failed to load effect (make sure it is a valid .efk file): %s", filename.c_str());
	}
	void *mem = lua_newuserdata(L, sizeof(::Effekseer::EffectRef));
	::Effekseer::EffectRef *dat = new(mem) ::Effekseer::EffectRef();
	*dat = effect;
	luaL_getmetatable(L, "Effect");
	lua_setmetatable(L, -2);
	return 1;
}

int w_EffectManager_play(lua_State *L)
{
	EffectManager *manager = *(EffectManager**)luaL_checkudata(L, 1, "EffectManager");
	::Effekseer::EffectRef effect = *(::Effekseer::EffectRef*)luaL_checkudata(L, 2, "Effect");
	float x = luaL_optnumber(L, 3, 0.0);
	float y = luaL_optnumber(L, 4, 0.0);
	float z = luaL_optnumber(L, 5, 0.0);
	::Effekseer::Handle handle = 0;
	LUA_TRYWRAP(handle = manager->getManager()->Play(effect, ::Effekseer::Vector3D(x, y, z)););
	// Invert y axis so coordinates match love's
	manager->getManager()->SetScale(handle, 1.0, -1.0, 1.0);
	lua_pushinteger(L, handle);
	return 1;
}

int w_EffectManager_stop(lua_State *L)
{
	EffectManager *manager = *(EffectManager**)luaL_checkudata(L, 1, "EffectManager");
	::Effekseer::Handle handle = luaL_checkinteger(L, 2);
	LUA_TRYWRAP(manager->getManager()->StopEffect(handle););
	return 0;
}

int w_EffectManager_stopRoot(lua_State *L)
{
	EffectManager *manager = *(EffectManager**)luaL_checkudata(L, 1, "EffectManager");
	::Effekseer::Handle handle = luaL_checkinteger(L, 2);
	LUA_TRYWRAP(manager->getManager()->StopRoot(handle););
	return 0;
}

int w_EffectManager_stopAll(lua_State *L)
{
	EffectManager *manager = *(EffectManager**)luaL_checkudata(L, 1, "EffectManager");
	LUA_TRYWRAP(manager->getManager()->StopAllEffects(););
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
	float z = luaL_optnumber(L, 5, 0.0);
	LUA_TRYWRAP(manager->getManager()->SetLocation(handle, x, y, z););
	return 0;
}

int w_EffectManager_setTargetLocation(lua_State *L)
{
	EffectManager *manager = *(EffectManager**)luaL_checkudata(L, 1, "EffectManager");
	::Effekseer::Handle handle = luaL_checkinteger(L, 2);
	float x = luaL_checknumber(L, 3);
	float y = luaL_checknumber(L, 4);
	float z = luaL_optnumber(L, 5, 0.0);
	LUA_TRYWRAP(manager->getManager()->SetTargetLocation(handle, x, y, z););
	return 0;
}

int w_EffectManager_setSpeed(lua_State *L)
{
	EffectManager *manager = *(EffectManager**)luaL_checkudata(L, 1, "EffectManager");
	::Effekseer::Handle handle = luaL_checkinteger(L, 2);
	float x = luaL_checknumber(L, 3);
	LUA_TRYWRAP(manager->getManager()->SetSpeed(handle, x););
	return 0;
}

int w_EffectManager_setScale(lua_State *L)
{
	EffectManager *manager = *(EffectManager**)luaL_checkudata(L, 1, "EffectManager");
	::Effekseer::Handle handle = luaL_checkinteger(L, 2);
	float x = luaL_checknumber(L, 3);
	float y = luaL_optnumber(L, 4, x);
	float z = luaL_optnumber(L, 5, x);
	// Invert y axis so coordinates match love's
	LUA_TRYWRAP(manager->getManager()->SetScale(handle, x, -y, z););
	return 0;
}

int w_EffectManager_setRotation(lua_State *L)
{
	EffectManager *manager = *(EffectManager**)luaL_checkudata(L, 1, "EffectManager");
	::Effekseer::Handle handle = luaL_checkinteger(L, 2);
	float x = luaL_checknumber(L, 3);
	float y = luaL_checknumber(L, 4);
	float z = luaL_checknumber(L, 5);
	LUA_TRYWRAP(manager->getManager()->SetRotation(handle, x, y, z););
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
	{ "newEffect", w_EffectManager_newEffect },
	{ "play", w_EffectManager_play },
	{ "stop", w_EffectManager_stop },
	{ "stopRoot", w_EffectManager_stopRoot },
	{ "stopAll", w_EffectManager_stopAll },
	{ "exists", w_EffectManager_exists },
	{ "setLocation", w_EffectManager_setLocation },
	{ "setTargetLocation", w_EffectManager_setTargetLocation },
	{ "setSpeed", w_EffectManager_setSpeed },
	{ "setScale", w_EffectManager_setScale },
	{ "setRotation", w_EffectManager_setRotation },
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
