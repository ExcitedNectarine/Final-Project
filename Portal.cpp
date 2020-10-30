#include "Portal.h"
#include "Core.h"

namespace Game
{
	EntityID createPortal(Core& core)
	{
		ENG::EntityID portal = core.entities.addEntity<CS::Transform, Portal, CS::Model>();
		
		CS::Model& m = core.entities.getComponent<CS::Model>(portal);
		m.mesh = "portal_border.obj";
		m.texture = "portal_border.png";

		ENG::EntityID b1 = core.entities.addEntity<CS::Transform, CS::BoxCollider>();
		ENG::EntityID b2 = core.entities.addEntity<CS::Transform, CS::BoxCollider>();
		ENG::EntityID b3 = core.entities.addEntity<CS::Transform, CS::BoxCollider>();

		// How big the colliders are, and how distance from centre.
		glm::vec3 size(0.3f, 1.5f, 0.3f);
		float dist = 1.3f;

		core.entities.getComponent<CS::BoxCollider>(b1).size = size;
		core.entities.getComponent<CS::BoxCollider>(b2).size = size;
		core.entities.getComponent<CS::BoxCollider>(b3).size = { 1.5f, 0.3f, 0.3f };

		CS::Transform& t1 = core.entities.getComponent<CS::Transform>(b1);
		t1.position.x = -dist;
		t1.parent = portal;

		CS::Transform& t2 = core.entities.getComponent<CS::Transform>(b2);
		t2.position.x = dist;
		t2.parent = portal;

		CS::Transform& t3 = core.entities.getComponent<CS::Transform>(b3);
		t3.position.y = dist;
		t3.parent = portal;

		return portal;
	}

	void startPortals(Entities& entities, const glm::ivec2& size)
	{
		ComponentMap<CS::Transform>& transforms = entities.getPool<CS::Transform>();
		ComponentMap<Portal>& portals = entities.getPool<Portal>();
		ComponentMap<Traveller>& travellers = entities.getPool<Traveller>();

		// Create the textures for the portal's cameras to draw to.
		for (EntityID portal : entities.entitiesWith<CS::Transform, Portal>())
			portals[portal].frame.create(size);

		// Set the traveller's positions at the start to avoid unwanted teleportations.
		for (EntityID traveller : entities.entitiesWith<CS::Transform, Traveller>())
			travellers[traveller].position_last_frame = getWorldT(entities, traveller).position;
	}

	void updatePortals(Entities& entities)
	{
		ComponentMap<CS::Transform>& transforms = entities.getPool<CS::Transform>();
		ComponentMap<CS::BoxCollider>& boxes = entities.getPool<CS::BoxCollider>();
		ComponentMap<Portal>& portals = entities.getPool<Portal>();
		ComponentMap<Traveller>& travellers = entities.getPool<Traveller>();

		for (EntityID traveller : entities.entitiesWith<CS::Transform, CS::BoxCollider, Traveller>())
		{
			CS::Transform traveller_t = getWorldT(entities, traveller);

			bool teleported = false;
			for (EntityID portal : entities.entitiesWith<CS::Transform, Portal>())
			{
				EntityID other = portals[portal].other;

				CS::Transform portal_t = getWorldT(entities, portal);
				CS::Transform other_t = getWorldT(entities, other);

				glm::vec3 p_size = glm::vec3(1.0f) * portal_t.scale;
				glm::vec3 t_size = boxes[traveller].size * traveller_t.scale;

				// Is the traveller colliding with the portal? Basically check if the traveller could travel through the portal.
				if (intersectOBBvOBB(portal_t, glm::vec3(1.0f), traveller_t, t_size).intersects)
				{
					// Check which side of portal player is on, and previous side.
					int side_this_frame = static_cast<int>(glm::sign(glm::dot(portal_t.forward(), portal_t.position - traveller_t.position)));
					int side_last_frame = static_cast<int>(glm::sign(glm::dot(portal_t.forward(), portal_t.position - travellers[traveller].position_last_frame)));

					// If the player moves from one side of the portal to the other, teleport them.
					if (side_this_frame != side_last_frame)
					{
						glm::mat4 m = other_t.get() * glm::inverse(portal_t.get()) * traveller_t.get();
						transforms[traveller] = decompose(m);

						// Never rotate on Z, only X and Y.
						transforms[traveller].rotation.z = 0.0f;

						travellers[traveller].position_last_frame = getWorldT(entities, traveller).position;
						teleported = true;
					}
				}
			}

			if (!teleported)
				travellers[traveller].position_last_frame = traveller_t.position;
		}
	}

	void drawToPortals(Core& core)
	{
		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<Portal>& portals = core.entities.getPool<Portal>();

		CS::Transform view_t;
		CS::Transform* view_d = core.renderer.view;

		for (EntityID portal : core.entities.entitiesWith<CS::Transform, Portal>())
		{
			EntityID other = portals[portal].other;

			CS::Transform portal_t = getWorldT(core.entities, portal);
			CS::Transform other_t = getWorldT(core.entities, other);

			// Don't render to portal if its not in view.
			if (!intersectOBBvFrustum(portal_t, glm::vec3(1.0f), *view_d, core.camera).intersects) continue;
			
			// Get the view for the camera pointed at the portal.
			glm::mat4 camera = other_t.get() * glm::inverse(portal_t.get()) * view_d->get();
			glm::mat4 view = glm::inverse(camera);
			view_t = decompose(camera);

			// Set the core's view to the camera view
			core.renderer.view = &view_t;

			// Update with new view
			updateRenderer(core);
			updateSprites(core);

			portals[portal].frame.bind();

			core.resources.shader("default.shdr").setUniform("view", view);
			core.resources.shader("skybox.shdr").setUniform("view", glm::mat4(glm::mat3(view)));

			// Draw the scene to the framebuffer.
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			drawSkybox(core.resources);
			drawModels(core);
			drawColliders(core);
			drawSprites3D(core);

			portals[portal].frame.unbind();
		}

		core.renderer.view = view_d;
	}

	void drawPortals(Core& core)
	{
		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<Portal>& portals = core.entities.getPool<Portal>();
		ComponentMap<CS::BoxCollider>& boxes = core.entities.getPool<CS::BoxCollider>();

		core.resources.shader("portals.shdr").setUniform("view", glm::inverse(core.renderer.view->get()));
		core.resources.shader("portals.shdr").setUniform("projection", core.perspective);

		// Disable backface culling for drawing portals, so that portals become 2-way.
		glDisable(GL_CULL_FACE);
		
		Mesh& cube = core.resources.mesh("cube.obj");
		cube.bind();

		CS::Transform view_t = *core.renderer.view;

		for (EntityID id : core.entities.entitiesWith<CS::Transform, Portal>())
		{
			CS::Transform portal_t = getWorldT(core.entities, id);

			if (!intersectOBBvFrustum(portal_t, glm::vec3(1.0f), *core.renderer.view, core.camera).intersects) continue;

			glm::vec3 p_size = glm::vec3(1.0f) * portal_t.scale;
			glm::vec3 pl_size = glm::vec3(0.5f) * view_t.scale;

			// Only try to prevent the near plane clipping if the player is actually able to pass through the portal.
			// This allows the portal to be a flat plane otherwise.
			if (intersectOBBvOBB(portal_t, glm::vec3(1.0f), view_t, pl_size).intersects)
				portal_t = preventNearClipping(core.settings, portal_t, view_t);
			else
				portal_t.scale.z = 0.0f;

			core.resources.shader("portals.shdr").setUniform("transform", portal_t.get());
			core.resources.shader("portals.shdr").bind();
			portals[id].frame.getTexture().bind();

			glDrawArrays(GL_TRIANGLES, 0, cube.vertexCount());
		}
		
		glEnable(GL_CULL_FACE);
	}

	/**
	* Move screen position back and scale wall along Z.
	*/
	CS::Transform preventNearClipping(Settings& settings, CS::Transform screen, CS::Transform player)
	{
		// Calculate distance from camera to corner of near plane.
		float fov = settings.getf("fov");
		float aspect = settings.getf("width") / settings.getf("height");
		float near_dist = 0.1f;

		float half_height = near_dist * glm::tan(fov);
		float half_width = half_height * aspect;
		float corner_dist = glm::length(glm::vec3(half_width, half_height, near_dist));

		// Set scale of screen, and move back from whichever way player is facing.
		bool facing = glm::dot(screen.forward(), screen.position - player.position) > 0;
		screen.scale.z *= corner_dist;
		screen.position += (screen.forward() * (facing ? 1.0f : -1.0f));

		return screen;
	}
}