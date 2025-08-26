#include <pch.h>
#include <base.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


//-----------------------------------------------------------------
// Window Message Procedure for ImGUI overlay
LRESULT CALLBACK Overlay::Hooks::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (Data::bDisplay && ImGui_ImplWin32_WndProcHandler( hWnd, uMsg, wParam, lParam ))
	{
		return true;
	}
	if (uMsg == WM_QUIT)
	{
		Overlay::Hooks::Shutdown();
		TerminateProcess( GetCurrentProcess(), 0 );
	}

	return CallWindowProc(Overlay::Data::OriginalWndProc, hWnd, uMsg, wParam, lParam);
}