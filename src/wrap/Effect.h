#ifndef LOVE_GRAPHICS_EFFECT_H
#define LOVE_GRAPHICS_EFFECT_H

#include "Effekseer.h"

class EffectManager;

class Effect
{
public:
	Effect(EffectManager *manager, std::string &filename);

	::Effekseer::Effect *getEffect();

private:
	::Effekseer::Effect *effect;
};

#endif // LOVE_GRAPHICS_EFFECT_H
