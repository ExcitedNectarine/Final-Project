#include "Portal.h"
#include "Core.h"

namespace ENG
{
	void startPortals(Entities& entities, const glm::ivec2& size)
	{
		ComponentMap<CS::Transform>& transforms = entities.getPool<CS::Transform>();
		ComponentMap<CS::Portal>& portals = entities.getPool<CS::Portal>();

		for (EntityID id : entities.entitiesWith<CS::Transform, CS::Portal>())
		{
			portals[id].prev_side = static_cast<int>(glm::sign(glm::dot(transforms[id].forward(), transforms[portals[id].player].position - transforms[id].position)));
			portals[id].framebuffer.create(size);
		}
	}

	void updatePortals(Entities& entities)
	{
		ComponentMap<CS::Transform>& transforms = entities.getPool<CS::Transform>();
		ComponentMap<CS::Portal>& portals = entities.getPool<CS::Portal>();

		EntityID player;
		EntityID other;

		for (EntityID portal : entities.entitiesWith<CS::Transform, CS::Portal>())
		{
			player = portals[portal].player;
			other = portals[portal].other;

			// Check which side of portal player is on
			int side = static_cast<int>(glm::sign(glm::dot(transforms[portal].forward(), transforms[portal].position - transforms[player].position)));

			// Is the player colliding with the portal? Basically check if the player could travel through the portal.
			if (intersectAABBvAABB(transforms[portal].position, { 2.0f, 2.0f, 1.0f }, transforms[player].position, { 0.5f, 0.5f, 0.5f }))
			{
				// If the player moves from one side of the portal to the other, teleport them.
				if (side != portals[portal].prev_side)
				{
					glm::mat4 m = transforms[other].get() * glm::inverse(transforms[portal].get()) * transforms[player].get();
					transforms[player] = ENG::decompose(m);

					// Prevents double teleporting
					portals[other].prev_side = side;
				}
			}

			// Transform camera match players transform relative to the portal.
			portals[portal].camera = transforms[portal].get() * glm::inverse(transforms[other].get()) * transforms[player].get();
			portals[portal].prev_side = side;
		}
	}

	void drawToPortals(Core& core)
	{
		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<CS::Portal>& portals = core.entities.getPool<CS::Portal>();

		CS::Transform view_t;
		CS::Transform* view_d = core.view;

		for (EntityID id : core.entities.entitiesWith<CS::Transform, CS::Portal>())
		{
			portals[id].framebuffer.bind();
			view_t = decompose(portals[portals[id].other].camera);
			glm::mat4 view = glm::inverse(portals[portals[id].other].camera);

			core.view = &view_t;
			updateSprites(core);

			core.resources.shader("default.shdr").setUniform("view", view);
			core.resources.shader("default.shdr").setUniform("view_pos", portals[portals[id].other].camera[3]);
			core.resources.shader("unshaded.shdr").setUniform("view", view);
			core.resources.shader("skybox.shdr").setUniform("view", glm::mat4(glm::mat3(view)));

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			drawSkybox(core.resources);
			drawModels(core);
			drawSprites3D(core);
			portals[id].framebuffer.unbind();
		}

		core.view = view_d;
	}

	void drawPortals(Core& core)
	{
		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<CS::Portal>& portals = core.entities.getPool<CS::Portal>();

		core.resources.shader("portals.shdr").setUniform("view", glm::inverse(core.view->get()));
		core.resources.shader("portals.shdr").setUniform("projection", core.perspective);

		// Disable backface culling for drawing portals, so that portals become 2-way.
		glDisable(GL_CULL_FACE);
		
		Mesh& cube = core.resources.mesh("cube.obj");
		cube.bind();

		for (EntityID id : core.entities.entitiesWith<CS::Transform, CS::Portal>())
		{
			core.resources.shader("portals.shdr").setUniform("transform", preventNearClipping(core.settings, transforms[id], transforms[portals[id].player]).get());
			core.resources.shader("portals.shdr").bind();
			portals[id].framebuffer.getTexture().bind();
			glDrawArrays(GL_TRIANGLES, 0, cube.vertexCount());
		}
		
		glEnable(GL_CULL_FACE);
	}

	/**
	* Move screen position back and scale wall, so that far side is the same as when camera clips
	* near side.
	*/
	CS::Transform preventNearClipping(Settings& settings, CS::Transform screen, CS::Transform player)
	{
		float fov = settings.getf("fov");
		float aspect = settings.getf("width") / settings.getf("height");

		float half_height = 0.1f * glm::tan(fov);
		float half_width = half_height * aspect;
		float corner_dist = glm::length(glm::vec3(half_width, half_height, 0.1f));

		bool facing = glm::dot(screen.forward(), screen.position - player.position) > 0;
		screen.scale.z = corner_dist;
		screen.position += screen.forward() * (facing ? 1.0f : -1.0f);

		return screen;
	}
}