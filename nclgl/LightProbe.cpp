#include "LightProbe.h"
#include "GameObject.h"

const std::string LightProbe::Type = "LightProbe";
std::vector<std::weak_ptr<LightProbe>> LightProbe::lightProbes;

void LightProbe::AddInstance(std::shared_ptr<Component> component) {
	lightProbes.emplace_back(std::dynamic_pointer_cast<LightProbe>(component));
	
	auto camera = std::make_shared<Camera>();
	camera->SetFOV(90);
	gameObject->GetComponent<Transform>()->SetPosition(Vector3(0, 10, 0));
}

