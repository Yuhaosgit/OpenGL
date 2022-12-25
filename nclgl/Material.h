#pragma once
#include <memory>
#include <unordered_map>
#include <string>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"

#include "Shader.h"
#include "Texture.h"

enum class RenderType {
	Opaque, Transparent
};

class Material {
public:
	Material(){}
	Material(const Material& material) = default;

	void SetFloat(std::string name, float value);
	void SetInt(std::string name, int value);
	void SetBool(std::string name, bool value);

	void SetVector2(std::string name, const Vector2& vec);
	void SetVector3(std::string name, const Vector3& vec);
	void SetVector4(std::string name, const Vector4& vec);

	void SetMatrix4(std::string name, const Matrix4& mat4);
	void SetTexture(std::string name, std::shared_ptr<Texture> tex);

	void SubmitData(bool sendModelMat = true, bool sendViewMat = true, bool sendProjMat = true, bool sendShadowMat = true);

	Shader* shader;
	RenderType renderType = RenderType::Opaque;

	static Matrix4 viewMatrix;
	static Matrix4 projMatrix;
	static Matrix4 modelMatrix;
	static Matrix4 shadowMatrix;
private:
	void BindShader();

	//data sets
	std::unordered_map<std::string, float> floatDataSet;
	std::unordered_map<std::string, int> intDataSet;
	std::unordered_map<std::string, bool> boolDataSet;

	std::unordered_map<std::string, Vector2> Vector2Set;
	std::unordered_map<std::string, Vector3> Vector3Set;
	std::unordered_map<std::string, Vector4> Vector4Set;

	std::unordered_map<std::string, Matrix4> MatrixSet;

	std::unordered_map<std::string, std::weak_ptr<Texture>> TextureSet;

	void SubmitFloatDataSet();
	void SubmitIntDataSet();
	void SubmitBoolDataSet();
	void SubmitVector2Set();
	void SubmitVector3Set();
	void SubmitVector4Set();
	void SubmitMatrixSet(bool sendModelMat, bool sendViewMat, bool sendProjMat, bool sendShadowMat);
	void SubmitTextureSet();
};