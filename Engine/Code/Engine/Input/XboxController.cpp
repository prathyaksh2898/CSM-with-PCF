#include "XboxController.hpp"
#include "Engine/Math/MathUtils.hpp"
#define WIN32_LEAN_AND_MEAN	
#include <windows.h>
#include <Xinput.h>
#pragma comment( lib, "xinput9_1_0" ) 

constexpr short JOYSTICK_START_VALUE = -32768;
constexpr short JOYSTICK_END_VALUE = 32767;
constexpr float NORMALIZED_INNERDEADZONE = 0.3f;
constexpr float NORMALIZED_OUTERDEADZONE = 0.95f;
constexpr unsigned char TRIGGER_LOWERBOUND = 30;
constexpr unsigned char TRIGGER_UPPERBOUND = 250;

XboxController::XboxController() {
	m_leftStick.SetDeadZoneThreshholds(NORMALIZED_INNERDEADZONE, NORMALIZED_OUTERDEADZONE);
	m_rightStick.SetDeadZoneThreshholds(NORMALIZED_INNERDEADZONE, NORMALIZED_OUTERDEADZONE);
}

XboxController::~XboxController() {

}

bool XboxController::IsConnected() const {
	return m_isConnected;
}

int XboxController::GetControllerID() const {
	return m_id;
}

AnalogJoystick const& XboxController::GetLeftStick() const {
	return m_leftStick;
}

AnalogJoystick const& XboxController::GetRightStick() const {
	return m_rightStick;
}

float XboxController::GetLeftTrigger() const {
	return m_leftTrigger;
}

float XboxController::GetRightTrigger() const {
	return m_rightTrigger;
}

KeyButtonState const& XboxController::GetButton(XboxButtonID buttonID) const {
	return m_buttons[buttonID];
}

bool XboxController::IsButtonDown(XboxButtonID buttonID) const {
	return m_buttons[buttonID].m_isPressed;
}

bool XboxController::wasButtonJustPressed(XboxButtonID buttonID) const {
	return (m_buttons[buttonID].m_isPressed && !m_buttons[buttonID].m_wasPressedLastFrame);
}

bool XboxController::wasButtonJustReleased(XboxButtonID buttonID) const {
	return (!m_buttons[buttonID].m_isPressed && m_buttons[buttonID].m_wasPressedLastFrame);
}

void XboxController::Reset() {
	m_leftTrigger = 0.0f;
	m_rightTrigger = 0.0f;
	for (int buttonNum = 0; buttonNum < NUM_XBOX_BUTTONS; ++buttonNum) {
		m_buttons[buttonNum].m_isPressed = false;
		m_buttons[buttonNum].m_wasPressedLastFrame = false;
	}
	m_leftStick.Reset();
	m_rightStick.Reset();
}

void XboxController:: Update() {
	
	XINPUT_STATE xboxControllerState;	
	memset(&xboxControllerState, 0, sizeof(xboxControllerState));
	DWORD errorStatus = XInputGetState(m_id, &xboxControllerState);
	if (errorStatus != ERROR_SUCCESS) {
		Reset();
		m_isConnected = false;
		return;
	}
	m_isConnected = true;

	XINPUT_GAMEPAD const& state = xboxControllerState.Gamepad;

	UpdateJoyStick(m_leftStick, state.sThumbLX, state.sThumbLY);
	UpdateJoyStick(m_rightStick, state.sThumbRX, state.sThumbRY);

	UpdateTrigger(m_leftTrigger, state.bLeftTrigger);
	UpdateTrigger(m_rightTrigger, state.bRightTrigger);

	UpdateButton(XBOX_BUTTON_A, state.wButtons, XINPUT_GAMEPAD_A);
	UpdateButton(XBOX_BUTTON_B, state.wButtons, XINPUT_GAMEPAD_B);
	UpdateButton(XBOX_BUTTON_X, state.wButtons, XINPUT_GAMEPAD_X);
	UpdateButton(XBOX_BUTTON_Y, state.wButtons, XINPUT_GAMEPAD_Y);
	UpdateButton(XBOX_BUTTON_BACK, state.wButtons, XINPUT_GAMEPAD_BACK);
	UpdateButton(XBOX_BUTTON_START, state.wButtons, XINPUT_GAMEPAD_START);
	UpdateButton(XBOX_BUTTON_LSHOULDER, state.wButtons, XINPUT_GAMEPAD_LEFT_SHOULDER);
	UpdateButton(XBOX_BUTTON_RSHOULDER, state.wButtons, XINPUT_GAMEPAD_RIGHT_SHOULDER);
	UpdateButton(XBOX_BUTTON_LTHUMB, state.wButtons, XINPUT_GAMEPAD_LEFT_THUMB);
	UpdateButton(XBOX_BUTTON_RTHUMB, state.wButtons, XINPUT_GAMEPAD_RIGHT_THUMB);
	UpdateButton(XBOX_BUTTON_DPAD_LEFT, state.wButtons, XINPUT_GAMEPAD_DPAD_LEFT);
	UpdateButton(XBOX_BUTTON_DPAD_DOWN, state.wButtons, XINPUT_GAMEPAD_DPAD_DOWN);
	UpdateButton(XBOX_BUTTON_DPAD_RIGHT, state.wButtons, XINPUT_GAMEPAD_DPAD_RIGHT);
	UpdateButton(XBOX_BUTTON_DPAD_UP, state.wButtons, XINPUT_GAMEPAD_DPAD_UP);
}

void XboxController::UpdateJoyStick(AnalogJoystick& out_joystick, short rawX, short rawY) {
	float normalizationBegin = -1.0f;
	float normalizationEnd = 1.0f;
	float rawNormalizedX = RangeMap(rawX, JOYSTICK_START_VALUE, JOYSTICK_END_VALUE, normalizationBegin, normalizationEnd);
	float rawNormalizedY = RangeMap(rawY, JOYSTICK_START_VALUE, JOYSTICK_END_VALUE, normalizationBegin, normalizationEnd);
	
	out_joystick.UpdatePosition(rawNormalizedX, rawNormalizedY);
}

void XboxController::UpdateTrigger(float& out_triggerValue, unsigned char rawValue) {
	out_triggerValue = RangeMapClamped(static_cast<float>(rawValue), TRIGGER_LOWERBOUND, TRIGGER_UPPERBOUND, 0.f, 1.f);
}

void XboxController::UpdateButton(XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag) {
	KeyButtonState& button = m_buttons[buttonID];
	button.m_wasPressedLastFrame = button.m_isPressed;
	button.m_isPressed = (buttonFlags & buttonFlag) == buttonFlag;
}