#pragma once

#include "Collision.h"
#include "Rendering.h"
#include "FrameBuffer.h"
#include "Settings.h"

namespace Game
{
	using namespace ENG;

	struct Portal : ECSComponent<Portal>
	{
		EntityID other;
		EntityID player;
		glm::mat4 camera;
		FrameBuffer frame;
		int prev_side = 0;
	};

	void startPortals(Entities& entities, const glm::ivec2& size);
	void updatePortals(Entities& entities);
	void drawToPortals(Core& core);
	void drawPortals(Core& core);

	/**
	* Move screen position back and scale wall, so that far side is the same as when camera clips
	* near side.
	*/
	CS::Transform preventNearClipping(Settings& settings, CS::Transform screen, CS::Transform player);
}