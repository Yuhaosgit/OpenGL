#include "Texture.h"

void Texture2D::Submit(Shader* targetShader, const std::string& variableName, int layer) {
	glActiveTexture(GL_TEXTURE0 + layer);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(targetShader->GetProgram(), variableName.c_str()), layer);
}

void TextureCube::Submit(Shader* targetShader, const std::string& variableName, int layer) {
	glActiveTexture(GL_TEXTURE0 + layer);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glUniform1i(glGetUniformLocation(targetShader->GetProgram(), variableName.c_str()), layer);
}

bool RenderTexture::Generate(GLenum attatchment, GenerateFunc generateFunc, int width_, int height_, GLuint in_frameBuffer) {
	width = width_;
	height = height_;

	frameBuffer = in_frameBuffer;

	if (attatchment >= GL_COLOR_ATTACHMENT0 && attatchment <= GL_COLOR_ATTACHMENT31) {
		format = RenderTextureFormat::COLOR;
	}
	else if (attatchment == GL_DEPTH_ATTACHMENT) {
		format = RenderTextureFormat::DEPTH;
	}
	else if (attatchment == GL_STENCIL_ATTACHMENT) {
		format = RenderTextureFormat::STENCIL;
	}
	else if (attatchment == GL_DEPTH24_STENCIL8 || attatchment == GL_DEPTH_STENCIL) {
		format = RenderTextureFormat::STENCIL_DEPTH;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	generateFunc(width, height, &texture, attatchment);

	return (texture != 0);
}

bool RenderTexture::GenerateCubemap(int size, GLuint in_frameBuffer){
	width = size;
	height = size;
	frameBuffer = in_frameBuffer;
	format = RenderTextureFormat::COLOR;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP , texture);

	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8,
			width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, texture, 0);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return (texture != 0);
}
