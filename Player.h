#pragma once

#include "Core.h"

namespace Game
{
	struct Pickup : ENG::ECSComponent<Pickup>
	{
		bool active = false;
	};

	ENG::EntityID createPlayer(ENG::Core& core);
	ENG::EntityID createPickup(ENG::Core& core, glm::vec3 pos);

	struct Player: ENG::Script
	{
		ENG::CS::Transform* transform;
		ENG::CS::Controller* controller;
		ENG::CS::BoxCollider* box;
		glm::vec3 direction;
		glm::vec3 velocity;
		glm::vec3 rotation;
		float speed = 6.0f;

		glm::dvec2 last_mouse;
		glm::dvec2 mouse_offset;
		float sensitivity = 0.05f;
		float dist;
		ENG::EntityID pickup = 0;

		void start(ENG::Core& core);
		void mouselook(ENG::Core& core);
		void movement(ENG::Core& core);
		void actions(ENG::Core& core);
		void update(ENG::Core& core);
	};
}