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
#if 0
	gfx->flushStreamDraws();

	Matrix4 t(gfx->getTransform(), m);
	int i, j;
	const float *e = t.getElements();
	::Effekseer::Matrix44 matrix;

	// Convert Love matrix to Effekseer matrix
	for(j=0; j<4; j++) {
		for(i=0; i<4; i++) {
			matrix.Values[i][j] = e[i*4 + j];
		}
	}
#endif

	::EffekseerRendererGL::Renderer *renderer = manager->getRenderer();
//	renderer->SetCameraMatrix(matrix);
	::Effekseer::Matrix44 matrix;
	renderer->SetCameraMatrix(matrix);

	renderer->BeginRendering();
	manager->getManager()->DrawHandle(handle);
	renderer->EndRendering();
	glUseProgram(prog); // TODO Effekseer changes the glUseProgram to something else, so we need to restore it, otherwise everything done in love.draw() doesn't show up.
}
