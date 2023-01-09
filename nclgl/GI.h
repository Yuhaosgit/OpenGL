#pragma once
#include "Script.h"

class TextureCube;
class Camera;

class IBL :public BaseScript {
public:
	static const std::string Type;
	std::string GetType() override { return Type; }

	virtual void Start() override;
	virtual void Update() override;
}; 