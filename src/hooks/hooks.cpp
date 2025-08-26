#include <pch.h>
#include <base.h>


static bool FetchMethodTables(void **ppSwapChainTable, size_t	cbSwapChainTable, void **ppDeviceTable, size_t	cbDeviceTable);

using namespace Overlay;


//-----------------------------------------------------------------
// Initialize & create hook functions
bool Hooks::Initialize()
{
	CoInitializeEx( NULL, COINITBASE_MULTITHREADED );
	if (FetchMethodTables(Data::rgDXGISwapChainTable, sizeof(Data::rgDXGISwapChainTable), Data::rgD3D11DeviceTable, sizeof(Data::rgD3D11DeviceTable)))
	{
		/* Set the original hook function addresses */
		Data::pPresent			= (Present_t)Data::rgDXGISwapChainTable[8];
		Data::pResizeBuffers	= (ResizeBuffers_t)Data::rgDXGISwapChainTable[13];

		//
		// Create & enable the hooks
		MH_STATUS Status = MH_CreateHook( reinterpret_cast<void **>( Data::pPresent ),
			&Hooks::Present,
			reinterpret_cast<void **>( &Data::OriginalPresent )
		);
		MH_CreateHook( reinterpret_cast<void **>( Data::pResizeBuffers ),
			&Hooks::ResizeBuffers,
			reinterpret_cast<void **>( &Data::OriginalResizeBuffers )
		);
		if (Status == MH_OK)
		{
			Status = MH_EnableHook( Data::pPresent );
			Status = MH_EnableHook( Data::pResizeBuffers );
		}

		if (Status != MH_OK)
		{
			return false;
		}

		return true;
	}
	
	return false;
}

//-----------------------------------------------------------------
// Shutdown & restore hooked functions
bool Hooks::Shutdown()
{
	if (Data::pMainRenderTargetView)
	{
		Data::pDeviceContext->OMSetRenderTargets( 0, 0, 0 );

		Data::pMainRenderTargetView->Release();
		Data::pMainRenderTargetView = nullptr;
	}

	if (Data::bImGUIInitialized)
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		if (Data::pDeviceContext)
		{
			Data::pDeviceContext->Release();
			Data::pDeviceContext = nullptr;
		}
		if (Data::pD3D11Device)
		{
			Data::pD3D11Device->Release();
			Data::pD3D11Device = nullptr;
		}
	}
	
	MH_DisableHook( MH_ALL_HOOKS );
	MH_RemoveHook( MH_ALL_HOOKS );
	SetWindowLongPtr(Data::hWindow, WNDPROC_INDEX, (LONG_PTR)Data::OriginalWndProc);

	return true;
}


//-----------------------------------------------------------------
// Copies the method table addresses of ID3D11Device, ID3D11DeviceContext, and IDXGISwapChain
bool FetchMethodTables(void **ppSwapChainTable, size_t cbSwapChainTable, void **ppDeviceTable, size_t cbDeviceTable)
{
	WNDCLASSEX				wndClassEx{};
	HWND					hDummyWnd;
	DXGI_SWAP_CHAIN_DESC	swapChainDesc{};
	D3D_FEATURE_LEVEL		dFeatureLevel;
	ID3D11Device			*pDummyDevice;
	IDXGISwapChain			*pDummySwapChain;

	hDummyWnd = NULL;

	//
	// Create dummy window for swap chain
	wndClassEx.cbSize			= sizeof(wndClassEx);
	wndClassEx.lpfnWndProc		= DefWindowProc;
	wndClassEx.lpszClassName	= TEXT("OutfledOverlayWndClass");
	if (RegisterClassEx(&wndClassEx))
	{
		hDummyWnd = CreateWindow(wndClassEx.lpszClassName, TEXT(""), WS_DISABLED, 0, 0, 0, 0, NULL, NULL, NULL, nullptr);
	}

	if (!hDummyWnd)
	{
		return false;
	}

	//
	// Create D3D11Device & DXGISwapchain
	swapChainDesc.BufferCount					= 1;
	swapChainDesc.BufferUsage					= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferDesc.Format				= DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.OutputWindow					= hDummyWnd;
	swapChainDesc.SampleDesc.Count				= 1;
	swapChainDesc.BufferDesc.ScanlineOrdering	= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling			= DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect					= DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Windowed						= TRUE;

	HRESULT hResult = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		NULL, 
		0,
		D3D11_SDK_VERSION,
		&swapChainDesc, 
		&pDummySwapChain,
		&pDummyDevice,
		&dFeatureLevel,
		NULL
	);
	if (FAILED(hResult))
	{
		DestroyWindow(swapChainDesc.OutputWindow);
		UnregisterClass(wndClassEx.lpszClassName, GetModuleHandle(nullptr));
		return false;
	}

	//
	// Copy the method tables
	if (ppSwapChainTable && pDummySwapChain)
	{
		memcpy(ppSwapChainTable, *reinterpret_cast<void ***>(pDummySwapChain), cbSwapChainTable);
	}
	if (ppDeviceTable && pDummyDevice)
	{
		memcpy(ppDeviceTable, *reinterpret_cast<void ***>(pDummyDevice), cbDeviceTable);
	}

	//
	// Cleanup
	if (pDummySwapChain)
	{
		pDummySwapChain->Release();
	}
	if (pDummyDevice)
	{
		pDummyDevice->Release();
	}

	DestroyWindow(swapChainDesc.OutputWindow);
	UnregisterClass(wndClassEx.lpszClassName, GetModuleHandle(nullptr));

	return true;
}
