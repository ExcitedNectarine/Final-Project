#include "Collision.h"

namespace ENG
{
	/**
	* Tests every AABB component for collisions against any other AABB components.
	*/
	void AABBCollision(ENG::Entities& entities)
	{
		auto& ts = entities.getPool<CS::Transform>();
		auto& aabbs = entities.getPool<CS::AABB>();

		std::vector<ENG::EntityID> ents = entities.entitiesWith<CS::Transform, CS::AABB>();
		for (ENG::EntityID a : ents)
		{
			glm::vec3 a_pos = ts[a].position;
			glm::vec3 a_size = aabbs[a].size;

			//a_pos -= a_size / 2.0f;

			for (ENG::EntityID b : ents)
			{
				if (b == a) continue;

				glm::vec3 b_pos = ts[b].position;
				glm::vec3 b_size = aabbs[b].size;

				//b_pos -= b_pos / 2.0f;

				if (a_pos.x < b_pos.x + b_size.x &&
					a_pos.x + a_size.x > b_pos.x &&
					a_pos.y < b_pos.y + b_size.y &&
					a_pos.y + a_size.y > b_pos.y &&
					a_pos.z < b_pos.z + b_size.z &&
					a_pos.z + a_size.z > b_pos.z)
				{
					aabbs[a].hit = true;
					aabbs[a].hit_id = b; // store which entity it hit

					if (aabbs[a].solid && aabbs[b].solid) // collision response
					{
					}

					break;
				}
				else
				{
					aabbs[a].hit = false;
				}
			}
		}
	}
}