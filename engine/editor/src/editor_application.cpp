#include "engine/light.h"

#include "editor_layer.h"

namespace light
{
	class EditorApplication : public Application
	{
	public:
		EditorApplication()
			: Application("Light Editor")
		{
		}

		void Init() override
		{
			Application::Init();

			PushLayer(new editor::EditorLayer());
		}
	private:

	};

	Application* light::CreateApplication()
	{
		return new EditorApplication();
	}
}