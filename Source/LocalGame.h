#ifndef __LOCGAME__H
#define __LOCGAME__H

#include "PlayerController.h"
#include "PlayerJoystickController.h"
#include "PlayerPcController.h"
#include "Player.h"

enum ControllerIds
{
	C_JOYSTICK0,
	C_JOYSTICK1,
	C_JOYSTICK2,
	C_JOYSTICK3,
	C_KEYBOARDMOUSE
};

class LocalGame
{
public:
	LocalGame();
	~LocalGame();

	void EnablePlayer(const int& which, const int& controllerId);
	void DisablePlayer(const int& which);
	void SetPlayerController(int which, int controllerId);

	Player* GetPlayer(const int& which);
	bool GetPlayerEnabled(const int& which);
	PlayerController* GetController(const int& which);

	void JoystickButtonPressed(const int& buttonId, const int& stickId);
	void JoystickButtonReleased(const int& buttonId, const int& stickId);
	void JoystickMoved(const int& axis, const int& stickId, const float& amount);

	void KeyPressed(const int& key);
	void KeyReleased(const int& key);
	void MousePressed(const int& button);
	void MouseReleased(const int& button);
	void MouseMoved(const int& x, const int& y);

	void Update(const float& deltaTime);

private:
	Player players[4];
	bool playersEnabled[4];
	PlayerController* controllers[4];

	JoystickTracker joystickTracker[4];
	KeyboardMouseTracker keyboardMouseTracker;
};

#endif