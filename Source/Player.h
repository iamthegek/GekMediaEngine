#ifndef __PLAYER__H
#define __PLAYER__H

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
#include "Collision.h"
#include "AnimatedModel.h"
#include "ModelContainer.h"

const int CONTROL_JOYSTICK = 0;
const int CONTROL_KEYBOARD_MOUSE = 1;
const unsigned int BUTTON_STATE_DOWN = 0x01;
const unsigned int BUTTON_STATE_JUST_DOWN = 0x02;
const unsigned int BUTTON_STATE_JUST_UP = 0x04;
const int AXIS_L_X = 0;
const int AXIS_L_Y = 1;
const int AXIS_R_X = 2;
const int AXIS_R_Y = 3;
const int AXIS_TRIGGER = 4;
const int AXIS_DPAD_X = 5;
const int AXIS_DPAD_Y = 6;
const int BUTTON_JUMP = 0;
const int BUTTON_USE = 1;
const int BUTTON_FORWARD = 2;
const int BUTTON_BACKWARD = 3;
const int BUTTON_LEFT = 4;
const int BUTTON_RIGHT = 5;

const int JOYSTICK_AXIS_COUNT = 7;
const int JOYSTICK_BUTTON_COUNT = 2;
const int KEYBOARD_BUTTON_COUNT = 6;

struct ControlButton
{
	unsigned int ID;
	unsigned int FLAGS;
};

struct ControlAxis
{
	unsigned int ID;
	float POSITION;
};

struct ControlSetJoystick
{
	ControlAxis AXES[JOYSTICK_AXIS_COUNT];
	ControlButton BUTTONS[JOYSTICK_BUTTON_COUNT];
	bool INVERT_X, INVERT_Y;

	void Rebind(const int& buttonIndex, const int& newValue)
	{
		BUTTONS[buttonIndex].ID = newValue;
	}
};

struct ControlSetKeyboard
{
	glm::vec2 MOUSE, MOUSEOLD;
	unsigned int MOUSE_LEFT_FLAGS, MOUSE_RIGHT_FLAGS;
	ControlButton BUTTONS[KEYBOARD_BUTTON_COUNT];
	bool INVERT_X, INVERT_Y;

	void Rebind(const int& buttonIndex, const int& newValue)
	{
		BUTTONS[buttonIndex].ID = newValue;
	}
};

class Player
{
public:
	Player();
	~Player();

	void LoadModel(ModelContainer* deposit);
	void SetControlType(const int& id) 	{ controllerId = id;  }
	Collision::AABB* GetHitbox() { return hitbox; }
	glm::vec3 GetPosition() { return position; }
	glm::vec3 GetCamPosition() { return camPosition; }

	void JoystickButtonPressed(const int& buttonId);
	void JoystickButtonReleased(const int& buttonId);
	void JoystickMoved(const int& axis, const float& amount);

	void KeyPressed(const int& key);
	void KeyReleased(const int& key);
	void MousePressed(const int& button);
	void MouseReleased(const int& button);
	void MouseMoved(const int& x, const int& y);

	void Update(const float& deltaTime, Collision::Terrain* terr, const glm::vec3& gravity);

private:
	void UpdateInput(glm::vec3& targetMove, float& dYaw, float& dPitch);
	void UpdateCollisionObjects();
	void UpdateCamera(const float& dYaw, const float& dPitch, const float& deltaTime);
	glm::vec3 CollideWithTerrain(Collision::Terrain* terr, const glm::vec3& pos, const glm::vec3& vel);
	void UpdateModel();
	void ResetButtonFlags();

private:
	int controllerId;
	ControlSetJoystick joystick;
	ControlSetKeyboard keyboard;
	unsigned int jumpFlags, useFlags;
	bool invX, invY;

	int coins, hearts, lives;
	const char* name;
	unsigned int flags;

	float camYaw, camPitch, camDist;
	glm::vec3 camPosition;
	glm::vec3 characterDir;
	AnimatedModel* model;

	int collisionRecurDepth;
	glm::vec3 position, velocity;
	Collision::AABB* hitbox;
	Collision::CollisionPacket* packet;
};

#endif