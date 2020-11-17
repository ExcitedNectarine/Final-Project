#include "Pickup.h"
#include "Core.h"

namespace Game
{
	ENG::EntityID createPickup(ENG::Core& core, glm::vec3 pos)
	{
		ENG::EntityID prop = core.entities.addEntity<
			ENG::CS::Transform,
			ENG::CS::Model,
			ENG::CS::Light,
			ENG::CS::BoxCollider,
			ENG::CS::Controller,
			Game::Pickup
			//Game::Traveller
		>();

		core.entities.getComponent<ENG::CS::Transform>(prop).position = pos;
		core.entities.getComponent<ENG::CS::Light>(prop).colour = glm::normalize(glm::vec3(ENG::randomFloat(1.0f, 10.0f), ENG::randomFloat(1.0f, 10.0f), ENG::randomFloat(1.0f, 10.0f))) * 2.0f;
		core.entities.getComponent<ENG::CS::Light>(prop).radius = 10.0f;

		core.entities.getComponent<ENG::CS::Model>(prop).mesh = "lamp.obj";
		core.entities.getComponent<ENG::CS::Model>(prop).texture = "lamp.png";
		core.entities.getComponent<ENG::CS::BoxCollider>(prop).size = core.resources.mesh("lamp.obj").getSize() / 2.0f;

		return prop;
	}

	void updatePickups(Core& core)
	{
		using namespace ENG;

		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<CS::Controller>& controllers = core.entities.getPool<CS::Controller>();
		ComponentMap<Game::Pickup>& pickups = core.entities.getPool<Game::Pickup>();

		for (EntityID id : core.entities.entitiesWith<CS::Transform, Game::Pickup>())
		{
			if (pickups[id].active)
			{
				// Move pickup using controller component, instead of lerping.
				// This gives the pickup proper collisions with the world.
				glm::vec3 target = getWorldT(core.entities, pickups[id].holder).position;
				glm::vec3 distance = target - transforms[id].position;
				glm::vec3 direction = glm::normalize(distance);

				float speed = 8.0f;
				controllers[id].velocity = direction * speed;

				// If the velocity oversteps, just set the velocity equal to the distance,
				// effectively moving the pickup directly to the target.
				glm::vec3 movement_per_frame = direction * transforms[id].scale * speed * core.clock.deltaTime();
				if (glm::length(movement_per_frame) > glm::length(distance))
					controllers[id].velocity = distance;
			}
			else
			{
				controllers[id].velocity.x = 0.0f;
				controllers[id].velocity.z = 0.0f;
				if (!controllers[id].on_floor)
					controllers[id].velocity.y -= 9.8f * core.clock.deltaTime();
			}
		}
	}
}