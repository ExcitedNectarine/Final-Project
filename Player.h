#pragma once

#include "Collision.h"

namespace Game
{
	using namespace ENG;

	struct Player : ECSComponent<Player>
	{
		glm::vec3 direction;
		glm::vec3 velocity;
		float speed = 6.0f;
		glm::dvec2 last_mouse;
		glm::dvec2 mouse_offset;
		float sensitivity = 0.05f;
		float dist;
		ENG::EntityID pickup = 0;
	};

	void updatePlayers(Core& core);
}