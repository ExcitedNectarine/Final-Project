#include "Texture.h"

namespace ENG
{
	void Texture::createEmpty(const glm::ivec2& size)
	{
		this->size = size;

		glGenTextures(1, &id);
		
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	void Texture::createFromImage(const Image& image)
	{
		size = image.size;

		glGenTextures(1, &id);

		glBindTexture(GL_TEXTURE_2D, id);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	void Texture::resize(const glm::ivec2& size)
	{
		this->size = size;
		bind();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	void Texture::bind() { glBindTexture(GL_TEXTURE_2D, id); }
	void Texture::unbind() { glBindTexture(GL_TEXTURE_2D, NULL); }
	void Texture::cleanup() { glDeleteTextures(1, &id); }
	GLuint Texture::getID() { return id; }
	glm::ivec2 Texture::getSize() { return size; }
}