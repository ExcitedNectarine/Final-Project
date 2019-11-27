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
	
	struct Controllable : ECSComponent<Controllable>
	{
		Controllable() : velocity(0.0f, 0.0f, 0.0f), mouse_pos(0.0f, 0.0f), last_mouse_pos(0.0f, 0.0f), relative_mouse_pos(0.0f, 0.0f) {}
		glm::vec3 velocity;
		glm::dvec2 mouse_pos, last_mouse_pos, relative_mouse_pos;
		float speed = 15.0f;
	};

	struct Light : ECSComponent<Light>
	{
		glm::vec3 colour;
		float radius;
	};
}

void draw(Entities& entities, double delta)
{
	ComponentMap<Components::Transform>& ts = entities.getPool<Components::Transform>();
	ComponentMap<Components::Model>& ms = entities.getPool<Components::Model>();

	for (EntityID entity : entities.entitiesWith<Components::Transform, Components::Model>())
	{
		Components::Model& m = ms[entity];

		//ts[entity].rotation.y += 45.0f * static_cast<float>(delta);
		m.shader->setUniform("transform", ts[entity].get());

		m.shader->bind();
		m.mesh->bind();
		m.texture->bind();
		glDrawArrays(GL_TRIANGLES, 0, m.mesh->vertexCount());
	}
}

void move(Entities& entities, Window& window, Shader& shader, double delta)
{
	ComponentMap<Components::Transform>& ts = entities.getPool<Components::Transform>();
	ComponentMap<Components::Controllable>& cs = entities.getPool<Components::Controllable>();

	for (EntityID entity : entities.entitiesWith<Components::Transform, Components::Controllable>())
	{
		Components::Transform& t = ts[entity];
		Components::Controllable& c = cs[entity];

		c.mouse_pos = window.getMousePos();
		c.relative_mouse_pos = c.last_mouse_pos - c.mouse_pos;
		c.last_mouse_pos = c.mouse_pos;
		t.rotation.x += static_cast<float>(c.relative_mouse_pos.y * 0.25f);
		t.rotation.y += static_cast<float>(c.relative_mouse_pos.x * 0.25f);
		if (t.rotation.x >= 89) t.rotation.x = 89;
		if (t.rotation.x <= -89) t.rotation.x = -89;

		c.velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		if (window.isKeyPressed(GLFW_KEY_W)) c.velocity = -t.forward();
		if (window.isKeyPressed(GLFW_KEY_S)) c.velocity = t.forward();
		if (window.isKeyPressed(GLFW_KEY_D)) c.velocity = t.right();
		if (window.isKeyPressed(GLFW_KEY_A)) c.velocity = -t.right();
		t.position += c.velocity * c.speed * static_cast<float>(delta);

		shader.setUniform("view", glm::inverse(t.get()));
		shader.setUniform("view_pos", t.position);
	}
}

void setLights(Entities& entities, Shader& shader)
{
	ComponentMap<Components::Transform>& ts = entities.getPool<Components::Transform>();
	ComponentMap<Components::Light>& ls = entities.getPool<Components::Light>();

	std::vector<EntityID> ents = entities.entitiesWith<Components::Transform, Components::Light>();
	for (std::size_t i = 0; i < ents.size(); i++)
	{
		shader.setUniform("lights[" + std::to_string(i) + "].position", ts[ents[i]].position);
		shader.setUniform("lights[" + std::to_string(i) + "].colour", ls[ents[i]].colour);
		shader.setUniform("lights[" + std::to_string(i) + "].radius", ls[ents[i]].radius);
	}
}

int main()
{
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

	Entities entities;
	entities.addComponentPools<
		Components::Transform,
		Components::Model,
		Components::Controllable,
		Components::Light
	>();
	EntityID player = entities.addEntity<
		Components::Transform,
		Components::Controllable,
		Components::Model,
		Components::Light
	>();
	Components::Model& m = entities.getComponent<Components::Model>(player);
	m.mesh = &resources.mesh("car.obj");
	m.texture = &resources.texture("Rock.png");
	m.shader = &shader;

	Components::Light& l = entities.getComponent<Components::Light>(player);
	l.colour = glm::vec3(1.0f, 0.0f, 0.0f);
	l.radius = 10.0f;

	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			EntityID e = entities.addEntity<Components::Transform, Components::Model>();
			Components::Model& m = entities.getComponent<Components::Model>(e);
			m.mesh = &resources.mesh("car.obj");
			m.texture = &resources.texture("Rock.png");
			m.shader = &shader;

			Components::Transform& t = entities.getComponent<Components::Transform>(e);
			t.position.x += i * 10;
			t.position.z += j * 10;
			t.scale = { 0.1f, 0.1f, 0.1f };
		}
	}

	double current = 0.0f, last = 0.0f, delta = 0.0f;
	while (!window.shouldClose())
	{
		current = glfwGetTime();
		delta = current - last;
		last = current;

		if (window.isKeyPressed(GLFW_KEY_ESCAPE))
			window.close();

		move(entities, window, shader, delta);
		setLights(entities, shader);

		window.clear(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		draw(entities, delta);
		window.display();

		glfwPollEvents();
	}

	glfwTerminate();
	std::cin.get();
	return 0;
}