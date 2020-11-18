#include "Core.h"
#include "Components.h"
#include "Pickup.h"
#include "Player.h"
#include "Portal.h"

void createCore(ENG::Core& core, const std::string& setting_file)
{
	ENG::audioInit();
	core.settings.load(setting_file);

	glm::vec2 window_size(core.settings.getf("width"), core.settings.getf("height"));
	core.window.create(window_size, core.settings.get("title"));

	core.resources.loadMeshes(ENG::splitText(ENG::readTextFile(core.settings.get("meshes")), '\n'));
	core.resources.loadTextures(ENG::splitText(ENG::readTextFile(core.settings.get("textures")), '\n'));
	core.resources.loadSounds(ENG::splitText(ENG::readTextFile(core.settings.get("sounds")), '\n'));
	core.resources.loadShaders3D(ENG::splitText(ENG::readTextFile(core.settings.get("shaders3D")), '\n'));
	core.resources.loadShaders2D(ENG::splitText(ENG::readTextFile(core.settings.get("shaders2D")), '\n'));

	core.entities.addComponentPools<
		ENG::CS::Transform,
		ENG::CS::Transform2D,
		ENG::CS::Model,
		ENG::CS::Sprite,
		ENG::CS::Text,
		ENG::CS::Light,
		ENG::CS::Camera,
		ENG::CS::Script,
		ENG::CS::BoxCollider,
		ENG::CS::Controller
	>();
}

void run(ENG::Core& core)
{
	ENG::scriptStart(core);
	ENG::startRenderer(core);

	core.clock.reset();
	while (!core.window.shouldClose())
	{
		core.clock.update();

		ENG::scriptUpdate(core);
		ENG::moveControllers(core);
		ENG::updateSprites(core);
		ENG::scriptLateUpdate(core);

		core.window.clear({ 0.0f, 0.0f, 0.0f, 0.0f });
		ENG::drawToCameras(core);
		ENG::drawToScreen(core);
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
	core.entities.getComponent<ENG::CS::Model>(platform).texture = "metal.png";

	return platform;
}

void createScene(ENG::Core& core)
{
	ENG::EntityID player = Game::createPlayer(core);
	auto portals = Game::createPortalPair(core, player);
	core.entities.getComponent<ENG::CS::Transform>(portals.first).rotation.y = 45.0f;

	createBarrier(core, { 0.0f, -5.0f, 0.0f });
	core.entities.getComponent<ENG::CS::Transform>(core.entities.addEntity<ENG::CS::Transform, ENG::CS::Model>()).position.x = -10.0f;
}

int main()
{
	try
	{
		ENG::Core core;
		createCore(core, "Resources/settings.set");

		core.window.lockMouse(true);
		core.renderer.ambient = glm::vec3(0.4f);

		createScene(core);
		run(core);
	}
	catch (const std::exception& e)
	{
		OUTPUT_ERROR(e.what());
		PAUSE_CONSOLE;
	}

	PAUSE_CONSOLE;

	return 0;
}