#include <cassert>

#include "WindowUtil.h"
#include "D3D.h"
#include "D3DUtil.h"

bool WindowUtil::InitMainWindow(int _width, int _height, HINSTANCE _instanceHandle, 
	const std::string& _appName, WNDPROC _msgHandler, bool _centred) {
	// Enable run-time memory check for debug builds
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	assert(_msgHandler);
	// Assign window data handles
	m_windowData.mainWindowCaption = _appName;
	m_windowData.instanceHandle    = _instanceHandle;
	m_windowData.clientWidth	   = _width;
	m_windowData.clientHeight	   = _height;

	// Set up window class for registration
	WNDCLASS windowClass;
	windowClass.style		  = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc   = _msgHandler;
	windowClass.cbClsExtra    = 0;
	windowClass.cbWndExtra    = 0;
	windowClass.hInstance	  = m_windowData.instanceHandle;
	windowClass.hIcon		  = LoadIcon(0, IDI_APPLICATION);
	windowClass.hCursor		  = LoadCursor(0, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	windowClass.lpszMenuName  = 0;
	windowClass.lpszClassName = "D3DWindowClassName";

	if (!RegisterClass(&windowClass)) {
		MessageBox(0, "RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rect dimensions based on requested client area dimensions
	RECT windowRect = { 0,0,m_windowData.clientWidth, m_windowData.clientHeight };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

	int width = windowRect.right - windowRect.left;
	int height = windowRect.bottom - windowRect.top;

	int offsetX = CW_USEDEFAULT, offsetY = CW_USEDEFAULT;

	// Align window in centre of screen
	if (_centred) {
		offsetY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;
		offsetX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	}

	// Assign window handle with a new window
	m_windowData.windowHandle = CreateWindow("D3DWindowClassName", m_windowData.mainWindowCaption.c_str(),
		WS_OVERLAPPEDWINDOW, offsetX, offsetY, width, height, 0, 0, m_windowData.instanceHandle, 0);

	if (!m_windowData.windowHandle) {
		MessageBox(0, "CreateWindow Failed.", 0, 0);

		return false;
	}

	ShowWindow(m_windowData.windowHandle, SW_SHOW);
	UpdateWindow(m_windowData.windowHandle);

	return true;
}

D3D & WindowUtil::GetD3D() {
	assert(m_ptrD3D != nullptr);

	return *m_ptrD3D;
}

void WindowUtil::SetD3D(D3D& _d3d) {
	// Make sure there is no D3D assigned already
	assert(m_ptrD3D == nullptr);	
	m_ptrD3D = &_d3d;
}

int WindowUtil::Run(void(*_ptrUpdate)(float), void(*_ptrRender)(float)) {
	MSG windowMsg = { 0 };

	// Ensure pointers are valid
	assert(_ptrUpdate && _ptrRender);

	// For delta timing purposes using CPU clock
	__int64 ticksPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&ticksPerSec);
	double secondsPerTick = 1.0 / (double)ticksPerSec;

	float deltaTime = 0.f;

	while (windowMsg.message != WM_QUIT) {
		// Check if there are any window messages to process
		if (PeekMessage(&windowMsg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&windowMsg);
			DispatchMessage(&windowMsg);
		}
		else {	// If not, do animation/game stuff
			if (!m_windowData.appPaused) {
				_ptrUpdate(deltaTime);
				_ptrRender(deltaTime);

				// Used to calculate delta time
				static __int64 t1 = 0;
				__int64 t2;

				QueryPerformanceCounter((LARGE_INTEGER*)&t2);

				if (t1 != 0) {
					deltaTime = (float)((t2 - t1) * secondsPerTick);
				}

				t1 = t2;
				AddSecToClock(deltaTime);	// Update delta time
			}
			else {
				Sleep(100);
			}
		}
	}

	return (int)windowMsg.wParam;
}

bool WindowUtil::BeginLoop(bool& _canUpdateRender) {
	MSG windowMsg = { 0 };
	_canUpdateRender = false;

	// Check if there are any window messages to process
	if (PeekMessage(&windowMsg, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&windowMsg);
		DispatchMessage(&windowMsg);

		return windowMsg.message != WM_QUIT;
	}
	
	_canUpdateRender = !m_windowData.appPaused;

	return true;
}

float WindowUtil::EndLoop(bool& _didUpdateRender) {
	if (!_didUpdateRender) {
		return 0.f;
	}

	float deltaTime = 0.f;

	if (!m_windowData.appPaused) {
		// For delta timing purposes using CPU clock
		__int64 ticksPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER*)&ticksPerSec);
		double secondsPerTick = 1.0 / (double)ticksPerSec;

		// Used to calculate delta time
		static __int64 t1 = 0;
		__int64 t2;

		QueryPerformanceCounter((LARGE_INTEGER*)&t2);

		if (t1 != 0) {
			deltaTime = (float)((t2 - t1) * secondsPerTick);
		}

		t1 = t2;
		AddSecToClock(deltaTime);	// Update delta time
	}
	else {
		Sleep(100);
	}

	return deltaTime;
}

HINSTANCE WindowUtil::GetAppInstance() {
	return m_windowData.instanceHandle;
}

HWND WindowUtil::GetMainWindow() {
	return m_windowData.windowHandle;
}

void WindowUtil::GetClientExtends(int& _width, int& _height) const {
	_width = m_windowData.clientWidth;
	_height = m_windowData.clientHeight;
}

int WindowUtil::GetClientWidth() const {
	return m_windowData.clientWidth;
}

int WindowUtil::GetClientHeight() const {
	return m_windowData.clientHeight;
}

void WindowUtil::ChooseResolution(int& _width, int& _height, int _defaults[], int _resPairsCount) {
	assert(_defaults && (_resPairsCount > 0));

	// Get system width and height
	int sysWidth = GetSystemMetrics(SM_CXSCREEN);
	int sysHeight = GetSystemMetrics(SM_CYSCREEN);

	// Reset resolution dimensions
	_width = _height = 0;

	for (int i(0); i < _resPairsCount; ++i) {
		// Pick resolution from array
		int newWidth = _defaults[i * 2], newHeight = _defaults[i * 2 + 1];

		if (_width < newWidth && newWidth < sysWidth && newHeight < sysHeight) {
			_width = newWidth;
			_height = newHeight;
		}
	}

	assert(_width > 0);
}

LRESULT WindowUtil::InstancedDefaultMsgHandler(HWND _winHandle, UINT _msg, WPARAM _wParam, LPARAM _lParam) {
	switch (_msg) {
		/* 
		WM_ACTIVATE is sent when the window is activated or deactivated.  
		Pause the game when the window is deactivated and unpause it 
		when it becomes active. 
		*/
	case WM_ACTIVATE:
		m_windowData.appPaused = (LOWORD(_wParam) == WA_INACTIVE);
		return 0;

		// User resized window
	case WM_SIZE:
		// Save new client dimensions
		m_windowData.clientWidth  = LOWORD(_lParam);
		m_windowData.clientHeight = HIWORD(_lParam);

		// Check if D3D is ok and Device is ready
		if (m_ptrD3D && m_ptrD3D->GetDeviceReady()) {
			switch (_wParam)
			{
			case SIZE_MINIMIZED:
				m_windowData.appPaused   = true;
				m_windowData.isMinimised = true;
				m_windowData.isMaximised = false;
				break;

			case SIZE_MAXIMIZED:
				m_windowData.appPaused   = false;
				m_windowData.isMinimised = false;
				m_windowData.isMaximised = true;

				m_ptrD3D->OnResize(m_windowData.clientWidth, m_windowData.clientHeight, *m_ptrD3D);
				break;

			case SIZE_RESTORED:
				/* 
				If user is dragging the resize bars, we do not resize 
				the buffers here because as the user continuously 
				drags the resize bars, a stream of WM_SIZE messages are
				sent to the window, and it would be pointless (and slow)
				to resize for each WM_SIZE message received from dragging
				the resize bars.  So instead, we reset after the user is 
				done resizing the window and releases the resize bars, which 
				sends a WM_EXITSIZEMOVE message. 
				*/
				if (!m_windowData.isResizing) {
					if (m_windowData.isMinimised) {
						m_windowData.appPaused   = false;
						m_windowData.isMinimised = false;
					}
					else if (m_windowData.isMaximised) {
						m_windowData.appPaused   = false;
						m_windowData.isMaximised = false;
					}

					// This is always done unless user is resizing
					m_ptrD3D->OnResize(m_windowData.clientWidth, m_windowData.clientHeight, *m_ptrD3D);
				}
				break;

			default: break;	// Other wParam...
			}
		}
		return 0;

		// User grabs the resize bars
	case WM_ENTERSIZEMOVE:
		m_windowData.appPaused  = true;
		m_windowData.isResizing = true;
		return 0;

		// User releases the resize bars. Reset everything based on new window dimensions
	case WM_EXITSIZEMOVE:
		m_windowData.appPaused  = false;
		m_windowData.isResizing = false;

		if (m_ptrD3D) {
			m_ptrD3D->OnResize(m_windowData.clientWidth, m_windowData.clientHeight, *m_ptrD3D);
		}
		return 0;

		// Sent when the window is being destroyed
	case WM_DESTROY: 
		PostQuitMessage(0);
		return 0;

		/* Sent when a menu is active and the user presses a key 
		that does not correspond to any mnemonic or accelerator key. */
	case WM_MENUCHAR:
		// Don't beep on ALT+ENTER
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)_lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)_lParam)->ptMinTrackSize.y = 200;
		return 0;

		// Other message...
	default: break;
	}

	return DefWindowProc(_winHandle, _msg, _wParam, _lParam);
}