#include "Effect.h"
#include "EffectManager.h"

#include "Effekseer.h"
#include "EffekseerRendererGL.h"

Effect::Effect(EffectManager *manager, std::string &filename)
{
	EFK_CHAR filename16[256];
	::Effekseer::ConvertUtf8ToUtf16(filename16, 256, filename.c_str());
	effect = Effekseer::Effect::Create(manager->getManager(), filename16);
}

::Effekseer::Effect *Effect::getEffect()
{
	return this->effect;
}
