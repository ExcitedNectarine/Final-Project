#include "FrameBuffer.h"

namespace ENG
{
	void FrameBuffer::create(const glm::ivec2& size)
	{
		glGenFramebuffers(1, &id);
		glBindFramebuffer(GL_FRAMEBUFFER, id);

		glGenTextures(1, &tex_id);
		glBindTexture(GL_TEXTURE_2D, tex_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 

		glGenRenderbuffers(1, &depth_id);
		glBindRenderbuffer(GL_RENDERBUFFER, depth_id);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_id, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_id);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			throw std::exception("ENG::FrameBuffer failed.");
	}

	void FrameBuffer::bind() { glBindFramebuffer(GL_FRAMEBUFFER, id); }
	void FrameBuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, NULL); }
	void FrameBuffer::cleanup() { glDeleteFramebuffers(1, &id); }
}