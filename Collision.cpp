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
	void testCollisions(Entities& entities, Application& app)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& spheres = entities.getPool<CS::SphereCollider>();
		auto& boxes = entities.getPool<CS::BoxCollider>();
		auto& scripts = entities.getPool<CS::Script>();

		std::vector<EntityID> sphere_entities = entities.entitiesWith<CS::Transform, CS::SphereCollider>();
		std::vector<EntityID> box_entities = entities.entitiesWith<CS::Transform, CS::BoxCollider>();

		// Test spheres against spheres
		for (EntityID a : sphere_entities)
		{
			for (EntityID b : sphere_entities) // test spheres against other spheres
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
		}

		// Test boxes against boxes.
		for (EntityID a : box_entities)
		{
			for (EntityID b : box_entities)
			{
				if (a == b) continue;

				if (OBBcollision(transforms[a], boxes[a].size, transforms[b], boxes[b].size))
				{
					if (boxes[a].solid && boxes[b].solid)
					{
						float amount = 0.1f;
						float step = 0.1f;
						glm::vec3 position(0.0f);
						CS::Transform adjusted = transforms[a];

						while (true)
						{
							if (!OBBcollision(adjusted, boxes[a].size, transforms[b], boxes[b].size)) break;
							adjusted.position.x += amount;
							if (!OBBcollision(adjusted, boxes[a].size, transforms[b], boxes[b].size)) break;
							adjusted.position.x -= amount;
							adjusted.position.x -= amount;
							if (!OBBcollision(adjusted, boxes[a].size, transforms[b], boxes[b].size)) break;
							adjusted.position.x += amount;
							adjusted.position.z += amount;
							if (!OBBcollision(adjusted, boxes[a].size, transforms[b], boxes[b].size)) break;
							adjusted.position.z -= amount;
							adjusted.position.z -= amount;
						}
					}
					if (entities.hasComponent<CS::Script>(a))
						scripts[a].script->onCollision(app, b);
				}
			}
		}
	}

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
}