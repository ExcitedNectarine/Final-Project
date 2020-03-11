#pragma once

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "Collision.h"
#include "Rendering.h"
#include "FrameBuffer.h"

namespace ENG
{
	namespace CS
	{
		struct Portal : ECSComponent<Portal>
		{
			EntityID other;
			EntityID player;
			glm::mat4 camera;
			FrameBuffer framebuffer;
			int prev_side = 0;
			bool active = true;
		};
	}

	void startPortals(Entities& entities, const glm::ivec2& size);
	void updatePortals(Entities& entities);
	void drawToPortals(Entities& entities, Resources& resources, CS::Camera cam);
	void drawPortals(Entities& entities, Resources& resources, CS::Camera cam, glm::mat4 view);
}