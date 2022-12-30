#pragma once
#include "OGLRenderer.h"
#include "Material.h"
#include <memory>
#include <functional>

#define ShadowMapSize 2048

template <typename T>
class FrameBuffer {
public:
	static_assert(std::is_base_of<FrameBufferPrototype, T>::value);
	static T* instance() {
		if (target == nullptr)
			target = new T();
		return target;
	}
private:
	static T* target;
};

template <typename T>
T* FrameBuffer<T>::target;

class FrameBufferPrototype {
public:
	void Bind() {
		BindFrameBuffer();
		ClearBuffer();
	}

	friend void CopyRenderTexture
	(std::shared_ptr<RenderTexture> read, std::shared_ptr<RenderTexture> draw, RenderTextureFormat bufferType);
protected:
	FrameBufferPrototype() = default;

	void BindFrameBuffer();
	virtual ~FrameBufferPrototype() { glDeleteFramebuffers(1, &frameBuffer); }

	virtual void GenerateFrameBuffer() = 0;
	virtual void ClearBuffer() = 0;

	GLuint frameBuffer = 0;
};

class ShadowFBO :public FrameBufferPrototype {
	friend class FrameBuffer<ShadowFBO>;
public:
	std::shared_ptr<RenderTexture> depthTarget;
private:
	ShadowFBO() {
		depthTarget = std::make_shared<RenderTexture>();

		GenerateFrameBuffer();
	}
	~ShadowFBO() {}

	void GenerateFrameBuffer() override;
	void ClearBuffer() override;
};

class ScreenFBO :public FrameBufferPrototype {
	friend class FrameBuffer<ScreenFBO>;
private:
	ScreenFBO() {}
	
	void GenerateFrameBuffer() override {}
	void ClearBuffer() override;
};

class GeometryFBO :public FrameBufferPrototype {
	friend class FrameBuffer<GeometryFBO>;
public:
	std::shared_ptr<RenderTexture> stencilDepthTarget;
	std::shared_ptr<RenderTexture> colorTarget;
	std::shared_ptr<RenderTexture> normalTarget;
private:
	GeometryFBO() {
		stencilDepthTarget = std::make_shared<RenderTexture>();
		colorTarget = std::make_shared<RenderTexture>();
		normalTarget = std::make_shared<RenderTexture>();

		GenerateFrameBuffer();
	}
	~GeometryFBO() {}

	void GenerateFrameBuffer() override;
	void ClearBuffer() override;
};

class DecalFBO :public FrameBufferPrototype {
	friend class FrameBuffer<DecalFBO>;
public:
	std::shared_ptr<RenderTexture> stencilDepthTarget;
	std::shared_ptr<RenderTexture> colorTarget;
	std::shared_ptr<RenderTexture> normalTarget;
private:
	DecalFBO() {
		stencilDepthTarget = std::make_shared<RenderTexture>();
		colorTarget = std::make_shared<RenderTexture>();
		normalTarget = std::make_shared<RenderTexture>();

		GenerateFrameBuffer();
	}
	~DecalFBO() {}

	void GenerateFrameBuffer() override;
	void ClearBuffer() override;
};

class LightFBO :public FrameBufferPrototype {
	friend class FrameBuffer<LightFBO>;
public:
	std::shared_ptr<RenderTexture> diffuseTarget;
	std::shared_ptr<RenderTexture> specularTareget;
	std::shared_ptr<RenderTexture> stencilDepthTarget;
private:
	LightFBO() {
		diffuseTarget = std::make_shared<RenderTexture>();
		specularTareget = std::make_shared<RenderTexture>();
		stencilDepthTarget = std::make_shared<RenderTexture>();

		GenerateFrameBuffer();
	}
	~LightFBO() {}

	void GenerateFrameBuffer() override;
	void ClearBuffer() override;
};

class OITFBO :public FrameBufferPrototype {
	friend class FrameBuffer<OITFBO>;
public:
	std::shared_ptr<RenderTexture> accumulationTarget;
	std::shared_ptr<RenderTexture> revealTarget;
	std::shared_ptr<RenderTexture> depthTarget;
private:
	OITFBO() {
		accumulationTarget = std::make_shared<RenderTexture>();
		revealTarget = std::make_shared<RenderTexture>();
		depthTarget = std::make_shared<RenderTexture>();

		GenerateFrameBuffer();
	}
	~OITFBO() {}

	void GenerateFrameBuffer() override;
	void ClearBuffer() override;
};

class OutcomeFBO :public FrameBufferPrototype {
	friend class FrameBuffer<OutcomeFBO>;
public:
	std::shared_ptr<RenderTexture> colorTarget;
private:
	OutcomeFBO() {
		colorTarget = std::make_shared<RenderTexture>();

		GenerateFrameBuffer();
	}
	~OutcomeFBO() {}

	void GenerateFrameBuffer() override;
	void ClearBuffer() override;
};