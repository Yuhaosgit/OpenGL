#pragma once
#include <memory>
#include <cmath>
#include "../nclgl/OGLRenderer.h"

typedef void (*GenerateFunc) (int, int, GLuint*, GLenum);

class Texture {
public:
	Texture(int in_width, int in_height) :width(in_width), height(in_height) {
		maxMipLevel = std::log2(in_width);

		glGenTextures(1, &texture);
		BindTexture();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxMipLevel);
		UnbindTexture();
	}
	~Texture() {}

	void Delete() { glDeleteTextures(1, &texture); }
	virtual void BindTexture() = 0;
	virtual void UnbindTexture() = 0;

	static std::vector<Texture*> activeTextures;

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

	int GetWidht() { return width; }
	int GetHeight() { return height; }
protected:
	GLuint texture = 0;
	int width = 0;
	int height = 0;
	int maxMipLevel = 0;

	int slot = -1;
	static int activeCount;
};

class Texture2D: public Texture {
public:
	Texture2D(int in_width, int in_height) :Texture(in_width, in_height) {}
	~Texture2D() {}

	void BindTexture() override {
		glBindTexture(GL_TEXTURE_2D, texture);
	}
	void UnbindTexture() override {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void SetPixelData(void* data, GLsizei dataSize,int mipmapLevel, GLenum format) {
		BindTexture();
		int mapWidth = width * std::pow(0.5, mipmapLevel);
		int mapHeight = height * std::pow(0.5, mipmapLevel);
		glCompressedTexImage2D(GL_TEXTURE_2D, mipmapLevel, format, mapWidth, mapHeight, 0, dataSize, data);
		UnbindTexture();
	}

	void GenerateMipmap(void* data, GLsizei dataSize, int mipmapLevel, GLenum format) {
		BindTexture();
		glGenerateMipmap(GL_TEXTURE_2D);
		UnbindTexture();
	}
};

class TextureCube : public Texture {
public:
	TextureCube(int in_width, int in_height) :Texture(in_width, in_height) {}
	TextureCube() = default;

	~TextureCube() {}

	void BindTexture() override {
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	}
	void UnbindTexture() override {
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
};

enum class RenderTextureFormat {
	COLOR, DEPTH, STENCIL, STENCIL_DEPTH
};

class FrameBufferPrototype;
class RenderTexture: public Texture2D {
public:
	bool Generate(GLenum attatchment, GenerateFunc generateFunc, int width, int height, FrameBufferPrototype* in_frameBuffer);
	bool GenerateCubemap(int size, FrameBufferPrototype* in_frameBuffer);
	GLuint GetTextureContent() { return texture; frameBuffer = nullptr; }

	FrameBufferPrototype* frameBuffer;
	RenderTextureFormat format;

	int width = 0;
	int height = 0;
};