#include "Portal.h"
#include "Core.h"
#include <glm/gtx/string_cast.hpp>

namespace Game
{
	EntityID createPortal(Core& core)
	{
		ENG::EntityID portal = core.entities.addEntity<CS::Transform, Portal, CS::Model>();
		
		CS::Model& m = core.entities.getComponent<CS::Model>(portal);
		m.mesh = "portal_border.obj";

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

		for (EntityID id : entities.entitiesWith<CS::Transform, Portal>())
		{
			CS::Transform portal_t = getWorldT(entities, id);
			CS::Transform player_t = getWorldT(entities, portals[id].player);

			portals[id].prev_side = static_cast<int>(glm::sign(glm::dot(portal_t.forward(), player_t.position - portal_t.position)));
			portals[id].frame.create(size);
		}
	}

	void updatePortals(Entities& entities)
	{
		ComponentMap<CS::Transform>& transforms = entities.getPool<CS::Transform>();
		ComponentMap<Portal>& portals = entities.getPool<Portal>();
		ComponentMap<CS::BoxCollider>& boxes = entities.getPool<CS::BoxCollider>();

		for (EntityID portal : entities.entitiesWith<CS::Transform, Portal>())
		{
			EntityID player = portals[portal].player;
			EntityID other = portals[portal].other;

			CS::Transform portal_t = getWorldT(entities, portal);
			CS::Transform player_t = getWorldT(entities, player);
			CS::Transform other_t = getWorldT(entities, other);

			// Check which side of portal player is on
			int side = static_cast<int>(glm::sign(glm::dot(portal_t.forward(), portal_t.position - player_t.position)));

			glm::vec3 p_size = glm::vec3(1.0f) * portal_t.scale;
			glm::vec3 pl_size = boxes[player].size * player_t.scale;

			// Is the player colliding with the portal? Basically check if the player could travel through the portal.
			if (intersectOBBvOBB(portal_t, glm::vec3(1.0f), player_t, boxes[player].size).intersects)
			{
				// If the player moves from one side of the portal to the other, teleport them.
				if (side != portals[portal].prev_side)
				{
					glm::mat4 m = other_t.get() * glm::inverse(portal_t.get()) * player_t.get();
					transforms[player] = ENG::decompose(m);

					// Never rotate on Z, only X and Y.
					transforms[player].rotation.z = 0.0f;

					// Prevents double teleporting
					portals[other].prev_side = side;
				}
			}

			// Transform camera match players transform relative to the portal.
			portals[portal].camera = portal_t.get() * glm::inverse(other_t.get()) * getWorldT(entities, player).get();
			portals[portal].prev_side = side;
		}
	}

	void drawToPortals(Core& core)
	{
		ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
		ComponentMap<Portal>& portals = core.entities.getPool<Portal>();

		CS::Transform view_t;
		CS::Transform* view_d = core.renderer.view;

		for (EntityID id : core.entities.entitiesWith<CS::Transform, Portal>())
		{
			CS::Transform t = getWorldT(core.entities, id);

			// Don't render to portal if its not in view.
			if (!inView(*view_d, t.position, glm::vec3(1.0f))) continue;

			portals[id].frame.bind();
			view_t = decompose(portals[portals[id].other].camera);
			glm::mat4 view = glm::inverse(portals[portals[id].other].camera);

			core.renderer.view = &view_t;
			updateSprites(core);

			core.resources.shader("default.shdr").setUniform("view", view);
			core.resources.shader("skybox.shdr").setUniform("view", glm::mat4(glm::mat3(view)));

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			drawSkybox(core.resources);
			drawModels(core);
			drawSprites3D(core);

			portals[id].frame.unbind();
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

		for (EntityID id : core.entities.entitiesWith<CS::Transform, Portal>())
		{
			CS::Transform portal_t = getWorldT(core.entities, id);
			CS::Transform player_t = getWorldT(core.entities, portals[id].player);

			if (!inView(*core.renderer.view, portal_t.position, glm::vec3(1.0f))) continue;

			glm::vec3 p_size = glm::vec3(1.0f) * portal_t.scale;
			glm::vec3 pl_size = boxes[portals[id].player].size * player_t.scale;

			if (intersectOBBvOBB(portal_t, glm::vec3(1.0f), player_t, boxes[portals[id].player].size).intersects)
				portal_t = preventNearClipping(core.settings, portal_t, player_t);
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