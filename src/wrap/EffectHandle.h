#ifndef LOVE_GRAPHICS_EFFECT_HANDLE_H
#define LOVE_GRAPHICS_EFFECT_HANDLE_H

#include "Effekseer.h"
#include "EffectManager.h"

class EffectHandle
{
public:
	EffectHandle(::Effekseer::Handle handle, EffectManager *manager);

	::Effekseer::Handle getHandle();

	bool exists();

//	void draw(Graphics *gfx, const Matrix4 &m);
private:
	::Effekseer::Handle handle;

	EffectManager *manager;
};

#endif // LOVE_GRAPHICS_EFFECT_HANDLE_H
