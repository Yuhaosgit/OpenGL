#pragma once
#include <memory>
#include "../nclgl/OGLRenderer.h"

typedef void (*GenerateFunc) (int, int, GLuint*, GLenum);

class Texture {
public:
	Texture() = default;
	~Texture() {}

	void Delete() { glDeleteTextures(1, &texture); }
	virtual void Submit(Shader* targetShader, const std::string& variableName, int layer) = 0;

	virtual void BindTexture() = 0;
	static void UnbindTexture2D(int position) {
		glActiveTexture(GL_TEXTURE0 + position);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	static void UnbindTextureCube(int position) {
		glActiveTexture(GL_TEXTURE0 + position);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
protected:
	GLuint texture = 0;
};

class Texture2D: public Texture {
public:
	Texture2D() = default;
	Texture2D(const GLuint& tex) { texture = tex; }

	~Texture2D() {}
	void Submit(Shader* targetShader, const std::string& variableName, int layer) override;
	void BindTexture() override {
		glBindTexture(GL_TEXTURE_2D, texture);
	}
};

class TextureCube : public Texture {
public:
	TextureCube() = default;
	TextureCube(const GLuint& tex) { texture = tex; }

	~TextureCube() {}
	void Submit(Shader* targetShader, const std::string& variableName, int layer) override;
	void BindTexture() override {
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
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