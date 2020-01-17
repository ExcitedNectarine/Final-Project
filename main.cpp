#include "Application.h"
#include <random>
#include <chrono>

struct PlayerScript : ENG::Script
{
	CS::Transform* t;
	CS::AABB* aabb;
	glm::vec3 velocity;
	glm::vec3 direction;
	float speed = 5.0f;
	ENG::SoundSource s;
	int score = 0;

	void start(ENG::Application& app)
	{
		t = &app.getEntities().getComponent<CS::Transform>(id);
		aabb = &app.getEntities().getComponent<CS::AABB>(id);

		app.getEntities().getComponent<CS::Light>(id).colour *= 30.0f;

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

		if (app.getWindow().isKeyPressed(GLFW_KEY_LEFT)) t->rotation.y += 90.0f * app.getDeltaTime();
		if (app.getWindow().isKeyPressed(GLFW_KEY_RIGHT)) t->rotation.y -= 90.0f * app.getDeltaTime();

		app.setView(t->get());
		app.getShader().setUniform("view_pos", t->position);
	}
};

float randomFloat(const float from, const float to)
{
	static std::default_random_engine engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
	std::uniform_real_distribution<float> dist(from, to);
	return dist(engine);
}

void createPickup(ENG::Application& app)
{
	// Create the pickup entity.
	// The entity has a transform, a model and an AABB bounding box.
	ENG::EntityID pickup = app.getEntities().addEntity<CS::Transform, CS::Model, CS::AABB>();

	// Set the position of the pickup.
	app.getEntities().getComponent<CS::Transform>(pickup).position = { randomFloat(-20.0f, 20.0f), 0.0f, randomFloat(-20.0f, 20.0f) };
	
	// Set the mesh, texture and shader the pickup will use for drawing.
	CS::Model& m = app.getEntities().getComponent<CS::Model>(pickup);
	m.mesh = &app.getResources().mesh("cube2.obj");
	m.texture = &app.getResources().texture("rock.png");
	m.shader = &app.getShader();
}

int main()
{
	ENG::Application app("Resources/settings.set");
	app.getShader().setUniform("ambient", { 0.1f, 0.1f, 0.1f });

	for (int i = 0; i < 10; i++)
		createPickup(app);

	// CREATE PLAYER ENTITY
	ENG::EntityID player = app.getEntities().addEntity<CS::Transform, CS::Script, CS::AABB, CS::Light>();
	app.getEntities().getComponent<CS::Script>(player).script = std::make_unique<PlayerScript>();

	app.run();
	return 0;
}