#include <pch.h>
#include <base.h>
#include <map>
#include <string>
#include <random>
#include "overlayreg.h"
#include "microphone.h"
#include "icon_resources.h"

//
// Overlay Icon Placement Anchor
enum class IconImageAnchor
{
	TOPLEFT,
	TOPRIGHT,
	BOTTOMLEFT,
	BOTTOMRIGHT,
	NONE
};

//-----------------------------------------------------------------
// Globals
static IconImageAnchor			g_hPreviousAnchorType		= IconImageAnchor::NONE;
static HWND						g_hAppWnd					= NULL;
static ID3D11ShaderResourceView *g_pMutedShaderResView		= nullptr;
static ID3D11ShaderResourceView *g_pUnMutedShaderResView	= nullptr;
static ID3D11ShaderResourceView *g_pSelectedShaderResView	= nullptr;
static int						g_nAccumulatedFrames		= 0;


static IconImageAnchor GetIconImageAnchor();
static VOID LoadImageShaderResources(ID3D11Device *pDevice);
static VOID LoadImageShaderResourceFromFile(ID3D11Device *pDevice, LPCSTR lpszFilePath, ID3D11ShaderResourceView **ppShaderResourceView);
static VOID InitializeImGUI();

using namespace Overlay;


//-----------------------------------------------------------------
// Procedure for the hooked IDXGISwapChain::Present(). Called whenever a frame is being presented to the output window
// Responsible for displaying the overlay window
HRESULT PRESENT_CALL Hooks::Present(IDXGISwapChain* pThis, UINT SyncInterval, UINT Flags)
{
	DWORD dwBuffer;

	using namespace Data;

	if (!bImGUIInitialized)
	{
		pSwapChain = pThis;

		/* Create ID3D11Device from the swap chain */
		if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&pD3D11Device))))
		{
			InitializeImGUI();
		}
	}
	if (!bImGUIInitialized)
	{
		return OriginalPresent( pThis, SyncInterval, Flags );
	}

	/* Check if the overlay has been disabled */
	if (!IsOverlayEnabled())
	{
		::Detach();
		return OriginalPresent( pThis, SyncInterval, Flags );
	}

	//
	// Select which image shader resource to display
	if (g_nAccumulatedFrames == 10 || g_pSelectedShaderResView == nullptr)
	{
		if (g_nAccumulatedFrames) {
			g_nAccumulatedFrames = 0;
		}

		BOOL bMuted;
		GetMicrophoneMuted(&bMuted);

		if (bMuted)
		{
			g_pSelectedShaderResView = g_pMutedShaderResView;
		}
		else
		{
			g_pSelectedShaderResView = g_pUnMutedShaderResView;
		}
	}
	if (!g_pSelectedShaderResView)
	{
		return OriginalPresent( pThis, SyncInterval, Flags );
	}

	//
	// Begin new ImGUI frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//
	// Determine the image size
	ImVec2 vImageSize{ OVERLAY_IMAGE_DEFAULT_WIDTH, OVERLAY_IMAGE_DEFAULT_HEIGHT };
	if (NO_ERROR == GetAppRegistryValue(OVERLAY_REG_VALUE_OVERLAY_IMAGE_WIDTH, &dwBuffer, sizeof(DWORD)))
	{
		if (dwBuffer && dwBuffer < (nWndWidth / 2))
		{
			vImageSize.x = (FLOAT)dwBuffer;
		}

		dwBuffer = 0;
	}
	if (NO_ERROR == GetAppRegistryValue(OVERLAY_REG_VALUE_OVERLAY_IMAGE_HEIGHT, &dwBuffer, sizeof(DWORD)))
	{
		if (dwBuffer && dwBuffer < (nWndHeight / 2))
		{
			vImageSize.y = (FLOAT)dwBuffer;
		}

		dwBuffer = 0;
	}

	/* Set the image window size */
	ImVec2 vWindowSize{ (FLOAT)vImageSize.x + 2, (FLOAT)vImageSize.y + 2};
	ImGui::SetNextWindowSize(vWindowSize, ImGuiCond_Always);

	//
	// Set the image position
	IconImageAnchor CurrentImageAnchor = GetIconImageAnchor();
	switch (CurrentImageAnchor)
	{
		case IconImageAnchor::TOPLEFT:
			ImGui::SetNextWindowPos(ImVec2(0.0F, 0.0F), ImGuiCond_Always);
			break;
		case IconImageAnchor::TOPRIGHT:
			ImGui::SetNextWindowPos(ImVec2(nWndWidth - vWindowSize.x, 0.0F), ImGuiCond_Always);
			break;
		case IconImageAnchor::BOTTOMLEFT:
			ImGui::SetNextWindowPos(ImVec2(0.0F, nWndHeight - vWindowSize.y), ImGuiCond_Always);
			break;
		case IconImageAnchor::BOTTOMRIGHT:
			ImGui::SetNextWindowPos(ImVec2(nWndWidth - vWindowSize.x, nWndHeight - vWindowSize.y), ImGuiCond_Always);
			break;
	}

	++g_nAccumulatedFrames;

	/* Draw the ImGUI image window to the output window */
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin( "Image",
		&bDisplay,
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
	);
	ImGui::Image( (void *)g_pSelectedShaderResView, vImageSize);

	ImGui::PopStyleVar();

	//
	// Render the ImGUI image window 
	ImGui::End();
	ImGui::Render();

	pDeviceContext->OMSetRenderTargets(1, &pMainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return OriginalPresent(pThis, SyncInterval, Flags);
}

//-----------------------------------------------------------------
// Procedure for the hooked IDXGISwapChain::ResizeBuffers(). Called whenever the output window is resized
// Uninitializes ImGUI data & releases the current Render Target View (RTV)
HRESULT PRESENT_CALL Hooks::ResizeBuffers(
	IDXGISwapChain	*pThis,
	UINT			BufferCount,
	UINT			Width,
	UINT			Height,
	DXGI_FORMAT		NewFormat,
	UINT			SwapChainFlags
)
{
	using namespace Data;

	if (pMainRenderTargetView)
	{
		//
		// The RTV is invalid now. It must be released and re-initialized in Hooks::Present()
		pDeviceContext->OMSetRenderTargets( 0, 0, 0 );
		pMainRenderTargetView->Release();
	}

	/* Restore the output window procedure */
	SetWindowLongPtr( hWindow, GWLP_WNDPROC, (LONG_PTR)OriginalWndProc );

	//
	// Unitialize ImGui
	if (bImGUIInitialized)
	{
		bImGUIInitialized		= false;
		g_nAccumulatedFrames	= 0;

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		//
		// Release D3D11 device
		pDeviceContext->Release();
		pD3D11Device->Release();
	}

	pSwapChain		= NULL;
	pDeviceContext	= nullptr;
	pD3D11Device	= nullptr;

	return OriginalResizeBuffers( pThis, BufferCount, Width, Height, NewFormat, SwapChainFlags );
}


//-----------------------------------------------------------------
// Initialize ImGUI & various global variables from Overlay::Data
VOID InitializeImGUI()
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ID3D11Texture2D		*pBackBufferTexture;

	using namespace Data;

	g_hPreviousAnchorType		= IconImageAnchor::NONE;
	g_pSelectedShaderResView	= nullptr;

	pD3D11Device->GetImmediateContext(&Data::pDeviceContext);

	/* Get the swap chain descriptor */
	pSwapChain->GetDesc(&swapChainDesc);

	//
	// Get the output window info.
	hWindow		= swapChainDesc.OutputWindow;
	nWndWidth	= swapChainDesc.BufferDesc.Width;
	nWndHeight	= swapChainDesc.BufferDesc.Height;
	bWndResized = TRUE;

	//
	// Create a new RTV (render target view)
	pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBufferTexture));
	Data::pD3D11Device->CreateRenderTargetView(pBackBufferTexture, NULL, &pMainRenderTargetView);
	pBackBufferTexture->Release();

	/* Replace the output window msg proc with Overlay::Hooks::WndProc() */
	OriginalWndProc = (WNDPROC)SetWindowLongPtr(hWindow, GWLP_WNDPROC, (LONG_PTR)::Hooks::WndProc);

	//
	// Initialize ImGUI context
	ImGui::CreateContext();

	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange; //| ImGuiConfigFlags_NoMouse;

	ImGui_ImplWin32_Init(hWindow);
	ImGui_ImplDX11_Init(pD3D11Device, pDeviceContext);

	bImGUIInitialized = true;

	//
	// Load the overlay images
	if (!g_pMutedShaderResView || !g_pUnMutedShaderResView)
	{
		LoadImageShaderResources(pD3D11Device);
	}
}

//-----------------------------------------------------------------
// Determine the image icon placement anchor from the registry
IconImageAnchor GetIconImageAnchor()
{
	LRESULT lResult;
	DWORD	dwStatus;

	lResult = GetAppRegistryValue(REG_VALUE_OVERLAY_ANCHOR, &dwStatus, sizeof(DWORD));
	if (lResult == ERROR_SUCCESS)
	{
		if (dwStatus & INGAME_OVERLAY_ANCHOR_BOTTOM)
		{
			if (dwStatus & INGAME_OVERLAY_ANCHOR_RIGHT) {
				return IconImageAnchor::BOTTOMRIGHT;
			}
			else {
				return IconImageAnchor::BOTTOMLEFT;
			}
		}
		else /* dwStatus & INGAME_OVERLAY_ANCHOR_TOP */
		{
			if (dwStatus & INGAME_OVERLAY_ANCHOR_RIGHT) {
				return IconImageAnchor::TOPRIGHT;
			}
			else {
				return IconImageAnchor::TOPLEFT;
			}
		}
	}

	/* Default anchor type */
	return IconImageAnchor::TOPLEFT;
}

//-----------------------------------------------------------------
// Create ID3D11ShaderResourceView of the muted & unmuted images 
VOID LoadImageShaderResources(ID3D11Device *pDevice)
{
	D3DX11_IMAGE_LOAD_INFO imgLoadInfo;

	if (!g_pMutedShaderResView)
	{
		D3DX11CreateShaderResourceViewFromMemory(pDevice,
			g_rgMutedIconBytes,
			sizeof(g_rgMutedIconBytes),
			&imgLoadInfo,
			NULL,
			&g_pMutedShaderResView,
			NULL
		);
	}
	if (!g_pUnMutedShaderResView)
	{
		D3DX11CreateShaderResourceViewFromMemory(pDevice,
			g_rgUnmutedIconBytes,
			sizeof(g_rgUnmutedIconBytes),
			&imgLoadInfo,
			NULL,
			&g_pUnMutedShaderResView,
			NULL
		);
	}
}

//-----------------------------------------------------------------
// Demonstrates how to create ID3D11ShaderResourceView from a (supported) image file instead of memory
// Currently unused
VOID LoadImageShaderResourceFromFile(ID3D11Device *pDevice, LPCSTR lpszFilePath, ID3D11ShaderResourceView **ppShaderResourceView)
{
	D3DX11_IMAGE_LOAD_INFO	imgLoadInfo;
	HRESULT					hResult;
	HRESULT					hReturnStatus;

	hResult = D3DX11CreateShaderResourceViewFromFileA(pDevice,
		lpszFilePath,
		&imgLoadInfo,
		NULL,
		ppShaderResourceView,
		NULL
	);
	if (FAILED(hResult))
	{
		// TODO
	}
}