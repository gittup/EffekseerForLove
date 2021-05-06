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

#include <stdio.h>
extern "C" {
#include "lua.h"
}

#include "wrap_EffectManager.h"
#include "wrap_Effect.h"

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

extern "C" int luaopen_effekseer(lua_State *newL)
{
	L = newL;
	if(luaopen_effectmanager(L) < 0) {
		lua_error(L);
	}
	if(luaopen_effect(L) < 0) {
		lua_error(L);
	}

	lua_createtable(L, 0, 0);
	lua_pushstring(L, "newEffectManager");
	lua_pushcfunction(L, w_newEffectManager);
	lua_rawset(L, -3);
	return 1;
}
