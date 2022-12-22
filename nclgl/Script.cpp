#include "Script.h"

std::vector<std::weak_ptr<BaseScript>> BaseScript::scripts;

void BaseScript::AddInstance(std::shared_ptr<Component> component) {
	scripts.emplace_back(std::dynamic_pointer_cast<BaseScript>(component));
}