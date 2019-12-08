#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace ENG
{
	class FrameBuffer
	{
	public:
		void create(const glm::ivec2& size);
		void bind();
		void unbind();
		void cleanup(); 

	private:
		GLuint id;
		GLuint tex_id;
		GLuint depth_id;
	};
}