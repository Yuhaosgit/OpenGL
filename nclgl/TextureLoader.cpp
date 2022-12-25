#include <vector>

#include "TextureLoader.h"
#include "stb_image.h"

void SetTextureRepeating(GLuint target) {
	glBindTexture(GL_TEXTURE_2D, target);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint TextureLoader::LoadTexture2D(const std::string fileName, bool repeat) {
	GLuint texID = SOIL_load_OGL_texture
	(fileName.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	if (repeat) {
		SetTextureRepeating(texID);
	}
	return texID;
}