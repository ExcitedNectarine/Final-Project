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
		ENG::EntityID other;
		ENG::EntityID screen;
		std::map<ENG::EntityID, int> travellers;

		Portal(ENG::EntityID other, ENG::EntityID screen) : other(other), screen(screen) {}
		void start(ENG::Core& core);
		void update(ENG::Core& core);
		void onTriggerEnter(ENG::Core& core, ENG::EntityID entered_id);
		void onTriggerExit(ENG::Core& core, ENG::EntityID exited_id);

		float cornerDistance(const ENG::CS::Camera& camera);
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
}