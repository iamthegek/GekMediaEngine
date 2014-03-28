#include "Player.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::AttachBoneMesh(BoneMesh* mesh)
{
}

glm::vec2 Player::GetPosition() const
{
	return position;
}

glm::vec2 Player::GetFaceDirection() const
{
	return faceDirection;
}

void Player::Update(PlayerController* ctrl, const float& deltaTime)
{
	moveDirection = ctrl->GetMoveDirection();
	faceDirection = ctrl->GetFaceDirection();

	position += moveDirection*10.0f * deltaTime;
}
