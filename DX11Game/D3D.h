#ifndef D3DH
#define D3DH
#include <d3d11.h>

#include "SimpleMath.h"
#include "TextureCache.h"

#define EXTRA_INFO true
#define WINDOWED true
#define ENABLE_MSAA true
#define NO_EXTRA_INFO false
#define FULLSCREEN false
#define DISABLE_MSAA false

class D3D
{
public:
	// Setup D3D
	bool InitDirect3D(void(*_ptrOnResize)(int, int, D3D&));

	/* 
	Default minimum behaviour when ALT+ENTER or drag or resize
	parameters are new width and height of window
	*/
	void OnResize_Default();
	// See m_ptrOnResize
	void OnResize(int _screenWidth, int _screenHeight, D3D& _d3d);

	/* 
	Main Release function to be called on Shutdown
	<param "extraInfo"> Provides more information about any objects that haven't been released properly </param>
	*/
	void ReleaseD3D(bool _extraInfo = EXTRA_INFO);

	// Get the Windows current aspect ratio (letterbox, square, etc.)
	float GetAspectRatio() const;

	// Handles rendering on screen
	void BeginRender(const DirectX::SimpleMath::Vector4& _colour);
	void EndRender();

	ID3D11Device& GetDevice();
	ID3D11DeviceContext& GetDeviceCtx();
	// Has the D3D Device been assigned?
	bool GetDeviceReady() const;

	TextureCache& GetTextureCache();

	ID3D11SamplerState& GetWrapSampler();
private:
	TextureCache m_textureCache;
	// Type of GPU owned
	D3D_DRIVER_TYPE m_d3dDriverType = D3D_DRIVER_TYPE_UNKNOWN;
	// Texture multisampling quality level supported
	UINT m_4xMSAAQuality = 0;
	bool m_enable4xMSAA = true;
	// Depth buffer for sorting pixels by distance from camera
	ID3D11Texture2D* m_ptrDepthStencilBuffer = nullptr;
	// Is the game running in windowed?
	bool m_windowedMode = false;
	// Main handle used to create resources and access D3D
	ID3D11Device* m_ptrD3DDevice = nullptr;
	// Handle off the device to give rendering commands
	ID3D11DeviceContext* m_ptrD3DImmediateCtx = nullptr;
	// A Number of surfaces that can be rendered onto and displayed
	IDXGISwapChain* m_ptrSwapChain = nullptr;
	// Target for rendering objects
	ID3D11RenderTargetView* m_ptrRenderTargetView = nullptr;
	// Test depth of pixels to cull out objects that are behind other objects
	ID3D11DepthStencilView* m_ptrDepthStencilView = nullptr;
	// Position, Height, Width, Min/Max Depth of the view being rendered
	D3D11_VIEWPORT m_screenViewport;
	/*
	Function to be called whenever window is changed (via dragging or ALT+ENTER)
	*/
	void(*m_ptrOnResize)(int, int, D3D&) = nullptr;

	ID3D11SamplerState* m_ptrWrapSampler = nullptr;		// Wrapping sprites

	// Heavy lifting to start D3D11
	void CreateD3D(D3D_FEATURE_LEVEL _desiredFeatLvl = D3D_FEATURE_LEVEL_11_0);
	// Buffers in the swap chain must match the screen resolution
	void ResizeSwapChain(int _screenWidth, int _screenHeight);
	// The kind of depth stencil required
	void CreateDepthStencilDescription(D3D11_TEXTURE2D_DESC& _textureDesc, int _screenWidth, int _screenHeight, 
		bool _enableMSAA4X, int _maxQuality);
	// Create all render buffers
	void CreateDepthStencilBufferAndView(D3D11_TEXTURE2D_DESC& _textureDesc);
	// Bind the render target view and depth/stencil view to the pipeline
	void BindRenderTargetViewAndDepthStencilView();
	// Update Viewport dimensions when the window changes size
	void SetViewportDimensions(int _screenWidth, int _screenHeight);
	// Verify if GPU can support 4x Sampling
	void CheckMultiSamplingSupport(UINT& _msaaQuality);
	// Specify the type of swap chain needed
	void CreateSwapChainDescription(DXGI_SWAP_CHAIN_DESC& _swapChainDesc, HWND _windowHandle, bool _isWindowed, 
		int _screenWidth, int _screenHeight);
	// Create swap chain once description is determined
	void CreateSwapChain(DXGI_SWAP_CHAIN_DESC& _swapChainDesc);

	void CreateSampler(ID3D11SamplerState*& _ptrSampler);
};

#endif