#include <iostream>
#include <glad/glad.h>
#include "ImageLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

UncompressedImage::UncompressedImage(const std::string& fileName, int channel, bool flip) {
	int readChannels;
	stbi_set_flip_vertically_on_load(flip);
	data = stbi_load(fileName.c_str(), &width, &height, &readChannels, channel);

	if (!data) {
		std::cout << fileName << ": Load Image Fail!\n";
	}
}

UncompressedImage::~UncompressedImage() {
	stbi_image_free(data);
}

//***********************************************************************//
CompressedImage::CompressedImage(const std::string& fileName, bool flip) {
	image = gli::load(fileName);
	if (image.empty()) {
		std::cout << "The texture: " + fileName + " does not exist" << std::endl;
		return;
	}
	if (flip)
		image = gli::flip(image);
	gli::gl GL(gli::gl::PROFILE_GL33);
	mipMapLevel = image.levels();
	format = GL.translate(image.format(), image.swizzles());
}

CompressedImage::~CompressedImage() {
	image.clear();
}

gli::gl::internal_format CompressedImage::TransformFormat_SRGB(gli::gl::internal_format in_format) {
	if (in_format == gli::gl::internal_format::INTERNAL_RGB_DXT1) {
		return gli::gl::internal_format::INTERNAL_SRGB_DXT1;
	}
	else if (in_format == gli::gl::internal_format::INTERNAL_RGBA_DXT1) {
		return gli::gl::internal_format::INTERNAL_SRGB_ALPHA_DXT1;
	}
	else if (in_format == gli::gl::internal_format::INTERNAL_RGBA_DXT3) {
		return gli::gl::internal_format::INTERNAL_SRGB_ALPHA_DXT3;
	}
	else if (in_format == gli::gl::internal_format::INTERNAL_RGBA_DXT5) {
		return gli::gl::internal_format::INTERNAL_SRGB_ALPHA_DXT5;
	}
}