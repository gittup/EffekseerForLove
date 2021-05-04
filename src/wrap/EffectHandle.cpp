#include "EffectHandle.h"

#include "Effekseer.h"
#include "EffekseerRendererGL.h"

EffectHandle::EffectHandle(::Effekseer::Handle handle, EffectManager *manager)
{
	this->handle = handle;
	this->manager = manager;
}

::Effekseer::Handle EffectHandle::getHandle()
{
	return this->handle;
}

bool EffectHandle::exists()
{
	return manager->getManager()->Exists(handle);
}

void EffectHandle::draw()
{
	GLint prog;
	glGetIntegerv(GL_CURRENT_PROGRAM, &prog);

	manager->setProjection();

	::EffekseerRendererGL::Renderer *renderer = manager->getRenderer();

	renderer->BeginRendering();
	manager->getManager()->DrawHandle(handle);
	renderer->EndRendering();
	glUseProgram(prog); // TODO Effekseer changes the glUseProgram to something else, so we need to restore it, otherwise everything done in love.draw() doesn't show up.
}
