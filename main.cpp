#include "Core.h"
#include "Components.h"
#include "Portal.h"

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
		ENG::CS::Controller
	>();
}

void run(ENG::Core& core)
{
	ENG::scriptStart(core);
	Game::startPortals(core.entities, core.window.getSize());
	ENG::spriteStart();

	double current = 0.0, last = 0.0;
	while (!core.window.shouldClose())
	{
		current = glfwGetTime();
		core.delta = static_cast<float>(current - last);
		last = current;

		ENG::scriptUpdate(core);
		ENG::moveControllers(core);

		Game::updatePortals(core.entities);
		Game::drawToPortals(core);

		ENG::updateSprites(core);
		ENG::updateRenderer(core);

		core.window.clear({ 0.0f, 0.0f, 0.0f, 0.0f });

		ENG::drawSkybox(core.resources);
		ENG::drawModels(core);
		ENG::drawSprites3D(core);
		Game::drawPortals(core);
		ENG::drawModelsToHUD(core);
		ENG::drawSprites(core);

		core.window.display();

		glfwPollEvents();
	}

	ENG::scriptEnd(core);
}

////////////////////////////////////

namespace Game
{
	struct Pickup : ENG::ECSComponent<Pickup>
	{
		bool active = false;
	};
}

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

		controller->velocity = velocity * transform->scale;
	}

	void actions(ENG::Core& core)
	{
		if (pickup == 0 && core.window.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			pickup = ENG::castRay(core.entities, transform->position, -transform->forward(), id, dist);

			if (pickup != 0 && core.entities.hasComponent<Game::Pickup>(pickup))
			{
				OUTPUT("Pickup up " << pickup);

				core.entities.getComponent<ENG::CS::Transform>(pickup).parent = id;
				core.entities.getComponent<ENG::CS::Transform>(pickup).position = { 0.0f, 0.0f, -3.5f };
				core.entities.getComponent<ENG::CS::Transform>(pickup).rotation = glm::vec3(0.0f);
				core.entities.getComponent<ENG::CS::Model>(pickup).hud = true;
				core.entities.getComponent<ENG::CS::BoxCollider>(pickup).trigger = true;
			}
			else pickup = 0;
		}

		if (pickup != 0 && core.window.isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
		{
			ENG::EntityID floor = ENG::castRay(core.entities, transform->position, -transform->forward(), id, dist);
			if (floor != 0)
			{
				glm::vec3 new_pos = transform->position + dist * -transform->forward();
				new_pos.y += core.entities.getComponent<ENG::CS::BoxCollider>(pickup).size.y;

				core.entities.getComponent<ENG::CS::Transform>(pickup).parent = 0;
				core.entities.getComponent<ENG::CS::Transform>(pickup).position = new_pos;
				core.entities.getComponent<ENG::CS::Model>(pickup).hud = false;
				core.entities.getComponent<ENG::CS::BoxCollider>(pickup).trigger = false;

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

		core.renderer.view = transform;
	}
};

ENG::EntityID createProp(ENG::Core& core, glm::vec3 pos)
{
	ENG::EntityID prop = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Model, ENG::CS::Light, ENG::CS::BoxCollider, Game::Pickup>();

	core.entities.getComponent<ENG::CS::Transform>(prop).position = pos;
	core.entities.getComponent<ENG::CS::Light>(prop).colour = { 2.5f, 1.0f, 0.5f };
	core.entities.getComponent<ENG::CS::Light>(prop).radius = 10.0f;

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

		core.window.lockMouse(true);
		core.entities.addComponentPool<Game::Portal>();
		core.entities.addComponentPool<Game::Pickup>();
		core.renderer.ambient = glm::vec3(0.5f);

		// Create player
		ENG::EntityID player = core.entities.addEntity<ENG::CS::Script, ENG::CS::Transform, ENG::CS::BoxCollider, ENG::CS::Controller, ENG::CS::Model>();
		core.entities.getComponent<ENG::CS::Script>(player).script = std::make_shared<PlayerScript>();

		ENG::EntityID table_scene = core.entities.addEntity<ENG::CS::Transform>();
		ENG::CS::Transform& ts_t = core.entities.getComponent<ENG::CS::Transform>(table_scene);
		ts_t.position.x = -5.0f;
		ts_t.rotation.y = -31.0f;
		ts_t.scale = glm::vec3(0.2f);

		// Create portals
		ENG::EntityID portal_a = core.entities.addEntity<ENG::CS::Transform, Game::Portal, ENG::CS::Model>();
		ENG::EntityID portal_b = core.entities.addEntity<ENG::CS::Transform, Game::Portal, ENG::CS::Model>();
		core.entities.getComponent<ENG::CS::Model>(portal_a).mesh = "portal_border.obj";
		core.entities.getComponent<ENG::CS::Model>(portal_b).mesh = "portal_border.obj";

		Game::Portal& pa = core.entities.getComponent<Game::Portal>(portal_a);
		pa.player = player;
		pa.other = portal_b;

		Game::Portal& pb = core.entities.getComponent<Game::Portal>(portal_b);
		pb.player = player;
		pb.other = portal_a;

		ENG::CS::Transform& ta = core.entities.getComponent<ENG::CS::Transform>(portal_a);
		ta.position = { 0.0f, 0.01f, -10.0f };

		ENG::CS::Transform& tb = core.entities.getComponent<ENG::CS::Transform>(portal_b);
		tb.position = { 0.0f, 1.0f, 0.0f };
		tb.parent = table_scene;

		for (int i = 0; i < 5; i++)
			createProp(core, { ENG::randomFloat(-15.0f, 15.0f), 0.0f, ENG::randomFloat(-15.0f, 15.0f) });

		createBarrier(core, { 0.0f, -2.0f, 0.0f });
		auto b = createBarrier(core, { 0.0f, -0.5f, 0.0f });
		core.entities.getComponent<ENG::CS::Transform>(b).scale = { 2.0f, 0.5f, 2.0f };
		core.entities.getComponent<ENG::CS::Transform>(b).parent = table_scene;

		run(core);
	}
	catch (const std::exception& e)
	{
		OUTPUT_ERROR(e.what());
		PAUSE_CONSOLE;
	}

	return 0;
}