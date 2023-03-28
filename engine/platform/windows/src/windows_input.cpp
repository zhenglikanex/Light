#include "engine/core/application.h"

#include "engine/input/input.h"

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

namespace light
{
	Input::Status Input::GetKey(Key key)
	{
		 GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeWindow());
		 
		 return static_cast<Input::Status>(glfwGetKey(window, static_cast<int>(key)));
	}

	Input::Status Input::GetMouseButton(MouseButton button)
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow()->GetNativeWindow());

		return static_cast<Input::Status>(glfwGetMouseButton(window, static_cast<int>(button)));
	}

	bool Input::IsKeyPressed(Key key)
	{
		return Input::GetKey(key) == Status::PRESS;
	}

	bool Input::IsMouseButtonPressed(MouseButton button)
	{
		return Input::GetMouseButton(button) == Status::PRESS;
	}
}