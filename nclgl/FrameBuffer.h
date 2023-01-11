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
	virtual ~FrameBufferPrototype() { glDeleteFramebuffers(1, &content); }

	virtual void GenerateFrameBuffer() = 0;
	virtual void ClearBuffer() = 0;

	GLuint content = 0;
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
	std::shared_ptr<RenderTexture> PBRTarget;
	std::shared_ptr<RenderTexture> stencilDepthTarget;
private:
	LightFBO() {
		PBRTarget = std::make_shared<RenderTexture>();
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

class EnvironmentMapFBO :public FrameBufferPrototype {
	friend class FrameBuffer<EnvironmentMapFBO>;
public:
	std::shared_ptr<RenderTexture> environmentTarget;
	int diffuseResolution = 128;
	int specularResolution = 256;

	void GenerateMipmap(int in_width, int in_height);
private:
	EnvironmentMapFBO() {
		environmentTarget = std::make_shared<RenderTexture>();

		GenerateFrameBuffer();
	}
	~EnvironmentMapFBO() {}

	void GenerateFrameBuffer() override;
	void ClearBuffer() override;
};

class SpecularLUTFBO :public FrameBufferPrototype {
	friend class FrameBuffer<SpecularLUTFBO>;
public:
	std::shared_ptr<RenderTexture> LUT_Target;
	int LUTResolution = 512;

private:
	SpecularLUTFBO() {
		LUT_Target = std::make_shared<RenderTexture>();

		GenerateFrameBuffer();
	}
	~SpecularLUTFBO() {}

	void GenerateFrameBuffer() override;
	void ClearBuffer() override;
};