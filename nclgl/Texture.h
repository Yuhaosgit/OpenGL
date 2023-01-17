#pragma once
#include <memory>
#include <cmath>
#include <glad/glad.h>
#include <vector>
#include "Shader.h"

enum class WrapMode {
	REPEAT = GL_REPEAT, CLAMP_EDGE = GL_CLAMP_TO_EDGE, CLAMP_BORDER = GL_CLAMP_TO_BORDER
};

enum class FilterMode {
	NEAREST = GL_NEAREST, LINEAR = GL_LINEAR, NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
	LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST, NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
	LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
};

enum class Dimension {
	Texture2D = GL_TEXTURE_2D, Cubemap = GL_TEXTURE_CUBE_MAP
};

template <typename T>
GLuint EnumToGLuint(T enumVal) {
	GLuint val = *reinterpret_cast<GLuint*>(&enumVal);
	return val;
}

class Texture {
public:
	static std::vector<Texture*> activeTextures;

	Texture(int in_width, int in_height) :width(in_width), height(in_height) {
		maxMipLevel = width > 1 ? std::log2(in_width) : 0;
		glGenTextures(1, &texture);
	}
	virtual ~Texture() {}
	void Delete() { glDeleteTextures(1, &texture); }

	virtual void BindTexture() = 0;
	virtual void UnbindTexture() = 0;

	void Submit(Shader* targetShader, const std::string& variableName) {
		glActiveTexture(GL_TEXTURE0 + activeCount);
		BindTexture();
		slot = activeCount;
		activeTextures.emplace_back(this);

		glUniform1i(glGetUniformLocation(targetShader->GetProgram(), variableName.c_str()), slot);
		activeCount++;
	}

	void DetachFromGPU() {
		if (activeCount < 0 || slot < 0)
			return;
		glActiveTexture(GL_TEXTURE0 + slot);
		UnbindTexture();
		activeCount--;
		slot = -1;
	}

	void GenerateMipmap() {
		glGenerateTextureMipmap(texture);
	}

	void SetMinFilter(FilterMode in_filter) {
		GLuint filter = EnumToGLuint(in_filter);
		glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, filter);
	}

	void SetMagFilter(FilterMode in_filter) {
		if (in_filter != FilterMode::NEAREST && in_filter != FilterMode::LINEAR)
			return;
		GLuint filter = EnumToGLuint(in_filter);
		glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, filter);
	}

	void SetWrapMode(WrapMode in_wrapMode) {
		GLuint wrap = EnumToGLuint(in_wrapMode);
		glTextureParameteri(texture, GL_TEXTURE_WRAP_S, wrap);
		glTextureParameteri(texture, GL_TEXTURE_WRAP_T, wrap);

		if (dimension == Dimension::Cubemap)
			glTextureParameteri(texture, GL_TEXTURE_WRAP_R, wrap);
	}

	int GetWidth() { return width; }
	int GetHeight() { return height; }

	GLuint GetContent() { return texture; }
	Dimension GetDimension() { return dimension; }
protected:
	GLuint texture = 0;
	Dimension dimension;
	int width = 0;
	int height = 0;
	int maxMipLevel = 0;

	int slot = -1;
	static int activeCount;

	void InitSetting() {
		glTextureParameteri(texture, GL_TEXTURE_BASE_LEVEL, 0);
		glTextureParameteri(texture, GL_TEXTURE_MAX_LEVEL, maxMipLevel);
		glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		SetWrapMode(WrapMode::REPEAT);
	}
};

class Texture2D : public Texture {
public:
	Texture2D(int in_width, int in_height) :Texture(in_width, in_height) {
		dimension = Dimension::Texture2D;
	}
	~Texture2D() {}

	virtual void BindTexture() override {
		glBindTexture(GL_TEXTURE_2D, texture);
	}

	virtual void UnbindTexture() override {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void SetPixelData(void* data, GLsizei dataSize, int mipmapLevel, GLenum format) {
		BindTexture();
		int mapWidth = width * std::pow(0.5, mipmapLevel);
		int mapHeight = height * std::pow(0.5, mipmapLevel);
		glCompressedTexImage2D(EnumToGLuint(dimension), mipmapLevel, format, mapWidth, mapHeight, 0, dataSize, data);
		UnbindTexture();

		InitSetting();
	}

	void SetPixelData(void* data, int mipmapLevel, GLuint interFormat, GLenum format, GLenum type) {
		BindTexture();
		int mapWidth = width * std::pow(0.5, mipmapLevel);
		int mapHeight = height * std::pow(0.5, mipmapLevel);
		glTexImage2D(EnumToGLuint(dimension), mipmapLevel, interFormat, mapWidth, mapHeight, 0, format, type, data);
		UnbindTexture();

		InitSetting();
	}
};

class TextureCube : public Texture {
public:
	TextureCube(int in_width, int in_height) :Texture(in_width, in_height) {
		dimension = Dimension::Cubemap;
	}

	~TextureCube() {}

	virtual void BindTexture() override {
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	}

	virtual void UnbindTexture() override {
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void SetPixelData(GLuint axis, void* data, GLsizei dataSize, int mipmapLevel, GLenum format) {
		BindTexture();
		int mapWidth = width * std::pow(0.5, mipmapLevel);
		int mapHeight = height * std::pow(0.5, mipmapLevel);
		glCompressedTexImage2D(axis, mipmapLevel, format, mapWidth, mapHeight, 0, dataSize, data);
		UnbindTexture();

		InitSetting();
	}

	void SetPixelData(GLuint axis, void* data, int mipmapLevel, GLuint interFormat, GLenum format, GLenum type) {
		BindTexture();
		int mapWidth = width * std::pow(0.5, mipmapLevel);
		int mapHeight = height * std::pow(0.5, mipmapLevel);
		glTexImage2D(axis, mipmapLevel, interFormat, mapWidth, mapHeight, 0, format, type, data);
		UnbindTexture();

		InitSetting();
	}

	void SetSize(int in_width, int in_height) {
		width = in_width;
		height = in_height;
	}
};

#pragma region RenderTexture
enum class RenderTextureFormat {
	DEPTH, STENCIL_DEPTH,
	RGBA8 = GL_RGBA8, RGB8 = GL_RGB8, RG8 = GL_RG8, R8 = GL_R8,
	RGBA16F = GL_RGBA16F, RGB16F = GL_RGB16F, RG16F = GL_RG16F, R16F = GL_R16F,
	RGBA32F = GL_RGBA32F, RGB32F = GL_RGB32F, RG32F = GL_RG32F, R32F = GL_R32F,
};

enum class IOstate {
	Write, Read
};

class RenderTexture : public Texture {
	friend class FrameBuffer;
public:
	static GLenum InterformatToFormat(GLuint format);

	RenderTexture(int in_width, int in_height, Dimension in_dimension, RenderTextureFormat in_format)
		:Texture(in_width, in_height), format(in_format) {
		dimension = in_dimension;

		BindTexture();
		auto SetRenderTexture = [&](GLenum target) {
			if (format == RenderTextureFormat::DEPTH) {
				glTexImage2D(target, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			}
			else if (format == RenderTextureFormat::STENCIL_DEPTH) {
				glTexImage2D(target, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
			}
			else {
				auto interFormat = EnumToGLuint(format);
				auto colorFormat = InterformatToFormat(interFormat);
				glTexImage2D(target, 0, interFormat, width, height, 0, colorFormat, GL_UNSIGNED_BYTE, NULL);
			}
		};

		if (dimension == Dimension::Texture2D) {
			SetRenderTexture(EnumToGLuint(dimension));
		}
		else if (dimension == Dimension::Cubemap) {
			for (int i = 0; i < 6; ++i) {
				SetRenderTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
			}
		}
		UnbindTexture();

		SetMagFilter(FilterMode::NEAREST);
		SetMinFilter(FilterMode::NEAREST);
		SetWrapMode(WrapMode::CLAMP_EDGE);
	}

	RenderTextureFormat GetFormat(){
		return format;
	}

	virtual ~RenderTexture() {
		frameBuffer = nullptr;
	}
	
	void SetFrameBufferMipmapAndFace(GLenum target, int miplevel, IOstate io);

	virtual void BindTexture() override {
		glBindTexture(EnumToGLuint(dimension), texture);
	}

	virtual void UnbindTexture() override {
		glBindTexture(EnumToGLuint(dimension), 0);
	}
private:
	RenderTextureFormat format;

	FrameBuffer* frameBuffer;
	GLenum Attachment = 0;
};
#pragma endregion
