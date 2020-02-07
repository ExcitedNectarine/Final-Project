#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Transform.h"
#include "Resources.h"
#include "FrameBuffer.h"

namespace ENG
{
	namespace CS
	{
		struct Model : ECSComponent<Model>
		{
			std::string mesh, texture, shader;
		};

		/**
		* Screen component is designed to display a mesh that uses a framebuffer as a texture,
		* rather than a texture loaded in.
		*/
		struct Screen : ECSComponent<Screen>
		{
			std::string mesh, shader;
			ENG::EntityID framebuffer_id;
		};

		struct FrameBuffer : ECSComponent<FrameBuffer>, ENG::FrameBuffer {};
	}

	void drawModels(Entities& entities, Resources& resources);
	void drawScreens(Entities& entities, Resources& resources);
	void drawToFrameBuffers(Entities& entities, Resources& resources);
}