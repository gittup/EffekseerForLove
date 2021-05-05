#ifndef LOVE_GRAPHICS_EFFECT_MANAGER_H
#define LOVE_GRAPHICS_EFFECT_MANAGER_H

#include "Effekseer.h"
#include "EffekseerRendererGL.h"
#include "runtime.h"

class Effect;

class EffectManager
{
public:
	EffectManager();

	::Effekseer::Handle play(Effect *effect);

	void stop(::Effekseer::Handle handle);

	void stopAll();

	void setLocation(::Effekseer::Handle handle, float x, float y, float z);

	::Effekseer::Manager *getManager();
	::EffekseerRendererGL::Renderer *getRenderer();

	void setProjection();

	void update(float dt);

	void flushStreamDraws();

	void draw();

private:
	::EffekseerRendererGL::Renderer *renderer;
	::Effekseer::Manager *manager;

	float updateCounter;
};

#endif // LOVE_GRAPHICS_EFFECT_MANAGER_H
