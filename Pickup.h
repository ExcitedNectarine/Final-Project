#pragma once

#include <glm/gtx/compatibility.hpp>
#include "Portal.h"

namespace Game
{
	struct Pickup : ENG::Script
	{
		const float SPEED = 10.0f;

		ENG::CS::Controller* controller;
		ENG::EntityID target;

		void start(ENG::Core& core);
		void update(ENG::Core& core);
	};

	ENG::EntityID createPickup(ENG::Core& core, glm::vec3 pos);

	//using namespace ENG;

	//struct Pickup : ECSComponent<Pickup>
	//{
	//	bool active = false;
	//	EntityID holder;
	//};
	//void updatePickups(Core& core);
}