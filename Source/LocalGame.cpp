#include "LocalGame.h"

LocalGame::LocalGame()
{
	player = new Player();
	terrain = new Collision::Terrain("Data/CollisionMeshes/test.obj");
}

LocalGame::~LocalGame()
{
	delete terrain;
	delete player;
	player = 0;
}

void LocalGame::JoystickButtonPressed(const int& buttonId, const int& stickId)
{
	if(stickId == 0)
		player->JoystickButtonPressed(buttonId);
}

void LocalGame::JoystickButtonReleased(const int& buttonId, const int& stickId)
{
	if(stickId == 0)
		player->JoystickButtonReleased(buttonId);
}

void LocalGame::JoystickMoved(const int& axis, const int& stickId, const float& amount)
{
	if(stickId == 0)
		player->JoystickMoved(axis, amount);
}

void LocalGame::KeyPressed(const int& key)
{
	player->KeyPressed(key);
}

void LocalGame::KeyReleased(const int& key)
{
	player->KeyReleased(key);
}

void LocalGame::MousePressed(const int& button)
{
	player->MousePressed(button);
}

void LocalGame::MouseReleased(const int& button)
{
	player->MouseReleased(button);
}

void LocalGame::MouseMoved(const int& x, const int& y)
{
	player->MouseMoved(x, y);
}

void LocalGame::Update(const float& deltaTime)
{
	player->Update(deltaTime, terrain, glm::vec3(0.0f, -9.8f, 0.0f));
}