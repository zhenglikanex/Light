#pragma once

namespace light
{
	class Application
	{
	public:
		static Application& Get();

		Application();

		virtual void OnUpdate();

		void Run();

	private:
		bool running_;
	};

	extern Application* CreateApplication();

	extern int main();
}