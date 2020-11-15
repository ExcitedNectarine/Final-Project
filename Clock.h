#pragma once

#include <GLFW/glfw3.h>

namespace ENG
{
	class Clock
	{
	public:
		float deltaTime();
		void reset() { glfwSetTime(0.0f); }
		void update();

	private:
		float delta;
		double current = 0.0f;
		double last = 0.0f;
	};
}