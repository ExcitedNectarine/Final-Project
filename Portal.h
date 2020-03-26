#pragma once

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/vector_angle.hpp>
#include "Collision.h"
#include "Rendering.h"
#include "FrameBuffer.h"
#include "Settings.h"

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
		};
	}

	void startPortals(Entities& entities, const glm::ivec2& size);
	void updatePortals(Entities& entities);
	void drawToPortals(Core& core);
	void drawPortals(Entities& entities, Resources& resources, Settings& settings, glm::mat4 perspective, glm::mat4 view);

	/**
	* Move screen position back and scale wall, so that far side is the same as when camera clips
	* near side.
	*/
	CS::Transform preventNearClipping(Settings& settings, CS::Transform screen, CS::Transform player);
}