#include "Application.h"

struct PlayerScript : ENG::Script
{
	ENG::CS::Controller* c;
	glm::vec3 velocity;
	float speed = 5.0f;

	void start(ENG::Application& app)
	{
		c = &app.getEntities().getComponent<ENG::CS::Controller>(id);

		ENG::CS::Transform& t = app.getEntities().getComponent<ENG::CS::Transform>(id);
		t.position.z = -5.0f;
		t.rotation = { ENG::randomFloat(0.0f, 180.0f), ENG::randomFloat(0.0f, 180.0f), ENG::randomFloat(0.0f, 180.0f) };

		ENG::CS::Model& m = app.getEntities().getComponent<ENG::CS::Model>(id);
		m.mesh = &app.getResources().mesh("cube2.obj");
		m.texture = &app.getResources().texture("rock.png");
		m.shader = &app.getShader();

		app.getEntities().getComponent<ENG::CS::BoxCollider>(id).size *= 2.0f;
	}

	void update(ENG::Application& app)
	{
		velocity = { 0.0f, 0.0f, 0.0f };
		if (app.getWindow().isKeyPressed(GLFW_KEY_UP)) velocity.y = speed;
		if (app.getWindow().isKeyPressed(GLFW_KEY_DOWN)) velocity.y = -speed;
		if (app.getWindow().isKeyPressed(GLFW_KEY_LEFT)) velocity.x = -speed;
		if (app.getWindow().isKeyPressed(GLFW_KEY_RIGHT)) velocity.x = speed;

		c->velocity = velocity * app.getDeltaTime();
	}

	void onCollision(ENG::Application& app, ENG::EntityID hit_id)
	{
		OUTPUT("Collision with " << hit_id << " " << app.getDeltaTime());
	}
};

void createPickup(ENG::Application& app)
{
	// Create the pickup entity.
	// The entity has a transform, a model and an AABB bounding box.
	ENG::EntityID pickup = app.getEntities().addEntity<ENG::CS::Transform, ENG::CS::Model, ENG::CS::BoxCollider>();

	// Set the position of the pickup.
	app.getEntities().getComponent<ENG::CS::Transform>(pickup).position = { ENG::randomFloat(-5.0f, 5.0f), ENG::randomFloat(-5.0f, 5.0f), -5.0f };
	
	// Set the mesh, texture and shader the pickup will use for drawing.
	ENG::CS::Model& m = app.getEntities().getComponent<ENG::CS::Model>(pickup);
	m.mesh = &app.getResources().mesh("cube2.obj");
	m.texture = &app.getResources().texture("skull.jpg");
	m.shader = &app.getShader();

	app.getEntities().getComponent<ENG::CS::BoxCollider>(pickup).size *= 2.0f;
	app.getEntities().getComponent<ENG::CS::Transform>(pickup).rotation = { ENG::randomFloat(0.0f, 180.0f), ENG::randomFloat(0.0f, 180.0f), ENG::randomFloat(0.0f, 180.0f) };
}

int main()
{
	ENG::Application app("Resources/settings.set");
	app.getShader().setUniform("ambient", { 0.1f, 0.1f, 0.1f });

	createPickup(app);

	// CREATE PLAYER ENTITY
	ENG::EntityID player = app.getEntities().addEntity<ENG::CS::Transform, ENG::CS::Model, ENG::CS::Script, ENG::CS::BoxCollider, ENG::CS::Controller>();
	app.getEntities().getComponent<ENG::CS::Script>(player).script = std::make_unique<PlayerScript>(); // attach player script.

	app.setView(glm::mat4(1.0f));
	app.run();

	return 0;
}