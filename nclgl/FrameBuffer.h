#pragma once
#include "OGLRenderer.h"
#include "Material.h"
#include <memory>

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

	friend void CopyRenderTexture(const RenderTexture& read, const RenderTexture& draw, BufferType bufferType);
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
	RenderTexture depthTarget;
private:
	ShadowFBO() {
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
	RenderTexture stencilDepthTarget;
	RenderTexture colorTarget;
	RenderTexture normalTarget;
private:
	GeometryFBO() {
		GenerateFrameBuffer();
	}
	~GeometryFBO() {}

	void GenerateFrameBuffer() override;
	void ClearBuffer() override;
};

class DecalFBO :public FrameBufferPrototype {
	friend class FrameBuffer<DecalFBO>;
public:
	RenderTexture stencilDepthTarget;
	RenderTexture colorTarget;
	RenderTexture normalTarget;
private:
	DecalFBO() {
		GenerateFrameBuffer();
	}
	~DecalFBO() {}

	void GenerateFrameBuffer() override;
	void ClearBuffer() override;
};

class LightFBO :public FrameBufferPrototype {
	friend class FrameBuffer<LightFBO>;
public:
	RenderTexture diffuseTarget;
	RenderTexture specularTareget;
	RenderTexture stencilDepthTarget;
private:
	LightFBO() {
		GenerateFrameBuffer();
	}
	~LightFBO() {}

	void GenerateFrameBuffer() override;
	void ClearBuffer() override;
};

class OITFBO :public FrameBufferPrototype {
	friend class FrameBuffer<OITFBO>;
public:
	RenderTexture accumulationTarget;
	RenderTexture revealTarget;
	RenderTexture depthTarget;
private:
	OITFBO() {
		GenerateFrameBuffer();
	}
	~OITFBO() {}

	void GenerateFrameBuffer() override;
	void ClearBuffer() override;
};

class OutcomeFBO :public FrameBufferPrototype {
	friend class FrameBuffer<OutcomeFBO>;
public:
	RenderTexture colorTarget;
private:
	OutcomeFBO() {
		GenerateFrameBuffer();
	}
	~OutcomeFBO() {}

	void GenerateFrameBuffer() override;
	void ClearBuffer() override;
};