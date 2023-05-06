#include "engine/utils/platform_utils.h"
#include "engine/core/application.h"

#include <Windows.h>
#include <commdlg.h>

#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#pragma comment(lib,"Comdlg32.lib")

namespace light
{
	std::string FileDialogs::OpenFile(std::string_view filter)
	{
		OPENFILENAMEA ofn;
		CHAR sz_file[256] = { 0 };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = (HWND)(Application::Get().GetWindow()->GetHwnd());
		ofn.lpstrFile = sz_file;
		ofn.nMaxFile = sizeof(sz_file);
		ofn.lpstrFilter = filter.data();
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetOpenFileNameA(&ofn) == TRUE)
		{
			return std::string(ofn.lpstrFile);
		}

		return std::string();
	}

	std::string FileDialogs::SaveFile(std::string_view filter)
	{
		OPENFILENAMEA ofn;
		CHAR sz_file[256] = { "test.scene" };
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = (HWND)(Application::Get().GetWindow()->GetHwnd());
		ofn.lpstrFile = sz_file;
		ofn.nMaxFile = sizeof(sz_file);
		ofn.lpstrFilter = filter.data();
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
		if (GetSaveFileNameA(&ofn) == TRUE)
		{
			return std::string(ofn.lpstrFile);
		}
		return std::string();
	}
}