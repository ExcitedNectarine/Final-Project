#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Window.h"
#include "Transform.h"
#include "Tools.h"
#include "Entities.h"
#include "Resources.h"
#include <glm/gtx/string_cast.hpp>

constexpr auto WIDTH = 1920;
constexpr auto HEIGHT = 1080;

using namespace ENG;

namespace Components
{
	struct Transform : ::Transform, ECSComponent<Transform> {};
	struct Model : ECSComponent<Model>
	{
		Mesh* mesh;
		Texture* texture;
		Shader* shader;
	};
}

void draw(Entities& entities, double delta)
{
	ComponentMap<Components::Transform>& ts = entities.getPool<Components::Transform>();
	ComponentMap<Components::Model>& ms = entities.getPool<Components::Model>();

	for (EntityID entity : entities.entitiesWith<Components::Transform, Components::Model>())
	{
		Components::Model& m = ms[entity];

		ts[entity].rotation.y += 45.0f * static_cast<float>(delta);
		m.shader->setUniform("transform", ts[entity].get());

		m.shader->bind();
		m.mesh->bind();
		m.texture->bind();
		glDrawArrays(GL_TRIANGLES, 0, m.mesh->vertexCount());
	}
}

int main()
{
	Entities entities;
	entities.addComponentPools<
		Components::Transform,
		Components::Model
	>();

	Window window(glm::ivec2(WIDTH, HEIGHT), "Final Project");
	window.lockMouse(true);
	glfwSwapInterval(1);

	glEnable(GL_CULL_FACE);

	std::string vertex = readTextFile("Resources/Shaders/simple.vert");
	std::string fragment = readTextFile("Resources/Shaders/simple.frag");
	Shader shader(vertex, fragment);
	shader.setUniform("projection", glm::perspective(90.0f, static_cast<float>(WIDTH) / HEIGHT, 0.1f, 500.0f));
	shader.setUniform("light_colour", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setUniform("lightpos", glm::vec3(0.0f, 0.0f, 15.0f));

	Resources resources;
	resources.loadMeshes({ "Resources/Meshes/car.obj" });
	resources.loadTextures({ "Resources/Textures/Rock.png" });

	for (int i = 0; i < 5; i++)
	{
		EntityID e = entities.addEntity<Components::Transform, Components::Model>();
		Components::Model& m = entities.getComponent<Components::Model>(e);
		m.mesh = &resources.mesh("car.obj");
		m.texture = &resources.texture("Rock.png");
		m.shader = &shader;

		entities.getComponent<Components::Transform>(e).position.x += i * 10;
		entities.getComponent<Components::Transform>(e).scale = { 0.1f, 0.1f, 0.1f };
	}

	Transform view;

	float speed = 5.0f;
	glm::vec3 velocity(0.0f, 0.0f, 0.0f);
	glm::dvec2 mouse_pos(0.0f, 0.0f), last_mouse_pos(0.0f, 0.0f), relative_mouse_pos(0.0f, 0.0f);

	double current = 0.0f, last = 0.0f, delta = 0.0f;
	while (!window.shouldClose())
	{
		current = glfwGetTime();
		delta = current - last;
		last = current;

		if (window.isKeyPressed(GLFW_KEY_ESCAPE)) window.close();

		mouse_pos = window.getMousePos();
		relative_mouse_pos = last_mouse_pos - mouse_pos;
		last_mouse_pos = mouse_pos;

		view.rotation.x += static_cast<float>(relative_mouse_pos.y * 0.25f);
		view.rotation.y += static_cast<float>(relative_mouse_pos.x * 0.25f);

		if (view.rotation.x >= 89) view.rotation.x = 89;
		if (view.rotation.x <= -89) view.rotation.x = -89;

		velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		if (window.isKeyPressed(GLFW_KEY_W)) velocity = -view.forward();
		if (window.isKeyPressed(GLFW_KEY_S)) velocity = view.forward();
		if (window.isKeyPressed(GLFW_KEY_D)) velocity = view.right();
		if (window.isKeyPressed(GLFW_KEY_A)) velocity = -view.right();
		view.position += velocity * speed * static_cast<float>(delta);

		shader.setUniform("view", glm::inverse(view.get()));
		shader.setUniform("view_pos", view.position);

		window.clear(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		draw(entities, delta);
		window.display();

		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}