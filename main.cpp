#include "Core.h"
#include <glm/gtx/string_cast.hpp>

struct PlayerScript : ENG::Script
{
	ENG::CS::Transform* transform;
	ENG::CS::Controller* controller;
	glm::vec3 direction;
	glm::vec3 velocity;
	float speed = 8.0f;

	glm::dvec2 last_mouse;
	glm::dvec2 mouse_offset;
	float sensitivity = 0.1f;

	void start(ENG::Core& core)
	{
		transform = &core.entities.getComponent<ENG::CS::Transform>(id);
		controller = &core.entities.getComponent<ENG::CS::Controller>(id);
		transform->position.y = 20.0f;

		core.entities.getComponent<ENG::CS::BoxCollider>(id).size.y = 2.0f;
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
			transform->position = { 0.0f, 10.0f, 0.0f };

		if (controller->on_floor && core.window.isKeyPressed(GLFW_KEY_SPACE))
		{
			velocity.y = 5.0f;
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

	void update(ENG::Core& core)
	{
		if (core.window.isKeyPressed(GLFW_KEY_ESCAPE))
			core.window.close();

		mouselook(core);
		movement(core);

		core.view = transform;
	}
};

void createProp(ENG::Core& core, glm::vec3 pos)
{
	ENG::EntityID prop = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Model, ENG::CS::BoxCollider, ENG::CS::Light>();

	ENG::CS::Model& prop_m = core.entities.getComponent<ENG::CS::Model>(prop);
	prop_m.mesh = "skull.obj";
	prop_m.texture = "skull.jpg";

	core.entities.getComponent<ENG::CS::Transform>(prop).position = pos;
	core.entities.getComponent<ENG::CS::Transform>(prop).rotation.x = -90;
	core.entities.getComponent<ENG::CS::Transform>(prop).scale *= 0.1f;
	core.entities.getComponent<ENG::CS::BoxCollider>(prop).size = { 20.0f, 20.0f, 20.0f };
	core.entities.getComponent<ENG::CS::Light>(prop).colour = { 25.0f, 25.0f, 50.0f };
	core.entities.getComponent<ENG::CS::Light>(prop).radius = 15.0f;
}

void createBarrier(ENG::Core& core, glm::vec3 pos)
{
	ENG::EntityID platform = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Model, ENG::CS::BoxCollider>();

	core.entities.getComponent<ENG::CS::Transform>(platform).position = pos;
	core.entities.getComponent<ENG::CS::Transform>(platform).scale = { 20.0f, 1.0f, 20.0f };
	core.entities.getComponent<ENG::CS::BoxCollider>(platform).size = { 2.0f, 2.0f, 2.0f };
}

int main()
{
	try
	{
		ENG::Core core("Resources/settings.set");
		core.resources.shader("default.shdr").setUniform("ambient", glm::vec3(0.2f));

		core.window.lockMouse(true);

		// Create player
		ENG::EntityID player = core.entities.addEntity<ENG::CS::Script, ENG::CS::Transform, ENG::CS::BoxCollider, ENG::CS::Controller, ENG::CS::Camera>();
		core.entities.getComponent<ENG::CS::Script>(player).script = std::make_shared<PlayerScript>();

		ENG::EntityID gun = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Model>();
		ENG::CS::Model& gm = core.entities.getComponent<ENG::CS::Model>(gun);
		gm.mesh = "gun.obj";
		gm.texture = "gun.png";
		gm.hud = true;

		ENG::CS::Transform& t = core.entities.getComponent<ENG::CS::Transform>(gun);
		t.parent = player;
		t.position = { 0.5f, -0.5f, -0.5f };
		t.scale *= 0.25f;
		t.rotation = { 0.0f, 180.0f, 180.0f };

		// Create portals
		ENG::EntityID portal_a = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Portal>();
		ENG::EntityID portal_b = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Portal>();

		ENG::CS::Portal& pa = core.entities.getComponent<ENG::CS::Portal>(portal_a);
		pa.player = player;
		pa.other = portal_b;

		ENG::CS::Portal& pb = core.entities.getComponent<ENG::CS::Portal>(portal_b);
		pb.player = player;
		pb.other = portal_a;

		ENG::CS::Transform& ta = core.entities.getComponent<ENG::CS::Transform>(portal_a);
		ta.position = { 0.0f, 0.05f, -7.5f };

		ENG::CS::Transform& tb = core.entities.getComponent<ENG::CS::Transform>(portal_b);
		tb.position = { 0.0f, 22.05f, 7.5f };
		tb.rotation.y = 180.0f;

		// Create other portals
		//ENG::EntityID portal_c = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Portal>();
		//ENG::EntityID portal_d = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Portal>();

		//ENG::CS::Portal& pc = core.entities.getComponent<ENG::CS::Portal>(portal_c);
		//pc.player = player;
		//pc.other = portal_d;

		//ENG::CS::Portal& pd = core.entities.getComponent<ENG::CS::Portal>(portal_d);
		//pd.player = player;
		//pd.other = portal_c;

		//ENG::CS::Transform& tc = core.entities.getComponent<ENG::CS::Transform>(portal_c);
		//tc.position = { -15.0f, 0.05f, 0.0f };
		//tc.rotation.y = 90.0f;

		//ENG::CS::Transform& td = core.entities.getComponent<ENG::CS::Transform>(portal_d);
		//td.position = { 15.0f, 0.05f, 0.0f };
		//td.rotation.y = 90.0f;

		// Environment
		createProp(core, { -15.0f, 0.0f, -7.5f });
		createProp(core, { -5.0f, 0.0f, 7.5f });

		createBarrier(core, { 0.0f, -2.0f, 0.0f });
		createBarrier(core, { 0.0f, 20.0f, 0.0f });

		core.run();
	}
	catch (const std::exception& e)
	{
		OUTPUT_ERROR(e.what());
		PAUSE_CONSOLE;
	}

	return 0;
}