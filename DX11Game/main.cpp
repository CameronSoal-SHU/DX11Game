#include "MainGame.h"
#include "WindowUtil.h"
#include "GameSettings.h"

// On resize default uses the rendering dimensions declared in GameSettings as opposed to window dimensions.
void OnResize(int _screenWidth, int _screenHeight, D3D& _d3d) {
	_d3d.OnResize_Default();
}

LRESULT CALLBACK MainWndProc(HWND _windowHandle, UINT _msg, WPARAM _wParam, LPARAM _lParam) {
	switch (_msg) {
	case WM_CHAR:
		switch (_wParam) {
		case 27:
		case 'q':
		case 'Q':
			PostQuitMessage(0);
			break;
		}
		break;
	case WM_INPUT:
		MainGame::mouseKeyboardInput.MessageEvent((HRAWINPUT)_lParam);	// Grabbing raw keyboard events
	}
	return WindowUtil::Get().DefaultMsgHandler(_windowHandle, _msg, _wParam, _lParam);
}

// Code Entry point
int WINAPI WinMain(HINSTANCE _currInstance, HINSTANCE _prevInstance,
	PSTR _cmdLine, int _showCmd) {

	assert(WindowUtil::Get().InitMainWindow((int)Settings::SCREEN_DIM.x, (int)Settings::SCREEN_DIM.y, _currInstance, "DX11 Game Test", MainWndProc, true));

	D3D d3d;
	assert(d3d.InitDirect3D(OnResize));

	WindowUtil::Get().SetD3D(d3d);
	d3d.GetTextureCache().SetAssetPath("../bin/data/");

	MainGame mainGame(d3d);

	bool canUpdateRender;
	float deltaTime(0.f);

	while (WindowUtil::Get().BeginLoop(canUpdateRender)) {
		if (canUpdateRender && deltaTime > 0.f) {
			mainGame.Update(deltaTime);
			mainGame.Render(deltaTime);
		}

		deltaTime = WindowUtil::Get().EndLoop(canUpdateRender);
	}

	mainGame.Release();
	d3d.ReleaseD3D(true);

	return 0;
}