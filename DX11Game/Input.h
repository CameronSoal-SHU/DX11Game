#pragma once
#define WIN32_LEAN_AND_MEAN
#define MOUSE_CONFINED true
#define MOUSE_VISIBLE true
#define MOUSE_FREE false
#define MOUSE_INVISIBLE false

#include "D3DUtil.h"

#include <Xinput.h>


class Input
{
public:
	Input();
	void Init(HWND _winHandle, bool _showMouse = MOUSE_VISIBLE, bool _confineMouse = MOUSE_FREE);

	// Zero everything and reset
	void Reset();

	std::wstring GetPressedKeys() const;
	unsigned short* GetKeyBuffer();
	bool IsPressed(const unsigned short vKeyCode) const;

	// Returns a text list of all mouse buttons and cursor positioning information
	std::wstring GetMouseInfo();
	// Check if a button is currently down
	typedef enum { LMB = 0, MMB = 1, RMB = 2 } MouseButton;
	bool GetMouseDown(const MouseButton& _refMButton) const;
	// Get absolute or relative mouse position
	// (Absolute = 2D windows co-ords of cursor, Relative = 2D windows co-ords change in position since last update)
	DirectX::SimpleMath::Vector2 GetMousePos(bool _absolute) const;
	// Get mouse position adjusted to games scale (Resolution : Screen size)
	DirectX::SimpleMath::Vector2 GetMousePosScaled(bool _absolute) const;
	// Call this after game has finished using the mouse, probably at end of render
	void PostProcess();
	void OnLost();
	void OnReset();
	void MessageEvent(HRAWINPUT _rawInput);
	void RestrictMouse();
private:
	enum {
		RAWBUFF_SIZE = 512, KEYBUFF_SIZE = 255, MAX_MBUTTONS = 3
	};

	HWND m_windowHandle;

	unsigned char m_inputBuffer[RAWBUFF_SIZE];
	unsigned short m_keyBuffer[KEYBUFF_SIZE];
	//unsigned short m_keyUpBuffer[KEYBUFF_SIZE];

	bool m_mouseButtons[MAX_MBUTTONS];
	DirectX::SimpleMath::Vector2 m_mouseScrPos;		// Mouse position on screen
	DirectX::SimpleMath::Vector2 m_mouseDeltaPos;	// Change in relative mouse position each frame
	RECT m_newCursorClip;							// New area for ClipCursor
	RECT m_oldCursorClip;							// Previous area for ClipCursor
	bool m_confineMouse;

	void RegisterForRawInput();
	void ProcessKeys(RAWINPUT* _ptrRawInput);
	void ProcessMouse(RAWINPUT* _ptrRawInput);
	void GetMousePosAbs(DirectX::SimpleMath::Vector2& _refPos) const;
};

