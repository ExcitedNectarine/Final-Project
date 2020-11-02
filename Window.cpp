#include "Window.h"

namespace ENG
{
	namespace { std::map<int, int> button_presses; }
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) { button_presses[key] = action; }
	void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) { button_presses[button] = action; }

	// Window funcs
	Window::Window() {}
	Window::Window(const glm::ivec2& size, const std::string& title) { create(size, title); }

	void Window::create(const glm::ivec2& size, const std::string& title)
	{
		this->size = size;

		glfwInit();

		window = glfwCreateWindow(size.x, size.y, title.c_str(), NULL, NULL);
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);
		glewInit();

		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		position = { mode->width / 2, mode->height / 2 };
		position -= size / 2;

		glfwSetWindowPos(window, position.x, position.y);
		glfwSetKeyCallback(window, keyCallback);
		glfwSetMouseButtonCallback(window, mouseButtonCallback);

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

	void Window::display() { glfwSwapBuffers(window); }
	bool Window::shouldClose() { return static_cast<bool>(glfwWindowShouldClose(window)); }
	void Window::close() { glfwSetWindowShouldClose(window, true); }
	glm::ivec2 Window::getSize() { return size; }
	glm::ivec2 Window::getPosition() { return position; }

	// Input funcs
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


	bool Window::isKeyPressed(int key) { return glfwGetKey(window, key); }
	bool Window::isKeyPressedOnce(int key)
	{
		if (button_presses[key] == GLFW_PRESS)
		{
			button_presses[key] = GLFW_REPEAT;
			return true;
		}
		return false;
	}

	bool Window::isMouseButtonPressed(int button) { return glfwGetMouseButton(window, button); }
	bool Window::isMouseButtonPressedOnce(int button)
	{
		if (button_presses[button] == GLFW_PRESS)
		{
			button_presses[button] = GLFW_REPEAT;
			return true;
		}
		return false;
	}
}