#pragma once
#include <memory>
#include <map>
#include <string>
#include <vector>

#include "Texture.h"

class FrameBuffer :public std::enable_shared_from_this<FrameBuffer> {
public:
	static void BindScreenFrameBuffer() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	FrameBuffer() {
		glGenFramebuffers(1, &buffer);
	}

	~FrameBuffer() {
		for (auto target : targets) {
			target.second->Delete();
		}
		targets.clear();
		glDeleteFramebuffers(1, &buffer);
	}

	void BindFrameBuffer() {
		Bind();
		Clear();
	}

	void Bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, buffer);
	}
	void Clear() {
		int clearMask = 0;
		if (stencilExist) {
			glStencilMask(0xFF);
			clearMask |= GL_STENCIL_BUFFER_BIT;
		}
		if (depthExist)
			clearMask |= GL_DEPTH_BUFFER_BIT;
		if (!colorAttachments.empty())
			clearMask |= GL_COLOR_BUFFER_BIT;

		if (clearMask != 0)
			glClear(clearMask);
	}

	static void BlitColor(std::weak_ptr<RenderTexture> source, std::weak_ptr<RenderTexture> dest) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, source.lock()->frameBuffer->buffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest.lock()->frameBuffer->buffer);
		glBlitFramebuffer(0, 0, source.lock()->GetWidth(), source.lock()->GetHeight(), 0, 0, dest.lock()->GetWidth(), dest.lock()->GetHeight(), 
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	static void BlitDepth(std::weak_ptr<RenderTexture> source, std::weak_ptr<RenderTexture> dest) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, source.lock()->frameBuffer->buffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest.lock()->frameBuffer->buffer);
		glBlitFramebuffer(0, 0, source.lock()->GetWidth(), source.lock()->GetHeight(), 0, 0, dest.lock()->GetWidth(), dest.lock()->GetHeight(),
			GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	static void BlitStencil(std::weak_ptr<RenderTexture> source, std::weak_ptr<RenderTexture> dest) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, source.lock()->frameBuffer->buffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dest.lock()->frameBuffer->buffer);
		glBlitFramebuffer(0, 0, source.lock()->GetWidth(), source.lock()->GetHeight(), 0, 0, dest.lock()->GetWidth(), dest.lock()->GetHeight(),
			GL_STENCIL_BUFFER_BIT, GL_NEAREST);
	}

	void SetRenderTexture(const std::string& name, std::shared_ptr<RenderTexture> target) {
		Bind();
		if (target->GetFormat() == RenderTextureFormat::DEPTH) {
			BindTextureToFrameBuffer(GL_DEPTH_ATTACHMENT, target->GetDimension(), target->GetContent());
			target->Attachment = GL_DEPTH_ATTACHMENT;

			depthExist = true;
		}
		else if (target->GetFormat() == RenderTextureFormat::STENCIL_DEPTH) {
			BindTextureToFrameBuffer(GL_DEPTH_ATTACHMENT, target->GetDimension(), target->GetContent());
			BindTextureToFrameBuffer(GL_STENCIL_ATTACHMENT, target->GetDimension(), target->GetContent());
			target->Attachment = GL_DEPTH_ATTACHMENT | GL_STENCIL_ATTACHMENT;

			stencilExist = true;
			depthExist = true;
		}
		else {
			GLenum attachment = GL_COLOR_ATTACHMENT0 + colorAttachments.size();
			BindTextureToFrameBuffer(attachment, target->GetDimension(), target->GetContent());
			target->Attachment = attachment;

			colorAttachments.emplace_back(attachment);
			glDrawBuffers(colorAttachments.size(), colorAttachments.data());
		}
		target->frameBuffer = this;
		targets.emplace(name, target);
	}

	std::shared_ptr<RenderTexture> GetRenderTexture(const std::string& name) {
		auto iter = targets.find(name);
		if (iter != targets.end())
			return iter->second;
		return nullptr;
	}

private:
	std::map<std::string, std::shared_ptr<RenderTexture>> targets;
	std::vector<GLenum> colorAttachments;
	bool depthExist = false;
	bool stencilExist = false;
	
	GLuint buffer = 0;

	void BindTextureToFrameBuffer(GLenum attachment, Dimension dimension, GLuint texture, int mipLevel = 0) {
		if (dimension == Dimension::Texture2D)
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, EnumToGLuint(dimension), texture, mipLevel);

		else if (dimension == Dimension::Cubemap) {
			for (int i = 0; i < 6; ++i) {
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, texture, mipLevel);
			}
		}
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			throw "FrameBuffer Initiate fail.";
		}
	}
};