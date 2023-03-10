#include <filesystem>
#include "Exporter.h"
#include "GameObject.h"
#include "FrameBuffer.h"
#include "Texture.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void Exporter::ExportCubemap(const std::string& path, RenderTexture* exportingTexture, int mipLevel) {
	CheckFolderExist(path);
	std::string name[6] = { "Left.png","Right.png","Up.png","Down.png","Front.png","Back.png" };

	int width = exportingTexture->width * std::pow(0.5, mipLevel);
	int height = exportingTexture->height * std::pow(0.5, mipLevel);
	int dataSize = 3 * width * height;

	auto indirectDiffuseBuffer = FrameBuffer<EnvironmentMapFBO>().instance();
	for (int i = 0; i < 6; ++i) {
		GLubyte* data = new GLubyte[dataSize];
		FrameBuffer<EnvironmentMapFBO>().instance()->Bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, indirectDiffuseBuffer->environmentTarget->GetTextureContent(), mipLevel);
		glReadBuffer(GL_COLOR_ATTACHMENT0);

		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

		int channels = 3;
		int strideInBytes = channels * width;
		stbi_write_png((path + name[i]).c_str(), width, height, channels, data, strideInBytes);
	}
}

void Exporter::ExportTexture2D
(const std::string& path, const std::string& fileName, RenderTexture* exportingTexture, int mipLevel) {
	CheckFolderExist(path);

	int width = exportingTexture->width * std::pow(0.5, mipLevel);
	int height = exportingTexture->height * std::pow(0.5, mipLevel);
	int dataSize = 3 * width * height;

	auto specularLUTBuffer = FrameBuffer<SpecularLUTFBO>().instance();
	GLubyte* data = new GLubyte[dataSize];
	specularLUTBuffer->Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D, specularLUTBuffer->LUT_Target->GetTextureContent(), mipLevel);
	glReadBuffer(GL_COLOR_ATTACHMENT0);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

	int channels = 3;
	int strideInBytes = channels * width;
	stbi_write_png((path + fileName).c_str(), width, height, channels, data, strideInBytes);
}

void Exporter::CheckFolderExist(const std::string& path) {
	if (!std::filesystem::exists(path)) {
		std::filesystem::create_directories(path);
	}
}