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
			Game::Traveller
		>();

		core.entities.getComponent<ENG::CS::Camera>(player).size = core.window.getSize();

		ENG::EntityID gun = core.entities.addEntity<ENG::CS::Transform, ENG::CS::Model>();
		ENG::CS::Transform& t = core.entities.getComponent<ENG::CS::Transform>(gun);
		t.parent = player;
		t.position = { 0.2f, -0.2f, -0.2f };

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

		ENG::CS::Script& scr = core.entities.getComponent<ENG::CS::Script>(player);
		scr.script = std::make_shared<Game::Player>();

		core.renderer.view_id = player;

		return player;
	}

	void Player::start(ENG::Core& core)
	{
		transform = &core.entities.getComponent<ENG::CS::Transform>(id);
		controller = &core.entities.getComponent<ENG::CS::Controller>(id);
		transform->position.y = 5.0f;

		box = &core.entities.getComponent<ENG::CS::BoxCollider>(id);
		box->size = { 0.25f, 0.5f, 0.25f };

		pickup_position = core.entities.addEntity<ENG::CS::Transform>();
		core.entities.getComponent<ENG::CS::Transform>(pickup_position).position.z = -3.0f;
		core.entities.getComponent<ENG::CS::Transform>(pickup_position).parent = id;
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
		direction = glm::vec3(0.0f);
		if (core.window.isKeyPressed(GLFW_KEY_W)) direction -= transform->forward();
		else if (core.window.isKeyPressed(GLFW_KEY_S)) direction += transform->forward();
		if (core.window.isKeyPressed(GLFW_KEY_A)) direction -= transform->right();
		else if (core.window.isKeyPressed(GLFW_KEY_D)) direction += transform->right();

		if (controller->on_floor && core.window.isKeyPressed(GLFW_KEY_SPACE))
		{
			velocity.y = 7.5f;
			controller->on_floor = false;
		}

		if (direction != glm::vec3(0.0f))
			direction = glm::normalize(direction);

		velocity.x = direction.x * speed;
		velocity.z = direction.z * speed;

		if (!controller->on_floor)
			velocity.y -= 9.8f * core.delta;
		else
			velocity.y = 0.0f;

		controller->velocity = velocity * transform->scale;
	}

	void Player::actions(ENG::Core& core)
	{
		if (pickup == 0 && core.window.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
		{
			pickup = ENG::castRay(core.entities, transform->position, -transform->forward(), id, dist);
			if (pickup != 0 && core.entities.hasComponent<Game::Pickup>(pickup))
			{
				core.entities.getComponent<Game::Pickup>(pickup).active = true;
				core.entities.getComponent<Game::Pickup>(pickup).holder = pickup_position;
				core.entities.getComponent<ENG::CS::BoxCollider>(pickup).trigger = true;
			}
			else pickup = 0;
		}

		if (pickup != 0 && core.window.isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
		{
			ENG::EntityID floor = ENG::castRay(core.entities, transform->position, -transform->forward(), id, dist);
			if (floor != 0)
			{
				glm::vec3 new_pos = transform->position + dist * -transform->forward();
				new_pos.y += core.entities.getComponent<ENG::CS::BoxCollider>(pickup).size.y * core.entities.getComponent<ENG::CS::Transform>(pickup).scale.y;

				core.entities.getComponent<ENG::CS::Transform>(pickup).position = new_pos;
				core.entities.getComponent<ENG::CS::BoxCollider>(pickup).trigger = false;
				core.entities.getComponent<Game::Pickup>(pickup).active = false;

				pickup = 0;
			}
		}
	}

	void Player::update(ENG::Core& core)
	{
		if (core.window.isKeyPressed(GLFW_KEY_ESCAPE))
			core.window.close();

		if (core.window.isKeyPressed(GLFW_KEY_Q))
			core.renderer.draw_colliders = !core.renderer.draw_colliders;

		if (core.window.isKeyPressed(GLFW_KEY_E))
			transform->position = glm::vec3(2.0f);

		mouselook(core);
		movement(core);
		actions(core);

		core.renderer.view = transform;
	}
}