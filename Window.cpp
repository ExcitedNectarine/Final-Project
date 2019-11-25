#include "Window.h"

namespace ENG
{
	Window::Window(const glm::ivec2& size, const std::string& title) { create(size, title); }

	void Window::create(const glm::ivec2& size, const std::string& title)
	{
		glfwInit();

		window = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(window);

		glewInit();

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}

	void Window::clear(const glm::vec4& colour)
	{
		glClearColor(colour.x, colour.y, colour.z, colour.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Window::display() { glfwSwapBuffers(window); }
	bool Window::shouldClose() { return static_cast<bool>(glfwWindowShouldClose(window)); }
	void Window::close() { glfwSetWindowShouldClose(window, true); }
	bool Window::isKeyPressed(int key) { return glfwGetKey(window, key); }

	glm::dvec2 Window::getMousePos()
	{
		glm::dvec2 mouse_pos;
		glfwGetCursorPos(window, &mouse_pos.x, &mouse_pos.y);
		return mouse_pos;
	}

	void Window::lockMouse(const bool lock)
	{
		if (lock) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}