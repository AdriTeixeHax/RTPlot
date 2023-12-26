#ifndef _INPUT__H_
#define _INPUT__H_

// IO
#include <iostream>

// OpenGL Libraries
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>

inline void OnKeyboard(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	// Actions triggered when pressed
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_W:
			std::cout << "w" << std::endl;
			break;

		case GLFW_KEY_A:
			break;

		case GLFW_KEY_S:
			break;

		case GLFW_KEY_D:
			break;
		}
	}

	// Actions triggered when releasing
	if (action == GLFW_RELEASE)
	{
		switch (key)
		{
		case GLFW_KEY_W:
			break;

		case GLFW_KEY_A:
			break;

		case GLFW_KEY_S:
			break;

		case GLFW_KEY_D:
			break;
		}
	}

	// Actions triggered when the switch is continuously pressed
	if (action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_W:
			break;

		case GLFW_KEY_A:
			break;

		case GLFW_KEY_S:
			break;

		case GLFW_KEY_D:
			break;
		}
	}
}

#endif