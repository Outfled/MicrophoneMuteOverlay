#include "pch.h"
#include "base.h"
#include "overlayreg.h"
#include <Shlwapi.h>

#pragma comment( lib, "Shlwapi.lib" )

static BOOL g_bInitialized = FALSE;

DWORD WINAPI WaitForOverlayEnable( LPVOID lpParameter )
{
	while (!IsOverlayEnabled())
	{
		Sleep( 7500 );
	}

	Overlay::Data::bImGUIInitialized	= false;
	Overlay::Data::bDetached			= false;

	if (!g_bInitialized)
	{
		g_bInitialized = TRUE;

		MH_STATUS Status = MH_Initialize();
		if (Status == MH_OK) {
			Status = (MH_STATUS)Overlay::Hooks::Initialize();
		}
	}
	else {
		MH_EnableHook( MH_ALL_HOOKS );
	}

	return TRUE;
}

DWORD WINAPI MainThread( LPVOID lpThreadParameter )
{
	Overlay::Data::hModule = (HMODULE)lpThreadParameter;

	MH_STATUS Status = MH_Initialize();
	if (Status == MH_OK) {
		Status = (MH_STATUS)Overlay::Hooks::Initialize();
	}

	g_bInitialized = TRUE;
	return TRUE;
}

DWORD WINAPI DetachedThread(LPVOID lpThreadParameter)
{
	if (!Overlay::Data::bDetached)
	{
		Overlay::Data::bDetached = true;

		HANDLE hThread = CreateThread( nullptr, 0, WaitForOverlayEnable, lpThreadParameter, 0, nullptr );
		if (hThread) {
			CloseHandle( hThread );
		}
	}

	return TRUE;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	HANDLE hThread = NULL;

	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		if (IsOverlayEnabled()) {
			hThread = CreateThread( nullptr, 0, MainThread, hModule, 0, nullptr );
		}
		else {
			hThread = CreateThread( nullptr, 0, WaitForOverlayEnable, hModule, 0, nullptr );
		}
		break;
	case DLL_PROCESS_DETACH:
		if (!Overlay::Data::bDetached) {
			hThread = CreateThread( nullptr, 0, DetachedThread, hModule, 0, nullptr );
		}

		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	default:
		break;
	}

	if (hThread) {
		CloseHandle( hThread );
	}

	return TRUE;
}
