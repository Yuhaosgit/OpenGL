#pragma once
#include <memory>
#include "../nclgl/OGLRenderer.h"

typedef void (*GenerateFunc) (int, int, GLuint*, GLenum);

class Texture2D {
public:
	Texture2D() { texture = 0; }

	Texture2D(const GLuint &tex) { texture = tex; }
	void operator=(const GLuint &tex) { texture = tex; }

	~Texture2D() {}
	void Delete() { glDeleteTextures(1, &texture); }
	void Submit(Shader* targetShader, const std::string& variableName, int layer);

protected:
	GLuint texture;
};

enum class BufferType {
	COLOR, DEPTH, STENCIL, STENCIL_DEPTH
};

class RenderTexture:public Texture2D {
public:
	bool Generate(GLenum attatchment, GenerateFunc generateFunc, int width, int height);
	GLuint frameBuffer;
	BufferType bufferType;
	int width = 0;
	int height = 0;
};