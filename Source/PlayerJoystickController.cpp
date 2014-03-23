#include "PlayerJoystickController.h"

#include <SFML\Window.hpp>
#include <iostream>

PlayerJoystickController::PlayerJoystickController() : PlayerController(PlayerController::P_JOYSTICK)
{
	invertX = invertY = false;
	controller = 0;
	for(int i = 0; i < NUM_BUTTONS; i++)
	{
		buttonIndices[i] = 0;
		buttonFlags[i] = false;
	}
}

PlayerJoystickController::~PlayerJoystickController()
{
	controller = 0;
}

void PlayerJoystickController::AttachToJoystick(JoystickTracker* j)
{
	controller = j;
}

void PlayerJoystickController::Rebind(int control, int newButtonIndex)
{
	buttonIndices[control] = newButtonIndex;
}

bool PlayerJoystickController::GetControlState(const int& control)
{
	return (buttonIndices[control] >= ControlFlags::C_JUSTPRESSED);
}

void PlayerJoystickController::InvertXAxis(bool flag)
{
	invertX = flag;
}

void PlayerJoystickController::InvertYAxis(bool flag)
{
	invertY = flag;
}

void PlayerJoystickController::Update(const float& deltaTime)
{
	for(int i = 0; i < NUM_BUTTONS; i++)
	{
		if(controller->IsButtonDown(buttonIndices[i]))
		{
			if(controller->IsButtonDownThisFrame(buttonIndices[i]))
			{
				buttonFlags[i] = ControlFlags::C_JUSTPRESSED;
			}
			else
			{
				buttonFlags[i] = ControlFlags::C_PRESSED;
			}
		}
		else
		{
			if(controller->IsButtonUpThisFrame(buttonIndices[i]))
			{
				buttonFlags[i] = ControlFlags::C_JUSTRELEASED;
			}
			else
			{
				buttonFlags[i] = ControlFlags::C_RELEASED;
			}
		}
	}

	float xPosition = controller->GetJoystickPosition(sf::Joystick::Axis::X) / 100.0f;
	float yPosition = controller->GetJoystickPosition(sf::Joystick::Axis::Y) / 100.0f;

	if(invertX)
		xPosition = -xPosition;
	if(invertY)
		yPosition = -yPosition;

	float length = glm::length(glm::vec2(xPosition, yPosition));
	if(length > 0.2f)
	{
		if(length > 1.0f)
		{
			moveDirection = glm::vec2(xPosition, yPosition) / length;
		}
		else
		{
			moveDirection = glm::vec2(xPosition, yPosition);
		}
	}
	else
	{
		moveDirection = glm::vec2(0.0f, 0.0f);
	}

	xPosition = controller->GetJoystickPosition(sf::Joystick::Axis::U) / 100.0f;
	yPosition = controller->GetJoystickPosition(sf::Joystick::Axis::R) / 100.0f;

	length = glm::length(glm::vec2(xPosition,yPosition));
	if(length > 1.0f)
	{
		faceDirection = glm::vec2(xPosition, yPosition) / length;
	}
	else
	{
		faceDirection = glm::vec2(xPosition, yPosition);
	}
}