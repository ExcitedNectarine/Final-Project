#include "FrameBuffer.h"

namespace ENG
{
	FrameBuffer::FrameBuffer() {}
	FrameBuffer::FrameBuffer(const glm::ivec2& size) { create(size); }

	void FrameBuffer::create(const glm::ivec2& size)
	{
		texture.createEmpty(size);

		glGenFramebuffers(1, &id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);

		glGenRenderbuffers(1, &depth_id);
		glBindRenderbuffer(GL_RENDERBUFFER, depth_id);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.getID(), 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_id);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			throw std::exception("ENG::FrameBuffer failed.");
	}

	void FrameBuffer::resize(const glm::ivec2& size)
	{
		if (id == 0) create(size);
		else
		{
			texture.resize(size);
			bind();
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
		}
	}

	void FrameBuffer::bind() { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id); }
	void FrameBuffer::unbind() { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, NULL); }
	void FrameBuffer::cleanup() { glDeleteFramebuffers(1, &id); }
	Texture& FrameBuffer::getTexture() { return texture; }
}