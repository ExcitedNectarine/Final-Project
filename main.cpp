#include "Core.h"

struct PlayerScript : ENG::Script
{
	ENG::CS::Controller* c;
	ENG::CS::Transform* t;
	glm::vec3 velocity;
	float speed = 5.0f;

	void start(ENG::Core& core)
	{
		c = &core.entities.getComponent<ENG::CS::Controller>(id);

		t = &core.entities.getComponent<ENG::CS::Transform>(id);
		t->position.z = -5.0f;
		t->rotation = { ENG::randomFloat(0.0f, 180.0f), ENG::randomFloat(0.0f, 180.0f), ENG::randomFloat(0.0f, 180.0f) };

		ENG::CS::Model& m = core.entities.getComponent<ENG::CS::Model>(id);
		m.mesh = &core.resources.mesh("cube2.obj");
		m.texture = &core.resources.texture("rock.png");
		m.shader = &core.resources.shader("default.shader");

		core.entities.getComponent<ENG::CS::BoxCollider>(id).size *= 2.0f;
	}

	void update(ENG::Core& core)
	{
		if (core.window.isKeyPressed(GLFW_KEY_SPACE))
			t->rotation = { ENG::randomFloat(0.0f, 180.0f), ENG::randomFloat(0.0f, 180.0f), ENG::randomFloat(0.0f, 180.0f) };

		velocity = { 0.0f, 0.0f, 0.0f };
		if (core.window.isKeyPressed(GLFW_KEY_UP)) velocity.y = speed;
		if (core.window.isKeyPressed(GLFW_KEY_DOWN)) velocity.y = -speed;
		if (core.window.isKeyPressed(GLFW_KEY_LEFT)) velocity.x = -speed;
		if (core.window.isKeyPressed(GLFW_KEY_RIGHT)) velocity.x = speed;

		c->velocity = velocity * core.delta;
	}

	void onCollision(ENG::Core& core, ENG::EntityID hit_id)
	{
		OUTPUT("Collision with " << hit_id << " " << core.delta);
	}
};

void createPickup(ENG::Core& core)
{
	// Create the pickup entity.
	// The entity has a transform, a model and an AABB bounding box.
	ENG::EntityID pickup = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Model, ENG::CS::BoxCollider, ENG::CS::Light>();

	// Set the position of the pickup.
	core.entities.getComponent<ENG::CS::Transform>(pickup).position = { ENG::randomFloat(-5.0f, 5.0f), ENG::randomFloat(-5.0f, 5.0f), -5.0f };
	
	// Set the mesh, texture and shader the pickup will use for drawing.
	ENG::CS::Model& m = core.entities.getComponent<ENG::CS::Model>(pickup);
	m.mesh = &core.resources.mesh("cube2.obj");
	m.texture = &core.resources.texture("skull.jpg");
	m.shader = &core.resources.shader("default.shader");

	core.entities.getComponent<ENG::CS::BoxCollider>(pickup).size *= 2.0f;
}

int main()
{
	try
	{
		ENG::Core core("Resources/settings.set");
		core.resources.shader("default.shader").setUniform("ambient", { 0.1f, 0.1f, 0.1f });

		createPickup(core);

		// CREATE PLAYER ENTITY
		ENG::EntityID player = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Model, ENG::CS::Script, ENG::CS::BoxCollider, ENG::CS::Controller, ENG::CS::Light>();
		core.entities.getComponent<ENG::CS::Script>(player).script = std::make_unique<PlayerScript>(); // attach player script.

		core.view = glm::mat4(1.0f);
		core.run();
	}
	catch (const std::exception& e)
	{
		OUTPUT_ERROR(e.what());
		PAUSE_CONSOLE;
	}

	return 0;
}