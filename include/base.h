#pragma once
#ifndef BASE_H
#define BASE_H

#include "pch.h"

#if defined(MEM_86)
#define WNDPROC_INDEX GWL_WNDPROC
#define PRESENT_CALL __stdcall
#elif defined(MEM_64)
#define WNDPROC_INDEX GWLP_WNDPROC
#define PRESENT_CALL __fastcall
#endif

#define D3D11_DEVICE_LEN		40
#define D3D11_SWAPCHAIN_LEN		18
#define D3D11_DVC_CONTEXT_LEN	108


DWORD WINAPI MainThread(LPVOID lpThreadParameter);
DWORD WINAPI DetachedThread(LPVOID lpThreadParameter);


//
// Hooked Function Types
typedef HRESULT(PRESENT_CALL* Present_t)(IDXGISwapChain*, UINT, UINT);
typedef LRESULT(CALLBACK*  WndProc_t) (HWND, UINT, WPARAM, LPARAM);
typedef HRESULT( PRESENT_CALL *ResizeBuffers_t )( IDXGISwapChain *pSwapChain,
	UINT BufferCount,
	UINT Width,
	UINT Height,
	DXGI_FORMAT NewFormat,
	UINT SwapChainFlags );

//
// Overlay Data
namespace Overlay
{
	const UINT ToggleMenu	= VK_INSERT;
	const UINT DetachDll	= VK_END;

	void Detach();

	//
	// Hooking Functions
	namespace Hooks
	{
		bool Initialize();
		bool Shutdown();
		void DisplayMediaOverlay();

		HRESULT PRESENT_CALL Present(IDXGISwapChain *pThis, UINT SyncInterval, UINT Flags);
		HRESULT PRESENT_CALL ResizeBuffers(IDXGISwapChain *pThis,
			UINT BufferCount,
			UINT Width,
			UINT Height,
			DXGI_FORMAT NewFormat,
			UINT SwapChainFlags
		);
		LRESULT CALLBACK  WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	}

	//
	// Global overlay variables
	namespace Data
	{
		extern void*					rgD3D11DeviceTable[D3D11_DEVICE_LEN];
		extern void*					rgDXGISwapChainTable[D3D11_SWAPCHAIN_LEN];
		extern void*					rgD3D11DeviceContextTable[D3D11_DVC_CONTEXT_LEN];
		extern Present_t				pPresent;
		extern Present_t				OriginalPresent;
		extern ResizeBuffers_t			pResizeBuffers;			/* Hooked IDXGISwapChain::ResizeBuffers function */
		extern ResizeBuffers_t			OriginalResizeBuffers;	/* Original IDXGISwapChain::ResizeBuffers function */
		extern WndProc_t				OriginalWndProc;		/* Original WndProc function */

		extern HMODULE					hModule;
		extern ID3D11Device*			pD3D11Device;
		extern IDXGISwapChain*			pSwapChain;
		extern ID3D11DeviceContext*		pDeviceContext;
		extern ID3D11RenderTargetView*	pMainRenderTargetView;

		extern HWND						hWindow;
		extern UINT						nWndWidth;
		extern UINT						nWndHeight;
		extern BOOL						bWndResized;
		extern ImVec2					g_vPrevPosition;

		extern bool						bDisplay;
		extern std::size_t				szPresent;
		extern bool						bDetached;
		extern bool						bImGUIInitialized;
	}
}

#endif
