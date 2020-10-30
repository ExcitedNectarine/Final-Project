#pragma once

#include <glm/gtx/compatibility.hpp>
#include "Portal.h"

namespace Game
{
	using namespace ENG;

	struct Pickup : ECSComponent<Pickup>
	{
		bool active = false;
		EntityID holder;
	};

	ENG::EntityID createPickup(ENG::Core& core, glm::vec3 pos);
	void updatePickups(Core& core);
}