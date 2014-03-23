#include "Input.h"
#include "SFML\Window.hpp"

JoystickTracker::JoystickTracker()
{
	for(int i = 0; i < MAX_BUTTONS_JOYSTICK; i++)
	{
		buttonsDown[i] = buttonsDownThisFrame[i] = buttonsUpThisFrame[i] = false;
	}
	for(int i = 0; i < MAX_AXES_JOYSTICK; i++)
	{
		axes[i] = 0.0f;
	}
}
JoystickTracker::~JoystickTracker()
{
}

float JoystickTracker::GetJoystickPosition(const int& axis) const
{
	return axes[axis];
}

bool JoystickTracker::IsButtonDown(const int& button) const
{
	return buttonsDown[button];
}

bool JoystickTracker::IsButtonDownThisFrame(const int& button) const
{
	return buttonsDownThisFrame[button];
}

bool JoystickTracker::IsButtonUpThisFrame(const int& button) const
{
	return buttonsUpThisFrame[button];
}

void JoystickTracker::PressButton(const int& button)
{
	buttonsDown[button] = true;
	buttonsDownThisFrame[button] = true;
}

void JoystickTracker::ReleaseButton(const int& button)
{
	buttonsDown[button] = false;
	buttonsUpThisFrame[button] = true;
}

void JoystickTracker::PlaceAxis(const float& pos, const int& axis)
{
	axes[axis] = pos;
}

void JoystickTracker::Update()
{
	for(int i = 0; i < MAX_BUTTONS_JOYSTICK; i++)
	{
		buttonsDownThisFrame[i] = buttonsUpThisFrame[i] = false;
	}
}

KeyboardMouseTracker::KeyboardMouseTracker()
{
	for(int i = 0; i < MAX_KEYS; i++)
	{
		keysDown[i] = keysDownThisFrame[i] = keysUpThisFrame[i] = false;
	}
	mouseLeftDown = mouseRightDown = mouseLeftDownThisFrame = mouseLeftUpThisFrame = mouseRightDownThisFrame = mouseRightUpThisFrame = false;
}

KeyboardMouseTracker::~KeyboardMouseTracker()
{
}

bool KeyboardMouseTracker::IsKeyDown(const int& button) const
{
	return keysDown[button];
}

bool KeyboardMouseTracker::IsKeyDownThisFrame(const int& button) const
{
	return keysDownThisFrame[button];
}

bool KeyboardMouseTracker::IsKeyUpThisFrame(const int& button) const
{
	return keysUpThisFrame[button];
}
void KeyboardMouseTracker::PressKey(const int& key)
{
	keysDown[key] = true;
	keysDownThisFrame[key] = true;
}
void KeyboardMouseTracker::ReleaseKey(const int& key)
{
	keysDown[key] = false;
	keysUpThisFrame[key] = true;
}
void KeyboardMouseTracker::PressMouse(const int& button)
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
void KeyboardMouseTracker::ReleaseMouse(const int& button)
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
void KeyboardMouseTracker::MouseMove(const int& x, const int& y)
{
	mousePositionOld = mousePosition;
	mousePosition.x = x;
	mousePosition.y = y;
}

void KeyboardMouseTracker::Update()
{
	for(int i = 0; i < MAX_KEYS; i++)
	{
		keysDownThisFrame[i] = keysUpThisFrame[i] = false;
	}
	mouseLeftDownThisFrame = mouseLeftUpThisFrame = mouseRightDownThisFrame = mouseRightUpThisFrame = false;
}