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

	core.camera = ENG::CS::Camera(window_size, core.settings.getf("fov"), 0.1f, 250.0f);
	core.perspective = core.camera.get();
	core.orthographic = glm::ortho(0.0f, window_size.x, window_size.y, 0.0f);

	core.window.create(window_size, core.settings.get("title"));

	core.resources.loadMeshes(ENG::splitText(ENG::readTextFile(core.settings.get("meshes")), '\n'));
	core.resources.loadTextures(ENG::splitText(ENG::readTextFile(core.settings.get("textures")), '\n'));
	core.resources.loadSounds(ENG::splitText(ENG::readTextFile(core.settings.get("sounds")), '\n'));
	core.resources.loadShaders(ENG::splitText(ENG::readTextFile(core.settings.get("shaders")), '\n'));

	core.resources.shader("default.shdr").setUniform("projection", core.perspective);
	core.resources.shader("skybox.shdr").setUniform("projection", core.perspective);
	core.resources.shader("sprite.shdr").setUniform("projection", core.orthographic);

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

	Game::startPortals(core.entities, core.window.getSize());

	glfwSetTime(0.0f);
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

		//ENG::drawSkybox(core.resources);
		//ENG::drawModels(core);
		//ENG::drawColliders(core);
		//ENG::drawSprites3D(core);
		//Game::drawPortals(core);
		//ENG::drawModelsToHUD(core);
		//ENG::drawSprites(core);
		//ENG::renderText(core);

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

void createTableScene(ENG::Core& core)
{
	//ENG::EntityID words = core.entities.addEntity<ENG::CS::Transform2D, ENG::CS::Text>();
	//core.entities.getComponent<ENG::CS::Transform2D>(words).position = glm::vec2(50.0f);

	ENG::EntityID x = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Sprite>();
	core.entities.getComponent<ENG::CS::Transform>(x).position.y = 5.0f;
	core.entities.getComponent<ENG::CS::Transform>(x).scale *= 0.2f;
	core.entities.getComponent<ENG::CS::Sprite>(x).texture = "Space3.jpg";

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
	ENG::EntityID box = core.entities.addEntity<ENG::CS::Transform, ENG::CS::BoxCollider>();
	core.entities.getComponent<ENG::CS::BoxCollider>(box).trigger = true;
	core.entities.getComponent<ENG::CS::Transform>(box).position.x = -7.5f;
	core.entities.getComponent<ENG::CS::Transform>(box).rotation.y = 45.0f;

	for (int i = 0; i < 5; i++)
		ENG::EntityID id = Game::createPickup(core, { ENG::randomFloat(-15.0f, 15.0f), 0.0f, ENG::randomFloat(-15.0f, 15.0f) });

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
	//room2_t.position.x = 50.0f;
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

struct CBox : ENG::Script
{
	glm::vec3 vel;
	float speed = 5.0f;
	ENG::EntityID frustum;

	void update(ENG::Core& core)
	{
		ENG::CS::Transform& t = core.entities.getComponent<ENG::CS::Transform>(id);
		t.rotation.y += 45.0f * core.delta;

		vel = glm::vec3(0.0f);

		if (core.window.isKeyPressed(GLFW_KEY_UP))
			vel.z = -speed;
		if (core.window.isKeyPressed(GLFW_KEY_DOWN))
			vel.z = speed;
		if (core.window.isKeyPressed(GLFW_KEY_LEFT))
			vel.x = -speed;
		if (core.window.isKeyPressed(GLFW_KEY_RIGHT))
			vel.x = speed;
		if (core.window.isKeyPressed(GLFW_KEY_U))
			vel.y = speed;
		if (core.window.isKeyPressed(GLFW_KEY_J))
			vel.y = -speed;

		t.position += vel * core.delta;

		ENG::CS::Camera cam(glm::vec2(1.0f), 70.0f, 1.0f, 10.0f);
		ENG::CS::Transform& ct = core.entities.getComponent<ENG::CS::Transform>(frustum);

		ENG::IntersectData d = ENG::intersectOBBvFrustum(t, glm::vec3(1.0f), ct, cam);
		if (d.intersects)
		{
			core.entities.getComponent<ENG::CS::Model>(id).texture = "Space3.jpg";
			t.position -= d.distance * d.normal;
		}
		else
		{
			core.entities.getComponent<ENG::CS::Model>(id).texture = "notexture.png";
		}
	}
};

void frustumCollisions(ENG::Core& core)
{
	using namespace ENG;

	EntityID p = Game::createPlayer(core);
	createBarrier(core, { 0.0f, -2.0f, 0.0f });

	CS::Camera cam(glm::vec2(1.0f), 70.0f, 1.0f, 10.0f);

	EntityID frustum = core.entities.addEntity<CS::Transform, CS::Model>();
	CS::Transform& frustum_t = core.entities.getComponent<CS::Transform>(frustum);
	frustum_t.position = { 0.0f, 5.0f, 0.0f };
	frustum_t.rotation = { randomFloat(0.0f, 180.0f), randomFloat(0.0f, 180.0f), randomFloat(0.0f, 180.0f) };
	frustum_t.scale *= 0.5f;

	CS::Model& fmod = core.entities.getComponent<CS::Model>(frustum);
	fmod.mesh = "frustumcube.obj";

	ENG::Mesh& m = core.resources.mesh(fmod.mesh);
	for (std::size_t i = 0; i < m.vertexCount(); i++)
	{
		glm::vec4 new_p = glm::inverse(cam.get()) * glm::vec4(m[i].position, 1.0f);
		m[i].position = glm::vec3(new_p) / new_p.w;
	}

	// Test box
	EntityID box = core.entities.addEntity<CS::Transform, CS::Model, CS::BoxCollider, CS::Script>();
	core.entities.getComponent<CS::Script>(box).script = std::make_shared<CBox>();
	std::dynamic_pointer_cast<CBox>(core.entities.getComponent<CS::Script>(box).script)->frustum = frustum;
}

int main()
{
	try
	{
		int i = 0;
		do {
			std::cout << "Press 1 for portal demo 1 (table scene), or press 2 for portal demo 2 (impossible room). ";
			std::cin >> i;
		} while (i != 1 && i != 2 && i != 3);

		ENG::Core core;
		createCore(core, "Resources/settings.set");

		core.entities.addComponentPools<Game::Portal, Game::Traveller, Game::Pickup>();
		core.window.lockMouse(true);
		core.renderer.ambient = glm::vec3(0.1f);

		if (i == 1)
			createTableScene(core);
		else if (i == 2)
			createImpossibleRooms(core);
		else if (i == 3)
			frustumCollisions(core);

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