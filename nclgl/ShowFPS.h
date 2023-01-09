#pragma once
#include "Script.h"

class Light;

class ShowFPS :public BaseScript {
public:
	static const std::string Type;
	std::string GetType() override { return Type; }

	virtual void Start() override;
	virtual void Update() override;

	float parallelRotateY = 0.0;
	float parallelRotateX = 45.0;
	std::weak_ptr<Light> parallel_Light;
};