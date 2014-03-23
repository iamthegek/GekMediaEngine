#ifndef __LOCPLAYER__H
#define __LOCPLAYER__H

#include <glm\glm.hpp>
#include <string>

#define MAX_LOCAL_PLAYERS 4

enum ControlFlags
{
	C_RELEASED=100,
	C_JUSTRELEASED,
	C_JUSTPRESSED,  //if >= C_JUSTPRESSED, Control is active
	C_PRESSED
};

class PlayerController
{
public:
	enum
	{
		P_JOYSTICK,
		P_KEYBOARDMOUSE
	};

	PlayerController(const int& c) : controlType(c), faceDirection(glm::vec2(0,1)) { }
	
	virtual ~PlayerController() {}

	int GetControlType() const
	{
		return controlType;
	}
	
	virtual void Rebind(int control, int newIndex) {}

	virtual void Update(const float& deltaTime) { /*Nothing by Default*/ }
	
	virtual bool GetControlState(const int& control) 
	{ 
		return true; 
	}

	glm::vec2 GetMoveDirection() const
	{
		return moveDirection;
	}

	glm::vec2 GetFaceDirection() const
	{
		return faceDirection;
	}

protected:
	glm::vec2 moveDirection;
	glm::vec2 faceDirection;

private:
	int controlType;
};

#endif