#include "TextureLoader.h"
#include <gli/gli.hpp>

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

GLuint TextureLoader::LoadTextureCube(const std::array<std::string, 6> fileName){
	GLuint axis[6] = { 
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

	GLuint cubeTex;
	glGenTextures(1, &cubeTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

	GLubyte* imagedata = nullptr;
	int width, height, channels;

	for (int i = 0; i < 6; ++i) {
		//load image information
		imagedata = SOIL_load_image(fileName[i].c_str(), &width, &height, &channels, 0);
		//test if load succeed
		if (!imagedata) {
			std::cout << "Load Image Fail!\n";
			cubeTex = 0;
			return cubeTex;
		}
		glTexImage2D(axis[i], 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imagedata);

		SOIL_free_image_data(imagedata);
	}

	//set texture attributes
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return cubeTex;
}