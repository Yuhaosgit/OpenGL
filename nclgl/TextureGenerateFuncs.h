#pragma once
#include "../nclgl/OGLRenderer.h"

#pragma region TextureGeneration
void GenerateShadowTexture(int width, int height, GLuint* texture, GLenum attatchment) {
	glTexImage2D
	(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *texture, 0);
}

void GenerateDepthTexture(int width, int height, GLuint* texture, GLenum attatchment) {
	glTexImage2D
	(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, *texture, 0);
}

void GenerateColorTexture(int width, int height, GLuint* texture, GLenum attatchment) {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attatchment, GL_TEXTURE_2D, *texture, 0);
}

void GenerateAccumulationTexture(int width, int height, GLuint* texture, GLenum attatchment) {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_HALF_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attatchment, GL_TEXTURE_2D, *texture, 0);
}

void GenerateRevealTexture(int width, int height, GLuint* texture, GLenum attatchment) {
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attatchment, GL_TEXTURE_2D, *texture, 0);
}
#pragma endregion