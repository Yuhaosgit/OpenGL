//#include "Skybox.h"
//#include "../nclgl/TextureLoader.h"
//#include "Renderer.h"
//
//Skybox::Skybox(std::unique_ptr<Shader> SkyboxShader, const std::array<std::string, 6> fileName)
//	:shader(std::move(SkyboxShader)){
//	skyBox = std::make_shared<Texture2D>(TextureLoader::LoadTextureCube(fileName));
//}
//
//void Skybox::Render() {
//	glStencilMask(0x00);
//	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
//	Renderer::instance()->BindShader(shader.get());
//	Renderer::instance()->UpdateShaderMatrices();	//transport matrices to shader
//
//	Renderer::instance()->quad->Draw();
//}