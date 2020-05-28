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

	core.perspective = glm::perspective(glm::radians(core.settings.getf("fov")), window_size.x / window_size.y, 0.1f, 250.0f);
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
		Game::updatePickups(core);

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
	core.entities.getComponent<ENG::CS::Model>(platform).texture = "metal.png";

	return platform;
}

void createTableScene(ENG::Core& core)
{
	// Create player
	ENG::EntityID player = Game::createPlayer(core);

	ENG::EntityID table_scene = core.entities.addEntity<ENG::CS::Transform>();
	ENG::CS::Transform& ts_t = core.entities.getComponent<ENG::CS::Transform>(table_scene);
	ts_t.position.y = 1.0f;
	ts_t.position.z = 5.0f;
	ts_t.rotation.y = 45.0f;
	ts_t.scale = glm::vec3(0.2f);

	// Create portals
	ENG::EntityID portal_a = Game::createPortal(core);
	ENG::EntityID portal_b = Game::createPortal(core);

	// Connect portals to eachother
	Game::Portal& pa = core.entities.getComponent<Game::Portal>(portal_a);
	pa.other = portal_b;
	Game::Portal& pb = core.entities.getComponent<Game::Portal>(portal_b);
	pb.other = portal_a;

	// Position portals
	ENG::CS::Transform& ta = core.entities.getComponent<ENG::CS::Transform>(portal_a);
	ta.position = { 0.0f, 0.01f, -10.0f };

	ENG::CS::Transform& tb = core.entities.getComponent<ENG::CS::Transform>(portal_b);
	tb.position = { 0.0f, 0.01f, 0.0f };
	tb.parent = table_scene;

	// Create lamps
	for (int i = 0; i < 5; i++)
		Game::createPickup(core, { ENG::randomFloat(-15.0f, 15.0f), 0.0f, ENG::randomFloat(-15.0f, 15.0f) });

	// Create floors
	createBarrier(core, { 0.0f, -2.0f, 0.0f });
	auto b = createBarrier(core, { 0.0f, -2.0f, 0.0f });
	core.entities.getComponent<ENG::CS::Transform>(b).parent = table_scene;
}

ENG::EntityID createRoom(ENG::Core& core, std::string texture)
{
	using namespace ENG;

	EntityID room = core.entities.addEntity<CS::Transform>();

	// floor
	EntityID floor = createBarrier(core, { 0.0f, -2.0f, 0.0f });
	core.entities.getComponent<CS::Model>(floor).texture = texture;
	CS::Transform& floor_t = core.entities.getComponent<CS::Transform>(floor);
	floor_t.parent = room;

	// ceiling
	EntityID ceiling = createBarrier(core, { 0.0f, 4.0f, 0.0f });
	core.entities.getComponent<CS::Model>(ceiling).texture = texture;
	CS::Transform& ceiling_t = core.entities.getComponent<CS::Transform>(ceiling);
	ceiling_t.parent = room;

	//left wall
	EntityID left = createBarrier(core, { -20.0f, 0.0f, 0.0f });
	core.entities.getComponent<CS::Model>(left).texture = texture;
	CS::Transform& left_t = core.entities.getComponent<CS::Transform>(left);
	left_t.scale = { 1.0f, 3.0f, 20.0f };
	left_t.parent = room;

	//right wall
	EntityID right = createBarrier(core, { 20.0f, 0.0f, 0.0f });
	core.entities.getComponent<CS::Model>(right).texture = texture;
	CS::Transform& right_t = core.entities.getComponent<CS::Transform>(right);
	right_t.scale = { 1.0f, 3.0f, 20.0f };
	right_t.parent = room;

	//front wall
	EntityID front = createBarrier(core, { 0.0f, 0.0f, -20.0f });
	core.entities.getComponent<CS::Model>(front).texture = texture;
	CS::Transform& front_t = core.entities.getComponent<CS::Transform>(front);
	front_t.scale = { 20.0f, 3.0f, 1.0f };
	front_t.parent = room;

	//back wall
	EntityID back = createBarrier(core, { 0.0f, 0.0f, 20.0f });
	core.entities.getComponent<CS::Model>(back).texture = texture;
	CS::Transform& back_t = core.entities.getComponent<CS::Transform>(back);
	back_t.scale = { 20.0f, 3.0f, 1.0f };
	back_t.parent = room;

	return room;
}

void createImpossibleRooms(ENG::Core& core)
{
	using namespace ENG;

	EntityID room1 = createRoom(core, "metal.png");
	CS::Transform& room1_t = core.entities.getComponent<CS::Transform>(room1);
	room1_t.position.x = -50.0f;

	EntityID room2 = createRoom(core, "notexture.png");
	CS::Transform& room2_t = core.entities.getComponent<CS::Transform>(room2);
	room2_t.position.x = 50.0f;
	room2_t.rotation.y = 45.0f;
	room2_t.scale.x *= 0.25f;
	room2_t.scale.z *= 0.25f;

	EntityID player = Game::createPlayer(core);
	CS::Transform& player_t = core.entities.getComponent<CS::Transform>(player);
	player_t.position.x = room1_t.position.x;

	// Create portals
	EntityID portal_a = Game::createPortal(core);
	EntityID portal_b = Game::createPortal(core);

	// Connect portals to eachother
	Game::Portal& pa = core.entities.getComponent<Game::Portal>(portal_a);
	pa.other = portal_b;
	Game::Portal& pb = core.entities.getComponent<Game::Portal>(portal_b);
	pb.other = portal_a;

	// Position portals
	CS::Transform& ta = core.entities.getComponent<CS::Transform>(portal_a);
	ta.position = room1_t.position;
	ta.position.y = 0.01f;

	CS::Transform& tb = core.entities.getComponent<CS::Transform>(portal_b);
	tb.position = room2_t.position;
	tb.position.y = 0.01f;
}

int main()
{
	try
	{
		int i = 0;
		do {
			std::cout << "Press 1 for portal demo 1 (table scene), or press 2 for portal demo 2 (impossible room). ";
			std::cin >> i;
		} while (i != 1 && i != 2);

		ENG::Core core;
		createCore(core, "Resources/settings.set");

		core.entities.addComponentPools<Game::Portal, Game::Traveller, Game::Pickup>();
		core.window.lockMouse(true);
		core.renderer.ambient = glm::vec3(0.8f);

		if (i == 1)
			createTableScene(core);
		else if (i == 2)
			createImpossibleRooms(core);

		run(core);
	}
	catch (const std::exception& e)
	{
		OUTPUT_ERROR(e.what());
		PAUSE_CONSOLE;
	}

	return 0;
}