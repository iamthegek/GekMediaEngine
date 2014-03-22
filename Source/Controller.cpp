#include "Controller.h"
#include "SFML\Window.hpp"

JoystickController::JoystickController(const int& id)
{
	joystickId = id;
	for(int i = 0; i < MAX_BUTTONS_JOYSTICK; i++)
	{
		buttonsDown[i] = buttonsDownThisFrame[i] = buttonsUpThisFrame[i] = false;
	}
	for(int i = 0; i < MAX_AXES_JOYSTICK; i++)
	{
		axes[i] = 0.0f;
	}
}
JoystickController::~JoystickController()
{
}

int JoystickController::GetJoystickId() const
{
	return joystickId;
}
float JoystickController::GetJoystickPosition(const int& axis) const
{
	return axes[axis];
}
bool JoystickController::IsButtonDown(const int& button) const
{
	return buttonsDown[button];
}
bool JoystickController::IsButtonDownThisFrame(const int& button) const
{
	return buttonsDownThisFrame[button];
}
bool JoystickController::IsButtonUpThisFrame(const int& button) const
{
	return buttonsUpThisFrame[button];
}

void JoystickController::PressButton(const int& button)
{
	buttonsDown[button] = true;
	buttonsDownThisFrame[button] = true;
}
void JoystickController::ReleaseButton(const int& button)
{
	buttonsDown[button] = false;
	buttonsUpThisFrame[button] = true;
}
void JoystickController::PlaceAxis(const float& pos, const int& axis)
{
	axes[axis] = pos;
}

void JoystickController::Update()
{
	for(int i = 0; i < MAX_BUTTONS_JOYSTICK; i++)
	{
		buttonsDownThisFrame[i] = buttonsUpThisFrame[i] = false;
	}
}

PcController::PcController()
{
	for(int i = 0; i < MAX_KEYS; i++)
	{
		keysDown[i] = keysDownThisFrame[i] = keysUpThisFrame[i] = false;
	}
	mouseLeftDown = mouseRightDown = mouseLeftDownThisFrame = mouseLeftUpThisFrame = mouseRightDownThisFrame = mouseRightUpThisFrame = false;
}
PcController::~PcController()
{
}

bool PcController::IsKeyDown(const int& button) const
{
	return keysDown[button];
}
bool PcController::IsKeyDownThisFrame(const int& button) const
{
	return keysDownThisFrame[button];
}
bool PcController::IsKeyUpThisFrame(const int& button) const
{
	return keysUpThisFrame[button];
}
void PcController::PressKey(const int& key)
{
	keysDown[key] = true;
	keysDownThisFrame[key] = true;
}
void PcController::ReleaseKey(const int& key)
{
	keysDown[key] = false;
	keysUpThisFrame[key] = true;
}
void PcController::PressMouse(const int& button)
{
	if(button == sf::Mouse::Button::Left)
	{
		mouseLeftDown = true;
		mouseLeftDownThisFrame = true;
		return;
	}
	mouseRightDown = true;
	mouseRightDownThisFrame = true;
}
void PcController::ReleaseMouse(const int& button)
{
	if(button == sf::Mouse::Button::Left)
	{
		mouseLeftDown = false;
		mouseLeftUpThisFrame = true;
		return;
	}
	mouseRightDown = false;
	mouseRightUpThisFrame = true;
}
void PcController::MouseMove(const int& x, const int& y)
{
	mousePositionOld = mousePosition;
	mousePosition.x = x;
	mousePosition.y = y;
}

void PcController::Update()
{
	for(int i = 0; i < MAX_KEYS; i++)
	{
		keysDownThisFrame[i] = keysUpThisFrame[i] = false;
	}
	mouseLeftDownThisFrame = mouseLeftUpThisFrame = mouseRightDownThisFrame = mouseRightUpThisFrame = false;
}