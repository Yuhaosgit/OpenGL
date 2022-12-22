#pragma once
#include <string>
#include <array>
#include "OGLRenderer.h"

class TextureLoader {
public:
	static GLuint LoadTexture2D(const std::string fileName, bool repeat = false);
	static GLuint LoadTextureCube(const std::array<std::string, 6> fileName);
};