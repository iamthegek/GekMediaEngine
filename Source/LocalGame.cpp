#include "LocalGame.h"

LocalGame::LocalGame()
{
	for(int i = 0; i < 4; i++)
	{
		playersEnabled[i] = false;
		controllers[i] = 0;
	}
}

LocalGame::~LocalGame()
{
}

void LocalGame::EnablePlayer(const int& which, const int& controllerId)
{
	playersEnabled[which] = true;
	SetPlayerController(which, controllerId);
}

void LocalGame::DisablePlayer(const int& which)
{
	playersEnabled[which] = false;
}

void LocalGame::SetPlayerController(int which, int controllerId)
{
	if(controllers[which] != 0)
	{
		delete controllers[which];
		controllers[which] = 0;
	}

	switch(controllerId)
	{
		case ControllerIds::C_JOYSTICK0:
		{
			controllers[which] = new PlayerJoystickController();
			((PlayerJoystickController*)controllers[which])->AttachToJoystick(&joystickTracker[0]);
			break;
		}
		case ControllerIds::C_JOYSTICK1:
		{
			controllers[which] = new PlayerJoystickController();
			((PlayerJoystickController*)controllers[which])->AttachToJoystick(&joystickTracker[1]);
			break;
		}
		case ControllerIds::C_JOYSTICK2:
		{
			controllers[which] = new PlayerJoystickController();
			((PlayerJoystickController*)controllers[which])->AttachToJoystick(&joystickTracker[2]);
			break;
		}
		case ControllerIds::C_JOYSTICK3:
		{
			controllers[which] = new PlayerJoystickController();
			((PlayerJoystickController*)controllers[which])->AttachToJoystick(&joystickTracker[3]);
			break;
		}
		case ControllerIds::C_KEYBOARDMOUSE:
		{
			controllers[which] = new PlayerPcController();
			((PlayerPcController*)controllers[which])->AttachToPc(&keyboardMouseTracker);
			break;
		}
	}
}

Player* LocalGame::GetPlayer(const int& which)
{
	return &players[which];
}

bool LocalGame::GetPlayerEnabled(const int& which)
{
	return playersEnabled[which];
}

PlayerController* LocalGame::GetController(const int& which)
{
	return controllers[which];
}

void LocalGame::JoystickButtonPressed(const int& buttonId, const int& stickId)
{
	joystickTracker[stickId].PressButton(buttonId);
}

void LocalGame::JoystickButtonReleased(const int& buttonId, const int& stickId)
{
	joystickTracker[stickId].ReleaseButton(buttonId);
}

void LocalGame::JoystickMoved(const int& axis, const int& stickId, const float& amount)
{
	joystickTracker[stickId].PlaceAxis(amount, axis);
}

void LocalGame::KeyPressed(const int& key)
{
	keyboardMouseTracker.PressKey(key);
}

void LocalGame::KeyReleased(const int& key)
{
	keyboardMouseTracker.ReleaseKey(key);
}

void LocalGame::MousePressed(const int& button)
{
	keyboardMouseTracker.PressMouse(button);
}

void LocalGame::MouseReleased(const int& button)
{
	keyboardMouseTracker.ReleaseMouse(button);
}

void LocalGame::MouseMoved(const int& x, const int& y)
{
	keyboardMouseTracker.MouseMove(x, y);
}

void LocalGame::Update(const float& deltaTime)
{
	for(int i = 0; i < 4; i++)
	{
		if(playersEnabled[i])
		{
			controllers[i]->Update(deltaTime);
			players[i].Update(controllers[i], deltaTime);
		}
	}

	keyboardMouseTracker.Update();
	for(int i = 0; i < 4; i++)
	{
		joystickTracker[4].Update();
	}
}