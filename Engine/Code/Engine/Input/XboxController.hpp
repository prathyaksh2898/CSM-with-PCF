#pragma once
#include "AnalogJoystick.hpp"
#include "KeyButtonState.hpp"

enum XboxButtonID {
	XBOX_BUTTON_INVALID = -1,

	XBOX_BUTTON_A,
	XBOX_BUTTON_B,
	XBOX_BUTTON_X,
	XBOX_BUTTON_Y,
	XBOX_BUTTON_DPAD_LEFT,
	XBOX_BUTTON_DPAD_DOWN,
	XBOX_BUTTON_DPAD_RIGHT,
	XBOX_BUTTON_DPAD_UP,
	XBOX_BUTTON_LTHUMB,
	XBOX_BUTTON_LSHOULDER,
	XBOX_BUTTON_RTHUMB,
	XBOX_BUTTON_RSHOULDER,
	XBOX_BUTTON_BACK,
	XBOX_BUTTON_START,
	
	NUM_XBOX_BUTTONS
};


class XboxController {
	friend class InputSystem;

public:
	XboxController();
	~XboxController();
	bool IsConnected() const;
	int GetControllerID() const;
	AnalogJoystick const& GetLeftStick() const;
	AnalogJoystick const& GetRightStick() const;
	float GetLeftTrigger() const;
	float GetRightTrigger() const;
	KeyButtonState const& GetButton(XboxButtonID buttonID) const;
	bool IsButtonDown(XboxButtonID buttonID) const;
	bool wasButtonJustPressed(XboxButtonID buttonID) const;
	bool wasButtonJustReleased(XboxButtonID buttonID) const;

private:
	void Update();
	void Reset();
	void UpdateJoyStick(AnalogJoystick& out_joystick, short rawX, short rawY);
	void UpdateTrigger(float& out_triggerValue, unsigned char rawValue);
	void UpdateButton(XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag);

private:
	int m_id = -1;
	bool m_isConnected = false;
	float m_leftTrigger = 0.0f;
	float m_rightTrigger = 0.0f;
	KeyButtonState m_buttons[(int)XboxButtonID::NUM_XBOX_BUTTONS];
	AnalogJoystick m_leftStick;
	AnalogJoystick m_rightStick;
};