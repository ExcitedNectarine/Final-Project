#include "Collision.h"
#include "Core.h"

namespace ENG
{
	/**
	* Move controller components through the world, blocking the movement if they encounter a solid
	* collider.
	*/
	void moveControllers(Core& core)
	{
		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<CS::Controller>& controllers = core.entities.getPool<CS::Controller>();
		ComponentMap<CS::BoxCollider>& boxes = core.entities.getPool<CS::BoxCollider>();

		for (EntityID a : core.entities.entitiesWith<CS::Transform, CS::Controller, CS::BoxCollider>())
		{
			glm::vec3 a_size = boxes[a].size * transforms[a].scale;

			transforms[a].position += controllers[a].velocity * core.delta;
			controllers[a].on_floor = false;

			for (EntityID b : core.entities.entitiesWith<CS::Transform, CS::BoxCollider>())
			{
				if (a == b || !boxes[b].solid) continue;

				glm::vec3 b_size = boxes[b].size * transforms[b].scale;
				IntersectData d = intersectAABBvAABB(transforms[a].position, a_size, transforms[b].position, b_size);
				if (d.intersects)
				{
					transforms[a].position -= d.normal * d.distance;
					if (!controllers[a].on_floor)
						controllers[a].on_floor = approximate(d.normal.y, 1.0f, 0.1f);
				}
			}
		}
	}

	/**
	* Cast a ray into the world, and return the closest intersecting box ID.
	*/
	EntityID castRay(Entities& entities, const glm::vec3& r_pos, const glm::vec3& r_dir, EntityID ignore, float& t)
	{
		ComponentMap<CS::Transform>& transforms = entities.getPool<CS::Transform>();
		ComponentMap<CS::BoxCollider>& boxes = entities.getPool<CS::BoxCollider>();

		float distance = 0.0f;
		std::map<float, EntityID> distances;
		for (EntityID id : entities.entitiesWith<CS::Transform, CS::BoxCollider>())
		{
			if (id == ignore || !boxes[id].solid) continue;

			if (intersectAABBvRay(transforms[id].position, boxes[id].size * transforms[id].scale, r_pos, r_dir, distance))
				distances[distance] = id;
		}

		if (distances.size() > 0)
		{
			t = distances.begin()->first;
			return distances.begin()->second;
		}

		return 0;
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

	/**
	* Checks if an AABB intersects a plane.
	*/
	bool intersectAABBvPlane(glm::vec3 b_pos, glm::vec3 b_size, glm::vec3 p_pos, glm::vec3 p_norm)
	{
		glm::vec3 bhs = b_size / 2.0f;
		glm::vec3 b_min = b_pos - bhs;
		glm::vec3 b_max = b_pos + bhs;

		// All 8 vertices in AABB
		glm::vec3 verts[8] =
		{
			b_min,
			b_max,
			{b_max.x, b_min.y, b_min.z},
			{b_min.x, b_max.y, b_min.z},
			{b_min.x, b_min.y, b_max.z},
			{b_min.x, b_max.y, b_max.z},
			{b_max.x, b_min.y, b_max.z},
			{b_max.x, b_max.y, b_min.z}
		};

		// Check if all vertices are on the same side of the plane. If not, then they intersect
		int sign = 0, prev_sign = static_cast<int>(glm::sign(glm::dot(p_norm, p_pos - verts[0])));
		for (int i = 1; i < 8; i++)
		{
			sign = static_cast<int>(glm::sign(glm::dot(p_norm, p_pos - verts[i])));
			if (sign != prev_sign)
				return true;

			prev_sign = sign;
		}

		return false;
	}

	///

	/**
	* Checks if two axis-aligned (not rotated) bounding boxes are colliding
	*/
	IntersectData intersectAABBvAABB(const glm::vec3& a_pos, const glm::vec3& a_size, const glm::vec3& b_pos, const glm::vec3& b_size)
	{
		glm::vec3 a_min = a_pos - (a_size / 2.0f);
		glm::vec3 a_max = a_pos + (a_size / 2.0f);
		glm::vec3 b_min = b_pos - (b_size / 2.0f);
		glm::vec3 b_max = b_pos + (b_size / 2.0f);

		glm::vec3 dist1 = b_min - a_max;
		glm::vec3 dist2 = a_min - b_max;
		glm::vec3 max_dist(
			glm::max(dist1.x, dist2.x),
			glm::max(dist1.y, dist2.y),
			glm::max(dist1.z, dist2.z)
		);

		// Max distance on X, Y and Z. If less than 0, the AABBs intersect
		float dist = glm::max(max_dist.x, glm::max(max_dist.y, max_dist.z));

		// Find which axis the distance is on, and which direction to move in.
		glm::vec3 norm(
			approximate(dist, max_dist.x, 0.01f) ? (a_pos.x > b_pos.x ? 1.0f : -1.0f) : 0.0f,
			approximate(dist, max_dist.y, 0.01f) ? (a_pos.y > b_pos.y ? 1.0f : -1.0f) : 0.0f,
			approximate(dist, max_dist.z, 0.01f) ? (a_pos.z > b_pos.z ? 1.0f : -1.0f) : 0.0f
		);

		return { dist < 0, dist, norm };
	}

	/**
	* Checks if a ray intersects an AABB.
	*/
	IntersectData intersectAABBvRay2(glm::vec3 b_pos, const glm::vec3& b_size, glm::vec3 r_pos, const glm::vec3& r_dir)
	{
		glm::vec3 dir_frac = 1.0f / r_dir;

		glm::vec3 b_min = b_pos - (b_size / 2.0f);
		glm::vec3 b_max = b_pos + (b_size / 2.0f);

		glm::vec3 t1 = (b_min - r_pos) * dir_frac;
		glm::vec3 t2 = (b_max - r_pos) * dir_frac;

		float tmin = glm::max(glm::max(glm::min(t1.x, t2.x), glm::min(t1.y, t2.y)), glm::min(t1.z, t2.z));
		float tmax = glm::min(glm::min(glm::max(t1.x, t2.x), glm::max(t1.y, t2.y)), glm::max(t1.z, t2.z));

		IntersectData data;

		if (tmax < 0.0f || tmin > tmax)
			data.distance = tmax;
		else
		{
			data.distance = tmin;
			data.intersects = true;
		}

		return data;
	}

	/**
	* Checks if an AABB intersects a plane.
	*/
	IntersectData intersectAABBvPlane2(glm::vec3 b_pos, glm::vec3 b_size, glm::vec3 p_pos, glm::vec3 p_norm)
	{
	}
}