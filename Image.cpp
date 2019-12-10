#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION

namespace ENG
{
	void Image::cleanup() { stbi_image_free(pixels); }
	Image loadImage(const std::string& filename)
	{
		Image image;
		image.pixels = stbi_load(filename.c_str(), &image.size.x, &image.size.y, &image.channels, 4);
		return image;
	}
}