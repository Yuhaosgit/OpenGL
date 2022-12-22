#include "Texture.h"

void Texture2D::Submit(Shader* targetShader, const std::string& variableName, int layer) {
	glActiveTexture(GL_TEXTURE0 + layer);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(targetShader->GetProgram(), variableName.c_str()), layer);
}

bool RenderTexture::Generate(GLenum attatchment, GenerateFunc generateFunc, int width_, int height_) {
	width = width_;
	height = height_;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	generateFunc(width, height, &texture, attatchment);

	return (texture != 0);
}