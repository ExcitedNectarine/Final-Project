#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace ENG
{
	Texture::Texture() { glGenTextures(1, &id); }
	Texture::~Texture() { glDeleteTextures(1, &id); }

	void Texture::load(const std::string& filename)
	{
		stbi_set_flip_vertically_on_load(1);

		unsigned char* data = stbi_load(filename.c_str(), &size.x, &size.y, &channels, 4);
		if (!data)
			throw std::exception();

		if (!id)
			throw std::exception();

		glBindTexture(GL_TEXTURE_2D, id);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		stbi_image_free(data);
	}

	void Texture::bind() { glBindTexture(GL_TEXTURE_2D, id); }
	void Texture::unbind() { glBindTexture(GL_TEXTURE_2D, NULL); }
}