#include "GameObject.h"
#include "Importer.h"

GameObject::GameObject(const std::string& name_){
	this->AddComponent(std::make_shared<Transform>());
	name = name_;
}

GameObject::GameObject(const GameObject& copy) {
	for (auto& component : copy.components) {
		this->AddComponent(component.second);
	}
}
GameObject::~GameObject() {
	for (auto child : GetComponent<Transform>()->children) {
		delete child.lock()->gameObject;
	}
	components.clear();
}

void GameObject::AddComponent(std::shared_ptr<Component> component) {
	if (components[component->GetType()] != nullptr) {
		return;
	}
	component->gameObject = this;
	components[component->GetType()] = component;
	component->AddInstance(component);
}