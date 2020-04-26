#pragma once
#define WIN32_LEAN_AND_MEAN

#include "D3DUtil.h"

#include <Xinput.h>

/*
Handle inputs from a maximum of 4 gamepads
*/
class GamePadInput
{
public:
	// Holds info on the controller
	struct ControllerData {
		int port = -1;	// (-1 = not connected to a port)
		// Stick movement
		DirectX::SimpleMath::Vector2 leftStick { 0, 0 };
		DirectX::SimpleMath::Vector2 rightStick{ 0, 0 };
		// Stick deadzones to prevent bad inputs from loose controllers
		DirectX::SimpleMath::Vector2 stickDeadZone{ 0.075f, 0.075f };

		// Left and right trigger "pressed" amount
		float leftTrigger = 0.f;
		float rightTrigger = 0.f;

		XINPUT_STATE inputState;
	};

	// Assigns all controllers with a port of -1
	void Init();
	// Check for any controller inputs (analogue stick movement, trigger movement, etc.) every frame
	void Update();

	// Adjust the analogue stick deadzone if necessary
	void SetDeadZone(int _idx, float _xAxis, float _yAxis);
	void SetDeadZone(int _idx, const DirectX::SimpleMath::Vector2& _axes);

	const ControllerData& GetGamePadData(int _idx) const;

	// Is a controller connected to a given port?
	bool IsConnected(int _idx);
	// Is a controller from a given port hitting a given button?
	bool IsPressed(int _idx, unsigned short _buttonId);
	// Is a controller from a given port in use?
	bool IsEnabled(int _idx);

	// Get a list of pad activity - Motion of sticks and triggers
	std::wstring GetAnalogueNames(int _idx);
	void GetAnalogueNames(int _idx, std::wstring& _msg);
	// Get a list of pad activity - Button presses
	void GetDigitalNames(int _idx, std::wstring& _msg);
private:
	ControllerData m_gamePads[XUSER_MAX_COUNT];
	bool m_controllerEnabled[XUSER_MAX_COUNT];
};

