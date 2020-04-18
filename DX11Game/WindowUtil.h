#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <sstream>
#include <assert.h>

#include "Singleton.h"

class D3D;

class WindowUtil : public Singleton<WindowUtil>
{
public:
	static LRESULT DefaultMsgHandler(HWND _winHandle, UINT _msg, WPARAM _wParam, LPARAM _lParam) {
		return WindowUtil::Get().InstancedDefaultMsgHandler(_winHandle, _msg, _wParam, _lParam);
	}
	bool InitMainWindow(int _width, int _height, HINSTANCE _instanceHandle, const std::string& _appName, WNDPROC _msgHandler, bool _centred = true);
	D3D& GetD3D();
	void SetD3D(D3D& _d3d);
	// Wrap game in 1 function call if using basic functions
	int Run(void(*_ptrUpdate)(float), void(*_ptrRender)(float));
	// If using class member functions, call these instead
	bool BeginLoop(bool& _canUpdateRender);
	float EndLoop(bool& _didUpdateRender);

	//void OnResize_Internal();
	HINSTANCE GetAppInstance();
	HWND GetMainWindow();

	void GetClientExtends(int& _width, int& _height) const;
	int GetClientWidth() const;
	int GetClientHeight() const;
	void ChooseResolution(int& _width, int& _height, int _defaults[], int _resPairsCount);

private:
	// Window application handles
	struct WinData {
		HINSTANCE instanceHandle = 0;
		HWND	  windowHandle   = 0;
		bool	  appPaused		 = false;
		bool	  isMinimised	 = false;
		bool	  isMaximised	 = false;
		bool	  isResizing	 = false;
		std::string mainWindowCaption;
		int clientWidth;
		int clientHeight;
	};

	WinData m_windowData;
	D3D* m_ptrD3D = nullptr;

	LRESULT InstancedDefaultMsgHandler(HWND _winHandle, UINT _msg, WPARAM _wParam, LPARAM _lParam);
};

