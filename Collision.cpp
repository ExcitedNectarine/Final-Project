#include "Collision.h"
#include "Core.h"
#include "Rendering.h"

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
		ComponentMap<CS::Script>& scripts = core.entities.getPool<CS::Script>();

		for (EntityID a : core.entities.entitiesWith<CS::Transform, CS::Controller, CS::BoxCollider>())
		{
			// Should controllers that are triggers be updated?
			if (boxes[a].trigger) continue;

			transforms[a].position += controllers[a].velocity * core.delta;
			controllers[a].on_floor = false;

			CS::Transform a_t = getWorldT(core.entities, a);
			for (EntityID b : core.entities.entitiesWith<CS::Transform, CS::BoxCollider>())
			{
				if (a == b) continue;
				CS::Transform b_t = getWorldT(core.entities, b);

				IntersectData d;
				if (a_t.rotation == glm::vec3(0.0f) && b_t.rotation == glm::vec3(0.0f))
					d = intersectAABBvAABB(a_t.position, boxes[a].size * a_t.scale, b_t.position, boxes[b].size * b_t.scale);
				else
					d = intersectOBBvOBB(a_t, boxes[a].size, b_t, boxes[b].size);

				if (d.intersects)
				{
					if (boxes[b].trigger)
					{
						if (core.entities.hasComponent<CS::Script>(a))
							scripts[a].script->onTriggerEnter(core, b);
					}
					else
					{
						// Move the controller out of the collision
						transforms[a].position -= d.normal * d.distance;

						// Check if the controller is on the floor, if the normal's y is pointing up enough.
						if (!controllers[a].on_floor)
							controllers[a].on_floor = approximate(d.normal.y, 1.0f, 0.1f) || approximate(d.normal.y, -1.0f, 0.1f);
					}
				}
			}
		}
	}

	/**
	* This function returns a vector containing every entities intersecting with entity a.
	*/
	std::vector<IntersectData> getIntersectingEntities(Entities& entities, EntityID a)
	{
		ComponentMap<CS::Transform>& transforms = entities.getPool<CS::Transform>();
		ComponentMap<CS::BoxCollider>& boxes = entities.getPool<CS::BoxCollider>();

		std::vector<IntersectData> intersecting;
		for (EntityID b : entities.entitiesWith<CS::Transform, CS::BoxCollider>())
		{
			IntersectData d = intersectOBBvOBB(transforms[a], boxes[a].size, transforms[a], boxes[a].size);
			if (d.intersects)
				intersecting.push_back(d);
		}

		return intersecting;
	}

	/**
	* Cast a ray into the world, and return the closest intersecting box ID.
	*/
	IntersectData castRay(Entities& entities, const glm::vec3& r_pos, const glm::vec3& r_dir, const std::vector<EntityID>& ignore)
	{
		ComponentMap<CS::Transform>& transforms = entities.getPool<CS::Transform>();
		ComponentMap<CS::BoxCollider>& boxes = entities.getPool<CS::BoxCollider>();

		std::map<float, IntersectData> distances;
		for (EntityID id : entities.entitiesWith<CS::Transform, CS::BoxCollider>())
		{
			// Don't collide with ignored entities.
			if (boxes[id].trigger || std::find(ignore.begin(), ignore.end(), id) != ignore.end()) continue;

			CS::Transform t = getWorldT(entities, id);

			IntersectData d = intersectOBBvRay(t, boxes[id].size, r_pos, r_dir);
			if (d.intersects)
			{
				d.id = id;
				distances[d.distance] = d;
			}
		}

		if (distances.size() > 0)
			return distances.begin()->second;

		return IntersectData();
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

	bool planetest(glm::vec3 axis, glm::vec3 dist, glm::vec3 r_dir, float min, float max, float& tmin, float& tmax)
	{
		float e = glm::dot(axis, dist);
		float f = glm::dot(r_dir, axis);

		if (fabs(f) > 0.001f)
		{
			float t1 = (e + min) / f;
			float t2 = (e + max) / f;

			if (t1 > t2)
			{
				float w = t1;
				t1 = t2;
				t2 = w;
			}

			if (t2 < tmax) tmax = t2;
			if (t1 > tmin) tmin = t1;

			if (tmax < tmin) return false;
		}
		else if (-e + min > 0.0f || -e + max < 0.0f)
			return false;

		return true;
	}

	/**
	* Checks if a ray intersects an OBB
	*/
	IntersectData intersectOBBvRay(CS::Transform a_t, glm::vec3 a_size, glm::vec3 r_pos, glm::vec3 r_dir)
	{
		float tmin = 0.0f;
		float tmax = 100000.0f;
		glm::vec3 dist = a_t.position - r_pos;

		a_size *= a_t.scale;
		glm::vec3 min = -a_size;
		glm::vec3 max = a_size;

		glm::vec3 x_axis = glm::normalize(a_t.right());
		glm::vec3 y_axis = glm::normalize(a_t.up());
		glm::vec3 z_axis = glm::normalize(a_t.forward());

		IntersectData data;
		data.intersects = planetest(x_axis, dist, r_dir, min.x, max.x, tmin, tmax);
		if (data.intersects) data.normal = x_axis;
		data.intersects = planetest(y_axis, dist, r_dir, min.y, max.y, tmin, tmax);
		if (data.intersects) data.normal = y_axis;
		data.intersects = planetest(z_axis, dist, r_dir, min.z, max.z, tmin, tmax);
		if (data.intersects) data.normal = z_axis;
		data.distance = tmin;

		return data;
	}

	/**
	* Get the world space vertex positions of bounding box.
	*/
	std::vector<glm::vec3> getBoxVerts(glm::vec3 size, glm::mat4 t)
	{
		glm::vec3 min = -size;
		glm::vec3 max = size;

		// All 8 vertices in box
		// First generate as axis aligned.
		std::vector<glm::vec3> verts =
		{
			min,
			{ max.x, min.y, min.z },
			{ min.x, max.y, min.z },
			{ max.x, max.y, min.z },
			max,
			{ min.x, min.y, max.z },
			{ min.x, max.y, max.z },
			{ max.x, min.y, max.z }
		};

		// Rotate, scale and translate vertices
		for (glm::vec3& v : verts)
			v = glm::vec3(t * glm::vec4(v, 1.0f));

		return verts;
	}

	/**
	* Get the world-space vertex positions of the view frustum.
	*/
	std::vector<glm::vec3> getFrustumVerts(glm::mat4 view, glm::mat4 perspective)
	{
		glm::vec3 min{ -1.0f };
		glm::vec3 max{ 1.0f };

		// All 8 vertices in box
		// First generate as axis aligned.
		std::vector<glm::vec3> verts =
		{
			min,
			{ max.x, min.y, min.z },
			{ min.x, max.y, min.z },
			{ max.x, max.y, min.z },
			max,
			{ min.x, min.y, max.z },
			{ min.x, max.y, max.z },
			{ max.x, min.y, max.z }
		};

		// Then turn the box into a frustum based on the inverse perspective matrix
		// and apply view matrix as transformation
		for (glm::vec3& v : verts)
		{
			glm::vec4 new_p = view * glm::inverse(perspective) * glm::vec4(v, 1.0f);
			v = glm::vec3(new_p) / new_p.w;
		}

		return verts;
	}

	/**
	* Find the minimum and maximum projected vertices of a box along an axis.
	*/
	void findMinMaxAlongAxis(glm::vec3 axis, const std::vector<glm::vec3>& verts, float& min, float& max)
	{
		min = std::numeric_limits<float>::max();
		max = -min;

		for (std::size_t i = 0; i < verts.size(); i++)
		{
			float dot = glm::dot(verts[i], axis);
			min = glm::min(min, dot);
			max = glm::max(max, dot);
		}
	}


	IntersectData seperatedAxisTest(const std::vector<glm::vec3>& a_verts, const std::vector<glm::vec3>& b_verts, const std::vector<glm::vec3>& axes)
	{
		IntersectData data;
		data.intersects = true;
		data.distance = std::numeric_limits<float>::max();

		for (std::size_t i = 0; i < axes.size(); i++)
		{
			if (approximate(glm::length2(axes[i]), 0.0f, 0.0001f)) continue;
			glm::vec3 axis = glm::normalize(axes[i]);

			float min_a, max_a, min_b, max_b;
			findMinMaxAlongAxis(axis, a_verts, min_a, max_a);
			findMinMaxAlongAxis(axis, b_verts, min_b, max_b);

			if (min_b > max_a || min_a > max_b)
			{
				data.intersects = false;
				break;
			}
			else
			{
				float dist = glm::abs(glm::min(max_a, max_b) - glm::max(min_a, min_b));
				if (dist < data.distance)
				{
					data.distance = dist;
					data.normal = axis * (min_a < min_b ? 1.0f : -1.0f);
				}
			}
		}

		return data;
	}

	IntersectData intersectOBBvOBB(CS::Transform a_t, glm::vec3 a_size, CS::Transform b_t, glm::vec3 b_size)
	{
		// Get vertices of boxes A and B
		std::vector<glm::vec3> a_verts = getBoxVerts(a_size, a_t.get());
		std::vector<glm::vec3> b_verts = getBoxVerts(b_size, b_t.get());

		glm::vec3 a_x_axis = a_t.right();
		glm::vec3 a_z_axis = a_t.forward();
		glm::vec3 a_y_axis = a_t.up();

		glm::vec3 b_x_axis = b_t.right();
		glm::vec3 b_z_axis = b_t.forward();
		glm::vec3 b_y_axis = b_t.up();

		std::vector<glm::vec3> all_axes =
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

		return seperatedAxisTest(a_verts, b_verts, all_axes);
	}

	IntersectData intersectOBBvFrustum(CS::Transform a_t, glm::vec3 a_size, CS::Transform view_t, CS::Camera c)
	{
		// Get vertices of box A and frustum B
		std::vector<glm::vec3> a_verts = getBoxVerts(a_size, a_t.get());
		std::vector<glm::vec3> b_verts = getFrustumVerts(view_t.get(), c.get());

		// Box face and edge normals.
		glm::vec3 a_x_axis = a_t.right();
		glm::vec3 a_z_axis = a_t.forward();
		glm::vec3 a_y_axis = a_t.up();

		// Frustum face Face normals
		glm::vec3 b_z_axis = view_t.forward();
		glm::vec3 b_r_axis = glm::rotateY(b_z_axis, -glm::radians(90.0f + (c.fov_y / 2.0f)));
		glm::vec3 b_l_axis = glm::rotateY(b_z_axis, glm::radians(90.0f + (c.fov_y / 2.0f)));
		glm::vec3 b_u_axis = glm::rotateX(b_z_axis, -glm::radians(90.0f + (c.fov_y / 2.0f)));
		glm::vec3 b_d_axis = glm::rotateX(b_z_axis, glm::radians(90.0f + (c.fov_y / 2.0f)));

		// Frustum edge normals
		glm::vec3 b_x_axis = view_t.right();
		glm::vec3 b_y_axis = view_t.up();
		glm::vec3 b_edge_1 = glm::normalize(b_verts[0] - view_t.position);
		glm::vec3 b_edge_2 = glm::normalize(b_verts[1] - view_t.position);
		glm::vec3 b_edge_3 = glm::normalize(b_verts[2] - view_t.position);
		glm::vec3 b_edge_4 = glm::normalize(b_verts[3] - view_t.position);

		std::vector<glm::vec3> a_edges
		{
			a_x_axis,
			a_y_axis,
			a_z_axis
		};

		std::vector<glm::vec3> b_edges
		{
			b_x_axis,
			b_y_axis,
			b_edge_1,
			b_edge_2,
			b_edge_3,
			b_edge_4
		};

		std::vector<glm::vec3> axes
		{
			a_x_axis,
			a_y_axis,
			a_z_axis,

			b_z_axis,
			b_r_axis,
			b_l_axis,
			b_u_axis,
			b_d_axis
		};

		for (glm::vec3 a : a_edges)
		{
			for (glm::vec3 b : b_edges)
			{
				glm::vec3 c = glm::cross(a, b);
				if (approximate(glm::length2(c), 0.0f, 0.0001f))
				{
					axes.push_back(a);
					axes.push_back(b);
				}
				else
					axes.push_back(c);
			}
		}

		return seperatedAxisTest(a_verts, b_verts, axes);
	}
}