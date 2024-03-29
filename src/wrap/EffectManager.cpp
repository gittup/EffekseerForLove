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
	/* Call getInfo first, since wasm will crash if we call read on a
	 * non-existent file.
	 */
	lua_prep("love.filesystem.getInfo");
	lua_pushstring(L, path);
	lua_call(L, 1, 2);
	if(lua_isnil(L, -2)) {
		lua_remove(L, -2);
		return NULL;
	}
	lua_remove(L, -2);

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

class LoveFileReader : public ::Effekseer::FileReader
{
	public:
		LoveFileReader(const char *path)
		{
			mem = file_read(path, size);
		}

		size_t Read(void *buffer, size_t len)
		{
			if(pos >= size)
				return 0;
			if(len > size - pos)
				len = size - pos;
			memcpy(buffer, (char*)mem + pos, len);
			pos += len;
			return len;
		}

		void Seek(int position)
		{
			if(position < 0)
				pos = 0;
			else
				pos = position;
		}

		int GetPosition() const
		{
			return pos;
		}

		size_t GetLength() const
		{
			return size;
		}

		bool IsValid()
		{
			return mem != NULL;
		}

	private:
		void *mem;
		size_t size;
		int pos = 0;
};

class LoveFileInterface : public ::Effekseer::FileInterface
{
	public:
		::Effekseer::FileReaderRef OpenRead(const char16_t* efkpath)
		{
			char path[256];
			::Effekseer::ConvertUtf16ToUtf8(path, 256, efkpath);
			LoveFileReader *reader = new LoveFileReader(path);
			if(reader->IsValid()) {
				return ::Effekseer::RefPtr<LoveFileReader>(reader);
			}
			return NULL;
		}

		::Effekseer::FileWriterRef OpenWrite(const char16_t* efkpath)
		{
			char path[256];
			::Effekseer::ConvertUtf16ToUtf8(path, 256, efkpath);
			printf("Fail to open write: %s\n", path);
			return NULL;
		}
};

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

		/* Make sure we have an rgba8 image, or convert it to one. */
		const char *format;
		lua_getfield(L, -1, "getFormat");
		lua_pushvalue(L, -2);
		lua_call(L, 1, 1);
		format = (const char*)lua_tostring(L, -1);
		lua_pop(L, 1);
		if(strcmp(format, "rgba8") != 0) {
			/* For non-rgba8 formats, make a new rgba8 ImageData
			 * and paste the original into it.
			 */
			lua_prep("love.image.newImageData");
			lua_pushnumber(L, width);
			lua_pushnumber(L, height);
			lua_pushstring(L, "rgba8");
			lua_call(L, 3, 1);
			lua_getfield(L, -1, "paste");
			lua_pushvalue(L, -2); /* The new rgba8 ImageData */
			lua_pushvalue(L, -4); /* The original ImageData above */
			lua_pushnumber(L, 0); /* dx */
			lua_pushnumber(L, 0); /* dy */
			lua_pushnumber(L, 0); /* sx */
			lua_pushnumber(L, 0); /* sy */
			lua_pushnumber(L, width); /* sw */
			lua_pushnumber(L, height); /* sh */
			lua_call(L, 8, 0);
			lua_remove(L, -2); /* Remove the original ImageData */
		}

		const char *data;
		lua_getfield(L, -1, "getPointer");
		lua_pushvalue(L, -2);
		lua_call(L, 1, 1);
		data = (const char*)lua_topointer(L, -1);
		lua_pop(L, 1);

		::Effekseer::Backend::TextureParameter param;
		param.Format = ::Effekseer::Backend::TextureFormatType::R8G8B8A8_UNORM;
		param.MipLevelCount = 0;
		param.Size[0] = width;
		param.Size[1] = height;
		Effekseer::CustomVector<uint8_t> initialData;
		initialData.assign(data, data + width * height * 4);

		auto texture = ::Effekseer::MakeRefPtr<::Effekseer::Texture>();
		texture->SetBackend(_graphicsDevice->CreateTexture(param, initialData));

		lua_pop(L, 1); // Pop image data
		return texture;
	}

	void Unload(Effekseer::TextureRef data) override
	{
	}
};

EffectManager::EffectManager(bool warn_on_missing_textures)
{
	g_warn_on_missing_textures = warn_on_missing_textures;
	EffekseerRendererGL::OpenGLDeviceType gltype;

#ifdef EMSCRIPTEN
	gltype = EffekseerRendererGL::OpenGLDeviceType::OpenGLES2;
#else
	char *use_opengles = getenv("LOVE_GRAPHICS_USE_OPENGLES");
	if(use_opengles && strcmp(use_opengles, "1") == 0) {
		gltype = EffekseerRendererGL::OpenGLDeviceType::OpenGLES2;
	} else {
		gltype = EffekseerRendererGL::OpenGLDeviceType::OpenGL3;
	}
#endif

	renderer = ::EffekseerRendererGL::Renderer::Create(8000, gltype);

	auto fileinterface = ::Effekseer::MakeRefPtr<LoveFileInterface>();
	manager = ::Effekseer::Manager::Create(8000);

	manager->SetSpriteRenderer(renderer->CreateSpriteRenderer());
	manager->SetRibbonRenderer(renderer->CreateRibbonRenderer());
	manager->SetRingRenderer(renderer->CreateRingRenderer());
	manager->SetTrackRenderer(renderer->CreateTrackRenderer());
	manager->SetModelRenderer(renderer->CreateModelRenderer());

	manager->SetEffectLoader(::Effekseer::Effect::CreateEffectLoader(fileinterface));
	manager->SetTextureLoader(::Effekseer::MakeRefPtr<LoveTextureLoader>(renderer->GetGraphicsDevice()));
	manager->SetModelLoader(renderer->CreateModelLoader(fileinterface));
	manager->SetMaterialLoader(renderer->CreateMaterialLoader(fileinterface));
 	manager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

	/* By default we don't invert the y-axis. This can be overridden with
	 * setInvert(), which is necessary if drawing Effekseer effects to a
	 * love2d canvas. This is necessary to account for the different
	 * orthographic projects when drawing to the screen and canvas that
	 * love uses in
	 * src/modules/graphics/opengl/Graphics.cpp:setCanvasInternal().
	 *
	 * Note that converting the y-axis for the orthographic projection is
	 * done with the adjustY() function, rather than inverting the axis
	 * outright.
	 */
	invert_y = false;

	is_ortho = true;
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
	/* Effekseer's manager update is in frames, which is 60fps. */
	manager->Update(dt * 60.0);
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

	if(is_ortho) {
		::Effekseer::Matrix44 proj = ::Effekseer::Matrix44().OrthographicRH((float)windowWidth, (float)windowHeight, -512.0, 512.0);

		proj.Values[3][0] = -1;
		if(invert_y) {
			// Invert y axis
			proj.Values[1][1] = -proj.Values[1][1];

			// And move 0, 0 to top-left
			proj.Values[3][1] = 1;
		} else {
			proj.Values[3][1] = -1;
		}

		renderer->SetProjectionMatrix(proj);

		::Effekseer::Matrix44 matrix;
		renderer->SetCameraMatrix(matrix);
	} else {
		renderer->SetProjectionMatrix(::Effekseer::Matrix44().PerspectiveFovRH_OpenGL(90.0f / 180.0f * 3.14f, (float)windowWidth / (float)windowHeight, 1.0f, 500.0f));
		renderer->SetCameraMatrix(::Effekseer::Matrix44().LookAtRH(::Effekseer::Vector3D(10.0f, 5.0f, 20.0f), ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f)));
	}
}

void EffectManager::setPerspective(void)
{
	is_ortho = false;
}

void EffectManager::setOrtho(void)
{
	is_ortho = true;
}

void EffectManager::setInvert(bool invert)
{
	invert_y = invert;
}

float EffectManager::getMagnification()
{
	// Default to a 10.0 magnification in orthographic mode. This seems to
	// work pretty well for most effects on 1080p
	if(is_ortho)
		return 10.0;
	return 1.0;
}

float EffectManager::adjustY(float y)
{
	// For an orthographic projection, convert the Y coordinate to love2d's
	// space (with 0, 0 at at the topleft).
	if(is_ortho) {
		lua_prep("love.graphics");
		lua_getfield(L, -1, "getHeight");
		lua_call(L, 0, 1);
		float windowHeight = lua_tonumber(L, -1);
		lua_pop(L, 1);
		lua_pop(L, 1);
		return windowHeight - y;
	}
	return y;
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

#ifdef __EMSCRIPTEN__
	/* Effekseer normally restores the bound vertex array to the original
	 * state, which resets all enabled vertex attrib arrays. However in
	 * love.js, LOVE doesn't use glBindVertexArray internally, so there is
	 * no previous array to return to. This makes LOVE and Effekseer step
	 * on each others' toes. Instead we can query all vertex attrib arrays,
	 * and reset them to their original states.
	 */
	std::vector<bool> vertex_enabled;
	GLint num;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &num);
	for(size_t i=0; i<num; i++) {
		GLint enabled;
		::glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled);
		GLExt::glDisableVertexAttribArray(i);
		vertex_enabled.push_back(enabled);
	}
#endif

	renderer->BeginRendering();
	manager->Draw();
	renderer->EndRendering();

#ifdef __EMSCRIPTEN__
	for(size_t i=0; i<vertex_enabled.size(); i++) {
		if(vertex_enabled[i]) {
			GLExt::glEnableVertexAttribArray(i);
		} else {
			GLExt::glDisableVertexAttribArray(i);
		}
	}
#endif
}
