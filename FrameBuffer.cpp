#include "FrameBuffer.h"

namespace ENG
{
	FrameBuffer::FrameBuffer() { glGenFramebuffers(1, &id); }
	FrameBuffer::~FrameBuffer() { glDeleteFramebuffers(1, &id); }
}