#ifndef __LOCGAME__H
#define __LOCGAME__H

#include "Player.h"

class LocalGame
{
public:
	LocalGame();
	~LocalGame();

	void JoystickButtonPressed(const int& buttonId, const int& stickId);
	void JoystickButtonReleased(const int& buttonId, const int& stickId);
	void JoystickMoved(const int& axis, const int& stickId, const float& amount);

	void KeyPressed(const int& key);
	void KeyReleased(const int& key);
	void MousePressed(const int& button);
	void MouseReleased(const int& button);
	void MouseMoved(const int& x, const int& y);

	Player* GetPlayer() {return player;} 

	void Update(const float& deltaTime);

private:
	Player* player;
	Collision::Terrain* terrain;
};

#endif