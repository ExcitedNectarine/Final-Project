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

namespace Components
{
	struct Transform : ENG::Transform, ENG::ECSComponent<Transform> {};
	struct Model : ENG::ECSComponent<Model>
	{
		ENG::Mesh* mesh;
		ENG::Texture* texture;
		ENG::Shader* shader;
	};
	
	struct Controllable : ENG::ECSComponent<Controllable>
	{
		Controllable() : velocity(0.0f, 0.0f, 0.0f), mouse_pos(0.0f, 0.0f), last_mouse_pos(0.0f, 0.0f), relative_mouse_pos(0.0f, 0.0f) {}
		glm::vec3 velocity;
		glm::dvec2 mouse_pos, last_mouse_pos, relative_mouse_pos;
		float speed = 5.0f;
	};

	struct Light : ENG::ECSComponent<Light>
	{
		Light() : colour(5.0f, 5.0f, 5.0f), radius(50.0f) {}

		glm::vec3 colour;
		float radius;
	};
}

void draw(ENG::Entities& entities, double delta)
{
	ENG::ComponentMap<Components::Transform>& ts = entities.getPool<Components::Transform>();
	ENG::ComponentMap<Components::Model>& ms = entities.getPool<Components::Model>();

	for (ENG::EntityID entity : entities.entitiesWith<Components::Transform, Components::Model>())
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

void move(ENG::Entities& entities, ENG::Window& window, ENG::Shader& shader, double delta)
{
	ENG::ComponentMap<Components::Transform>& ts = entities.getPool<Components::Transform>();
	ENG::ComponentMap<Components::Controllable>& cs = entities.getPool<Components::Controllable>();

	for (ENG::EntityID entity : entities.entitiesWith<Components::Transform, Components::Controllable>())
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

		//OUTPUT(glm::to_string(t.position));
	}
}

void setLights(ENG::Entities& entities, ENG::Shader& shader)
{
	ENG::ComponentMap<Components::Transform>& ts = entities.getPool<Components::Transform>();
	ENG::ComponentMap<Components::Light>& ls = entities.getPool<Components::Light>();

	std::vector<ENG::EntityID> ents = entities.entitiesWith<Components::Transform, Components::Light>();
	for (std::size_t i = 0; i < ents.size(); i++)
	{
		shader.setUniform("lights[" + std::to_string(i) + "].position", ts[ents[i]].position);
		shader.setUniform("lights[" + std::to_string(i) + "].colour", ls[ents[i]].colour);
		shader.setUniform("lights[" + std::to_string(i) + "].radius", ls[ents[i]].radius);
	}
}

class Application
{
public:
	Application()
	{
		window.create(glm::ivec2(WIDTH, HEIGHT), "Application");
	}

private:
	ENG::Window window;
	ENG::Resources resources;
	ENG::Entities entities;
	ENG::Shader default_shader;
};

int main()
{
	ENG::Window window(glm::ivec2(WIDTH, HEIGHT), "Final Project");
	window.lockMouse(true);
	glfwSwapInterval(1);

	glEnable(GL_CULL_FACE);

	std::string vertex = ENG::readTextFile("Resources/Shaders/simple.vert");
	std::string fragment = ENG::readTextFile("Resources/Shaders/simple.frag");
	ENG::Shader shader(vertex, fragment);
	shader.setUniform("projection", glm::perspective(90.0f, static_cast<float>(WIDTH) / HEIGHT, 0.1f, 500.0f));
	shader.setUniform("ambient", glm::vec3(0.2f, 0.2f, 0.2f));

	ENG::Resources resources;
	//resources.loadMeshes({ "Resources/Meshes/skull.obj", "Resources/Meshes/car.obj" });
	resources.loadMeshes({ "Resources/Meshes/car.obj" });
	resources.loadTextures({ "Resources/Textures/skull.jpg", "Resources/Textures/rock.png"  });

	ENG::Entities entities;
	entities.addComponentPools<Components::Transform, Components::Model, Components::Controllable, Components::Light>();

	entities.addEntity<Components::Transform, Components::Controllable, Components::Light>();
	ENG::EntityID light = entities.addEntity<Components::Transform, Components::Light>();
	entities.getComponent<Components::Light>(light).colour = { 50.0f, 0.0f, 50.0f };
	Components::Transform& t1 = entities.getComponent<Components::Transform>(light);
	t1.position = { 0.0f, 5.0f, 10.0f };

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			ENG::EntityID e = entities.addEntity<Components::Transform, Components::Model>();
			Components::Model& m = entities.getComponent<Components::Model>(e);
			m.mesh = &resources.mesh("car.obj");
			m.texture = &resources.texture("skull.jpg");
			m.shader = &shader;

			Components::Transform& t = entities.getComponent<Components::Transform>(e);
			t.position.x += i * 5;
			t.position.z += j * 5;
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

		t1.position.x += 5.0f * delta;
		if (t1.position.x >= 20.0f)
			t1.position.x = 0.0f;

		move(entities, window, shader, delta);
		setLights(entities, shader);

		window.clear(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		draw(entities, delta);
		window.display();

		glfwPollEvents();
	}

	glfwTerminate();
	//std::cin.get();
	return 0;
}