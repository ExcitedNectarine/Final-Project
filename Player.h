#pragma once

#include "Portal.h"
#include "Pickup.h"

namespace Game
{
	ENG::EntityID createPlayer(ENG::Core& core);

	struct Player: ENG::Script
	{
		Player() : last_mouse(0.0f), mouse_offset(0.0f), velocity(0.0f) {}

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
		IntersectData ray;
		ENG::EntityID pickup_position = 0;
		ENG::EntityID pos_text;

		void start(ENG::Core& core);
		void mouselook(ENG::Core& core);
		void movement(ENG::Core& core);
		void actions(ENG::Core& core);
		void update(ENG::Core& core);
		void onTriggerEnter(ENG::Core& core, ENG::EntityID hit_id);
	};
}