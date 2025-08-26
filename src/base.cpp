#include <pch.h>
#include <base.h>

//-----------------------------------------------------------------
// Overlay::Data Globals
void					*Overlay::Data::rgD3D11DeviceTable[D3D11_DEVICE_LEN];
void					*Overlay::Data::rgDXGISwapChainTable[D3D11_SWAPCHAIN_LEN];
//void					*Overlay::Data::rgD3D11DeviceContextTable[D3D11_DVC_CONTEXT_LEN];

Present_t				Overlay::Data::pPresent					= (Present_t)NULL;
Present_t               Overlay::Data::OriginalPresent			= (Present_t)NULL;
ResizeBuffers_t			Overlay::Data::pResizeBuffers			= (ResizeBuffers_t)NULL;
ResizeBuffers_t         Overlay::Data::OriginalResizeBuffers	= (ResizeBuffers_t)NULL;
WndProc_t               Overlay::Data::OriginalWndProc			= (WndProc_t)NULL;


HMODULE					Overlay::Data::hModule					= NULL;
ID3D11Device			*Overlay::Data::pD3D11Device			= NULL;
IDXGISwapChain			*Overlay::Data::pSwapChain				= NULL;
ID3D11DeviceContext		*Overlay::Data::pDeviceContext			= NULL;
ID3D11RenderTargetView	*Overlay::Data::pMainRenderTargetView	= NULL;

HWND					Overlay::Data::hWindow					= NULL;
UINT					Overlay::Data::nWndWidth				= 0;
UINT					Overlay::Data::nWndHeight				= 0;
BOOL					Overlay::Data::bWndResized				= false;
ImVec2					Overlay::Data::g_vPrevPosition			= {};


bool					Overlay::Data::bImGUIInitialized		= false;
bool					Overlay::Data::bDisplay					= true;
bool					Overlay::Data::bDetached				= false;

#if defined(MEM_86)
std::size_t             Overlay::Data::szPresent = 5;
#elif defined(MEM_64)
std::size_t             Overlay::Data::szPresent = 19;
#endif

void Overlay::Detach()
{
	Hooks::Shutdown();
	DetachedThread(Data::hModule);
}