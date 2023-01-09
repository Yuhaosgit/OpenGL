#pragma once

#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "MeshRender.h"
#include "Light.h"
#include "Terrain.h"
#include "Component.h"
#include "ScriptFiles.h"
#include "Panel.h"

#include <unordered_map>
#include <vector>

struct Prefab;
class GameObject{
private:
	std::unordered_map<std::string, std::shared_ptr<Component>> components;
public:
	GameObject(const std::string& name_ = "");
	~GameObject();
	GameObject(const GameObject& copy);

	void AddComponent(std::shared_ptr<Component> component);

	template<typename T>
	std::shared_ptr<T> GetComponent() {
		static_assert(std::is_base_of<Component, T>::value);
		std::shared_ptr<T> component = std::dynamic_pointer_cast<T>(components[T::Type]);
		if (component == nullptr) {
			throw "The component: " + T::Type + "not attached in the game object";
		}
		return component;
	}

	std::string name;
};