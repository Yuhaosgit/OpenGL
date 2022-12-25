#pragma once
#include <string>
#include "OGLRenderer.h"

class TextureLoader {
public:
	static GLuint LoadTexture2D(const std::string fileName, bool repeat = false);
};