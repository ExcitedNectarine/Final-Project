#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include "Window.h"
#include "Transform.h"
#include "Tools.h"
#include "Entities.h"
#include <glm/gtx/string_cast.hpp>

constexpr auto WIDTH = 1920;
constexpr auto HEIGHT = 1080;

namespace Components
{
	struct Model
	{
		Mesh& mesh;
		Texture& texture;
		Shader& shader;
	};
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

	Texture texture;
	Mesh mesh;
	Transform transform;
	texture.load("Resources/Textures/Rock.png");
	mesh.setVertices(loadOBJ(readTextFile("Resources/car.obj")));

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
		view.position += velocity * speed * float(delta);

		shader.setUniform("transform", transform.get());
		shader.setUniform("view", glm::inverse(view.get()));
		shader.setUniform("view_pos", view.position);

		window.clear(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

		shader.bind();
		texture.bind();
		mesh.bind();
		glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount());
		mesh.unbind();
		texture.unbind();
		shader.unbind();

		window.display();

		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}