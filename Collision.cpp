#include "Collision.h"
#include "Core.h"

namespace ENG
{
	/**
	* Move controller components through the world, blocking the movement if they encounter a solid
	* collider.
	*/
	void moveControllers(Entities& entities, Core& core)
	{
		ComponentMap<CS::Transform>& transforms = entities.getPool<CS::Transform>();
		ComponentMap<CS::Controller>& controllers = entities.getPool<CS::Controller>();
		ComponentMap<CS::BoxCollider>& boxes = entities.getPool<CS::BoxCollider>();
		ComponentMap<CS::Script>& scripts = entities.getPool<CS::Script>();

		std::vector<EntityID> box_ents = entities.entitiesWith<CS::Transform, CS::BoxCollider>();

		for (EntityID a : entities.entitiesWith<CS::Transform, CS::Controller, CS::BoxCollider>())
		{
			glm::vec3 new_pos = transforms[a].position;
			glm::vec3 a_size = boxes[a].size * transforms[a].scale;

			// Move and check collision along X axis.
			new_pos.x += controllers[a].velocity.x * core.delta;
			for (EntityID b : box_ents)
			{
				if (a == b || !boxes[b].solid) continue;

				glm::vec3 b_size = boxes[b].size * transforms[b].scale;
				if (intersectAABBvAABB(new_pos, a_size, transforms[b].position, b_size))
				{
					if (controllers[a].velocity.x > 0.0f) new_pos.x = (transforms[b].position.x - (b_size.x / 2.0f)) - (a_size.x / 2.0f);
					if (controllers[a].velocity.x < 0.0f) new_pos.x = (transforms[b].position.x + (b_size.x / 2.0f)) + (a_size.x / 2.0f);
				}
			}

			// Move and check collision along Y axis.
			new_pos.y += controllers[a].velocity.y * core.delta;
			for (EntityID b : box_ents)
			{
				if (a == b || !boxes[b].solid) continue;

				glm::vec3 b_size = boxes[b].size * transforms[b].scale;
				if (intersectAABBvAABB(new_pos, a_size, transforms[b].position, b_size))
				{
					if (controllers[a].velocity.y > 0.0f) new_pos.y = (transforms[b].position.y - (b_size.y / 2.0f)) - (a_size.y / 2.0f);
					if (controllers[a].velocity.y < 0.0f)
					{
						new_pos.y = (transforms[b].position.y + (b_size.y / 2.0f)) + (a_size.y / 2.0f);
						controllers[a].on_floor = true;
					}
				}
			}

			// Move and check collision along Z axis.
			new_pos.z += controllers[a].velocity.z * core.delta;
			for (EntityID b : box_ents)
			{
				if (a == b || !boxes[b].solid) continue;

				glm::vec3 b_size = boxes[b].size * transforms[b].scale;
				if (intersectAABBvAABB(new_pos, a_size, transforms[b].position, b_size))
				{
					if (controllers[a].velocity.z > 0.0f) new_pos.z = (transforms[b].position.z - (b_size.z / 2.0f)) - (a_size.z / 2.0f);
					if (controllers[a].velocity.z < 0.0f) new_pos.z = (transforms[b].position.z + (b_size.z / 2.0f)) + (a_size.z / 2.0f);
				}
			}

			transforms[a].position = new_pos;
		}
	}

	/**
	* Cast a ray into the world, and return the closest intersecting box ID.
	*/
	EntityID castRay(Entities& entities, const glm::vec3& r_pos, const glm::vec3& r_dir, EntityID ignore, float& t)
	{
		ComponentMap<CS::Transform>& transforms = entities.getPool<CS::Transform>();
		ComponentMap<CS::BoxCollider>& boxes = entities.getPool<CS::BoxCollider>();

		float t1 = 0.0f;
		std::map<float, EntityID> d;
		for (EntityID id : entities.entitiesWith<CS::Transform, CS::BoxCollider>())
		{
			if (id == ignore || !boxes[id].solid) continue;

			if (intersectAABBvRay(transforms[id].position, boxes[id].size * transforms[id].scale, r_pos, r_dir, t1))
				d[t1] = id;
		}

		if (d.size() > 0)
		{
			t = d.begin()->first;
			return d.begin()->second;
		}

		return 0;
	}

	/**
	* Checks if two axis-aligned (not rotated) bounding boxes are colliding
	*/
	bool intersectAABBvAABB(glm::vec3 a_pos, const glm::vec3& a_size, glm::vec3 b_pos, const glm::vec3& b_size)
	{
		a_pos -= a_size / 2.0f;
		b_pos -= b_size / 2.0f;

		return (a_pos.x < b_pos.x + b_size.x &&
				a_pos.x + a_size.x > b_pos.x &&
				a_pos.y < b_pos.y + b_size.y &&
				a_pos.y + a_size.y > b_pos.y &&
				a_pos.z < b_pos.z + b_size.z &&
				a_pos.z + a_size.z > b_pos.z);
	}

	/**
	* Checks if a ray intersects an AABB.
	*/
	bool intersectAABBvRay(glm::vec3 b_pos, const glm::vec3& b_size, glm::vec3 r_pos, const glm::vec3& r_dir, float& t)
	{
		glm::vec3 dir_frac = 1.0f / r_dir;

		glm::vec3 b_min = b_pos - (b_size / 2.0f);
		glm::vec3 b_max = b_pos + (b_size / 2.0f);

		glm::vec3 t1 = (b_min - r_pos) * dir_frac;
		glm::vec3 t2 = (b_max - r_pos) * dir_frac;

		float tmin = glm::max(glm::max(glm::min(t1.x, t2.x), glm::min(t1.y, t2.y)), glm::min(t1.z, t2.z));
		float tmax = glm::min(glm::min(glm::max(t1.x, t2.x), glm::max(t1.y, t2.y)), glm::max(t1.z, t2.z));

		if (tmax < 0.0f)
		{
			t = tmax;
			return false;
		}

		if (tmin > tmax)
		{
			t = tmax;
			return false;
		}

		t = tmin;
		return true;
	}
}