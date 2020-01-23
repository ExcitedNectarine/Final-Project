#include "Collision.h"

namespace ENG
{
	void testCollisions(ENG::Entities& entities, ENG::Application& app)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& spheres = entities.getPool<CS::SphereCollider>();
		auto& boxes = entities.getPool<CS::BoxCollider>();
		auto& scripts = entities.getPool<CS::Script>();

		std::vector<ENG::EntityID> sphere_entities = entities.entitiesWith<CS::Transform, CS::SphereCollider>();
		std::vector<ENG::EntityID> box_entities = entities.entitiesWith<CS::Transform, CS::BoxCollider>();

		for (ENG::EntityID a : sphere_entities)
		{
			for (ENG::EntityID b : sphere_entities) // test spheres against other spheres
			{
				if (a == b) continue;

				glm::vec3 distance = transforms[a].position - transforms[b].position;
				float length = glm::length(distance);
				float radii_sum = spheres[a].radius + spheres[b].radius;

				if (length <= radii_sum) // collision
				{
					if (spheres[a].solid && spheres[b].solid)
						transforms[a].position = transforms[b].position + (radii_sum * distance / length);
					else if (entities.hasComponent<CS::Script>(a))
						scripts[a].script->onCollision(app, b);
				}
			}

			for (ENG::EntityID b : box_entities)
			{
				if (a == b) continue;
			}
		}

		for (ENG::EntityID a : box_entities)
		{
			glm::vec3 vel = transforms[a].position - boxes[a].prev_position; // work out velocity of box a
			boxes[a].prev_position = transforms[a].position;

			for (ENG::EntityID b : box_entities)
			{
				if (a == b) continue;

				glm::vec3 a_size = boxes[a].size;
				glm::vec3 a_pos = transforms[a].position - (a_size / 2.0f);
				glm::vec3 b_size = boxes[b].size;
				glm::vec3 b_pos = transforms[b].position - (b_size / 2.0f);

				if (boxes[a].solid && boxes[b].solid)
				{
					glm::vec3 new_pos = a_pos;

					new_pos.x += vel.x;
					if (AABBcollision(new_pos, a_size, b_pos, b_size))
					{
						if (vel.x < 0.0f) new_pos.x = b_pos.x + b_size.x;
						else if (vel.x > 0.0f) new_pos.x = b_pos.x - a_size.x;

						transforms[a].position.x = new_pos.x + (a_size.x / 2.0f);
					}

					new_pos.y += vel.y;
					if (AABBcollision(new_pos, a_size, b_pos, b_size))
					{
						if (vel.y < 0.0f) new_pos.y = b_pos.y + b_size.y;
						else if (vel.y > 0.0f) new_pos.y = b_pos.y - a_size.y;

						transforms[a].position.y = new_pos.y + (a_size.y / 2.0f);
					}

					new_pos.z += vel.z;
					if (AABBcollision(new_pos, a_size, b_pos, b_size))
					{
						if (vel.z < 0.0f) new_pos.z = b_pos.z + b_size.z;
						else if (vel.z > 0.0f) new_pos.z = b_pos.z - a_size.z;

						transforms[a].position.z = new_pos.z + (a_size.z / 2.0f);
					}

					//a_pos = new_pos;
				}
				else if (AABBcollision(a_pos, a_size, b_pos, b_size))
					if (entities.hasComponent<CS::Script>(a))
						scripts[a].script->onCollision(app, b);
			}
		}
	}

	bool AABBcollision(const glm::vec3& a_pos, const glm::vec3& a_size, const glm::vec3& b_pos, const glm::vec3& b_size)
	{
		return a_pos.x < b_pos.x + b_size.x &&
			a_pos.x + a_size.x > b_pos.x &&
			a_pos.y < b_pos.y + b_size.y &&
			a_pos.y + a_size.y > b_pos.y &&
			a_pos.z < b_pos.z + b_size.z &&
			a_pos.z + a_size.z > b_pos.z;
	}
}