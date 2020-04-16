#include "GamePadInput.h"

void GamePadInput::Init() {
	for (ControllerData& gamePad : m_gamePads) {
		gamePad.port = -1;
	}
}

void GamePadInput::Update() {
	const unsigned magicNumber = 32767;

	// Go through each connected gamepad
	for (byte i(0); i < XUSER_MAX_COUNT; ++i) {
		ControllerData& controller = m_gamePads[i];
		controller.port = -1;

		// Cleanup
		ZeroMemory(&controller.inputState, sizeof(XINPUT_STATE));

		if (XInputGetState(i, &controller.inputState) == ERROR_SUCCESS) {
			// Normalised direction in left and right analogue sticks (how much the thumbstick is being pushed)
			const float normLX = fmaxf(-1, (float)controller.inputState.Gamepad.sThumbLX / magicNumber);
			const float normLY = fmaxf(-1, (float)controller.inputState.Gamepad.sThumbLY / magicNumber);
			const float normRX = fmaxf(-1, (float)controller.inputState.Gamepad.sThumbRX / magicNumber);
			const float normRY = fmaxf(-1, (float)controller.inputState.Gamepad.sThumbRY / magicNumber);

			// Store stick movement amount
			controller.leftStick.x  = (abs(normLX) < controller.stickDeadZone.x ? 0 : (abs(normLX) - controller.stickDeadZone.x) * (normLX / abs(normLX)));
			controller.leftStick.y  = (abs(normLY) < controller.stickDeadZone.y ? 0 : (abs(normLY) - controller.stickDeadZone.y) * (normLY / abs(normLY)));
			controller.rightStick.x = (abs(normRX) < controller.stickDeadZone.x ? 0 : (abs(normRX) - controller.stickDeadZone.x) * (normRX / abs(normRX)));
			controller.rightStick.y = (abs(normRY) < controller.stickDeadZone.y ? 0 : (abs(normRY) - controller.stickDeadZone.y) * (normRY / abs(normRY)));

			// Dead-zoning
			if (controller.stickDeadZone.x > 0.f) {
				controller.leftStick.x  *= 1 / (1 - controller.stickDeadZone.x);
				controller.rightStick.x *= 1 / (1 - controller.stickDeadZone.x);
			}
			if (controller.stickDeadZone.y > 0.f) {
				controller.leftStick.y  *= 1 / (1 - controller.stickDeadZone.y);
				controller.rightStick.y *= 1 / (1 - controller.stickDeadZone.y);
			}

			// Gamepad triggers
			controller.leftTrigger  = (float)controller.inputState.Gamepad.bLeftTrigger  / 255;
			controller.rightTrigger = (float)controller.inputState.Gamepad.bRightTrigger / 255;

			controller.port = i;
		}
	}
}

void GamePadInput::SetDeadZone(int _idx, float _xAxis, float _yAxis) {
	assert(_idx >= 0 && _idx < XUSER_MAX_COUNT);

	m_gamePads[_idx].stickDeadZone = { _xAxis, _yAxis };
}

void GamePadInput::SetDeadZone(int _idx, const DirectX::SimpleMath::Vector2 & _axes) {
	assert(_idx >= 0 && _idx < XUSER_MAX_COUNT);

	m_gamePads[_idx].stickDeadZone = _axes;
}

const GamePadInput::ControllerData& GamePadInput::GetGamePadData(int _idx) const {
	assert(_idx >= 0 && _idx < XUSER_MAX_COUNT);

	return m_gamePads[_idx];
}

bool GamePadInput::IsConnected(int _idx) {
	assert(_idx >= 0 && _idx < XUSER_MAX_COUNT);

	return m_gamePads[_idx].port != -1;
}

bool GamePadInput::IsPressed(int _idx, unsigned short _buttonId) {
	assert(_idx >= 0 && _idx < XUSER_MAX_COUNT);
	assert(m_gamePads[_idx].port != -1);

	return (m_gamePads[_idx].inputState.Gamepad.wButtons & _buttonId) != 0;
}

bool GamePadInput::IsEnabled(int _idx) {
	assert(_idx >= 0 && _idx < XUSER_MAX_COUNT);

	// Check if any of the analogue sticks are being used
	m_controllerEnabled[_idx] = GetGamePadData(_idx).leftStick != DirectX::SimpleMath::Vector2::Zero ||
		GetGamePadData(_idx).rightStick != DirectX::SimpleMath::Vector2::Zero;

	//DBOUT("Controller Enabled: " << m_controllerEnabled[_idx]);
	return m_controllerEnabled[_idx];
}

std::wstring GamePadInput::GetAnalogueNames(int _idx) {
	std::wstring str;
	GetAnalogueNames(_idx, str);
	return str;
}

void GamePadInput::GetAnalogueNames(int _idx, std::wstring& _msg) {
	assert(_idx >= 0 && _idx < XUSER_MAX_COUNT);

	ControllerData& pad = m_gamePads[_idx];
	_msg = L"";

	if (pad.port == -1)
		return;

	std::wstringstream str;
	str.precision(2);

	str << L"Left stick(x=" << pad.leftStick.x << L",y=" << pad.leftStick.y << L")";
	str << L" Right stick(x=" << pad.rightStick.x << L",y=" << pad.rightStick.y << L")";
	str << L" Trigger Left=" << pad.leftTrigger << L", right=" << pad.rightTrigger;
	_msg = str.str();
}

void GamePadInput::GetDigitalNames(int _idx, std::wstring& _msg) {
	assert(_idx >= 0 && _idx < XUSER_MAX_COUNT);

	ControllerData& controller = m_gamePads[_idx];
	_msg = L"";

	if (controller.port == -1)	// Ignore bad controllers
		return;

	std::wstringstream str;
	if (IsPressed(_idx, XINPUT_GAMEPAD_DPAD_UP))
		str << L"DPAD_UP ";
	if (IsPressed(_idx, XINPUT_GAMEPAD_DPAD_DOWN))
		str << L"DPAD DOWN ";
	if (IsPressed(_idx, XINPUT_GAMEPAD_DPAD_LEFT))
		str << L"DPAD LEFT ";
	if (IsPressed(_idx, XINPUT_GAMEPAD_DPAD_RIGHT))
		str << L"DPAD RIGHT ";
	if (IsPressed(_idx, XINPUT_GAMEPAD_START))
		str << L"START ";
	if (IsPressed(_idx, XINPUT_GAMEPAD_BACK))
		str << L"BACK ";
	if (IsPressed(_idx, XINPUT_GAMEPAD_LEFT_THUMB))
		str << L"LEFT THUMB ";
	if (IsPressed(_idx, XINPUT_GAMEPAD_RIGHT_THUMB))
		str << L"RIGHT THUMB ";
	if (IsPressed(_idx, XINPUT_GAMEPAD_LEFT_SHOULDER))
		str << L"LEFT SHOULDER ";
	if (IsPressed(_idx, XINPUT_GAMEPAD_RIGHT_SHOULDER))
		str << L"RIGHT SHOULDER ";
	if (IsPressed(_idx, XINPUT_GAMEPAD_A))
		str << L"A ";
	if (IsPressed(_idx, XINPUT_GAMEPAD_B))
		str << L"B ";
	if (IsPressed(_idx, XINPUT_GAMEPAD_X))
		str << L"X ";
	if (IsPressed(_idx, XINPUT_GAMEPAD_Y))
		str << L"Y ";

	_msg = str.str();
}
