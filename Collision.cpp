#include "Collision.h"

#define CASE_1 (abs(dot(dist, a_x_axis)) > a_half_size.x + abs(b_half_size.x * dot(a_x_axis, b_x_axis)) + abs(b_half_size.y * dot(a_x_axis, b_y_axis)) + abs(b_half_size.z * dot(a_x_axis, b_z_axis)))
#define CASE_2 (abs(dot(dist, a_y_axis)) > a_half_size.y + abs(b_half_size.x * dot(a_y_axis, b_x_axis)) + abs(b_half_size.y * dot(a_y_axis, b_y_axis)) + abs(b_half_size.z * dot(a_y_axis, b_z_axis)))
#define CASE_3 (abs(dot(dist, a_z_axis)) > a_half_size.z + abs(b_half_size.x * dot(a_z_axis, b_x_axis)) + abs(b_half_size.y * dot(a_z_axis, b_y_axis)) + abs(b_half_size.z * dot(a_z_axis, b_z_axis)))
#define CASE_4 (abs(dot(dist, b_x_axis)) > abs(a_half_size.x * dot(a_x_axis, b_x_axis)) + abs(a_half_size.y * dot(a_y_axis, b_x_axis)) + abs(a_half_size.z * dot(a_z_axis, b_x_axis) + b_half_size.x))
#define CASE_5 (abs(dot(dist, b_y_axis)) > abs(a_half_size.x * dot(a_x_axis, b_y_axis)) + abs(a_half_size.y * dot(a_y_axis, b_y_axis)) + abs(a_half_size.z * dot(a_z_axis, b_y_axis) + b_half_size.y))
#define CASE_6 (abs(dot(dist, b_z_axis)) > abs(a_half_size.x * dot(a_x_axis, b_z_axis)) + abs(a_half_size.y * dot(a_y_axis, b_z_axis)) + abs(a_half_size.z * dot(a_z_axis, b_z_axis) + b_half_size.z))
#define CASE_7 (abs(dot(dist, a_z_axis) * dot(a_y_axis, b_x_axis) - dot(dist, a_y_axis) * dot(a_z_axis, b_x_axis)) > abs(a_half_size.y * dot(a_z_axis, b_x_axis)) + abs(a_half_size.z * dot(a_y_axis, b_x_axis)) + abs(b_half_size.y * dot(a_x_axis, b_z_axis)) + abs(b_half_size.z * dot(a_x_axis, b_y_axis)))
#define CASE_8 (abs(dot(dist, a_z_axis) * dot(a_y_axis, b_y_axis) - dot(dist, a_y_axis) * dot(a_z_axis, b_y_axis)) > abs(a_half_size.y * dot(a_z_axis, b_y_axis)) + abs(a_half_size.z * dot(a_y_axis, b_y_axis)) + abs(b_half_size.x * dot(a_x_axis, b_z_axis)) + abs(b_half_size.z * dot(a_x_axis, b_x_axis)))
#define CASE_9 (abs(dot(dist, a_z_axis) * dot(a_y_axis, b_z_axis) - dot(dist, a_y_axis) * dot(a_z_axis, b_z_axis)) > abs(a_half_size.y * dot(a_z_axis, b_z_axis)) + abs(a_half_size.z * dot(a_y_axis, b_z_axis)) + abs(b_half_size.x * dot(a_x_axis, b_y_axis)) + abs(b_half_size.y * dot(a_x_axis, b_x_axis)))
#define CASE_10 (abs(dot(dist, a_x_axis) * dot(a_z_axis, b_x_axis) - dot(dist, a_z_axis) * dot(a_x_axis, b_x_axis)) > abs(a_half_size.x * dot(a_z_axis, b_x_axis)) + abs(a_half_size.z * dot(a_x_axis, b_x_axis)) + abs(b_half_size.y * dot(a_z_axis, b_z_axis)) + abs(b_half_size.z * dot(a_y_axis, b_y_axis)))
#define CASE_11 (abs(dot(dist, a_z_axis) * dot(a_y_axis, b_y_axis) - dot(dist, a_z_axis) * dot(a_z_axis, b_y_axis)) > abs(a_half_size.y * dot(a_z_axis, b_y_axis)) + abs(a_half_size.z * dot(a_y_axis, b_y_axis)) + abs(b_half_size.x * dot(a_x_axis, b_z_axis)) + abs(b_half_size.z * dot(a_x_axis, b_x_axis)))
#define CASE_12 (abs(dot(dist, a_x_axis) * dot(a_z_axis, b_z_axis) - dot(dist, a_z_axis) * dot(a_x_axis, b_z_axis)) > abs(a_half_size.x * dot(a_z_axis, b_z_axis)) + abs(a_half_size.z * dot(a_x_axis, b_z_axis)) + abs(b_half_size.x * dot(a_y_axis, b_y_axis)) + abs(b_half_size.y * dot(a_y_axis, b_x_axis)))
#define CASE_13 (abs(dot(dist, a_y_axis) * dot(a_x_axis, b_x_axis) - dot(dist, a_x_axis) * dot(a_y_axis, b_x_axis)) > abs(a_half_size.x * dot(a_y_axis, b_x_axis)) + abs(a_half_size.y * dot(a_x_axis, b_x_axis)) + abs(b_half_size.y * dot(a_z_axis, b_z_axis)) + abs(b_half_size.z * dot(a_z_axis, b_y_axis)))
#define CASE_14 (abs(dot(dist, a_y_axis) * dot(a_x_axis, b_y_axis) - dot(dist, a_x_axis) * dot(a_y_axis, b_y_axis)) > abs(a_half_size.x * dot(a_y_axis, b_y_axis)) + abs(a_half_size.y * dot(a_x_axis, b_y_axis)) + abs(b_half_size.x * dot(a_z_axis, b_z_axis)) + abs(b_half_size.z * dot(a_z_axis, b_x_axis)))
#define CASE_15 (abs(dot(dist, a_y_axis) * dot(a_x_axis, b_z_axis) - dot(dist, a_x_axis) * dot(a_y_axis, b_z_axis)) > abs(a_half_size.x * dot(a_y_axis, b_z_axis)) + abs(a_half_size.y * dot(a_x_axis, b_z_axis)) + abs(b_half_size.x * dot(a_z_axis, b_y_axis)) + abs(b_half_size.y * dot(a_z_axis, b_x_axis)))

namespace ENG
{
	/**
	* Checks if two oriented bounding boxes are colliding using seperated axis theorem (SAT)
	*/
	bool OBBcollision(CS::Transform& a, const glm::vec3& a_size, CS::Transform& b, const glm::vec3& b_size)
	{
		using namespace glm;

		mat4 a_mat = mat4(mat3(a.get()));
		vec3 a_x_axis = normalize(a_mat * vec4(1.0f, 0.0f, 0.0f, 1.0f));
		vec3 a_y_axis = normalize(a_mat * vec4(0.0f, 1.0f, 0.0f, 1.0f));
		vec3 a_z_axis = normalize(a_mat * vec4(0.0f, 0.0f, 1.0f, 1.0f));
		vec3 a_half_size = (a_size / 2.0f) * a.scale;

		mat4 b_mat = mat4(mat3(b.get()));
		vec3 b_x_axis = normalize(b_mat * vec4(1.0f, 0.0f, 0.0f, 1.0f));
		vec3 b_y_axis = normalize(b_mat * vec4(0.0f, 1.0f, 0.0f, 1.0f));
		vec3 b_z_axis = normalize(b_mat * vec4(0.0f, 0.0f, 1.0f, 1.0f));
		vec3 b_half_size = (b_size / 2.0f) * b.scale;

		vec3 dist = b.position - a.position;

		return !CASE_1 && !CASE_2 && !CASE_3 && !CASE_4 && !CASE_5 && !CASE_6 && !CASE_7 && !CASE_8 && !CASE_9 && !CASE_10 && !CASE_11 && !CASE_12 && !CASE_13 && !CASE_14 && !CASE_15;
	}

	void testCollisions(Entities& entities, Core& core)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& controllers = entities.getPool<CS::Controller>();
		auto& boxes = entities.getPool<CS::BoxCollider>();
		auto& scripts = entities.getPool<CS::Script>();

		for (EntityID a : entities.entitiesWith<CS::Transform, CS::Controller, CS::BoxCollider>())
		{
			transforms[a].position += controllers[a].velocity; // move forward
			for (EntityID b : entities.entitiesWith<CS::Transform, CS::BoxCollider>())
			{
				if (a == b) continue;

				if (OBBcollision(transforms[a], boxes[a].size, transforms[b], boxes[b].size))
				{
					if (boxes[a].solid && boxes[b].solid)
						transforms[a].position -= controllers[a].velocity; // move back

					else if (entities.hasComponent<CS::Script>(a))
						scripts[a].script->onCollision(core, b);
				}
			}
		}
	}
}