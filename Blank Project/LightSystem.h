//#pragma once
//#include <vector>
//#include "../nclgl/DirectLight.h"
//#include "../nclgl/PointLight.h"
//#include "FBO_Manager.h"
//
//class LightSystem {
//public:
//	LightSystem(std::unique_ptr<Shader> dlShader, std::unique_ptr<Shader> plShader):
//		directLightShader(std::move(dlShader)),pointLightShader(std::move(plShader)){}
//
//	void AddDirectLight(const Vector3& orientation, const Vector4& color);
//	void AddDirectLight(DirectLight* directLight);
//
//	void AddPointLight(const Vector3& position, const Vector4& color, const float& scale);
//	void AddPointLight(PointLight* pointLight);
//
//	void RenderAllVisualLight(Vector3& cameraPos);
//
//	void SubmitMainLightData(Shader* shader) { directLights[0]->SubmitData(shader); }
//
//	bool PointLightShadow = false;
//private:
//	void RenderAllDirectLight(Vector3& cameraPos);
//
//	void RenderAllPointLight(Vector3& cameraPos);
//	void RenderAllPointLightStencilPass(int index);
//	void RenderAllPointLightPass(Vector3& cameraPos, int index);
//
//	std::vector<DirectLight*> directLights;
//	std::vector<PointLight*> pointLights;
//
//	std::unique_ptr<Shader> directLightShader;
//	std::unique_ptr<Shader> pointLightShader;
//
//	float RePixelSize[2];
//};