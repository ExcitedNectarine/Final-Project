#pragma once

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace ENG
{
	/**
	* Simple window class, provides basic window control and input.
	*/
	class Window
	{
	public:
		Window();
		Window(const glm::ivec2& size, const std::string& title);
		void create(const glm::ivec2& size, const std::string& title);
		void clear(const glm::vec4& colour);
		glm::dvec2 getMousePos();
		void lockMouse(const bool lock);
		void display();
		bool shouldClose();
		void close();
		bool isKeyPressed(int key);

	private:
		GLFWwindow* window;
		glm::dvec2 mouse_pos;
	};
}