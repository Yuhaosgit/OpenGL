#pragma once
#include <memory>
#include "Camera.h"

template <typename T>
class RenderPass {
public:
	static_assert(std::is_base_of<RenderPassPrototype, T>::value);
	static void Pass(Camera* camera) {
		if (target == nullptr)
			target = new T();
		target->Pass(camera);
	}
private:
	static T* target;
};

template <typename T>
T* RenderPass<T>::target;

class RenderPassPrototype {
	friend class RenderPass<RenderPassPrototype>;
public:
	void Pass(Camera* camera) {
		RenderPreset();
		RenderFunction(camera);
		RenderAfterSet();
	}
protected:
	RenderPassPrototype() = default;
	virtual void RenderPreset() = 0;
	virtual void RenderFunction(Camera* camera) = 0;
	virtual void RenderAfterSet() = 0;
};

class ShadowPass :public RenderPassPrototype {
	friend class RenderPass<ShadowPass>;

	ShadowPass() = default;
	void RenderPreset() override;
	void RenderFunction(Camera* camera) override;
	void RenderAfterSet() override;
};

class GbufferPass :public RenderPassPrototype {
	friend class RenderPass<GbufferPass>;

	GbufferPass() = default;
	void RenderPreset() override;
	void RenderFunction(Camera* camera) override;
	void RenderAfterSet() override;
};

class DecalPass :public RenderPassPrototype {
	friend class RenderPass<DecalPass>;

	DecalPass() = default;
	void RenderPreset() override;
	void RenderFunction(Camera* camera) override;
	void RenderAfterSet() override;
};

class DirectLightPass :public RenderPassPrototype {
	friend class RenderPass<DirectLightPass>;

	DirectLightPass() = default;
	void RenderPreset() override;
	void RenderFunction(Camera* camera) override;
	void RenderAfterSet() override;
};

class CombinePass :public RenderPassPrototype {
	friend class RenderPass<CombinePass>;

	CombinePass() = default;
	void RenderPreset() override;
	void RenderFunction(Camera* camera) override;
	void RenderAfterSet() override;
};