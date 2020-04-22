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
		ComponentMap<CS::PlaneCollider>& planes = core.entities.getPool<CS::PlaneCollider>();
		ComponentMap<CS::Script>& scripts = core.entities.getPool<CS::Script>();

		for (EntityID a : core.entities.entitiesWith<CS::Transform, CS::Controller, CS::BoxCollider>())
		{
			glm::vec3 a_size = boxes[a].size * transforms[a].scale;

			transforms[a].position += controllers[a].velocity * core.delta;
			controllers[a].on_floor = false;

			for (EntityID b : core.entities.entitiesWith<CS::Transform, CS::BoxCollider>())
			{
				if (a == b) continue;

				CS::Transform t = getWorldT(core.entities, b);

				glm::vec3 b_size = boxes[b].size * t.scale;
				
				IntersectData d = intersectAABBvAABB(getWorldT(core.entities, a).position, a_size, t.position, b_size);
				if (d.intersects)
				{
					transforms[a].position -= d.normal * d.distance;
					if (!controllers[a].on_floor)
						controllers[a].on_floor = approximate(d.normal.y, 1.0f, 0.1f);

					if (boxes[b].trigger)
					{
						boxes[b]._collided = true;
						scripts[a].script->onTriggerEnter(core, b);
					}
				}
				else if (boxes[b]._collided)
				{
					boxes[b]._collided = false;
					scripts[a].script->onTriggerExit(core, b);
				}
			}

			for (EntityID b : core.entities.entitiesWith<CS::Transform, CS::PlaneCollider>())
			{
				if (a == b) continue;

				CS::Transform t = getWorldT(core.entities, b);
				glm::vec3 b_size = planes[b].size * t.scale;

				IntersectData d = intersectAABBvPlane(getWorldT(core.entities, a).position, a_size, t.position, -t.forward(), b_size);
				if (d.intersects)
				{
					// using vec3(0, -1, 0) allows moving up and down slopes
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

		IntersectData d;
		std::map<float, EntityID> distances;
		for (EntityID id : entities.entitiesWith<CS::Transform, CS::BoxCollider>())
		{
			if (id == ignore || boxes[id].trigger) continue;

			CS::Transform t = getWorldT(entities, id);

			d = intersectAABBvRay(t.position, boxes[id].size * t.scale, r_pos, r_dir);
			if (d.intersects)
				distances[d.distance] = id;
		}

		if (distances.size() > 0)
		{
			t = distances.begin()->first;
			return distances.begin()->second;
		}

		return 0;
	}

	/**
	* Checks if two axis-aligned (not rotated) bounding boxes are colliding
	*/
	IntersectData intersectAABBvAABB(const glm::vec3& a_pos, const glm::vec3& a_size, const glm::vec3& b_pos, const glm::vec3& b_size)
	{
		// Maximum distance between both AABBs mins and maxes on all axis.
		glm::vec3 max_dist = glm::max((b_pos - b_size) - (a_pos + a_size), (a_pos - a_size) - (b_pos + b_size));

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
	IntersectData intersectAABBvRay(glm::vec3 b_pos, const glm::vec3& b_size, glm::vec3 r_pos, const glm::vec3& r_dir)
	{
		glm::vec3 dir_frac = 1.0f / r_dir;

		glm::vec3 b_min = b_pos - b_size;
		glm::vec3 b_max = b_pos + b_size;

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
	IntersectData intersectAABBvPlane(glm::vec3 b_pos, glm::vec3 b_size, glm::vec3 p_pos, glm::vec3 p_norm, glm::vec3 p_size)
	{
		glm::vec3 min = b_pos - b_size;
		glm::vec3 max = b_pos + b_size;

		// All 8 vertices in AABB
		glm::vec3 verts[8] =
		{
			min,
			max,
			{ max.x, min.y, min.z },
			{ min.x, max.y, min.z },
			{ min.x, min.y, max.z },
			{ min.x, max.y, max.z },
			{ max.x, min.y, max.z },
			{ max.x, max.y, min.z }
		};

		IntersectData data;
		data.normal = glm::dot(p_norm, p_pos - b_pos) > 0 ? p_norm : -p_norm;

		// Check if all vertices are on the same side of the plane. If not, then they intersect
		float dist = 0.0f, last = std::numeric_limits<float>::max();
		int sign = 0, prev_sign = static_cast<int>(glm::sign(glm::dot(p_norm, p_pos - verts[0])));
		for (int i = 0; i < 8; i++)
		{
			sign = static_cast<int>(glm::sign(glm::dot(p_norm, p_pos - verts[i])));
			if (sign != prev_sign)
			{
				data.intersects = true;

				// Always use shortest distance to plane
				dist = glm::abs(glm::dot(p_pos - verts[i], p_norm));
				data.distance = glm::min(dist, last);
				last = data.distance;
			}

			prev_sign = sign;
		}

		return data;
	}

	/**
	* Checks if a ray intersects a plane.
	*/
	IntersectData intersectPlaneVRay(glm::vec3 p_pos, glm::vec3 p_norm, glm::vec3 r_pos, glm::vec3 r_dir)
	{
		IntersectData data;

		return data;
	}
}