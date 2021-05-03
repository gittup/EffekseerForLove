#include <stdio.h>
extern "C" {
#include "lua.h"
}

#include "wrap_EffectManager.h"
#include "wrap_Effect.h"
#include "wrap_EffectHandle.h"

lua_State *L;

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
	return 1;
}
