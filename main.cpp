#include "Core.h"
#include "Components.h"

////////////////////////////////////
void createCore(ENG::Core& core, const std::string& setting_file)
{
	ENG::audioInit();

	core.settings.load(setting_file);

	glm::vec2 window_size(core.settings.getf("width"), core.settings.getf("height"));

	core.perspective = glm::perspective(glm::radians(core.settings.getf("fov")), window_size.x / window_size.y, 0.1f, 500.0f);
	core.orthographic = glm::ortho(0.0f, window_size.x, window_size.y, 0.0f);

	core.window.create(window_size, core.settings.get("title"));

	core.resources.loadMeshes(ENG::splitText(ENG::readTextFile(core.settings.get("meshes")), '\n'));
	core.resources.loadTextures(ENG::splitText(ENG::readTextFile(core.settings.get("textures")), '\n'));
	core.resources.loadSounds(ENG::splitText(ENG::readTextFile(core.settings.get("sounds")), '\n'));
	core.resources.loadShaders(ENG::splitText(ENG::readTextFile(core.settings.get("shaders")), '\n'));

	core.skybox.create(ENG::splitText(ENG::readTextFile(core.settings.get("skybox")), '\n'));
	core.skybox.bind();

	core.resources.shader("default.shdr").setUniform("projection", core.perspective);
	core.resources.shader("unshaded.shdr").setUniform("projection", core.perspective);
	core.resources.shader("skybox.shdr").setUniform("projection", core.perspective);
	core.resources.shader("sprite.shdr").setUniform("projection", core.orthographic);

	core.entities.addComponentPools<
		ENG::CS::Transform,
		ENG::CS::Transform2D,
		ENG::CS::Model,
		ENG::CS::Sprite,
		ENG::CS::Light,
		ENG::CS::Script,
		ENG::CS::BoxCollider,
		ENG::CS::PlaneCollider,
		ENG::CS::Controller,
		ENG::CS::Portal
	>();
}

void run(ENG::Core& core)
{
	ENG::scriptStart(core);
	ENG::startPortals(core.entities, core.window.getSize());
	ENG::spriteStart();

	double current = 0.0, last = 0.0;
	while (!core.window.shouldClose())
	{
		current = glfwGetTime();
		core.delta = static_cast<float>(current - last);
		last = current;

		//if ((1000.0f / 60.0f) > core.delta)

		ENG::scriptUpdate(core);
		ENG::moveControllers(core);

		ENG::updatePortals(core.entities);
		ENG::drawToPortals(core);

		ENG::updateSprites(core);
		ENG::updateRenderer(core);

		core.window.clear({ 0.0f, 0.0f, 0.0f, 0.0f });

		ENG::drawSkybox(core.resources);
		ENG::drawModels(core);
		ENG::drawSprites3D(core);
		ENG::drawPortals(core);
		ENG::drawModelsToHUD(core);
		ENG::drawSprites(core);

		core.window.display();

		glfwPollEvents();
	}

	ENG::scriptEnd(core);
}

////////////////////////////////////

struct Pickup : ENG::ECSComponent<Pickup>
{
	bool active = false;
};

struct PlayerScript : ENG::Script
{
	ENG::CS::Transform* transform;
	ENG::CS::Controller* controller;
	ENG::CS::BoxCollider* box;
	glm::vec3 direction;
	glm::vec3 velocity;
	float speed = 6.0f;

	glm::dvec2 last_mouse;
	glm::dvec2 mouse_offset;
	float sensitivity = 0.05f;
	float dist;
	ENG::EntityID pickup = 0;

	void start(ENG::Core& core)
	{
		transform = &core.entities.getComponent<ENG::CS::Transform>(id);
		controller = &core.entities.getComponent<ENG::CS::Controller>(id);
		transform->position.y = 20.0f;

		box = &core.entities.getComponent<ENG::CS::BoxCollider>(id);
		box->size = { 0.5f, 1.5f, 0.5f };
	}

	void mouselook(ENG::Core& core)
	{
		mouse_offset = last_mouse - core.window.getMousePos();
		last_mouse = core.window.getMousePos();
		transform->rotation += glm::vec3(mouse_offset.y, mouse_offset.x, 0.0f) * sensitivity;

		if (transform->rotation.x > 89.0f) transform->rotation.x = 89.0f;
		else if (transform->rotation.x < -89.0f) transform->rotation.x = -89.0f;
	}

	void movement(ENG::Core& core)
	{
		direction = glm::vec3(0.0f);
		if (core.window.isKeyPressed(GLFW_KEY_W)) direction -= transform->forward();
		else if (core.window.isKeyPressed(GLFW_KEY_S)) direction += transform->forward();
		if (core.window.isKeyPressed(GLFW_KEY_A)) direction -= transform->right();
		else if (core.window.isKeyPressed(GLFW_KEY_D)) direction += transform->right();

		if (core.window.isKeyPressed(GLFW_KEY_Q))
			transform->position = { -5.0f, 10.0f, 0.0f };
		if (core.window.isKeyPressed(GLFW_KEY_E))
			transform->position = { 5.0f, 10.0f, 0.0f };

		if (controller->on_floor && core.window.isKeyPressed(GLFW_KEY_SPACE))
		{
			velocity.y = 7.5f;
			controller->on_floor = false;
		}

		if (direction != glm::vec3(0.0f))
			direction = glm::normalize(direction);

		velocity.x = direction.x * speed;
		velocity.z = direction.z * speed;
		
		if (!controller->on_floor)
			velocity.y -= 9.1f * core.delta;
		else
			velocity.y = 0.0f;

		controller->velocity = velocity;
	}

	void actions(ENG::Core& core)
	{
		if (pickup == 0 && core.window.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			pickup = ENG::castRay(core.entities, transform->position, -transform->forward(), id, dist);

			if (pickup != 0 && core.entities.hasComponent<Pickup>(pickup))
			{
				OUTPUT("Pickup up " << pickup);

				core.entities.getComponent<ENG::CS::Transform>(pickup).parent = id;
				core.entities.getComponent<ENG::CS::Transform>(pickup).position = { 0.0f, 0.0f, -3.5f };
				core.entities.getComponent<ENG::CS::Transform>(pickup).rotation = glm::vec3(0.0f);
				core.entities.getComponent<ENG::CS::Model>(pickup).hud = true;
				core.entities.getComponent<ENG::CS::BoxCollider>(pickup).solid = false;
			}
			else pickup = 0;
		}

		if (pickup != 0 && core.window.isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
		{
			ENG::EntityID floor = ENG::castRay(core.entities, transform->position, -transform->forward(), id, dist);
			if (floor != 0 && dist < 5.0f)
			{
				glm::vec3 new_pos = transform->position + dist * -transform->forward();
				new_pos.y += core.entities.getComponent<ENG::CS::BoxCollider>(pickup).size.y;

				core.entities.getComponent<ENG::CS::Transform>(pickup).parent = 0;
				core.entities.getComponent<ENG::CS::Transform>(pickup).position = new_pos;
				core.entities.getComponent<ENG::CS::Model>(pickup).hud = false;
				core.entities.getComponent<ENG::CS::BoxCollider>(pickup).solid = true;

				pickup = 0;
			}
		}
	}

	void update(ENG::Core& core)
	{
		if (core.window.isKeyPressed(GLFW_KEY_ESCAPE))
			core.window.close();

		mouselook(core);
		movement(core);
		actions(core);

		core.view = transform;
	}
};

ENG::EntityID createProp(ENG::Core& core, glm::vec3 pos)
{
	ENG::EntityID prop = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Model, ENG::CS::Light, ENG::CS::BoxCollider, Pickup>();

	//core.entities.getComponent<ENG::CS::Model>(prop).shaded = false;
	core.entities.getComponent<ENG::CS::Model>(prop).texture = "brickwall.jpg";
	core.entities.getComponent<ENG::CS::Model>(prop).normal = "brickwall_normal.jpg";
	core.entities.getComponent<ENG::CS::Transform>(prop).position = pos;
	core.entities.getComponent<ENG::CS::Light>(prop).colour = { 1.0f, 1.0f, 1.0f };
	core.entities.getComponent<ENG::CS::Light>(prop).radius = 2.5;

	return prop;
}

ENG::EntityID createBarrier(ENG::Core& core, glm::vec3 pos)
{
	ENG::EntityID platform = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Model, ENG::CS::BoxCollider>();

	core.entities.getComponent<ENG::CS::Transform>(platform).position = pos;
	core.entities.getComponent<ENG::CS::Transform>(platform).scale = { 20.0f, 1.0f, 20.0f };

	return platform;
}

int main()
{
	try
	{
		ENG::Core core;
		createCore(core, "Resources/settings.set");
		core.resources.shader("default.shdr").setUniform("ambient", glm::vec3(0.0f));
		core.resources.shader("unshaded.shdr").setUniform("ambient", glm::vec3(1.0f));
		core.window.lockMouse(true);
		core.entities.addComponentPool<Pickup>();

		// Create player
		ENG::EntityID player = core.entities.addEntity<ENG::CS::Script, ENG::CS::Transform, ENG::CS::BoxCollider, ENG::CS::Controller>();
		core.entities.getComponent<ENG::CS::Script>(player).script = std::make_shared<PlayerScript>();

		ENG::EntityID gun = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Model>();
		ENG::CS::Model& gm = core.entities.getComponent<ENG::CS::Model>(gun);
		gm.mesh = "gun.obj";
		//gm.texture = "gun.png";
		gm.hud = true;

		ENG::CS::Transform& t = core.entities.getComponent<ENG::CS::Transform>(gun);
		t.parent = player;
		t.position = { 0.5f, -0.5f, -0.5f };
		t.scale *= 0.25f;
		t.rotation = { 0.0f, 180.0f, 180.0f };

		ENG::EntityID spr = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Sprite>();
		ENG::CS::Sprite& s = core.entities.getComponent<ENG::CS::Sprite>(spr);
		s.texture = "Space3.jpg";
		s.animated = true;
		s.frames = { 4, 4 };
		s.frame_time = 2.0f;
		core.entities.getComponent<ENG::CS::Transform>(spr).position.y = 7.5f;

		// Crosshair
		ENG::EntityID ch = core.entities.addEntity<ENG::CS::Transform2D, ENG::CS::Sprite>();
		ENG::CS::Sprite& sp = core.entities.getComponent<ENG::CS::Sprite>(ch);
		sp.texture = "crosshair.png";
		ENG::CS::Transform2D& t2d = core.entities.getComponent<ENG::CS::Transform2D>(ch);
		t2d.position = glm::vec2(core.window.getSize()) / 2.0f;
		t2d.origin = glm::vec2(core.resources.texture(sp.texture).getSize()) / 2.0f;

		// Create portals
		ENG::EntityID portal_a = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Portal, ENG::CS::Model>();
		ENG::EntityID portal_b = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Portal, ENG::CS::Model>();
		core.entities.getComponent<ENG::CS::Model>(portal_a).mesh = "portal_border.obj";
		core.entities.getComponent<ENG::CS::Model>(portal_a).texture = "brickwall.jpg";
		core.entities.getComponent<ENG::CS::Model>(portal_a).normal = "brickwall_normal.jpg";
		core.entities.getComponent<ENG::CS::Model>(portal_b).mesh = "portal_border.obj";
		core.entities.getComponent<ENG::CS::Model>(portal_b).texture = "brickwall.jpg";
		core.entities.getComponent<ENG::CS::Model>(portal_b).normal = "brickwall_normal.jpg";

		ENG::CS::Portal& pa = core.entities.getComponent<ENG::CS::Portal>(portal_a);
		pa.player = player;
		pa.other = portal_b;

		ENG::CS::Portal& pb = core.entities.getComponent<ENG::CS::Portal>(portal_b);
		pb.player = player;
		pb.other = portal_a;

		ENG::CS::Transform& ta = core.entities.getComponent<ENG::CS::Transform>(portal_a);
		ta.position = { 0.0f, 0.1f, 0.0f };
		ta.rotation.y = 45.0f;

		ENG::CS::Transform& tb = core.entities.getComponent<ENG::CS::Transform>(portal_b);
		tb.position = { 0.0f, 22.1f, 0.0f };
		tb.rotation.y = 180.0f;

		// Create other portals
		ENG::EntityID portal_c = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Portal, ENG::CS::Model>();
		ENG::EntityID portal_d = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Portal, ENG::CS::Model>();
		core.entities.getComponent<ENG::CS::Model>(portal_c).mesh = "portal_border.obj";
		core.entities.getComponent<ENG::CS::Model>(portal_d).mesh = "portal_border.obj";

		ENG::CS::Portal& pc = core.entities.getComponent<ENG::CS::Portal>(portal_c);
		pc.player = player;
		pc.other = portal_d;

		ENG::CS::Portal& pd = core.entities.getComponent<ENG::CS::Portal>(portal_d);
		pd.player = player;
		pd.other = portal_c;

		ENG::CS::Transform& tc = core.entities.getComponent<ENG::CS::Transform>(portal_c);
		tc.position = { -20.0f, 8.0f, 3.5f };
		tc.rotation.y = 90.0f;

		ENG::CS::Transform& td = core.entities.getComponent<ENG::CS::Transform>(portal_d);
		td.position = { 15.0f, 3.0f, 3.5f };
		td.rotation = { 0.0f, 45.0f, 0.0f };

		// Environment
		createProp(core, { -15.0f, 0.0f, -7.5f });
		createProp(core, { 15.0f, 0.0f, -7.5f });

		createBarrier(core, { 0.0f, -2.0f, 0.0f });
		auto b2 = createBarrier(core, { 0.0f, 20.0f, 0.0f });
		core.entities.getComponent<ENG::CS::Model>(b2).texture = "brickwall.jpg";
		core.entities.getComponent<ENG::CS::Model>(b2).normal = "brickwall_normal.jpg";

		ENG::EntityID plane = core.entities.addEntity<ENG::CS::Transform, ENG::CS::PlaneCollider>();
		core.entities.getComponent<ENG::CS::Transform>(plane).position.z = -20.0f;

		ENG::EntityID plane2 = core.entities.addEntity<ENG::CS::Transform, ENG::CS::PlaneCollider>();
		core.entities.getComponent<ENG::CS::Transform>(plane2).position.z = 20.0f;

		ENG::EntityID plane3 = core.entities.addEntity<ENG::CS::Transform, ENG::CS::PlaneCollider>();
		core.entities.getComponent<ENG::CS::Transform>(plane3).position.x = -20.0f;
		core.entities.getComponent<ENG::CS::Transform>(plane3).rotation.y = 90.0f;

		ENG::EntityID plane4 = core.entities.addEntity<ENG::CS::Transform, ENG::CS::PlaneCollider>();
		core.entities.getComponent<ENG::CS::Transform>(plane4).position.x = 20.0f;
		core.entities.getComponent<ENG::CS::Transform>(plane4).rotation.y = 90.0f;

		run(core);
	}
	catch (const std::exception& e)
	{
		OUTPUT_ERROR(e.what());
		PAUSE_CONSOLE;
	}

	return 0;
}