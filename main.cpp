#include "Core.h"

struct PlayerScript : ENG::Script
{
	ENG::CS::Transform* transform;
	ENG::CS::Controller* controller;
	glm::vec3 direction;
	glm::vec3 velocity;
	float speed = 5.0f;

	glm::dvec2 last_mouse;
	glm::dvec2 mouse_offset;
	float sensitivity = 0.1f;

	void start(ENG::Core& core)
	{
		transform = &core.entities.getComponent<ENG::CS::Transform>(id);
		controller = &core.entities.getComponent<ENG::CS::Controller>(id);
	}

	void mouselook(ENG::Core& core)
	{
		mouse_offset = last_mouse - core.window.getMousePos();
		last_mouse = core.window.getMousePos();
		transform->rotation += glm::vec3(mouse_offset.y, mouse_offset.x, 0.0f) * sensitivity;
	}

	void movement(ENG::Core& core)
	{
		direction = glm::vec3(0.0f);
		if (core.window.isKeyPressed(GLFW_KEY_W)) direction -= transform->forward();
		else if (core.window.isKeyPressed(GLFW_KEY_S)) direction += transform->forward();
		if (core.window.isKeyPressed(GLFW_KEY_A)) direction -= transform->right();
		else if (core.window.isKeyPressed(GLFW_KEY_D)) direction += transform->right();

		if (direction != glm::vec3(0.0f))
			direction = glm::normalize(direction);

		velocity.x = direction.x * speed;
		velocity.z = direction.z * speed;

		controller->velocity = velocity * core.delta;
	}

	void update(ENG::Core& core)
	{
		if (core.window.isKeyPressed(GLFW_KEY_ESCAPE))
			core.window.close();

		mouselook(core);
		movement(core);
		core.view = *transform;
	}
};

void createProp(ENG::Core& core, glm::vec3 pos)
{
	ENG::EntityID prop = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Model>();
	ENG::CS::Model& prop_m = core.entities.getComponent<ENG::CS::Model>(prop);
	prop_m.mesh = "skull.obj";
	prop_m.shader = "default.shader";
	prop_m.texture = "skull.jpg";
	core.entities.getComponent<ENG::CS::Transform>(prop).position = pos;
	core.entities.getComponent<ENG::CS::Transform>(prop).scale *= 0.1f;
}

int main()
{
	try
	{
		ENG::Core core("Resources/settings.set");
		core.resources.shader("default.shader").setUniform("ambient", glm::vec3(0.5f));

		core.window.lockMouse(true);

		// Create player
		ENG::EntityID player = core.entities.addEntity<ENG::CS::Script, ENG::CS::Transform, ENG::CS::BoxCollider, ENG::CS::Controller, ENG::CS::Light>();
		core.entities.getComponent<ENG::CS::Script>(player).script = std::make_shared<PlayerScript>();

		// Create portals
		ENG::EntityID portal_a = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Portal>();
		ENG::EntityID portal_b = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Portal>();

		ENG::CS::Portal& pa = core.entities.getComponent<ENG::CS::Portal>(portal_a);
		pa.framebuffer.create(core.window.getSize());
		pa.player = player;
		pa.other = portal_b;

		ENG::CS::Portal& pb = core.entities.getComponent<ENG::CS::Portal>(portal_b);
		pb.framebuffer.create(core.window.getSize());
		pb.player = player;
		pb.other = portal_a;

		// Position portals
		ENG::CS::Transform& ta = core.entities.getComponent <ENG::CS::Transform>(portal_a);
		ta.position = { -15.0f, 0.0f, -2.0f };
		ta.rotation.y = 45.0f;

		ENG::CS::Transform& tb = core.entities.getComponent<ENG::CS::Transform>(portal_b);
		tb.position = { 15.0f, 0.0f, -2.0f };
		tb.rotation.y = -45.0f;

		createProp(core, { -15.0f, 0.0f, -10.0f });
		createProp(core, { -5.0f, 2.0f, 10.0f });

		ENG::EntityID platform = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Model>();
		ENG::CS::Model& platform_m = core.entities.getComponent<ENG::CS::Model>(platform);
		platform_m.mesh = "cube2.obj";
		platform_m.shader = "default.shader";
		platform_m.texture = "rock.png";
		core.entities.getComponent<ENG::CS::Transform>(platform).position.y = -2.0f;
		core.entities.getComponent<ENG::CS::Transform>(platform).scale = { 20.0f, 1.0f, 20.0f };

		core.run();
	}
	catch (const std::exception& e)
	{
		OUTPUT_ERROR(e.what());
		PAUSE_CONSOLE;
	}

	return 0;
}