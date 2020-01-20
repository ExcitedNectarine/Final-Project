#include "Application.h"

struct PlayerScript : ENG::Script
{
	ENG::CS::Transform* t;
	ENG::CS::AABB* aabb;
	glm::vec3 velocity;
	glm::vec3 direction;
	float speed = 5.0f;
	ENG::SoundSource s;
	int score = 0;

	void start(ENG::Application& app)
	{
		t = &app.getEntities().getComponent<ENG::CS::Transform>(id); // get components
		aabb = &app.getEntities().getComponent<ENG::CS::AABB>(id);

		app.getEntities().getComponent<ENG::CS::Light>(id).colour *= 30.0f;  // intensify light

		s.setBuffer(&app.getResources().sound("gunfire.ogg"));
	}

	void update(ENG::Application& app)
	{
		if (aabb->hit)
		{
			s.play();
			app.getEntities().removeEntity(aabb->hit_id);

			if (++score >= 10)
				app.getWindow().close();
		}
		 // movement
		direction = { 0.0f, 0.0f, 0.0f };
		if (app.getWindow().isKeyPressed(GLFW_KEY_W)) direction -= t->forward();
		if (app.getWindow().isKeyPressed(GLFW_KEY_S)) direction += t->forward();
		if (app.getWindow().isKeyPressed(GLFW_KEY_A)) direction -= t->right();
		if (app.getWindow().isKeyPressed(GLFW_KEY_D)) direction += t->right();

		if (direction != glm::vec3(0.0f, 0.0f, 0.0f))
			direction = glm::normalize(direction);

		velocity.x = direction.x * speed;
		velocity.z = direction.z * speed;

		t->position += velocity * app.getDeltaTime();

		// rotation
		if (app.getWindow().isKeyPressed(GLFW_KEY_LEFT)) t->rotation.y += 90.0f * app.getDeltaTime();
		if (app.getWindow().isKeyPressed(GLFW_KEY_RIGHT)) t->rotation.y -= 90.0f * app.getDeltaTime();

		// update app's view
		app.setView(t->get());
		app.getShader().setUniform("view_pos", t->position);
	}
};

void createPickup(ENG::Application& app)
{
	// Create the pickup entity.
	// The entity has a transform, a model and an AABB bounding box.
	ENG::EntityID pickup = app.getEntities().addEntity<ENG::CS::Transform, ENG::CS::Model, ENG::CS::AABB>();

	// Set the position of the pickup.
	app.getEntities().getComponent<ENG::CS::Transform>(pickup).position = { ENG::randomFloat(-20.0f, 20.0f), 0.0f, ENG::randomFloat(-20.0f, 20.0f) };
	
	// Set the mesh, texture and shader the pickup will use for drawing.
	ENG::CS::Model& m = app.getEntities().getComponent<ENG::CS::Model>(pickup);
	m.mesh = &app.getResources().mesh("skull.obj");
	m.texture = &app.getResources().texture("skull.jpg");
	m.shader = &app.getShader();

	app.getEntities().getComponent<ENG::CS::Transform>(pickup).scale = { 0.1f, 0.1f, 0.1f }; // lower skull scale
}

int main()
{
	ENG::Application app("Resources/settings.set");
	app.getShader().setUniform("ambient", { 0.1f, 0.1f, 0.1f });

	for (int i = 0; i < 100; i++)
		createPickup(app);

	// CREATE PLAYER ENTITY
	ENG::EntityID player = app.getEntities().addEntity<ENG::CS::Transform, ENG::CS::Script, ENG::CS::AABB, ENG::CS::Light>();
	app.getEntities().getComponent<ENG::CS::Script>(player).script = std::make_unique<PlayerScript>(); // attach player script.

	app.run();

	//ENG::Entities e;
	//e.addComponentPools<ENG::CS::Transform, ENG::CS::AABB>();

	//for (int i = 0; i < 400; i++)
	//	e.addEntity<ENG::CS::Transform, ENG::CS::AABB>();

	//auto start = std::chrono::high_resolution_clock::now();

	//ENG::AABBCollision(e);

	//auto end = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

	//OUTPUT("Time elapsed: " << duration << "ms");
	//PAUSE_CONSOLE;

	return 0;
}