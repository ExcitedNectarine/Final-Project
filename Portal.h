#pragma once

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/closest_point.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "Collision.h"
#include "Rendering.h"
#include "FrameBuffer.h"
#include "Camera.h"

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
			float prev_angle = 0;
			bool active = true;
		};
	}

	void startPortals(Entities& entities);
	void updatePortals(Entities& entities);
	void drawToPortals(Entities& entities, Resources& resources);
	void drawPortals(Entities& entities, Resources& resources, glm::mat4 perspective, glm::mat4 view);
}