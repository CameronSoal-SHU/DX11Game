#include "Input.h"
#include "WindowUtil.h"
#include "GameConstants.h"
#include "GameSettings.h"

Input::Input() {
	Reset();
}

void Input::Init(HWND _winHandle, bool _showMouse, bool _confineMouse) {
	m_windowHandle = _winHandle;
	RegisterForRawInput();
	ShowCursor(_showMouse);
	m_confineMouse = _confineMouse;

	if (m_confineMouse) {
		GetWindowRect(m_windowHandle, &m_newCursorClip);
		ClipCursor(&m_newCursorClip);
	}

	GetMousePosAbs(m_mouseScrPos);
}

void Input::Reset() {
	ZeroMemory(m_inputBuffer, sizeof(m_inputBuffer));
	ZeroMemory(m_keyBuffer, sizeof(m_keyBuffer));

	for (bool& mb : m_mouseButtons) {
		mb = false;
	}

	m_mouseScrPos = m_mouseDeltaPos = DirectX::SimpleMath::Vector2(0, 0);
}

std::wstring Input::GetPressedKeys() const {
	std::wstring pressedKeys = L"";

	for (int i(0); i < KEYBUFF_SIZE; ++i) {
		if (m_keyBuffer[i] == 0)
			continue;

		switch (i) {
		case VK_RIGHT:
			pressedKeys += L"RIGHT ARROW ";
			break;
		case VK_LEFT:
			pressedKeys += L"LEFT ARROW ";
			break;
		case VK_UP:
			pressedKeys += L"UP ARROW ";
			break;
		case VK_DOWN:
			pressedKeys += L"DOWN ARROW ";
			break;
		default:
			wchar_t key[KEYBUFF_SIZE + 1];

			if (GetKeyNameTextW(m_keyBuffer[i] << 16, key, sizeof(key))) {
				pressedKeys += key;
				pressedKeys += L" ";
			}
		}
	}

	return pressedKeys;
}

unsigned short* Input::GetKeyBuffer() {
	return m_keyBuffer;
}

bool Input::IsPressed(const unsigned short vKeyCode) const {
	assert(vKeyCode < KEYBUFF_SIZE);
	
	// Is that keys pressed down value true?
	return m_keyBuffer[vKeyCode] != 0;
}

std::wstring Input::GetMouseInfo() {
	std::wstring mouseInfo = L"";
	static float oldMousePosX(0.f), oldMousePosY(0.f);

	if (m_mouseDeltaPos.x != 0.f)
		oldMousePosX = m_mouseDeltaPos.x;
	if (m_mouseDeltaPos.y != 0.f)
		oldMousePosY = m_mouseDeltaPos.y;

	std::wstringstream temp;
	temp.precision(0);

	temp << L"x=" << m_mouseScrPos.x << L" y=" << m_mouseScrPos.y;
	temp << L" deltaX=" << oldMousePosX << " deltaY=" << oldMousePosY;

	if (m_mouseButtons[LMB])
		temp << L" LButton";
	if (m_mouseButtons[MMB])
		temp << L" MButton";
	if (m_mouseButtons[RMB])
		temp << L" RButton";

	mouseInfo = temp.str();

	return mouseInfo;
}

bool Input::GetMouseDown(const MouseButton& _refMButton) const {
	return m_mouseButtons[_refMButton];
}

DirectX::SimpleMath::Vector2 Input::GetMousePos(bool _absolute) const {
	return _absolute ? m_mouseScrPos : m_mouseDeltaPos;
}

DirectX::SimpleMath::Vector2 Input::GetMousePosScaled(bool _absolute) const
{
	int screenWidth, screenHeight;

	// Get ratio for (resolution : window size) scale to correct mouse position reading
	WindowUtil::Get().GetClientExtends(screenWidth, screenHeight);
	DirectX::SimpleMath::Vector2 screenScale = Settings::GAME_RES / 
		DirectX::SimpleMath::Vector2((float)screenWidth, (float)screenHeight);

	return GetMousePos(_absolute) * screenScale;
}

void Input::PostProcess() {
	m_mouseDeltaPos = { 0, 0 };
}

void Input::OnLost() {
	Reset();
	ClipCursor(&m_oldCursorClip);
}

void Input::OnReset() {
	Reset();
	ClipCursor(&m_newCursorClip);
}

void Input::MessageEvent(HRAWINPUT _rawInput) {
	UINT dWordSize;
	UINT rawInputData = GetRawInputData(_rawInput, RID_INPUT, NULL, &dWordSize, sizeof(RAWINPUTHEADER));

	if (rawInputData != 0)	// Ingore bad inputs
		return;

	if (dWordSize >= RAWBUFF_SIZE) {
		DBOUT("Buffer too small. Is " << RAWBUFF_SIZE << " wants " << dWordSize);
		assert(false);
	}

	if (GetRawInputData(_rawInput, RID_INPUT, m_inputBuffer, &dWordSize, sizeof(RAWINPUTHEADER)) != dWordSize)
	{
		DBOUT("GetRawInputData failed");
		assert(false);
	}

	RAWINPUT* ptrRawInput = (RAWINPUT*)m_inputBuffer;

	switch (ptrRawInput->header.dwType)
	{
	case RIM_TYPEKEYBOARD:
		ProcessKeys(ptrRawInput);
		break;
	case RIM_TYPEMOUSE:
		ProcessMouse(ptrRawInput);
		break;
	default: break;
	}
}

void Input::RegisterForRawInput() {
	/*
	0 | Mouse device
	1 | Keyboard device
	*/
	RAWINPUTDEVICE rawInputDevs[2];	// Mouse and Keyboard devices

	rawInputDevs[0].usUsage = 0x02;	// Trigger mouse input ID
	rawInputDevs[1].usUsage = 0x06; // Trigger keyboard input ID
	for (RAWINPUTDEVICE& rid : rawInputDevs) {
		rid.usUsagePage = 0x01;
		rid.dwFlags		= RIDEV_INPUTSINK; // RIDEV_NOLEGACY;
		rid.hwndTarget	= m_windowHandle;
	}

	// Register both devices
	if (RegisterRawInputDevices(rawInputDevs, 2, sizeof(RAWINPUTDEVICE)) == FALSE) {
		MessageBox(0, "Cannot get keyboard and mouse input", 0, 0);
		assert(false);
	}
}

void Input::ProcessKeys(RAWINPUT* _ptrRawInput) {
	// Standard KeyCode
	unsigned short flags = _ptrRawInput->data.keyboard.Flags;
	unsigned short vKey = _ptrRawInput->data.keyboard.VKey;
	unsigned short scanCode = _ptrRawInput->data.keyboard.MakeCode;

	// Ignore bad inputs
	if (vKey >= KEYBUFF_SIZE)
		return;

	switch (vKey) {
	case VK_SHIFT:		// Correct LSHIFT or RSHIFT
		vKey = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);
		break;
	case VK_NUMLOCK:	// Correct PAUSE/BREAK and NUM LOCK stuff
		scanCode = (MapVirtualKey(vKey, MAPVK_VK_TO_VSC) | 0x100);
		break;
	}

	// E0 and E1 are escape sequences used for certain special keys, such as PRINT and PAUSE/BREAK.
	const bool isE0 = ((flags & RI_KEY_E0) != 0);
	const bool isE1 = ((flags & RI_KEY_E1) != 0);

	if (isE1) {
		if (vKey == VK_PAUSE)
			scanCode = 0x45;
		else
			scanCode = MapVirtualKey(vKey, MAPVK_VK_TO_VSC);
	}

	switch (vKey)
	{
	case VK_CONTROL:	// RCTRL and ALT have E0 bit set
		vKey = isE0 ? VK_RCONTROL : VK_LCONTROL;
		break;
	case VK_MENU:
		vKey = isE0 ? GameConsts::KEY_RALT : GameConsts::KEY_LALT;	// Right Alt : Left Alt
		break;
	case VK_RETURN:
		if (isE0)
			vKey = GameConsts::KEY_NUMPAD_ENTER;
		break;
	default: break;
	}

	m_keyBuffer[vKey] = flags & RI_KEY_BREAK ? 0 : scanCode;
}

void Input::ProcessMouse(RAWINPUT* _ptrRawInput) {
	unsigned short flags = _ptrRawInput->data.mouse.usButtonFlags;

	if (flags & RI_MOUSE_LEFT_BUTTON_DOWN)
		m_mouseButtons[LMB] = true;
	if (flags & RI_MOUSE_LEFT_BUTTON_UP)
		m_mouseButtons[LMB] = false;
	if (flags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
		m_mouseButtons[MMB] = true;
	if (flags & RI_MOUSE_MIDDLE_BUTTON_UP)
		m_mouseButtons[MMB] = false;
	if (flags & RI_MOUSE_RIGHT_BUTTON_DOWN)
		m_mouseButtons[RMB] = true;
	if (flags & RI_MOUSE_RIGHT_BUTTON_UP)
		m_mouseButtons[RMB] = false;

	DirectX::SimpleMath::Vector2 lastMousePos(m_mouseScrPos);
	GetMousePosAbs(m_mouseScrPos);
	m_mouseDeltaPos = m_mouseScrPos - lastMousePos;
}

void Input::GetMousePosAbs(DirectX::SimpleMath::Vector2& _refPos) const {
	POINT mPos;

	if (GetCursorPos(&mPos) && ScreenToClient(m_windowHandle, &mPos)) {
		_refPos = { (float)mPos.x, (float)mPos.y };
	}
}
