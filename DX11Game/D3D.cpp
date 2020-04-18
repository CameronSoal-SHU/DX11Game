#include "D3D.h"
#include "D3DUtil.h"
#include "WindowUtil.h"
#include "GameSettings.h"

bool D3D::InitDirect3D(void(*_ptrOnResize)(int, int, D3D &))
{
	// Assign function
	assert(_ptrOnResize);
	m_ptrOnResize = _ptrOnResize;

	// Create the device and context
	CreateD3D();

	CheckMultiSamplingSupport(m_4xMSAAQuality);

	int width, height;
	WindowUtil::Get().GetClientExtends(width, height);

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	CreateSwapChainDescription(swapChainDesc, WindowUtil::Get().GetMainWindow(), true, width, height);
	CreateSwapChain(swapChainDesc);

	m_ptrOnResize(width, height, *this);

	// Create wrap and clamp samplers
	CreateSampler(m_ptrWrapSampler);
	//CreateSampler(m_ptrClampSampler, false);

	return true;
}

void D3D::OnResize_Default() {
	assert(m_ptrD3DImmediateCtx && m_ptrD3DDevice && m_ptrSwapChain);

	// Release the old views and depth/stencil buffer
	ReleaseCOM(m_ptrRenderTargetView);
	ReleaseCOM(m_ptrDepthStencilView);
	ReleaseCOM(m_ptrDepthStencilBuffer);

	ResizeSwapChain((int)Settings::GAME_RES.x, (int)Settings::GAME_RES.y);

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	CreateDepthStencilDescription(depthStencilDesc, (int)Settings::GAME_RES.x, (int)Settings::GAME_RES.y,
		m_enable4xMSAA, m_4xMSAAQuality - 1);
	CreateDepthStencilBufferAndView(depthStencilDesc);

	BindRenderTargetViewAndDepthStencilView();

	SetViewportDimensions((int)Settings::GAME_RES.x, (int)Settings::GAME_RES.y);
}

void D3D::ReleaseD3D(bool _extraInfo) {
	// Release Texture cache
	m_textureCache.Release();

	if (m_ptrSwapChain) {
		// Is game in full screen?
		BOOL fullscreen = false;
		HR(m_ptrSwapChain->GetFullscreenState(&fullscreen, nullptr));

		if (fullscreen) {	// If so, switch to windowed before releasing
			m_ptrSwapChain->SetFullscreenState(false, nullptr);
		}
	}

	ReleaseCOM(m_ptrRenderTargetView);
	ReleaseCOM(m_ptrDepthStencilView);
	ReleaseCOM(m_ptrSwapChain);
	ReleaseCOM(m_ptrDepthStencilBuffer);
	//ReleaseCOM(m_ptrClampSampler);
	//ReleaseCOM(m_ptrWrapSampler);

	// Restore default settings then release
	if (m_ptrD3DImmediateCtx) {
		m_ptrD3DImmediateCtx->ClearState();
		m_ptrD3DImmediateCtx->Flush();
	}

	ReleaseCOM(m_ptrD3DImmediateCtx);

	if (_extraInfo) {
		ID3D11Debug* ptrD3DDebug;
		HR(m_ptrD3DDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&ptrD3DDebug)));
		HR(ptrD3DDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY));

		ReleaseCOM(ptrD3DDebug);
	}

	ReleaseCOM(m_ptrD3DDevice);
}

float D3D::GetAspectRatio() {
	int width, height;
	WindowUtil::Get().GetClientExtends(width, height);

	return width / (float)height;
}

void D3D::BeginRender(const DirectX::SimpleMath::Vector4& _colour) {
	m_ptrD3DImmediateCtx->ClearRenderTargetView(m_ptrRenderTargetView, reinterpret_cast<const float*>(&_colour));
	m_ptrD3DImmediateCtx->ClearDepthStencilView(m_ptrDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

void D3D::EndRender() {
	HR(m_ptrSwapChain->Present(0, 0));
}

ID3D11Device& D3D::GetDevice() {
	assert(m_ptrD3DDevice);

	return *m_ptrD3DDevice;
}

ID3D11DeviceContext& D3D::GetDeviceCtx() {
	assert(m_ptrD3DImmediateCtx);

	return *m_ptrD3DImmediateCtx;
}

bool D3D::GetDeviceReady() const {
	return m_ptrD3DDevice != nullptr;
}

void D3D::OnResize(int _screenWidth, int _screenHeight, D3D& _d3d) {
	assert(m_ptrOnResize);

	m_ptrOnResize(_screenWidth, _screenHeight, _d3d);
}

TextureCache& D3D::GetTextureCache() {
	return m_textureCache;
}

ID3D11SamplerState& D3D::GetWrapSampler() {
	assert(m_ptrWrapSampler);

	return *m_ptrWrapSampler;
}

void D3D::CreateD3D(D3D_FEATURE_LEVEL _desiredFeatLvl) {
	UINT createDeviceFlags(0);

#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	m_d3dDriverType = D3D_DRIVER_TYPE_UNKNOWN;

	// How many GPUs?
	IDXGIAdapter* ptrAdapter;
	std::vector<IDXGIAdapter*> adapters;

	// Create factories
	IDXGIFactory* ptrFactory = NULL;
	HR(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&ptrFactory));

	SIZE_T useIndex(-1), mostVRam(-1);	// Used to find the GPU with the largest dedicated VRAM

	// Go through each GPU and get info on its specs. Search for highest dedicated VRAM across all adapters.
	for (UINT i(0); ptrFactory->EnumAdapters(i, &ptrAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		adapters.push_back(ptrAdapter);	// Add adapter onto vector list

		DXGI_ADAPTER_DESC adapterDesc;	// Setup Adapter description
		HR(ptrAdapter->GetDesc(&adapterDesc));
		// GPU info dump
		WDBOUT(L"Found adapter = (" << i << ") " << adapterDesc.Description << 
			L" VRAM = " << adapterDesc.DedicatedVideoMemory);

		// Has a new highest VRAM been found?
		if (adapterDesc.DedicatedVideoMemory > mostVRam || mostVRam == -1) {
			// If so, store it's index and VRAM
			useIndex = i;
			mostVRam = adapterDesc.DedicatedVideoMemory;
		}
	}

	if (ptrFactory) {
		ptrFactory->Release();
	}
	assert(useIndex >= 0);	// Ensure an adapter was found

	D3D_FEATURE_LEVEL featLvl;

	HR(D3D11CreateDevice(
		adapters.at(useIndex),		// Default adapter
		m_d3dDriverType,
		0,							// No software device
		createDeviceFlags,
		0, 0,						// Default feature level array
		D3D11_SDK_VERSION,
		&m_ptrD3DDevice,
		&featLvl,
		&m_ptrD3DImmediateCtx));

	if (featLvl != _desiredFeatLvl) {	// Was desired level met?
		DBOUT("Direct3D Feature Level Unsupported");
		assert(false);
	}
}

void D3D::ResizeSwapChain(int _screenWidth, int _screenHeight) {
	HR(m_ptrSwapChain->ResizeBuffers(1, _screenWidth, _screenHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

	ID3D11Texture2D* backBuffer;
	HR(m_ptrSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(m_ptrD3DDevice->CreateRenderTargetView(backBuffer, 0, &m_ptrRenderTargetView));
	ReleaseCOM(backBuffer);
}

void D3D::CreateDepthStencilDescription(D3D11_TEXTURE2D_DESC& _textureDesc, int _screenWidth, int _screenHeight, 
	bool _enableMSAA4X, int _maxQuality) {
	// Set up texture description
	_textureDesc.Width	   = _screenWidth;
	_textureDesc.Height	   = _screenHeight;
	_textureDesc.MipLevels = 1;
	_textureDesc.ArraySize = 1;
	_textureDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Should 4x MSAA be used?
	_textureDesc.SampleDesc.Count   = _enableMSAA4X ? 4 : 1;
	_textureDesc.SampleDesc.Quality = _enableMSAA4X ? (m_4xMSAAQuality - 1) : 0;

	_textureDesc.Usage			= D3D11_USAGE_DEFAULT;
	_textureDesc.BindFlags		= D3D11_BIND_DEPTH_STENCIL;
	_textureDesc.CPUAccessFlags = 0;
	_textureDesc.MiscFlags		= 0;
}

void D3D::CreateDepthStencilBufferAndView(D3D11_TEXTURE2D_DESC& _textureDesc) {
	HR(m_ptrD3DDevice->CreateTexture2D(&_textureDesc, 0, &m_ptrDepthStencilBuffer));
	HR(m_ptrD3DDevice->CreateDepthStencilView(m_ptrDepthStencilBuffer, 0, &m_ptrDepthStencilView));
}

void D3D::BindRenderTargetViewAndDepthStencilView() {
	m_ptrD3DImmediateCtx->OMSetRenderTargets(1, &m_ptrRenderTargetView, m_ptrDepthStencilView);
}

void D3D::SetViewportDimensions(int _screenWidth, int _screenHeight) {
	m_screenViewport.TopLeftX = 0;
	m_screenViewport.TopLeftY = 0;
	m_screenViewport.Width    = static_cast<float>(_screenWidth);
	m_screenViewport.Height   = static_cast<float>(_screenHeight);
	m_screenViewport.MinDepth = 0.f;
	m_screenViewport.MaxDepth = 1.f;

	m_ptrD3DImmediateCtx->RSSetViewports(1, &m_screenViewport);
}

void D3D::CheckMultiSamplingSupport(UINT& _msaaQuality) {
	// Check 4X MSAA quality support for our back buffer format.
	HR(m_ptrD3DDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &_msaaQuality));

	// If zero was returned then the hardware cannot do it
	assert(_msaaQuality > 0);
}

void D3D::CreateSwapChainDescription(DXGI_SWAP_CHAIN_DESC& _swapChainDesc, HWND _windowHandle, bool _isWindowed, 
	int _screenWidth, int _screenHeight) {
	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.
	_swapChainDesc.BufferDesc.Width					  = _screenWidth;
	_swapChainDesc.BufferDesc.Height				  = _screenHeight;
	_swapChainDesc.BufferDesc.RefreshRate.Numerator   = 165;	// 165hz
	_swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	_swapChainDesc.BufferDesc.Format				  = DXGI_FORMAT_R8G8B8A8_UNORM;
	_swapChainDesc.BufferDesc.ScanlineOrdering		  = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	_swapChainDesc.BufferDesc.Scaling				  = DXGI_MODE_SCALING_STRETCHED;
	

	// Use 4x MSAA?
	_swapChainDesc.SampleDesc.Count   = m_enable4xMSAA ? 4 : 1;
	_swapChainDesc.SampleDesc.Quality = m_enable4xMSAA ? (m_4xMSAAQuality - 1) : 0;

	_swapChainDesc.BufferUsage	= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	_swapChainDesc.BufferCount	= 1;
	_swapChainDesc.OutputWindow = _windowHandle;
	_swapChainDesc.Windowed		= _isWindowed;
	_swapChainDesc.SwapEffect	= DXGI_SWAP_EFFECT_DISCARD;
	_swapChainDesc.Flags		= 0;

	m_windowedMode = _isWindowed;
}

void D3D::CreateSwapChain(DXGI_SWAP_CHAIN_DESC& _swapChainDesc) {
	// To correctly create the swap chain, we must use the IDXGIFactory that was
	// used to create the device.

	IDXGIDevice* dxgiDevice = 0;
	HR(m_ptrD3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));
	
	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(m_ptrD3DDevice, &_swapChainDesc, &m_ptrSwapChain));

	// Factory objects are no longer needed
	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);
}

void D3D::CreateSampler(ID3D11SamplerState*& _ptrSampler) {
	D3D11_SAMPLER_DESC samplerDesc;

	ZeroMemory(&samplerDesc, sizeof(samplerDesc));

	samplerDesc.Filter		   = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU	   = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV	   = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW	   = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD		   = 0;
	samplerDesc.MaxLOD		   = D3D11_FLOAT32_MAX;

	HR(m_ptrD3DDevice->CreateSamplerState(&samplerDesc, &_ptrSampler));
}