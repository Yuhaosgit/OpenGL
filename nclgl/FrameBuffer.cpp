#include "FrameBuffer.h"
#include "TextureGenerateFuncs.h"

#pragma region Prototype
void FrameBufferPrototype::BindFrameBuffer() {
	glBindFramebuffer(GL_FRAMEBUFFER, content);
}

void CopyRenderTexture(std::shared_ptr<RenderTexture> read, std::shared_ptr<RenderTexture> draw, RenderTextureFormat RenderTextureFormat) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, read->frameBuffer->content);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw->frameBuffer->content);

	GLuint buffer = 0;
	if (RenderTextureFormat == RenderTextureFormat::COLOR)
		buffer = GL_COLOR_BUFFER_BIT;
	else if (RenderTextureFormat == RenderTextureFormat::DEPTH)
		buffer = GL_DEPTH_BUFFER_BIT;
	else if (RenderTextureFormat == RenderTextureFormat::STENCIL)
		buffer = GL_STENCIL_BUFFER_BIT;

	glBlitFramebuffer(0, 0, read->width, read->height, 0, 0, draw->width, draw->height, buffer, GL_NEAREST);
}
#pragma endregion

#pragma region shadowFBO
void ShadowFBO::GenerateFrameBuffer() {
	bool initFlag = false;
	glGenFramebuffers(1, &content);
	//lighting buffer
	glBindFramebuffer(GL_FRAMEBUFFER, content);

	initFlag = depthTarget->Generate(0, GenerateDepthTexture, ShadowMapSize, ShadowMapSize, this);

	initFlag = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (!initFlag) {
		throw "Shadow FBO initialization failed";
	}
}

void ShadowFBO::ClearBuffer() {
	glClear(GL_DEPTH_BUFFER_BIT);
}
#pragma endregion

#pragma region ScreenFBO
void ScreenFBO::ClearBuffer() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
#pragma endregion


#pragma region Gbuffer
void GeometryFBO::GenerateFrameBuffer() {
	int width = OGLRenderer::GetCurrentWidth();
	int height = OGLRenderer::GetCurrentHeight();

	bool initFlag = false;
	glGenFramebuffers(1, &content);
	glBindFramebuffer(GL_FRAMEBUFFER, content);

	initFlag = stencilDepthTarget->Generate(GL_DEPTH24_STENCIL8, GenerateDepthStencilTexture, width, height, this);
	initFlag = colorTarget->Generate(GL_COLOR_ATTACHMENT0, GenerateColorTexture, width, height, this);
	initFlag = normalTarget->Generate(GL_COLOR_ATTACHMENT1, GenerateColorTexture, width, height, this);

	initFlag = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (!initFlag) {
		throw "Gbuffer FBO initialization failed";
	}
}

void GeometryFBO::ClearBuffer() {
	glStencilMask(0xFF);
	
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
#pragma endregion

#pragma region DecalFBO
void DecalFBO::GenerateFrameBuffer() {
	int width = OGLRenderer::GetCurrentWidth();
	int height = OGLRenderer::GetCurrentHeight();

	bool initFlag = false;
	glGenFramebuffers(1, &content);
	//lighting buffer
	glBindFramebuffer(GL_FRAMEBUFFER, content);

	initFlag = colorTarget->Generate(GL_COLOR_ATTACHMENT0, GenerateColorTexture, width, height, this);
	initFlag = normalTarget->Generate(GL_COLOR_ATTACHMENT1, GenerateColorTexture, width, height, this);
	initFlag = stencilDepthTarget->Generate(GL_DEPTH24_STENCIL8, GenerateDepthStencilTexture, width, height, this);

	initFlag = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (!initFlag) {
		throw "Outcome FBO initialization failed";
	}
}

void DecalFBO::ClearBuffer() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//ConnectFrameBuffer(*Renderer::instance()->gBuffer, *this);

	//glReadBuffer(GL_COLOR_ATTACHMENT0);
	//glDrawBuffer(GL_COLOR_ATTACHMENT0);
	//glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	//glReadBuffer(GL_COLOR_ATTACHMENT1);
	//glDrawBuffer(GL_COLOR_ATTACHMENT1);
	//glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	//glDrawBuffers(2, attachment);
}
#pragma endregion


#pragma region LightFBO
void LightFBO::GenerateFrameBuffer() {
	int width = OGLRenderer::GetCurrentWidth();
	int height = OGLRenderer::GetCurrentHeight();

	bool initFlag = false;
	glGenFramebuffers(1, &content);

	glBindFramebuffer(GL_FRAMEBUFFER, content);

	initFlag = PBRTarget->Generate(GL_COLOR_ATTACHMENT0, GenerateColorTexture, width, height, this);
	initFlag = stencilDepthTarget->Generate(GL_DEPTH24_STENCIL8, GenerateDepthStencilTexture, width, height, this);

	initFlag = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (!initFlag) {
		throw "LightFBO FBO initialization failed";
	}
}

void LightFBO::ClearBuffer() {
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
#pragma endregion

#pragma region OITFBO
void OITFBO::GenerateFrameBuffer() {
	int width = OGLRenderer::GetCurrentWidth();
	int height = OGLRenderer::GetCurrentHeight();

	bool initFlag = false;
	glGenFramebuffers(1, &content);
	//lighting buffer
	glBindFramebuffer(GL_FRAMEBUFFER, content);

	initFlag = accumulationTarget->Generate(GL_COLOR_ATTACHMENT0, GenerateAccumulationTexture, width, height, this);
	initFlag = revealTarget->Generate(GL_COLOR_ATTACHMENT1, GenerateRevealTexture, width, height, this);
	initFlag = depthTarget->Generate(GL_DEPTH24_STENCIL8, GenerateDepthStencilTexture, width, height, this);

	initFlag = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (!initFlag) {
		throw "OIT FBO initialization failed";
	}
}

void OITFBO::ClearBuffer() {
	GLfloat zeroFill[4] = { 0.0f,0.0f,0.0f,0.0f };
	GLfloat oneFill[4] = { 1.0f,1.0f,1.0f,1.0f };

	glClearBufferfv(GL_COLOR, 0, zeroFill);
	glClearBufferfv(GL_COLOR, 1, oneFill);
	glClear(GL_DEPTH_BUFFER_BIT);
}

//void OITFBO::RenderPreset() {
//	glDepthMask(GL_TRUE);
//	glEnable(GL_BLEND);
//	glBlendFunci(0, GL_ONE, GL_ONE); // accumulation blend target
//	glBlendFunci(1, GL_ZERO, GL_ONE_MINUS_SRC_COLOR); // revealge blend target
//}
//
//void OITFBO::RenderAfterSet() {
//
//}
//
//void OITFBO::RenderFunction() {
//	Renderer::instance()->waterSurface->Render();
//}
#pragma endregion

#pragma region OutcomeFBO
void OutcomeFBO::GenerateFrameBuffer() {
	int width = OGLRenderer::GetCurrentWidth();
	int height = OGLRenderer::GetCurrentHeight();

	bool initFlag = false;
	glGenFramebuffers(1, &content);
	//lighting buffer
	glBindFramebuffer(GL_FRAMEBUFFER, content);

	initFlag = colorTarget->Generate(GL_COLOR_ATTACHMENT0, GenerateColorTexture, width, height, this);

	initFlag = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (!initFlag) {
		throw "Outcome FBO initialization failed";
	}
}

void OutcomeFBO::ClearBuffer() {
	glClear(GL_COLOR_BUFFER_BIT);
}
#pragma endregion

#pragma region Indirect
void EnvironmentMapFBO::GenerateFrameBuffer() {
	bool initFlag = false;
	glGenFramebuffers(1, &content);
	//lighting buffer
	glBindFramebuffer(GL_FRAMEBUFFER, content);

	initFlag = environmentTarget->GenerateCubemap(diffuseResolution, this);

	initFlag = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (!initFlag) {
		throw "Outcome FBO initialization failed";
	}
}

void EnvironmentMapFBO::ClearBuffer() {}

void EnvironmentMapFBO::GenerateMipmap(int in_width, int in_height) {
	environmentTarget->width = in_width;
	environmentTarget->height = in_height;

	glBindTexture(GL_TEXTURE_CUBE_MAP, environmentTarget->GetTextureContent());
	for (int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, 
			in_width, in_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void SpecularLUTFBO::GenerateFrameBuffer() {
	bool initFlag = false;
	glGenFramebuffers(1, &content);
	//lighting buffer
	glBindFramebuffer(GL_FRAMEBUFFER, content);

	initFlag = LUT_Target->Generate(GL_COLOR_ATTACHMENT0, GenerateRGTexture, LUTResolution, LUTResolution, this);

	initFlag = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (!initFlag) {
		throw "Outcome FBO initialization failed";
	}
}

void SpecularLUTFBO::ClearBuffer() {
}

#pragma endregion
