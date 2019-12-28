#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Output.h"

namespace ENG
{
	class FrameBuffer
	{
	public:
		FrameBuffer(const glm::ivec2& size);
		void create(const glm::ivec2& size);
		void bind();
		void unbind();
		void bindTex();
		void unbindTex();
		void cleanup(); 

	private:
		GLuint id;
		GLuint tex_id;
		GLuint depth_id;
	};
}