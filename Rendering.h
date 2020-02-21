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

		struct Portal : ECSComponent<Portal>
		{
			EntityID other;
			EntityID player;
			Transform camera;
			FrameBuffer framebuffer;
		};
	}

	void drawModels(Entities& entities, Resources& resources);
	void updatePortals(Entities& entities);
	void drawToPortals(Entities& entities, Resources& resources);
	void drawPortals(Entities& entities, Resources& resources, glm::mat4 perspective, glm::mat4 view);
}