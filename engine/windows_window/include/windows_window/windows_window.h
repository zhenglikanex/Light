#include "engine/platform/window/window.h"
#include "engine/event/event.h"

#include "GLFW/glfw3.h"

namespace light
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowParams& params);

		~WindowsWindow() override;

		virtual void CloseWindow() override;

		virtual void OnUpdate() override;

		virtual void* GetNativeWindow() const override;

		virtual void SetVSync(bool vsync);

		void OnEvent(const Event& event);
	private:
		GLFWwindow* window_;
	};
}