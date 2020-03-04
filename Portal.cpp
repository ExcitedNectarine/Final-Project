#include "Portal.h"
#include <glm/gtx/string_cast.hpp>

namespace ENG
{
	void startPortals(Entities& entities)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& portals = entities.getPool<CS::Portal>();

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Portal>())
		{
			glm::vec3 offset = transforms[portals[id].player].position - transforms[id].position;
			portals[id].prev_side = static_cast<int>(glm::sign(glm::dot(offset, transforms[id].forward())));
		}
	}

	void updatePortals(Entities& entities)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& portals = entities.getPool<CS::Portal>();

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Portal>())
		{
			// Transform camera match players transform relative to the portal.
			portals[id].camera = transforms[id].get() * glm::inverse(transforms[portals[id].other].get()) * transforms[portals[id].player].get();

			glm::vec3 offset = transforms[portals[id].player].position - transforms[id].position;
			int side = static_cast<int>(glm::sign(glm::dot(offset, transforms[id].forward())));

			// Is the player colliding with the portal? Basically check if the player could travel through the portal.
			bool colliding = AABBcollision(transforms[id].position, { 2.0f, 2.0f, 2.0f }, transforms[portals[id].player].position, { 1.0f, 1.0f, 1.0f });
			if (portals[id].active && colliding)
			{
				// If the player moves from one side of the portal to the other, teleport them.
				if (side != portals[id].prev_side)
				{
					glm::mat4 m = transforms[portals[id].other].get() * glm::inverse(transforms[id].get()) * transforms[portals[id].player].get();
					transforms[portals[id].player].position = m[3]; // Teleporting to left/right slightly. Could be a floating point error that adds up

					// Make sure player faces proper direction when entering portal.
					glm::vec3 new_rot(0.0f);
					glm::extractEulerAngleYXZ(m, new_rot.y, new_rot.x, new_rot.z);
					new_rot = glm::degrees(new_rot);
					transforms[portals[id].player].rotation = new_rot;
					
					// Prevents double teleporting
					portals[portals[id].other].active = false;
					portals[portals[id].other].prev_side = side;

					return;
				}
			}
			else if (!colliding) // not colliding, so reactive portal
				portals[id].active = true;

			portals[id].prev_side = side;
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

		// Disable backface culling for drawing portals, so that portals become 2-way.
		glDisable(GL_CULL_FACE);
		Mesh& quad = resources.mesh("quad.obj");
		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Portal>())
		{
			// Find if player is within 0.1f of portal plane
			// Then push portal back to be 0.11f in front of the player
			// Once portal is crossed, dont draw other portal

			resources.shader("portals.shader").setUniform("transform", transforms[id].get());
			resources.shader("portals.shader").setUniform("view", view);
			resources.shader("portals.shader").setUniform("projection", perspective);
			resources.shader("portals.shader").bind();

			quad.bind();
			portals[id].framebuffer.getTexture().bind();

			glDrawArrays(GL_TRIANGLES, 0, quad.vertexCount());
		}
		glEnable(GL_CULL_FACE);
	}
}