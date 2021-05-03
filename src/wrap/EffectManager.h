#ifndef LOVE_GRAPHICS_EFFECT_MANAGER_H
#define LOVE_GRAPHICS_EFFECT_MANAGER_H

#include "Effekseer.h"
#include "EffekseerRendererGL.h"
#include "runtime.h"

class Effect;
class EffectHandle;

class EffectManager
{
public:
	EffectManager();

	EffectHandle *play(Effect *effect);

	void stop(EffectHandle *handle);

	void stopAll();

	::Effekseer::Manager *getManager();
	::EffekseerRendererGL::Renderer *getRenderer();

	void setProjection();

	void update(float dt);

	void draw();

private:
	::EffekseerRendererGL::Renderer *renderer;
	::Effekseer::Manager *manager;

	float updateCounter;
};

#endif // LOVE_GRAPHICS_EFFECT_MANAGER_H
