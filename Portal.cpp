#include "Portal.h"
#include <glm/gtx/string_cast.hpp>

namespace ENG
{
	void startPortals(Entities& entities, const glm::ivec2& size)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& portals = entities.getPool<CS::Portal>();

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Portal>())
		{
			glm::vec3 offset = transforms[portals[id].player].position - transforms[id].position;
			portals[id].prev_side = static_cast<int>(glm::sign(glm::dot(offset, transforms[id].forward())));

			portals[id].framebuffer.create(size);
		}
	}

	void updatePortals(Entities& entities)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& portals = entities.getPool<CS::Portal>();

		EntityID player;
		EntityID other;

		for (EntityID portal : entities.entitiesWith<CS::Transform, CS::Portal>())
		{
			player = portals[portal].player;
			other = portals[portal].other;

			// Transform camera match players transform relative to the portal.
			portals[portal].camera = transforms[portal].get() * glm::inverse(transforms[other].get()) * transforms[player].get();

			glm::vec3 offset = transforms[portal].position - transforms[player].position;
			int side = static_cast<int>(glm::sign(glm::dot(transforms[portal].forward(), offset)));

			// Check if player facing portal
			//bool facing = glm::dot(transforms[player].forward(), glm::normalize(transforms[portal].position - transforms[player].position)) < 0;

			// Is the player colliding with the portal? Basically check if the player could travel through the portal.
			bool colliding = OBBcollision(transforms[portal], { 2.0f, 2.0f, 0.5f }, transforms[player], { 0.5f, 0.5f, 0.5f });
			if (portals[portal].active && colliding)
			{
				// If the player moves from one side of the portal to the other, teleport them.
				if (side != portals[portal].prev_side)
				{
					glm::mat4 m = transforms[other].get() * glm::inverse(transforms[portal].get()) * transforms[player].get();
					transforms[player] = ENG::decompose(m);

					// Prevents double teleporting
					portals[other].active = false;
					portals[other].prev_side = side;

					OUTPUT("Teleported " << portal << " to " << other);

					return;
				}
			}
			else if (!colliding) // not colliding, so reactive portal
				portals[portal].active = true;

			portals[portal].prev_side = side;
		}
	}

	void drawToPortals(Entities& entities, Resources& resources, CS::Camera cam)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& portals = entities.getPool<CS::Portal>();

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Portal>())
		{
			portals[id].framebuffer.bind();
			glm::mat4 view = glm::inverse(portals[portals[id].other].camera);

			resources.shader("default.shdr").setUniform("view", view);
			resources.shader("default.shdr").setUniform("view_pos", portals[portals[id].other].camera[3]);
			resources.shader("skybox.shdr").setUniform("view", glm::mat4(glm::mat3(view)));

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			drawSkybox(resources);
			drawModels(entities, resources, view[3]);
			portals[id].framebuffer.unbind();
		}
	}

	/**
	* Move screen position back and scale wall, so that far side is the same as when camera clips
	* near side.
	*/
	CS::Transform preventNearClipping(CS::Camera cam, CS::Transform screen, CS::Transform player)
	{
		float half_height = cam.near * glm::tan(cam.fov);
		float half_width = half_height * cam.aspect;
		float corner_dist = glm::length(glm::vec3(half_width, half_height, cam.near));

		bool facing = glm::dot(screen.forward(), screen.position - player.position) > 0;
		screen.scale.z = corner_dist;
		screen.position += screen.forward() * (facing ? 1.0f : -1.0f);

		return screen;
	}

	void drawPortals(Entities& entities, Resources& resources, CS::Camera cam, glm::mat4 view)
	{
		auto& transforms = entities.getPool<CS::Transform>();
		auto& portals = entities.getPool<CS::Portal>();

		resources.shader("portals.shdr").setUniform("view", view);
		resources.shader("portals.shdr").setUniform("projection", cam.get());

		// Disable backface culling for drawing portals, so that portals become 2-way.
		glDisable(GL_CULL_FACE);
		
		Mesh& cube = resources.mesh("cube.obj");
		cube.bind();

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Portal>())
		{
			if (!portals[id].active) continue;

			resources.shader("portals.shdr").setUniform("transform", preventNearClipping(cam, transforms[id], transforms[portals[id].player]).get());
			resources.shader("portals.shdr").bind();
			portals[id].framebuffer.getTexture().bind();

			glDrawArrays(GL_TRIANGLES, 0, cube.vertexCount());
		}
		
		glEnable(GL_CULL_FACE);
	}
}