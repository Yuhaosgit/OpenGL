#include "Texture.h"
#include "FrameBuffer.h"

int Texture::activeCount = 0;
std::vector<Texture*> Texture::activeTextures;
GLenum RenderTexture::InterformatToFormat(GLuint format) {
	if (format == GL_R8 || format == GL_R16F || format == GL_R32F)
		return GL_RED;
	else if (format == GL_RG8 || format == GL_RG16F || format == GL_RG32F)
		return GL_RG;
	else if (format == GL_RGB8 || format == GL_RGB16F || format == GL_RGB32F)
		return GL_RGB;
	else if (format == GL_RGBA8 || format == GL_RGBA16F || format == GL_RGBA32F)
		return GL_RGBA;
}

void RenderTexture::SetFrameBufferMipmapAndFace(GLenum target, int miplevel, IOstate io) {
	if (frameBuffer == nullptr || format == RenderTextureFormat::DEPTH || format == RenderTextureFormat::STENCIL_DEPTH)
		return;

	frameBuffer->Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, Attachment, target, texture, miplevel);
	if (io == IOstate::Write)
		glDrawBuffer(Attachment);
	else if (io == IOstate::Read)
		glReadBuffer(Attachment);
}