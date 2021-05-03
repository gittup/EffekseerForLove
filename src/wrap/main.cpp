#include <stdio.h>
extern "C" {
#include "lua.h"
}

#include "wrap_EffectManager.h"
lua_State *L;

extern "C" int luaopen_effekseer(lua_State *newL)
{
	L = newL;
	if(luaopen_effectmanager(L) < 0)
		return 0;
	printf("OHIA: Lua state: %p\n", L);
	return 1;
}
