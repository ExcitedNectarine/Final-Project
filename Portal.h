#pragma once

#include "Components.h"
#include "Settings.h"

namespace Game
{
	struct PortalCamera : ENG::Script
	{
		ENG::EntityID portal;
		ENG::EntityID other;
		ENG::EntityID player;

		PortalCamera(ENG::EntityID portal, ENG::EntityID other, ENG::EntityID player) : portal(portal), other(other), player(player) {}
		void lateUpdate(ENG::Core& core);
	};

	struct Portal : ENG::Script
	{
		ENG::CS::Transform* t;

		void start(ENG::Core& core);
	};

	std::pair<ENG::EntityID, ENG::EntityID> createPortalPair(ENG::Core& core, ENG::EntityID player);

	//using namespace ENG;

	//struct Portal : ECSComponent<Portal>
	//{
	//	EntityID other;
	//	FrameBuffer frame;
	//};

	//struct Traveller : ECSComponent<Traveller>
	//{
	//	glm::vec3 position_last_frame;
	//};

	//EntityID createPortal(Core& core);
	//void startPortals(Core& core);
	//void updatePortals(Core& core);
	//void drawToPortals(Core& core);
	//void drawPortals(Core& core);

	/**
	* Move screen position back and scale wall, so that far side is the same as when camera clips
	* near side.
	*/
	ENG::CS::Transform preventNearClipping(ENG::Settings& settings, ENG::CS::Transform screen, ENG::CS::Transform player);
}