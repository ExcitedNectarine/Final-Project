#include "Player.h"
#include "Core.h"

namespace Game
{
	ENG::EntityID createPlayer(ENG::Core& core)
	{
		ENG::EntityID player = core.entities.addEntity<
			ENG::CS::Script,
			ENG::CS::Transform,
			ENG::CS::BoxCollider,
			ENG::CS::Controller,
			ENG::CS::Camera,
			ENG::CS::Light,
			Game::Traveller
		>();

		core.entities.getComponent<ENG::CS::Camera>(player).size = core.window.getSize();

		ENG::EntityID gun = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Model>();
		ENG::CS::Transform& t = core.entities.getComponent<ENG::CS::Transform>(gun);
		t.parent = player;
		t.position = { 0.0f, -0.3f, -0.2f };

		ENG::CS::Model& model = core.entities.getComponent<ENG::CS::Model>(gun);
		model.mesh = "gun.obj";
		model.texture = "gun.png";
		model.hud = true;

		ENG::EntityID crosshair = core.entities.addEntity<ENG::CS::Transform2D, ENG::CS::Sprite>();
		ENG::CS::Transform2D& t2d = core.entities.getComponent<ENG::CS::Transform2D>(crosshair);
		t2d.origin = glm::vec2(core.resources.texture("crosshair.png").getSize()) / 2.0f;
		t2d.position = glm::vec2(core.window.getSize()) / 2.0f;

		ENG::CS::Sprite& s = core.entities.getComponent<ENG::CS::Sprite>(crosshair);
		s.texture = "crosshair.png";

		//ENG::addScript<Game::Player>(core, player);
		ENG::CS::Script& scr = core.entities.getComponent<ENG::CS::Script>(player);
		scr.script = std::make_shared<Game::Player>();

		core.renderer.view_id = player;

		return player;
	}

	void Player::start(ENG::Core& core)
	{
		transform = &core.entities.getComponent<ENG::CS::Transform>(id);
		transform->position.y = 5.0f;
		
		controller = &core.entities.getComponent<ENG::CS::Controller>(id);

		box = &core.entities.getComponent<ENG::CS::BoxCollider>(id);
		box->size = { 0.25f, 0.8f, 0.25f };

		pickup_position = core.entities.addEntity<ENG::CS::Transform>();
		core.entities.getComponent<ENG::CS::Transform>(pickup_position).position.z = -3.0f;
		core.entities.getComponent<ENG::CS::Transform>(pickup_position).parent = id;

		pos_text = core.entities.addEntity<CS::Transform2D, CS::Text>();
		core.entities.getComponent<CS::Transform2D>(pos_text).position = glm::vec2(50.0f);
		core.entities.getComponent<CS::Text>(pos_text).setText(glm::to_string(glm::ivec3(transform->position)));
	}

	void Player::mouselook(ENG::Core& core)
	{
		mouse_offset = last_mouse - core.window.getMousePos();
		last_mouse = core.window.getMousePos();
		transform->rotation += glm::vec3(mouse_offset.y, mouse_offset.x, 0.0f) * sensitivity;

		if (transform->rotation.x > 89.0f) transform->rotation.x = 89.0f;
		else if (transform->rotation.x < -89.0f) transform->rotation.x = -89.0f;
	}

	void Player::movement(ENG::Core& core)
	{
		// Movement
		direction = glm::vec3(0.0f);
		if (core.window.isKeyPressed(GLFW_KEY_W)) direction -= transform->forward();
		else if (core.window.isKeyPressed(GLFW_KEY_S)) direction += transform->forward();
		if (core.window.isKeyPressed(GLFW_KEY_A)) direction -= transform->right();
		else if (core.window.isKeyPressed(GLFW_KEY_D)) direction += transform->right();

		if (direction != glm::vec3(0.0f))
			direction = glm::normalize(direction);

		velocity.x = direction.x * speed;
		velocity.z = direction.z * speed;

		// Apply gravity and jumping
		if (!controller->on_floor)
			velocity.y -= 9.8f * core.delta;
		else if (core.window.isKeyPressed(GLFW_KEY_SPACE))
			velocity.y = 10.0f;
		else
			velocity.y = 0.0f;

		controller->velocity = velocity * transform->scale;
	}

	void Player::actions(ENG::Core& core)
	{
		if (ray.id == 0 && core.window.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			ray = ENG::castRay(core.entities, transform->position, -transform->forward(), { id, ray.id });
			if (ray.id != 0 && ray.distance < 5.0f && core.entities.hasComponent<Game::Pickup>(ray.id))
			{
				core.entities.getComponent<Game::Pickup>(ray.id).active = true;
				core.entities.getComponent<Game::Pickup>(ray.id).holder = pickup_position;
			}
			else ray.id = 0;
		}

		if (ray.id != 0 && core.window.isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
		{
			core.entities.getComponent<Game::Pickup>(ray.id).active = false;
			ray.id = 0;
		}
	}

	void Player::update(ENG::Core& core)
	{
		if (core.window.isKeyPressed(GLFW_KEY_ESCAPE))
			core.window.close();

		if (core.window.isKeyPressedOnce(GLFW_KEY_Q))
			core.renderer.draw_colliders = !core.renderer.draw_colliders;

		if (core.window.isKeyPressed(GLFW_KEY_E))
			transform->position = glm::vec3(0.0f, 10.0f, 0.0f);

		mouselook(core);
		movement(core);
		actions(core);

		core.entities.getComponent<CS::Text>(pos_text).setText(glm::to_string(glm::ivec3(transform->position)));
		core.renderer.view = transform;
	}
}