#include "PlayerPcController.h"
#include <SFML\Window.hpp>

PlayerPcController::PlayerPcController() : PlayerController(PlayerController::P_KEYBOARDMOUSE)
{
	for(int i = 0; i < NUM_KEYS; i++)
	{
		keyIndices[i] = 0;
		keyFlags[i] = false;
	}

	keyIndices[Y_POSITIVE] = sf::Keyboard::W;
	keyIndices[Y_NEGATIVE] = sf::Keyboard::S;
	keyIndices[X_POSITIVE] = sf::Keyboard::D;
	keyIndices[X_NEGATIVE] = sf::Keyboard::A;
}

PlayerPcController::~PlayerPcController()
{
}

void PlayerPcController::AttachToPc(KeyboardMouseTracker* p)
{
	controller = p;
}

void PlayerPcController::Rebind(int control, int newKeyIndex)
{
	keyIndices[control] = newKeyIndex;
}

bool PlayerPcController::GetControlState(const int& control)
{
	return keyFlags[control] >= ControlFlags::C_JUSTRELEASED;
}

void PlayerPcController::Update(const float& deltaTime)
{
	for(int i = 0; i < NUM_KEYS; i++)
	{
		if(controller->IsKeyDown(keyIndices[i]))
		{
			if(controller->IsKeyDownThisFrame(keyIndices[i]))
			{
				keyFlags[i] = ControlFlags::C_JUSTPRESSED;
			}
			else
			{
				keyFlags[i] = ControlFlags::C_PRESSED;
			}
		}
		else
		{
			if(controller->IsKeyUpThisFrame(keyIndices[i]))
			{
				keyFlags[i] = ControlFlags::C_RELEASED;
			}
			else
			{
				keyFlags[i] = ControlFlags::C_JUSTRELEASED;
			}
		}
	}

	float yPosition = 0.0f;
	if(keyFlags[Y_POSITIVE] >= ControlFlags::C_JUSTPRESSED)	yPosition += 1.0f;
	if(keyFlags[Y_NEGATIVE] >= ControlFlags::C_JUSTPRESSED)	yPosition -= 1.0f;

	float xPosition = 0.0f;
	if(keyFlags[X_POSITIVE] >= ControlFlags::C_JUSTPRESSED)	xPosition += 1.0f;
	if(keyFlags[X_NEGATIVE] >= ControlFlags::C_JUSTPRESSED)	xPosition -= 1.0f;

	moveDirection = glm::vec2(xPosition, yPosition);
	if(xPosition != 0.0f && yPosition != 0.0f)
		moveDirection = glm::normalize(moveDirection);
}