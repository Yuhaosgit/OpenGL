//#include "LightSystem.h"
//#include "Renderer.h"
//
//void LightSystem::AddDirectLight(const Vector3& orientation, const Vector4& color) {
//	directLights.emplace_back(new DirectLight(color, orientation));
//}
//void LightSystem::AddDirectLight(DirectLight* directLight) {
//	directLights.emplace_back(directLight);
//}
//
//void LightSystem::AddPointLight(const Vector3& position, const Vector4& color, const float& scale) {
//	pointLights.emplace_back(new PointLight(position, color, scale));
//}
//void LightSystem::AddPointLight(PointLight* pointLight){
//	pointLights.emplace_back(pointLight);
//}
//
//void LightSystem::RenderAllDirectLight(Vector3& cameraPos) {
//	Renderer* renderer = Renderer::instance();
//	renderer->BindShader(directLightShader.get());
//
//	renderer->gBuffer->stencilDepthTarget.Submit(directLightShader.get(), "depthTex", 0);
//	renderer->decalBuffer->normalTarget.Submit(directLightShader.get(), "normTex", 1);
//	renderer->shadowBuffer->depthTarget.Submit(directLightShader.get(), "shadowTex", 2);
//
//	directLightShader->SubmitVector3(cameraPos, "cameraPos");
//	directLightShader->SubmitFloat2(RePixelSize, "pixelSize");
//
//	Matrix4 invViewProj = (Renderer::instance()->projMatrix * Renderer::instance()->viewMatrix).Inverse();
//	directLightShader->SubmitMatrix4(invViewProj, "inverseProjView");
//
//	Renderer::instance()->UpdateShaderMatrices();
//
//	for (int i = 0; i < directLights.size(); ++i) {
//		directLights[i]->SubmitData(directLightShader.get());
//		Renderer::instance()->quad->Draw();
//	}
//}
//
//void LightSystem::RenderAllPointLight(Vector3& cameraPos) {
//	Renderer::instance()->BindShader(pointLightShader.get());
//
//	Renderer::instance()->UpdateShaderMatrices();
//	for (int i = 0; i < pointLights.size(); ++i) {
//		RenderAllPointLightStencilPass(i);
//		RenderAllPointLightPass(cameraPos, i);
//	}
//}
//
//void LightSystem::RenderAllPointLightStencilPass(int index){
//	glDisable(GL_CULL_FACE);
//	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
//	glDepthFunc(GL_LESS);
//
//	glStencilMask(0xFF);
//	glClear(GL_STENCIL_BUFFER_BIT);
//	glStencilFunc(GL_ALWAYS, 1, 0xFF);
//
//	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
//	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
//
//	pointLightShader->SubmitBool(true, "stencilPass");
//	pointLights[index]->SubmitData(pointLightShader.get());
//	Renderer::instance()->sphere->Draw();
//
//	glEnable(GL_CULL_FACE);
//	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
//	glStencilMask(0x00);
//}
//
//void LightSystem::RenderAllPointLightPass(Vector3& cameraPos, int index) {
//	glCullFace(GL_FRONT);
//
//	Renderer* renderer = Renderer::instance();
//	renderer->gBuffer->stencilDepthTarget.Submit(pointLightShader.get(), "depthTex", 0);
//	renderer->decalBuffer->normalTarget.Submit(pointLightShader.get(), "normTex", 1);
//
//	pointLightShader->SubmitVector3(cameraPos, "cameraPos");
//	pointLightShader->SubmitFloat2(RePixelSize, "pixelSize");
//
//	Matrix4 invViewProj = (Renderer::instance()->projMatrix * Renderer::instance()->viewMatrix).Inverse();
//	pointLightShader->SubmitMatrix4(invViewProj, "inverseProjView");
//
//	glDepthFunc(GL_ALWAYS);
//	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
//
//	pointLightShader->SubmitBool(false, "stencilPass");
//	pointLights[index]->SubmitData(pointLightShader.get());
//	Renderer::instance()->sphere->Draw();
//
//	glStencilFunc(GL_ALWAYS, 0, 0xFF);
//}
//
//
//void LightSystem::RenderAllVisualLight(Vector3& cameraPos)
//{
//	Renderer::instance()->modelMatrix.ToIdentity();
//
//	RePixelSize[0] = 1 / Renderer::instance()->GetWidth();
//	RePixelSize[1] = 1 / Renderer::instance()->GetHeight();
//
//	RenderAllDirectLight(cameraPos);
//	RenderAllPointLight(cameraPos);
//}