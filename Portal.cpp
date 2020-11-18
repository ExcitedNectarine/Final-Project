#include "Portal.h"
#include "Core.h"

namespace Game
{
	void PortalCamera::lateUpdate(ENG::Core& core)
	{
		glm::mat4 other_t = getWorldM(core.entities, other);
		glm::mat4 portal_t = getWorldM(core.entities, portal);
		glm::mat4 player_t = getWorldM(core.entities, player);
		glm::mat4 new_view = other_t * glm::inverse(portal_t) * player_t;

		core.entities.getComponent<ENG::CS::Transform>(id) = ENG::decompose(new_view);
	}

	void Portal::update(ENG::Core& core)
	{
		ENG::CS::Transform portal_t = ENG::getWorldT(core.entities, id);
		ENG::CS::Transform other_t = ENG::getWorldT(core.entities, other);
		ENG::CS::Transform traveller_t;

		for (auto& p : travellers)
		{
			traveller_t = ENG::getWorldT(core.entities, p.first);

			int side_last_frame = p.second;
			int side_this_frame = static_cast<int>(glm::sign(glm::dot(portal_t.forward(), portal_t.position - traveller_t.position)));

			if (side_last_frame != side_this_frame)
			{
				glm::mat4 m = other_t.get() * glm::inverse(portal_t.get()) * traveller_t.get();
				core.entities.getComponent<ENG::CS::Transform>(p.first) = ENG::decompose(m);
			}

			p.second = side_this_frame;
		}
	}

	void Portal::onTriggerEnter(ENG::Core& core, ENG::EntityID entered_id)
	{
		ENG::CS::Transform portal_t = ENG::getWorldT(core.entities, id);
		ENG::CS::Transform traveller_t = ENG::getWorldT(core.entities, entered_id);

		travellers[entered_id] = static_cast<int>(glm::sign(glm::dot(portal_t.forward(), portal_t.position - traveller_t.position)));;

		if (core.entities.hasComponent<ENG::CS::Camera>(entered_id))
		{
			ENG::CS::Transform& transform = core.entities.getComponent<ENG::CS::Transform>(screen);
			ENG::CS::Camera& camera = core.entities.getComponent<ENG::CS::Camera>(entered_id);
			glm::vec3 pos = ENG::getWorldT(core.entities, entered_id).position;

			transform = preventNearClipping(camera, transform, pos);
		}
	}

	void Portal::onTriggerExit(ENG::Core& core, ENG::EntityID exited_id)
	{
		travellers.erase(exited_id);

		//if (core.entities.hasComponent<ENG::CS::Camera>(exited_id))
		//{
		//	ENG::CS::Transform& transform = core.entities.getComponent<ENG::CS::Transform>(screen);
		//	//transform.scale.z = 0.0f;
		//}
	}

	ENG::CS::Transform Portal::preventNearClipping(const ENG::CS::Camera& camera, ENG::CS::Transform screen, const glm::vec3& view_pos)
	{
		float half_height = camera.near * glm::tan(camera.fov_y);
		float half_width = half_height * camera.aspect;
		float corner_dist = glm::length(glm::vec3(half_width, half_height, camera.near));

		bool facing = glm::dot(screen.forward(), screen.position - view_pos) > 0;
		screen.scale.z *= corner_dist;
		screen.position += (screen.forward() * (facing ? 1.0f : -1.0f));

		return screen;
	}

	ENG::EntityID createScreen(ENG::Core& core, ENG::EntityID parent, ENG::EntityID camera)
	{
		ENG::EntityID screen = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Model>();

		ENG::CS::Model& m = core.entities.getComponent<ENG::CS::Model>(screen);
		m.shader = "portals.shdr";
		m.camera_output = camera;
		m.layers[0] = false;
		m.layers[1] = true;
		m.backface_culling = false;
		m.camera_output = camera;

		ENG::CS::Transform& t = core.entities.getComponent<ENG::CS::Transform>(screen);
		t.parent = parent;
		t.scale.z = 0.1f;

		return screen;
	}

	ENG::EntityID createBorder(ENG::Core& core, ENG::EntityID parent)
	{
		ENG::EntityID border = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Model>();
		core.entities.getComponent<ENG::CS::Transform>(border).parent = parent;

		ENG::CS::Model& m = core.entities.getComponent<ENG::CS::Model>(border);
		m.mesh = "portal_border.obj";
		m.texture = "portal_border.png";

		ENG::EntityID b1 = core.entities.addEntity<ENG::CS::Transform, ENG::CS::BoxCollider>();
		ENG::EntityID b2 = core.entities.addEntity<ENG::CS::Transform, ENG::CS::BoxCollider>();
		ENG::EntityID b3 = core.entities.addEntity<ENG::CS::Transform, ENG::CS::BoxCollider>();

		// How big the colliders are, and how distance from centre.
		glm::vec3 size(0.3f, 1.5f, 0.3f);
		float dist = 1.3f;

		core.entities.getComponent<ENG::CS::BoxCollider>(b1).size = size;
		core.entities.getComponent<ENG::CS::BoxCollider>(b2).size = size;
		core.entities.getComponent<ENG::CS::BoxCollider>(b3).size = { 1.5f, 0.3f, 0.3f };

		ENG::CS::Transform& t1 = core.entities.getComponent<ENG::CS::Transform>(b1);
		t1.position.x = -dist;
		t1.parent = border;

		ENG::CS::Transform& t2 = core.entities.getComponent<ENG::CS::Transform>(b2);
		t2.position.x = dist;
		t2.parent = border;

		ENG::CS::Transform& t3 = core.entities.getComponent<ENG::CS::Transform>(b3);
		t3.position.y = dist;
		t3.parent = border;

		return border;
	}

	std::pair<ENG::EntityID, ENG::EntityID> createPortalPair(ENG::Core& core, ENG::EntityID player)
	{
		// Portal parents, just stores where whole portal object is.
		ENG::EntityID portal_a = core.entities.addEntity<ENG::CS::Transform, ENG::CS::BoxCollider, ENG::CS::Script>();
		ENG::EntityID portal_b = core.entities.addEntity<ENG::CS::Transform, ENG::CS::BoxCollider, ENG::CS::Script>();
		core.entities.getComponent<ENG::CS::Transform>(portal_a).position = { 0.0f, -3.0f, 10.0f };
		core.entities.getComponent<ENG::CS::Transform>(portal_b).position = { 0.0f, -3.0f, -10.0f };
		core.entities.getComponent<ENG::CS::BoxCollider>(portal_a).trigger = true;
		core.entities.getComponent<ENG::CS::BoxCollider>(portal_b).trigger = true;

		ENG::EntityID camera_a = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Camera, ENG::CS::Script>();
		ENG::EntityID camera_b = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Camera, ENG::CS::Script>();
		ENG::EntityID screen_a = createScreen(core, portal_a, camera_a);
		ENG::EntityID screen_b = createScreen(core, portal_b, camera_b);

		core.entities.getComponent<ENG::CS::Script>(portal_a).script = std::make_shared<Portal>(portal_b, screen_a);
		core.entities.getComponent<ENG::CS::Script>(portal_b).script = std::make_shared<Portal>(portal_a, screen_b);
		core.entities.getComponent<ENG::CS::Script>(camera_a).script = std::make_shared<PortalCamera>(portal_a, portal_b, player);
		core.entities.getComponent<ENG::CS::Script>(camera_b).script = std::make_shared<PortalCamera>(portal_b, portal_a, player);

		createBorder(core, portal_a);
		createBorder(core, portal_b);

		return std::make_pair(portal_a, portal_b);
	}

	//EntityID createPortal(Core& core)
	//{
	//	ENG::EntityID portal = core.entities.addEntity<CS::Transform, Portal, CS::Model>();
	//	
	//	CS::Model& m = core.entities.getComponent<CS::Model>(portal);
	//	m.mesh = "portal_border.obj";
	//	m.texture = "portal_border.png";

	//	ENG::EntityID b1 = core.entities.addEntity<CS::Transform, CS::BoxCollider>();
	//	ENG::EntityID b2 = core.entities.addEntity<CS::Transform, CS::BoxCollider>();
	//	ENG::EntityID b3 = core.entities.addEntity<CS::Transform, CS::BoxCollider>();

	//	// How big the colliders are, and how distance from centre.
	//	glm::vec3 size(0.3f, 1.5f, 0.3f);
	//	float dist = 1.3f;

	//	core.entities.getComponent<CS::BoxCollider>(b1).size = size;
	//	core.entities.getComponent<CS::BoxCollider>(b2).size = size;
	//	core.entities.getComponent<CS::BoxCollider>(b3).size = { 1.5f, 0.3f, 0.3f };

	//	CS::Transform& t1 = core.entities.getComponent<CS::Transform>(b1);
	//	t1.position.x = -dist;
	//	t1.parent = portal;

	//	CS::Transform& t2 = core.entities.getComponent<CS::Transform>(b2);
	//	t2.position.x = dist;
	//	t2.parent = portal;

	//	CS::Transform& t3 = core.entities.getComponent<CS::Transform>(b3);
	//	t3.position.y = dist;
	//	t3.parent = portal;

	//	return portal;
	//}

	//void startPortals(Core& core)
	//{
	//	ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
	//	ComponentMap<Portal>& portals = core.entities.getPool<Portal>();
	//	ComponentMap<Traveller>& travellers = core.entities.getPool<Traveller>();

	//	// Create the textures for the portal's cameras to draw to.
	//	for (EntityID portal : core.entities.entitiesWith<CS::Transform, Portal>())
	//		portals[portal].frame.create(core.window.getSize());

	//	// Set the traveller's positions at the start to avoid unwanted teleportations.
	//	for (EntityID traveller : core.entities.entitiesWith<CS::Transform, Traveller>())
	//		travellers[traveller].position_last_frame = getWorldT(core.entities, traveller).position;
	//}

	//void updatePortals(Core& core)
	//{
	//	ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
	//	ComponentMap<CS::BoxCollider>& boxes = core.entities.getPool<CS::BoxCollider>();
	//	ComponentMap<Portal>& portals = core.entities.getPool<Portal>();
	//	ComponentMap<Traveller>& travellers = core.entities.getPool<Traveller>();

	//	for (EntityID traveller : core.entities.entitiesWith<CS::Transform, CS::BoxCollider, Traveller>())
	//	{
	//		CS::Transform traveller_t = getWorldT(core.entities, traveller);

	//		bool teleported = false;
	//		for (EntityID portal : core.entities.entitiesWith<CS::Transform, Portal>())
	//		{
	//			EntityID other = portals[portal].other;

	//			CS::Transform portal_t = getWorldT(core.entities, portal);
	//			CS::Transform other_t = getWorldT(core.entities, other);

	//			glm::vec3 p_size = glm::vec3(1.0f) * portal_t.scale;
	//			glm::vec3 t_size = boxes[traveller].size * traveller_t.scale;

	//			// Is the traveller colliding with the portal? Basically check if the traveller could travel through the portal.
	//			if (intersectOBBvOBB(portal_t, glm::vec3(1.0f), traveller_t, t_size).intersects)
	//			{
	//				// Check which side of portal player is on, and previous side.
	//				int side_this_frame = static_cast<int>(glm::sign(glm::dot(portal_t.forward(), portal_t.position - traveller_t.position)));
	//				int side_last_frame = static_cast<int>(glm::sign(glm::dot(portal_t.forward(), portal_t.position - travellers[traveller].position_last_frame)));

	//				// If the player moves from one side of the portal to the other, teleport them.
	//				if (side_this_frame != side_last_frame)
	//				{
	//					glm::mat4 m = other_t.get() * glm::inverse(portal_t.get()) * traveller_t.get();
	//					transforms[traveller] = decompose(m);

	//					// Never rotate on Z, only X and Y.
	//					transforms[traveller].rotation.z = 0.0f;

	//					travellers[traveller].position_last_frame = getWorldT(core.entities, traveller).position;
	//					teleported = true;
	//				}
	//			}
	//		}

	//		if (!teleported)
	//			travellers[traveller].position_last_frame = traveller_t.position;
	//	}
	//}

	//void drawToPortals(Core& core)
	//{
	//	ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
	//	ComponentMap<Portal>& portals = core.entities.getPool<Portal>();

	//	CS::Transform view_t;
	//	ENG::EntityID view_id = core.renderer.view_id;

	//	CS::Camera c = core.entities.getComponent<CS::Camera>(view_id);

	//	for (EntityID portal : core.entities.entitiesWith<CS::Transform, Portal>())
	//	{
	//		EntityID other = portals[portal].other;

	//		CS::Transform portal_t = getWorldT(core.entities, portal);
	//		CS::Transform other_t = getWorldT(core.entities, other);
	//		CS::Transform main_view_t = getWorldT(core.entities, view_id);

	//		// Don't render to portal if its not in view.
	//		if (!intersectOBBvFrustum(portal_t, glm::vec3(1.0f), main_view_t, c).intersects) continue;
	//		
	//		// Get the view for the camera pointed at the portal.
	//		glm::mat4 camera = other_t.get() * glm::inverse(portal_t.get()) * main_view_t.get();
	//		glm::mat4 view = glm::inverse(camera);
	//		view_t = decompose(camera);

	//		// Update with new view
	//		updateRenderer(core, view, c.get());
	//		updateSprites(core);

	//		portals[portal].frame.bind();

	//		// Draw the scene to the framebuffer.
	//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//		drawSkybox(core.resources);
	//		drawModels(core);
	//		drawColliders(core);
	//		drawSprites3D(core);

	//		portals[portal].frame.unbind();
	//	}

	//	core.renderer.view_id = view_id;
	//}

	//void drawPortals(Core& core)
	//{
	//	ComponentMap<CS::Transform>& transforms = core.entities.getPool<CS::Transform>();
	//	ComponentMap<CS::BoxCollider>& boxes = core.entities.getPool<CS::BoxCollider>();
	//	ComponentMap<Portal>& portals = core.entities.getPool<Portal>();

	//	// Disable backface culling for drawing portals, so that portals become 2-way.
	//	glDisable(GL_CULL_FACE);
	//	
	//	Mesh& cube = core.resources.mesh("cube.obj");
	//	cube.bind();

	//	CS::Transform view_t = getWorldT(core.entities, core.renderer.view_id);
	//	for (EntityID id : core.entities.entitiesWith<CS::Transform, Portal>())
	//	{
	//		CS::Transform portal_t = getWorldT(core.entities, id);

	//		if (!intersectOBBvFrustum(portal_t, glm::vec3(1.0f), view_t, core.entities.getComponent<CS::Camera>(core.renderer.view_id)).intersects) continue;

	//		glm::vec3 p_size = glm::vec3(1.0f) * portal_t.scale;
	//		glm::vec3 pl_size = glm::vec3(0.5f) * view_t.scale;

	//		// Only try to prevent the near plane clipping if the player is actually able to pass through the portal.
	//		// This allows the portal to be a flat plane otherwise.
	//		if (intersectOBBvOBB(portal_t, glm::vec3(1.0f), view_t, pl_size).intersects)
	//			portal_t = preventNearClipping(core.settings, portal_t, view_t);
	//		else
	//			portal_t.scale.z = 0.0f;

	//		core.resources.shader3D("portals.shdr").setUniform("transform", portal_t.get());
	//		core.resources.shader3D("portals.shdr").bind();
	//		portals[id].frame.getTexture().bind();

	//		glDrawArrays(GL_TRIANGLES, 0, cube.vertexCount());
	//	}
	//	
	//	glEnable(GL_CULL_FACE);
	//}
}