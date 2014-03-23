/*
LocalPlayerJoystick - Child of LocalPlayer

This class represents a player controlled by a specific gamepad.
The Joysticks will ALWAYS control movement, yet there are options 
for inverted X and Y axes. The Left stick will control position,
the right stick will change the direction the character is facing.
All other controls are rebindable to buttons.
*/

#ifndef __LOCPLAYERJS__H
#define __LOCPLAYERJS__H

#include "PlayerController.h"
#include "Input.h"

class PlayerJoystickController : public PlayerController
{
public:
	enum JoystickControls //Non-movement based controls
	{
		SHOOT,
		NUM_BUTTONS
	};

	PlayerJoystickController();
	~PlayerJoystickController();

	void AttachToJoystick(JoystickTracker* j);
	void Rebind(int control, int newButtonIndex);
	bool GetControlState(const int& control);

	void InvertXAxis(bool flag);
	void InvertYAxis(bool flag);

	void Update(const float& deltaTime);

private:
	JoystickTracker * controller;

	//for Axis Movement
	bool invertX;
	bool invertY;

	//for Buttons
	int buttonIndices[NUM_BUTTONS];
	int buttonFlags[NUM_BUTTONS];
};

#endif