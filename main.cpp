#include "Core.h"
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/quaternion.hpp>

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

struct PortalCamScript : ENG::Script
{
	ENG::EntityID player;
	ENG::EntityID portal_a;
	ENG::EntityID portal_b;

	ENG::CS::Transform* player_t;
	ENG::CS::Transform* portal_a_t;
	ENG::CS::Transform* portal_b_t;
	ENG::CS::Transform* transform;

	PortalCamScript(ENG::EntityID player, ENG::EntityID portal_a, ENG::EntityID portal_b) : player(player), portal_a(portal_a), portal_b(portal_b) {}

	void start(ENG::Core& core)
	{
		player_t = &core.entities.getComponent<ENG::CS::Transform>(player);
		portal_a_t = &core.entities.getComponent<ENG::CS::Transform>(portal_a);
		portal_b_t = &core.entities.getComponent<ENG::CS::Transform>(portal_b);
		transform = &core.entities.getComponent<ENG::CS::Transform>(id);
	}

	void update(ENG::Core& core)
	{
		glm::vec3 player_offset = player_t->position - portal_b_t->position;
		transform->position = portal_a_t->position + player_offset;

		transform->rotation = player_t->rotation;
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
	}
};

int main()
{
	try
	{
		ENG::Core core("Resources/settings.set");
		core.resources.shader("default.shader").setUniform("ambient", { 0.5f, 0.5f, 0.5f });

		core.window.lockMouse(true);

		// Create player
		ENG::EntityID player = core.entities.addEntity<ENG::CS::Script, ENG::CS::Transform, ENG::CS::BoxCollider, ENG::CS::Controller>();
		core.entities.getComponent<ENG::CS::Script>(player).script = std::make_shared<PlayerScript>();

		// Create portals
		ENG::EntityID portal_a = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Screen, ENG::CS::Script, ENG::CS::BoxCollider>();
		ENG::EntityID portal_b = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Screen, ENG::CS::Script, ENG::CS::BoxCollider>();

		ENG::EntityID portal_a_cam = core.entities.addEntity<ENG::CS::Transform, ENG::CS::FrameBuffer, ENG::CS::Script>();
		ENG::EntityID portal_b_cam = core.entities.addEntity<ENG::CS::Transform, ENG::CS::FrameBuffer, ENG::CS::Script>();

		core.entities.getComponent<ENG::CS::Script>(portal_a_cam).script = std::make_shared<PortalCamScript>(player, portal_a, portal_b);
		core.entities.getComponent<ENG::CS::Script>(portal_b_cam).script = std::make_shared<PortalCamScript>(player, portal_b, portal_a);

		core.entities.getComponent<ENG::CS::FrameBuffer>(portal_a_cam).create(core.window.getSize());
		core.entities.getComponent<ENG::CS::FrameBuffer>(portal_b_cam).create(core.window.getSize());

		ENG::CS::Screen& m = core.entities.getComponent<ENG::CS::Screen>(portal_a);
		m.mesh = "quad.obj";
		m.framebuffer_id = portal_b_cam;
		m.shader = "default.shader";

		ENG::CS::Screen& m2 = core.entities.getComponent<ENG::CS::Screen>(portal_b);
		m2.mesh = "quad.obj";
		m2.framebuffer_id = portal_a_cam;
		m2.shader = "default.shader";

		ENG::CS::Transform& t = core.entities.getComponent<ENG::CS::Transform>(portal_a);
		t.position.x = -10;
		t.scale *= 2.0f;
		ENG::CS::Transform& t2 = core.entities.getComponent<ENG::CS::Transform>(portal_b);
		t2.position.z = -10;
		t2.scale *= 2.0f;

		ENG::CS::Script& ps = core.entities.getComponent<ENG::CS::Script>(portal_a);
		ps.script = std::make_shared<PortalScript>(portal_b);
		ENG::CS::Script& ps2 = core.entities.getComponent<ENG::CS::Script>(portal_b);
		ps2.script = std::make_shared<PortalScript>(portal_a);

		core.entities.getComponent<ENG::CS::BoxCollider>(portal_a).solid = false;
		core.entities.getComponent<ENG::CS::BoxCollider>(portal_a).size.z = 0.2f;
		core.entities.getComponent<ENG::CS::BoxCollider>(portal_b).solid = false;
		core.entities.getComponent<ENG::CS::BoxCollider>(portal_b).size.x = 0.2f;

		// Create platform
		//ENG::EntityID platform = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Model, ENG::CS::BoxCollider>();
		//ENG::CS::Model& pm = core.entities.getComponent<ENG::CS::Model>(platform);
		//pm.mesh = "cube2.obj";
		//pm.texture = "rock.png";
		//pm.shader = "default.shader";

		//core.entities.getComponent<ENG::CS::Transform>(platform).scale = { 25.0f, 1.0f, 25.0f };
		//core.entities.getComponent<ENG::CS::Transform>(platform).position.y = -2;

		ENG::EntityID prop = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Model>();
		ENG::CS::Model& prop_m = core.entities.getComponent<ENG::CS::Model>(prop);
		prop_m.mesh = "cube2.obj";
		prop_m.shader = "default.shader";
		prop_m.texture = "skull.jpg";

		core.entities.getComponent<ENG::CS::Transform>(prop).position = { -10.0f, 0.0f, -4.0f };

		core.run();
	}
	catch (const std::exception& e)
	{
		OUTPUT_ERROR(e.what());
		PAUSE_CONSOLE;
	}

	return 0;
}