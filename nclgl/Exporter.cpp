#include <filesystem>
#include "Exporter.h"
#include "GameObject.h"
#include "FrameBuffer.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void Exporter::ExportCubemap(const std::string& path, IBL* ibl) {
	std::string name[6] = { "Left.png","Right.png","Up.png","Down.png","Front.png","Back.png" };

	int width = ibl->gameObject->GetComponent<Camera>()->width;
	int height = ibl->gameObject->GetComponent<Camera>()->height;
	int dataSize = 3 * width * height;

	auto indirectDiffuseBuffer = FrameBuffer<IndirectDiffuseFBO>().instance();
	for (int i = 0; i < 6; ++i) {
		GLubyte* data = new GLubyte[dataSize];
		FrameBuffer<IndirectDiffuseFBO>().instance()->Bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, indirectDiffuseBuffer->indirectDiffuse->texture, 0);
		glReadBuffer(GL_COLOR_ATTACHMENT0);

		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

		WriteToFile((path + name[i]).c_str(), width, height, 3, data);
	}
}

void Exporter::WriteToFile(const std::string& fileName, int width, int height, int channels, void* data) {
	if (!std::filesystem::exists(fileName)) {

	}
	int strideInBytes = width * channels;
	stbi_write_png(fileName.c_str(), width, height, channels, data, strideInBytes);
}