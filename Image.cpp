#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace ENG
{
	void Image::cleanup() { stbi_image_free(pixels); }
	Image loadImage(const std::string& filename)
	{
		Image image;
		image.pixels = stbi_load(filename.c_str(), &image.size.x, &image.size.y, &image.channels, 4);
		if (!image.pixels)
			throw std::exception(std::string("Failed to load image: " + filename).c_str());
		return image;
	}
}