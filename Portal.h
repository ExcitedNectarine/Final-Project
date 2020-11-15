#pragma once

#include "Components.h"
#include "Settings.h"

namespace Game
{
	using namespace ENG;

	struct Portal : ECSComponent<Portal>
	{
		EntityID other;
		FrameBuffer frame;
	};

	struct Traveller : ECSComponent<Traveller>
	{
		glm::vec3 position_last_frame;
	};

	EntityID createPortal(Core& core);
	void startPortals(Core& core);
	void updatePortals(Core& core);
	void drawToPortals(Core& core);
	void drawPortals(Core& core);

	/**
	* Move screen position back and scale wall, so that far side is the same as when camera clips
	* near side.
	*/
	CS::Transform preventNearClipping(Settings& settings, CS::Transform screen, CS::Transform player);
}