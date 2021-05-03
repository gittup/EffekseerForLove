#include "EffectManager.h"
#include "Effect.h"
#include "EffectHandle.h"

#include "Effekseer.h"
#include "EffekseerRendererGL.h"

void dump_stack(lua_State *L)
{
	int i;
	int top = lua_gettop(L);
	printf("\n#### Stack ####\n");
	for(i = top; i >= 1; i--)
	{
		int t = lua_type(L, i);
		switch(t)
		{
		case LUA_TSTRING:
			printf("%i (%i) = `%s'", i, i - (top + 1), lua_tostring(L, i));
			break;

		case LUA_TBOOLEAN:
			printf("%i (%i) = %s", i, i - (top + 1), lua_toboolean(L, i) ? "true" : "false");
			break;

		case LUA_TNUMBER:
			printf("%i (%i) = %g", i, i - (top + 1), lua_tonumber(L, i));
			break;

		default:
			printf("%i (%i) = %s", i, i - (top + 1), lua_typename(L, t));
			break;
		}
		printf("\n");
	}
	printf("#### EOS ####\n");
	printf("\n");
}

void lua_prep(const char *funcname)
{
	lua_getfield(L, LUA_GLOBALSINDEX, "package");
	lua_getfield(L, -1, "loaded");
	lua_remove(L, -2); // remove package

	const char *p = funcname;
	while(p) {
		char buf[256];
		const char *dot = strchr(p, '.');
		if(dot) {
			memcpy(buf, p, dot-p);
			buf[dot-p] = 0;
			lua_getfield(L, -1, buf);
			p = dot + 1;
		} else {
			lua_getfield(L, -1, p);
			p = NULL;
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

static ::EffekseerRendererGL::Renderer *renderer;
static ::Effekseer::Manager *manager;

class LoveTextureLoader : public ::Effekseer::TextureLoader
{
public:
	LoveTextureLoader()
	{
	}
	virtual ~LoveTextureLoader() = default;

	Effekseer::TextureData* Load(const EFK_CHAR* efkpath, ::Effekseer::TextureType textureType) override
	{
		char path[256];
		::Effekseer::ConvertUtf16ToUtf8((int8_t*)path, 256, (const int16_t*)efkpath);
		auto textureData = new Effekseer::TextureData();
		textureData->UserPtr = nullptr;
		textureData->TextureFormat = Effekseer::TextureFormatType::ABGR8;
		textureData->HasMipmap = false;

		printf("Texture load: %s\n", path);

		lua_prep("love.image.newImageData");
		lua_pushstring(L, path);
		lua_call(L, 1, 1);

		lua_getfield(L, -1, "getWidth");
		lua_pushvalue(L, -2);
		lua_call(L, 1, 1);
		textureData->Width = lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "getHeight");
		lua_pushvalue(L, -2);
		lua_call(L, 1, 1);
		textureData->Height = lua_tonumber(L, -1);
		lua_pop(L, 1);

		const void *data;
		lua_getfield(L, -1, "getPointer");
		lua_pushvalue(L, -2);
		lua_call(L, 1, 1);
		data = lua_topointer(L, -1);
		lua_pop(L, 1);

		GLuint colorFormat = GL_RGBA;
		GLuint texture = 0;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D,
			     0,
			     colorFormat,
			     textureData->Width,
			     textureData->Height,
			     0,
			     GL_RGBA,
			     GL_UNSIGNED_BYTE,
			     data);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);

		textureData->UserID = texture;

		lua_pop(L, 1); // Pop image data
		return textureData;
	}

	void Unload(Effekseer::TextureData* data) override
	{
		if (data != nullptr && data->UserPtr != nullptr)
		{
			GLuint texture = static_cast<GLuint>(data->UserID);
			glDeleteTextures(1, &texture);
		}

		if (data != nullptr)
		{
			delete data;
		}
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
		::Effekseer::ConvertUtf16ToUtf8((int8_t*)path, 256, (const int16_t*)efkpath);
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

EffectManager::EffectManager()
{
#ifdef LOVE_EMSCRIPTEN
	renderer = ::EffekseerRendererGL::Renderer::Create(8000, EffekseerRendererGL::OpenGLDeviceType::Emscripten);
#else
	renderer = ::EffekseerRendererGL::Renderer::Create(8000, EffekseerRendererGL::OpenGLDeviceType::OpenGL3);
#endif

	if(!renderer) {
		throw "Unable to create Effekseer Renderer.\n";
	}

	manager = ::Effekseer::Manager::Create(8000);

	manager->SetSpriteRenderer(renderer->CreateSpriteRenderer());
	manager->SetRibbonRenderer(renderer->CreateRibbonRenderer());
	manager->SetRingRenderer(renderer->CreateRingRenderer());
	manager->SetTrackRenderer(renderer->CreateTrackRenderer());
	manager->SetModelRenderer(renderer->CreateModelRenderer());

	manager->SetEffectLoader(new LoveEffectLoader());
	manager->SetTextureLoader(new LoveTextureLoader());
	manager->SetModelLoader(renderer->CreateModelLoader());
	manager->SetMaterialLoader(renderer->CreateMaterialLoader());

	updateCounter = 0.0f;
}

EffectHandle *EffectManager::play(Effect *effect)
{
	EffectHandle *handle = new EffectHandle(manager->Play(effect->getEffect(), ::Effekseer::Vector3D(0, 0, 0)), this);
	// Scale x/y to make effects large enough to be seen, inverting y axis
	manager->SetScale(handle->getHandle(), 10.0, -10.0, 1.0);
	return handle;
}

void EffectManager::stop(EffectHandle *handle)
{
	manager->StopEffect(handle->getHandle());
}

void EffectManager::stopAll()
{
	manager->StopAllEffects();
}

::Effekseer::Manager *EffectManager::getManager()
{
	return this->manager;
}

::EffekseerRendererGL::Renderer *EffectManager::getRenderer()
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

#if 0
void EffectManager::setProjection(Graphics *gfx)
{
	float windowWidth = (float)gfx->getWidth();
	float windowHeight = (float)gfx->getHeight();
	::Effekseer::Matrix44 proj = ::Effekseer::Matrix44().OrthographicRH((float)windowWidth, (float)windowHeight, -128.0, 128.0);

	// Invert y axis
	proj.Values[1][1] = -proj.Values[1][1];

	// And move 0, 0 to top-left
	proj.Values[3][0] = -1;
	proj.Values[3][1] = 1;

	renderer->SetProjectionMatrix(proj);
}

void EffectManager::draw(Graphics *gfx, const Matrix4 &m)
{
	GLint prog;
	glGetIntegerv(GL_CURRENT_PROGRAM, &prog);
	gfx->flushStreamDraws();

	setProjection(gfx);

	// TODO: Inverse of this?
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

	renderer->SetCameraMatrix(matrix);

	renderer->BeginRendering();
	manager->Draw();
	renderer->EndRendering();
	glUseProgram(prog); // TODO Effekseer changes the glUseProgram to something else, so we need to restore it, otherwise everything done in love.draw() doesn't show up.
}
#endif