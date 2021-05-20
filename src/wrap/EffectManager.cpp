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

#include "EffectManager.h"

#include "Effekseer.h"
#include "EffekseerRendererGL.h"
#include "EffekseerRenderer/EffekseerRendererGL.GLExtension.h"

using namespace EffekseerRendererGL;

void lua_prep(const char *funcname)
{
	lua_getfield(L, LUA_GLOBALSINDEX, "package");
	lua_getfield(L, -1, "loaded");
	lua_remove(L, -2); // remove package

	char buf[256] = {0};
	const char *p = funcname;
	while(p) {
		const char *dot = strchr(p, '.');
		if(dot) {
			memcpy(buf, p, dot-p);
			buf[dot-p] = 0;
			p = dot + 1;
		} else {
			strcpy(buf, p);
			p = NULL;
		}
		lua_getfield(L, -1, buf);
		if(lua_isnil(L, -1)) {
			luaL_error(L, "Unable to find field '%s' for function: %s\n", buf, funcname);
		}
		lua_remove(L, -2); // remove previous field
	}
}

void *file_read(const char *path, size_t &size)
{
	lua_prep("love.filesystem.read");
	lua_pushstring(L, path);
	lua_call(L, 1, 2);

	if (lua_isnil(L, -2)) {
		lua_remove(L, -2);
		lua_error(L);
	}

	size = lua_tonumber(L, -1);
	char *data = new char[size];
	const char *ptr = lua_tostring(L, -2);
	memcpy(data, ptr, size);
	lua_pop(L, 2);
	return data;
}

static bool g_warn_on_missing_textures = false;

class LoveTextureLoader : public ::Effekseer::TextureLoader
{
public:
	::Effekseer::Backend::GraphicsDeviceRef _graphicsDevice;
	LoveTextureLoader(::Effekseer::Backend::GraphicsDeviceRef graphicsDevice)
		: _graphicsDevice(graphicsDevice)
	{
	}
	virtual ~LoveTextureLoader() = default;

	Effekseer::TextureRef Load(const char16_t* efkpath, ::Effekseer::TextureType textureType) override
	{
		char path[256];
		::Effekseer::ConvertUtf16ToUtf8(path, 256, efkpath);
		int width;
		int height;

		lua_prep("love.image.newImageData");
		lua_pushstring(L, path);
		int ret = lua_pcall(L, 1, 1, 0);
		if(ret != 0) {
			if(g_warn_on_missing_textures) {
				fprintf(stderr, "Missing Effekseer texture: %s\n", path);
				return NULL;
			}
			luaL_error(L, "Missing Effekseer texture: %s\n", path);
		}

		lua_getfield(L, -1, "getWidth");
		lua_pushvalue(L, -2);
		lua_call(L, 1, 1);
		width = lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "getHeight");
		lua_pushvalue(L, -2);
		lua_call(L, 1, 1);
		height = lua_tonumber(L, -1);
		lua_pop(L, 1);

		const char *data;
		lua_getfield(L, -1, "getPointer");
		lua_pushvalue(L, -2);
		lua_call(L, 1, 1);
		data = (const char*)lua_topointer(L, -1);
		lua_pop(L, 1);

		::Effekseer::Backend::TextureParameter param;
		param.Format = ::Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
		param.GenerateMipmap = true;
		param.Size[0] = width;
		param.Size[1] = height;
		param.InitialData.assign(data, data + width * height * 4);

		auto texture = ::Effekseer::MakeRefPtr<::Effekseer::Texture>();
		texture->SetBackend(_graphicsDevice->CreateTexture(param));

		lua_pop(L, 1); // Pop image data
		return texture;
	}

	void Unload(Effekseer::TextureRef data) override
	{
	}
};

class LoveEffectLoader : public ::Effekseer::EffectLoader
{
public:
	LoveEffectLoader()
	{
	}

	~LoveEffectLoader()
	{
	}

	bool Load(const EFK_CHAR* efkpath, void*& data, int32_t& size)
	{
		data = NULL;
		size = 0;

		char path[256];
		::Effekseer::ConvertUtf16ToUtf8(path, 256, efkpath);
		size_t fdsize = 0;
		data = file_read(path, fdsize);
		size = fdsize;
		return true;
	}

	void Unload(void* data, int32_t size)
	{
		uint8_t* data8 = (uint8_t*)data;
		ES_SAFE_DELETE_ARRAY(data8);
	}
};

class LoveModelLoader : public ::Effekseer::ModelLoader
{
public:
	LoveModelLoader()
	{
	}

	~LoveModelLoader()
	{
	}

	::Effekseer::ModelRef Load(const EFK_CHAR* efkpath)
	{
		void *data = NULL;
		int32_t size = 0;

		char path[256];
		::Effekseer::ConvertUtf16ToUtf8(path, 256, efkpath);
		size_t fdsize = 0;
		data = file_read(path, fdsize);
		size = fdsize;
		auto model = ::Effekseer::MakeRefPtr<::Effekseer::Model>((const uint8_t*)data, size);
		return model;
	}

	void Unload(::Effekseer::ModelRef data)
	{
	}
};

EffectManager::EffectManager(bool warn_on_missing_textures)
{
	g_warn_on_missing_textures = warn_on_missing_textures;
#ifdef EMSCRIPTEN
	renderer = ::EffekseerRendererGL::Renderer::Create(8000, EffekseerRendererGL::OpenGLDeviceType::OpenGLES2);
#else
	renderer = ::EffekseerRendererGL::Renderer::Create(8000, EffekseerRendererGL::OpenGLDeviceType::OpenGL3);
#endif

	manager = ::Effekseer::Manager::Create(8000);

	manager->SetSpriteRenderer(renderer->CreateSpriteRenderer());
	manager->SetRibbonRenderer(renderer->CreateRibbonRenderer());
	manager->SetRingRenderer(renderer->CreateRingRenderer());
	manager->SetTrackRenderer(renderer->CreateTrackRenderer());
	manager->SetModelRenderer(renderer->CreateModelRenderer());

	manager->SetEffectLoader(::Effekseer::EffectLoaderRef(new LoveEffectLoader()));
	manager->SetTextureLoader(::Effekseer::TextureLoaderRef(new LoveTextureLoader(renderer->GetGraphicsDevice())));
	manager->SetModelLoader(::Effekseer::ModelLoaderRef(new LoveModelLoader()));
	manager->SetMaterialLoader(renderer->CreateMaterialLoader());
 	manager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

	updateCounter = 0.0f;
}

::Effekseer::ManagerRef EffectManager::getManager()
{
	return this->manager;
}

::EffekseerRendererGL::RendererRef EffectManager::getRenderer()
{
	return this->renderer;
}

void EffectManager::update(float dt)
{
	/* Effekseer's manager update is in frames, which is 60fps. Only call
	 * Update once our internal counter is larger than one frame.
	 */
	updateCounter += dt * 60.0;
	if(updateCounter >= 1.0f) {
		manager->Update(updateCounter);
		updateCounter = 0.0f;
	}
}

void EffectManager::setProjection()
{
	flushStreamDraws();
	lua_prep("love.graphics");

	lua_getfield(L, -1, "getWidth");
	lua_call(L, 0, 1);
	float windowWidth = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_getfield(L, -1, "getHeight");
	lua_call(L, 0, 1);
	float windowHeight = lua_tonumber(L, -1);
	lua_pop(L, 1);

	lua_pop(L, 1);

	::Effekseer::Matrix44 proj = ::Effekseer::Matrix44().OrthographicRH((float)windowWidth, (float)windowHeight, -512.0, 512.0);

	// Invert y axis
	proj.Values[1][1] = -proj.Values[1][1];

	// And move 0, 0 to top-left
	proj.Values[3][0] = -1;
	proj.Values[3][1] = 1;

	renderer->SetProjectionMatrix(proj);

	::Effekseer::Matrix44 matrix;
	renderer->SetCameraMatrix(matrix);
}

void EffectManager::flushStreamDraws()
{
	/* Calling setColorMask is a pretty simple way to get LOVE to call
	 * flushStreamDraws(), which is necessary to get the effects shown in
	 * the right order. Otherwise something like this will draw the effects
	 * behind both images rather than on top of the first one.
	 *
	 * love.graphics.draw(someimg1)
	 * manager:draw()
	 * love.graphics.draw(someimg2)
	 */
	lua_prep("love.graphics.setColorMask");
	lua_pushboolean(L, true);
	lua_pushboolean(L, true);
	lua_pushboolean(L, true);
	lua_pushboolean(L, true);
	lua_call(L, 4, 0);
}

void EffectManager::draw()
{
	setProjection();

	GLint prog;
	glGetIntegerv(GL_CURRENT_PROGRAM, &prog);

	std::vector<bool> vertex_enabled;
	GLint num;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &num);
	for(size_t i=0; i<num; i++) {
		GLint enabled;
		GLExt::glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
		GLExt::glDisableVertexAttribArray(i);
		vertex_enabled.push_back(enabled);
	}

	renderer->BeginRendering();
	manager->Draw();
	renderer->EndRendering();

	GLExt::glUseProgram(prog); // TODO Effekseer changes the glUseProgram to something else, so we need to restore it, otherwise everything done in love.draw() doesn't show up.
	for(size_t i=0; i<vertex_enabled.size(); i++) {
		if(vertex_enabled[i]) {
			GLExt::glEnableVertexAttribArray(i);
		} else {
			GLExt::glDisableVertexAttribArray(i);
		}
	}
}
