#include "Portal.h"
#include <glm/gtx/string_cast.hpp>

namespace ENG
{
	void updatePortals(Entities& entities)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& portals = entities.getPool<CS::Portal>();

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Portal>())
		{
			portals[id].camera = transforms[id].get() * glm::inverse(transforms[portals[id].other].get()) * transforms[portals[id].player].get();

			// Is the player colliding with the portal? Basically check if the player could travel through the portal.
			bool colliding = OBBcollision(transforms[id], { 2.0f, 2.0f, 0.2f }, transforms[portals[id].player], { 0.5f, 0.5f, 0.2f });
			if (portals[id].active && colliding)
			{
				glm::vec3 offset = transforms[portals[id].player].position - transforms[id].position;
				float angle = glm::dot(offset, transforms[id].forward());

				// If the player moves from one side of the portal to the other, teleport them.
				if (glm::sign(angle) != glm::sign(portals[id].prev_angle))
				{
					transforms[portals[id].player].position = transforms[portals[id].other].position;

					glm::vec3 new_rot(0.0f);
					glm::extractEulerAngleYXZ(portals[id].camera, new_rot.y, new_rot.x, new_rot.z);
					new_rot = glm::degrees(new_rot);
					new_rot.y -= 180.0f;
					transforms[portals[id].player].rotation = new_rot;
					
					portals[portals[id].other].active = false;
				}

				portals[id].prev_angle = angle;
			}
			else if (!colliding) // not colliding, so reactive portal
				portals[id].active = true;
		}
	}

	void drawToPortals(Entities& entities, Resources& resources)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& portals = entities.getPool<CS::Portal>();

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Portal>())
		{
			portals[id].framebuffer.bind();
			glm::mat4 view = glm::inverse(portals[portals[id].other].camera);

			resources.shader("default.shader").setUniform("view", view);
			resources.shader("skybox.shader").setUniform("view", glm::mat4(glm::mat3(view)));

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glDepthMask(GL_FALSE);
			resources.shader("skybox.shader").bind();
			resources.mesh("cube.obj").bind();
			glDrawArrays(GL_TRIANGLES, 0, resources.mesh("cube.obj").vertexCount());
			glDepthMask(GL_TRUE);

			drawModels(entities, resources);

			portals[id].framebuffer.unbind();
		}
	}

	void drawPortals(Entities& entities, Resources& resources, glm::mat4 perspective, glm::mat4 view)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& portals = entities.getPool<CS::Portal>();

		Mesh& cube = resources.mesh("cube.obj"); // Use cube to prevent near plane clipping
		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Portal>())
		{
			CS::Transform transform = transforms[id];
			transform.scale.z = 0.1f;

			resources.shader("portals.shader").setUniform("transform", transform.get());
			resources.shader("portals.shader").setUniform("view", view);
			resources.shader("portals.shader").setUniform("projection", perspective);
			resources.shader("portals.shader").bind();

			cube.bind();
			portals[id].framebuffer.getTexture().bind();

			glDrawArrays(GL_TRIANGLES, 0, cube.vertexCount());
		}
	}
}