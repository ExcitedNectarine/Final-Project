#include "Core.h"
#include "Components.h"
#include "Portal.h"
#include "Player.h"

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
		ENG::drawColliders(core);
		ENG::drawSprites3D(core);
		Game::drawPortals(core);
		ENG::drawModelsToHUD(core);
		ENG::drawSprites(core);

		core.window.display();

		glfwPollEvents();
	}

	ENG::scriptEnd(core);
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

		core.entities.addComponentPools<Game::Portal, Game::Pickup>();
		core.window.lockMouse(true);
		core.renderer.ambient = glm::vec3(0.5f);
		//core.renderer.draw_colliders = true;

		// Create player
		ENG::EntityID player = Game::createPlayer(core);

		ENG::EntityID table_scene = core.entities.addEntity<ENG::CS::Transform>();
		ENG::CS::Transform& ts_t = core.entities.getComponent<ENG::CS::Transform>(table_scene);
		ts_t.position.x = -5.0f;
		ts_t.position.y = 1.0f;
		ts_t.rotation.y = -34.0f;
		ts_t.scale = glm::vec3(0.1f);

		// Create portals
		ENG::EntityID portal_a = Game::createPortal(core);
		ENG::EntityID portal_b = Game::createPortal(core);

		Game::Portal& pa = core.entities.getComponent<Game::Portal>(portal_a);
		pa.player = player;
		pa.other = portal_b;

		Game::Portal& pb = core.entities.getComponent<Game::Portal>(portal_b);
		pb.player = player;
		pb.other = portal_a;

		ENG::CS::Transform& ta = core.entities.getComponent<ENG::CS::Transform>(portal_a);
		ta.position = { 0.0f, 0.01f, -10.0f };

		ENG::CS::Transform& tb = core.entities.getComponent<ENG::CS::Transform>(portal_b);
		tb.position = { 0.0f, 0.01f, 0.0f };
		tb.parent = table_scene;

		for (int i = 0; i < 5; i++)
			Game::createPickup(core, { ENG::randomFloat(-15.0f, 15.0f), 0.0f, ENG::randomFloat(-15.0f, 15.0f) });

		createBarrier(core, { 0.0f, -2.0f, 0.0f });
		auto b = createBarrier(core, { 0.0f, -2.0f, 0.0f });
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