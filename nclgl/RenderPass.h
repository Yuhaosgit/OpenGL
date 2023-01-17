#pragma once
#include <memory>
#include "Camera.h"
#include "Importer.h"
#include "Material.h"

class Material;

template <typename T>
class RenderPass {
public:
	static_assert(std::is_base_of<RenderPassPrototype, T>::value);
	static void Pass(Camera* camera) {
		if (target == nullptr)
			target = new T();
		target->Pass(camera);
	}
	static T* Instance() {
		if (target == nullptr)
			target = new T();
		return target;
	}
private:
	static T* target;
};

template <typename T>
T* RenderPass<T>::target;

class RenderPassPrototype {
	friend class RenderPass<RenderPassPrototype>;
public:
	RenderPassPrototype() = default;
	virtual void Pass(Camera* camera) = 0;
};

class ShadowPass :public RenderPassPrototype {
	friend class RenderPass<ShadowPass>;
public:
	ShadowPass() = default;
	virtual void Pass(Camera* camera) override;
};

class GbufferPass :public RenderPassPrototype {
	friend class RenderPass<GbufferPass>;
public:
	GbufferPass() = default;
	virtual void Pass(Camera* camera) override;
};

class DirectLightPass :public RenderPassPrototype {
	friend class RenderPass<DirectLightPass>;
public:
	std::weak_ptr<RenderTexture> colorRoughnessBuffer;
	std::weak_ptr<RenderTexture> normalMetallicBuffer;
	std::weak_ptr<RenderTexture> stencilDepthBuffer;

	DirectLightPass() = default;
	virtual void Pass(Camera* camera) override;
};

class CombinePass :public RenderPassPrototype {
	friend class RenderPass<CombinePass>;
public:
	std::weak_ptr<RenderTexture> opaqueTex;
	std::weak_ptr<RenderTexture> depthTex;

	CombinePass() {
		material = std::make_unique<Material>();
		material->shader = Importer::GetShader("Combine");
	}
	std::unique_ptr<Material> material;

	virtual void Pass(Camera* camera) override;
};

//global illuminate
class RenderTexture;
class EnvironmentDiffusePass :public RenderPassPrototype {
	friend class RenderPass<EnvironmentDiffusePass>;
public:
	EnvironmentDiffusePass(){
		material = std::make_unique<Material>();
		material->shader = Importer::GetShader("IBLDiifuse");
	}
	std::unique_ptr<Material> material;

	virtual void Pass(Camera* camera) override;
	std::weak_ptr<RenderTexture> GIdiffuseBuffer;
};

class SpecularPrefilterPass :public RenderPassPrototype {
	friend class RenderPass<SpecularPrefilterPass>;
public:
	SpecularPrefilterPass() {
		material = std::make_unique<Material>();
		material->shader = Importer::GetShader("IBLSpecularPrefilter");
	}
	std::unique_ptr<Material> material;

	virtual void Pass(Camera* camera) override;
	const int maxMipLevel = 5;
	std::weak_ptr<RenderTexture> GIspecularPrefilter;
};

class SpecularLUTPass :public RenderPassPrototype {
	friend class RenderPass<SpecularLUTPass>;
public:
	SpecularLUTPass() {
		material = std::make_unique<Material>();
		material->shader = Importer::GetShader("IBLSpecularLUT");
	}
	std::unique_ptr<Material> material;

	virtual void Pass(Camera* camera) override;
};