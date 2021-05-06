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
