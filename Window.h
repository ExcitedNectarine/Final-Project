#pragma once

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace ENG
{
	class Window
	{
	public:
		Window(const glm::ivec2& size, const std::string& title);
		void create(const glm::ivec2& size, const std::string& title);
		bool shouldClose();
		void clear(const glm::vec4& colour);
		void display();
		void close();
		bool isKeyPressed(int key);
		glm::dvec2 getMousePos();
		void lockMouse(const bool lock);

	private:
		GLFWwindow* window;
	};
}