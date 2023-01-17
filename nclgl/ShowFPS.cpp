#include "ShowFPS.h"
#include "GameObject.h"
#include "RenderPass.h"

const std::string ShowFPS::Type = "ShowFPS";

void ShowFPS::Start() {
	auto panel = gameObject->GetComponent<Panel>();
	panel->AddText("FPS");
	panel->AddSlider("LightWidth");
	panel->AddSlider("NearPlane");
	panel->AddSlider("parallel-X");
	panel->AddSlider("parallel-Y");
	panel->AddSlider("intensity");

	parallel_Light = Light::lights[0];
}

void ShowFPS::Update() {
	auto panel = gameObject->GetComponent<Panel>();
	panel->SetText("FPS", std::string("FPS: ") + 
		std::to_string(static_cast<int>(1.0f / Window::GetTimer()->GetTimeDeltaSeconds())));
	panel->SetSlider("LightWidth", &parallel_Light.lock()->lightWidth, 0.1f, 7.0f);
	panel->SetSlider("NearPlane", &parallel_Light.lock()->nearPlane, 0.1f, 10.0f);

	panel->SetSlider("parallel-X", &parallelRotateX, 10.0f, 80.0f);
	panel->SetSlider("parallel-Y", &parallelRotateY, 0.0f, 360.0f);
	panel->SetSlider("intensity", &parallel_Light.lock()->intensity, 0.1f, 100.0f);

	parallel_Light.lock()->gameObject->GetComponent<Transform>()->
		SetRotate(Quaternion::AxisAngleToQuaterion(parallelRotateX, parallelRotateY, 0.0f));
}