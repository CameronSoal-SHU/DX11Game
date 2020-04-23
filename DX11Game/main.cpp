#include "MainGame.h"
#include "WindowUtil.h"
#include "GameSettings.h"

// On resize default uses the rendering dimensions declared in GameSettings as opposed to window dimensions.
void OnResize(int _screenWidth, int _screenHeight, D3D& _d3d) {
	_d3d.OnResize_Default();
	if (&MainGame::Get()) {
		MainGame::Get().mouseKeyboardInput.RestrictMouse();
	}
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
int WINAPI WinMain(HINSTANCE _curInstance, HINSTANCE _prevInstance,
	PSTR _cmdLine, int _showCmd) {
	new WindowUtil();

	// Works but causes memory leaks
	/*HICON hIcon = static_cast<HICON>(LoadImage(_curInstance, "../bin/data/Ship/ship_test.ico", IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_LOADFROMFILE));
	HICON hIconSmall = static_cast<HICON>(LoadImage(_curInstance, "../bin/data/Ship/ship_test.ico", IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_LOADFROMFILE));*/

	assert(WindowUtil::Get().InitMainWindow((int)Settings::SCREEN_DIM.x, (int)Settings::SCREEN_DIM.y, 
		_curInstance, "DX11 Game Test", MainWndProc, CENTRED));

	D3D d3d;
	assert(d3d.InitDirect3D(OnResize));

	WindowUtil::Get().SetD3D(d3d);
	d3d.GetTextureCache().SetAssetPath("../bin/data/");

	new MainGame(d3d);

	bool canUpdateRender;
	float deltaTime(0.f), goodDeltaTime(0.f);

	while (WindowUtil::Get().BeginLoop(canUpdateRender)) {
		if (canUpdateRender && deltaTime > 0.f) {
			goodDeltaTime = deltaTime;	// Record previous deltaTime
			MainGame::Get().Update(deltaTime);
			MainGame::Get().Render(deltaTime);
		}

		deltaTime = WindowUtil::Get().EndLoop(canUpdateRender);
		// If a bad deltaTime value occurs (generally in abnomally high Frame Rates exceeding 10000)
		// Set the deltaTime to the last good deltaTime recorded to avoid slow motion
		if (deltaTime < VERY_SMALL) deltaTime = goodDeltaTime;
	}

	MainGame::Get().Release();
	delete &MainGame::Get();
	d3d.ReleaseD3D(EXTRA_INFO);
	delete &WindowUtil::Get();
	return 0;
}