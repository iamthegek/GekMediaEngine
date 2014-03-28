#ifndef __PLAYER__H
#define __PLAYER__H

#include "PlayerController.h"
#include "PlayerJoystickController.h"
#include "PlayerPcController.h"
#include "BoneMesh.h"

#include <glm\glm.hpp>
#include <string>
#include <vector>

class Player
{
public:
	Player();
	~Player();

	void AttachBoneMesh(BoneMesh* mesh);

	glm::vec2 GetPosition() const;
	glm::vec2 GetFaceDirection() const;

	void Update(PlayerController* ctrl, const float& deltaTime);


private:

	glm::vec2 position;
	glm::vec2 moveDirection, faceDirection;
};

#endif