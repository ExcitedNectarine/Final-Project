#include "Core.h"

struct PortalScript : ENG::Script
{
	ENG::EntityID other;
	ENG::CS::Transform* other_t;

	PortalScript(ENG::EntityID other) : other(other) {}

	void start(ENG::Core& core)
	{
		other_t = &core.entities.getComponent<ENG::CS::Transform>(other);
	}
};

struct PlayerScript : ENG::Script
{
	ENG::CS::Transform* transform;
	ENG::CS::Controller* controller;
	glm::vec3 direction;
	glm::vec3 velocity;
	float speed = 15.0f;

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

		if (transform->rotation.x <= -89.0f)
			transform->rotation.x = -89.0f;
		else if (transform->rotation.x >= 89.0f)
			transform->rotation.x = 89.0f;
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

	void onCollision(ENG::Core& core, ENG::EntityID hit_id)
	{
		transform->position = std::dynamic_pointer_cast<PortalScript>(core.entities.getComponent<ENG::CS::Script>(hit_id).script)->other_t->position;
		OUTPUT("done");
	}
};

int main()
{
	try
	{
		ENG::Core core("Resources/settings.set");
		core.resources.shader("default.shader").setUniform("ambient", { 0.4f, 0.4f, 0.4f });

		core.window.lockMouse(true);

		// Create player
		ENG::EntityID player = core.entities.addEntity<ENG::CS::Script, ENG::CS::Transform, ENG::CS::BoxCollider, ENG::CS::Controller, ENG::CS::Light>();
		core.entities.getComponent<ENG::CS::Script>(player).script = std::make_shared<PlayerScript>();

		// Create portals
		ENG::EntityID portal = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Screen, ENG::CS::FrameBuffer, ENG::CS::Script, ENG::CS::BoxCollider>();
		ENG::EntityID portal2 = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Screen, ENG::CS::FrameBuffer, ENG::CS::Script, ENG::CS::BoxCollider>();

		core.entities.getComponent<ENG::CS::FrameBuffer>(portal).create({ 1280, 720 });
		core.entities.getComponent<ENG::CS::FrameBuffer>(portal2).create({ 1280, 720 });

		ENG::CS::Screen& m = core.entities.getComponent<ENG::CS::Screen>(portal);
		m.mesh = "quad.obj";
		m.framebuffer_id = portal2;
		m.shader = "default.shader";

		ENG::CS::Screen& m2 = core.entities.getComponent<ENG::CS::Screen>(portal2);
		m2.mesh = "quad.obj";
		m2.framebuffer_id = portal;
		m2.shader = "default.shader";

		ENG::CS::Transform& t = core.entities.getComponent<ENG::CS::Transform>(portal);
		t.position.x = -10;
		ENG::CS::Transform& t2 = core.entities.getComponent<ENG::CS::Transform>(portal2);
		t2.position.z = -10;

		ENG::CS::Script& ps = core.entities.getComponent<ENG::CS::Script>(portal);
		ps.script = std::make_shared<PortalScript>(portal2);
		ENG::CS::Script& ps2 = core.entities.getComponent<ENG::CS::Script>(portal2);
		ps2.script = std::make_shared<PortalScript>(portal);

		core.entities.getComponent<ENG::CS::BoxCollider>(portal).solid = false;
		core.entities.getComponent<ENG::CS::BoxCollider>(portal2).solid = false;

		// Create platform
		ENG::EntityID platform = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Model, ENG::CS::BoxCollider>();
		ENG::CS::Model& pm = core.entities.getComponent<ENG::CS::Model>(platform);
		pm.mesh = "cube2.obj";
		pm.texture = "rock.png";
		pm.shader = "default.shader";

		core.entities.getComponent<ENG::CS::Transform>(platform).scale = { 25.0f, 1.0f, 25.0f };
		core.entities.getComponent<ENG::CS::Transform>(platform).position.y = -2;

		core.run();
	}
	catch (const std::exception& e)
	{
		OUTPUT_ERROR(e.what());
		PAUSE_CONSOLE;
	}

	return 0;
}