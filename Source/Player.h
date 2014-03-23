#ifndef __PLAYER__H
#define __PLAYER__H

#include "PlayerController.h"
#include "PlayerJoystickController.h"
#include "PlayerPcController.h"
#include "BoneMesh.h"

#include <glm\glm.hpp>
#include <string>
#include <vector>

struct Animation
{
	float startTime, curTime, endTime;
	bool loop, done;
	std::string name;
};

class Player
{
public:
	Player();
	~Player();

	void AttachBoneMesh(BoneMesh* mesh);
	void AddAnimation(const float& start, const float& end, const std::string& name, const bool& loop);
	void SetAnimation(const std::string& name);
	float GetAnimationPosition() const;

	glm::vec2 GetPosition() const;
	glm::vec2 GetFaceDirection() const;

	void Update(PlayerController* ctrl, const float& deltaTime);

private:
	void UpdateAnimation(const float& deltaTime);

private:
	BoneMesh* charMesh;
	std::vector<Animation> animations;
	int currentAnimation;

	glm::vec2 position;
	glm::vec2 moveDirection, faceDirection;
};

#endif