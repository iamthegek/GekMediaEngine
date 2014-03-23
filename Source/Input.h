#ifndef __MYINPUT__H
#define __MYINPUT__H

#include <vector>
#include "glm\glm.hpp"

const int MAX_BUTTONS_JOYSTICK = 30;
const int MAX_AXES_JOYSTICK = 20;

class JoystickTracker
{
public:
	JoystickTracker();
	~JoystickTracker();

	float GetJoystickPosition(const int& axis) const;
	bool IsButtonDown(const int& button) const;
	bool IsButtonDownThisFrame(const int& button) const;
	bool IsButtonUpThisFrame(const int& button) const;

	void PressButton(const int& button);
	void ReleaseButton(const int& button);
	void PlaceAxis(const float& pos, const int& axis);

	void Update();

private:
	float axes[MAX_AXES_JOYSTICK];
	bool buttonsDown[MAX_BUTTONS_JOYSTICK];
	bool buttonsDownThisFrame[MAX_BUTTONS_JOYSTICK];
	bool buttonsUpThisFrame[MAX_BUTTONS_JOYSTICK];
};

const int MAX_KEYS = 70;

class KeyboardMouseTracker
{
public:
	KeyboardMouseTracker();
	~KeyboardMouseTracker();

	bool IsKeyDown(const int& button) const;
	bool IsKeyDownThisFrame(const int& button) const;
	bool IsKeyUpThisFrame(const int& button) const;

	void PressKey(const int& key);
	void ReleaseKey(const int& key);
	void PressMouse(const int& button);
	void ReleaseMouse(const int& button);
	void MouseMove(const int& x, const int& y);

	void Update();

private:
	bool keysDown[MAX_KEYS];
	bool keysDownThisFrame[MAX_KEYS];
	bool keysUpThisFrame[MAX_KEYS];
	bool mouseLeftDown, mouseLeftDownThisFrame, mouseLeftUpThisFrame;
	bool mouseRightDown, mouseRightDownThisFrame, mouseRightUpThisFrame;
	glm::vec2 mousePosition, mousePositionOld;
};

#endif