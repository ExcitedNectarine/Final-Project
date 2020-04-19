#include "Window.h"

namespace ENG
{
	Window::Window() {}
	Window::Window(const glm::ivec2& size, const std::string& title) { create(size, title); }

	void Window::create(const glm::ivec2& size, const std::string& title)
	{
		this->size = size;

		glfwInit();

		window = glfwCreateWindow(size.x, size.y, title.c_str(), NULL, NULL);
		glfwMakeContextCurrent(window);
		glewInit();

		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		position = { mode->width / 2, mode->height / 2 };
		position -= size / 2;

		glfwSetWindowPos(window, position.x, position.y);

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glDepthFunc(GL_LEQUAL);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	void Window::clear(const glm::vec4& colour)
	{
		glClearColor(colour.x, colour.y, colour.z, colour.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	glm::dvec2 Window::getMousePos()
	{
		glfwGetCursorPos(window, &mouse_pos.x, &mouse_pos.y);
		return mouse_pos;
	}

	void Window::lockMouse(const bool lock)
	{
		if (lock) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	void Window::display() { glfwSwapBuffers(window); }
	bool Window::shouldClose() { return static_cast<bool>(glfwWindowShouldClose(window)); }
	void Window::close() { glfwSetWindowShouldClose(window, true); }
	bool Window::isKeyPressed(int key) { return glfwGetKey(window, key); }
	bool Window::isMouseButtonPressed(int button) { return glfwGetMouseButton(window, button); }
	glm::ivec2 Window::getSize() { return size; }
	glm::ivec2 Window::getPosition() { return position; }
}