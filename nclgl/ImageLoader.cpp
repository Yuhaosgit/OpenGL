#include <iostream>

#include "ImageLoader.h"
#include "stb_image.h"

PNGInfo ImageLoader::LoadPNG(const std::string& fileName) {
	PNGInfo info;

	stbi_set_flip_vertically_on_load(true);
	info.data = stbi_load(fileName.c_str(), &info.width, &info.height, &info.channels, 3);

	if (!info.data) {
		std::cout << "Load Image Fail!\n";
	}
	return info;
}

void PNGInfo::Release() {
	if (data)
		stbi_image_free(data);
}