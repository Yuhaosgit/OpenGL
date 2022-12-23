#pragma once
#include "Script.h"

class CameraMove :public BaseScript {
public:
	static const std::string Type;
	std::string GetType() override { return Type; }

	float yaw = 0.0f;
	float pitch = 0.0f;
	float speed = 3.0f;

	virtual void Start() override;
	virtual void Update() override;

	CameraMove() = default;
	~CameraMove() = default;
};