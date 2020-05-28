#include "Collision.h"
#include "Core.h"

namespace ENG
{
	/**
	* Move controller components through the world, blocking the movement if they encounter a solid
	* collider, and detect when controllers enter and exit triggers.
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
			transforms[a].position += controllers[a].velocity * core.delta;
			controllers[a].on_floor = false;

			CS::Transform a_t = getWorldT(core.entities, a);
			for (EntityID b : core.entities.entitiesWith<CS::Transform, CS::BoxCollider>())
			{
				if (a == b || boxes[b].trigger) continue;
				CS::Transform b_t = getWorldT(core.entities, b);

				IntersectData d;
				if (a_t.rotation == glm::vec3(0.0f) && b_t.rotation == glm::vec3(0.0f))
					d = intersectAABBvAABB(a_t.position, boxes[a].size * a_t.scale, b_t.position, boxes[b].size * b_t.scale);
				else
					d = intersectOBBvOBB(a_t, boxes[a].size, b_t, boxes[b].size);

				if (d.intersects)
				{
					transforms[a].position -= d.normal * d.distance;
					if (!controllers[a].on_floor)
						controllers[a].on_floor = approximate(d.normal.y, 1.0f, 0.1f) || approximate(d.normal.y, -1.0f, 0.1f);
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

		IntersectData d;
		d.intersects = dist < 0;
		d.distance = dist;
		d.normal = norm;
		return d;
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

	// OBB STUFF --  WORK IN PROGRESS -----------------------------

	/**
	* Get the world space vertex positions of bounding box.
	*/
	std::array<glm::vec3, 8> getBoxVerts(glm::vec3 size, glm::mat4 t)
	{
		glm::vec3 min = -size;
		glm::vec3 max = size;

		// All 8 vertices in box
		// First generate as axis aligned.
		std::array<glm::vec3, 8> verts =
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

		// Rotate, scale and translate vertices
		for (glm::vec3& v : verts)
			v = glm::vec3(t * glm::vec4(v, 1.0f));

		return verts;
	}

	/**
	* Find the minimum and maximum projected vertices of a box along an axis.
	*/
	void findMinMaxAlongAxis(glm::vec3 axis, const std::array<glm::vec3, 8>& verts, float& min, float& max)
	{
		min = std::numeric_limits<float>::max();
		max = -min;

		for (int i = 0; i < 8; i++)
		{
			float dot = glm::dot(verts[i], axis);
			min = glm::min(min, dot);
			max = glm::max(max, dot);
		}
	}

	IntersectData intersectOBBvOBB(CS::Transform a_t, glm::vec3 a_size, CS::Transform b_t, glm::vec3 b_size)
	{
		IntersectData data;
		data.intersects = true;

		//glm::vec3 dist = glm::max(a_t.position, b_t.position) - glm::min(a_t.position, b_t.position);
		glm::vec3 dist = b_t.position - a_t.position;

		// Use transformation forward and right vectors as axis, and the up vector is cross between forward and right
		glm::vec3 a_x_axis = a_t.right();
		glm::vec3 a_z_axis = a_t.forward();
		glm::vec3 a_y_axis = glm::cross(a_x_axis, a_z_axis);

		glm::vec3 b_x_axis = b_t.right();
		glm::vec3 b_z_axis = b_t.forward();
		glm::vec3 b_y_axis = glm::cross(b_x_axis, b_z_axis);

		// All 15 axis
		std::array<glm::vec3, 15> all_axes =
		{
			a_x_axis,
			a_y_axis,
			a_z_axis,
			b_x_axis,
			b_y_axis,
			b_z_axis,
			glm::cross(a_x_axis, b_x_axis),
			glm::cross(a_x_axis, b_y_axis),
			glm::cross(a_x_axis, b_z_axis),
			glm::cross(a_y_axis, b_x_axis),
			glm::cross(a_y_axis, b_y_axis),
			glm::cross(a_y_axis, b_z_axis),
			glm::cross(a_z_axis, b_x_axis),
			glm::cross(a_z_axis, b_y_axis),
			glm::cross(a_z_axis, b_z_axis)
		};

		// Get vertices of boxes A and B
		std::array<glm::vec3, 8> a_verts = getBoxVerts(a_size, a_t.get());
		std::array<glm::vec3, 8> b_verts = getBoxVerts(b_size, b_t.get());

		float shortest_overlap = std::numeric_limits<float>::max();
		for (int i = 0; i < 15; i++)
		{
			// Stops cross products being zero.
			if (all_axes[i] == glm::vec3(0.0f))
				continue;

			glm::vec3 axis = glm::normalize(all_axes[i]);

			float a_min, a_max, b_min, b_max;
			findMinMaxAlongAxis(axis, a_verts, a_min, a_max);
			findMinMaxAlongAxis(axis, b_verts, b_min, b_max);

			float proj_dist = glm::abs(glm::dot(dist, axis));
			float a_radius = glm::abs((a_max - a_min) / 2.0f);
			float b_radius = glm::abs((b_max - b_min) / 2.0f);

			// if the projected distance is greater than the projected radius of both boxes, then they don't collide on the axis.
			if (proj_dist > (a_radius + b_radius))
			{
				data.intersects = false;
				break;
			}

			else // if they intersect on the axis
			{
				float overlap_dist = glm::abs(glm::min(a_max, b_max) - glm::max(a_min, b_min));
				if (overlap_dist < shortest_overlap)
				{
					shortest_overlap = overlap_dist;
					data.normal = axis; // use the least intersecting axis as the normal to resolve collision
				}
			}
		}

		if (data.intersects)
		{
			float a_pos = glm::dot(a_t.position, data.normal);
			float b_pos = glm::dot(b_t.position, data.normal);
			data.distance = shortest_overlap;
			data.normal *= (a_pos < b_pos ? 1.0f : -1.0f);
		}

		return data;
	}
}