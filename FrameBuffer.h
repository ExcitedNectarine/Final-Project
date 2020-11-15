#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Texture.h"

namespace ENG
{
	/**
	* Simple framebuffer, if drawn to, will draw to a texture that can be retrieved using getTexture().
	*/
	class FrameBuffer
	{
	public:
		FrameBuffer();
		FrameBuffer(const glm::ivec2& size);
		void create(const glm::ivec2& size);
		void resize(const glm::ivec2& size);
		void bind();
		void unbind();
		void cleanup();
		Texture& getTexture();

	private:
		GLuint id = 0;
		GLuint depth_id;
		Texture texture;
	};
}