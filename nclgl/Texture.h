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

protected:
	GLuint texture = 0;
};

class Texture2D: public Texture {
public:
	Texture2D() = default;
	Texture2D(const GLuint& tex) { texture = tex; }

	~Texture2D() {}
	void Submit(Shader* targetShader, const std::string& variableName, int layer) override;
};

class TextureCube : public Texture {
public:
	TextureCube() = default;
	TextureCube(const GLuint& tex) { texture = tex; }

	~TextureCube() {}
	void Submit(Shader* targetShader, const std::string& variableName, int layer) override;
};

enum class BufferType {
	COLOR, DEPTH, STENCIL, STENCIL_DEPTH
};

class RenderTexture: public Texture2D {
public:
	bool Generate(GLenum attatchment, GenerateFunc generateFunc, int width, int height);
	GLuint frameBuffer;
	BufferType bufferType;
	int width = 0;
	int height = 0;
};