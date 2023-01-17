#pragma once
#include "Script.h"

class CameraMove :public BaseScript {
public:
	static const std::string Type;
	std::string GetType() override { return Type; }

	float yaw = 95.0f;
	float pitch = 14.0f;
	float speed = 10.0f;

	bool cameraLock = false;

	virtual void Start() override;
	virtual void Update() override;

	CameraMove() = default;
	~CameraMove() = default;
};
