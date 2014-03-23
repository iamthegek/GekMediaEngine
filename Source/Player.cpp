#include "Player.h"

Player::Player()
{
	charMesh = 0;
	currentAnimation = 0;
}

Player::~Player()
{
	charMesh = 0;
}

void Player::AttachBoneMesh(BoneMesh* mesh)
{
	charMesh = mesh;
}

void Player::AddAnimation(const float& start, const float& end, const std::string& name, const bool& loop)
{
	Animation anim;
	anim.startTime = start;
	anim.curTime = start;
	anim.endTime = end;
	anim.done = false;
	anim.loop = loop;
	anim.name = name;
	animations.push_back(anim);
}

void Player::SetAnimation(const std::string& name)
{
	for(unsigned int i = 0; i < animations.size(); i++)
	{
		if(strcmp(animations[i].name.c_str(), name.c_str())==0)
		{
			animations[i].curTime = animations[i].startTime;
			animations[i].done = false;
			currentAnimation = i;
			return;
		}
	}
}

float Player::GetAnimationPosition() const
{
	if(animations.size() == 0)
		return 0.0f;

	return animations[currentAnimation].curTime;
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

	UpdateAnimation(deltaTime);
}

void Player::UpdateAnimation(const float& deltaTime)
{
	if(charMesh == 0 || animations.size() == 0)
		return;

	if(!animations[currentAnimation].done)
	{
		animations[currentAnimation].curTime += deltaTime;
		if(animations[currentAnimation].curTime > animations[currentAnimation].endTime)
		{
			if(animations[currentAnimation].loop)
			{
				animations[currentAnimation].curTime = animations[currentAnimation].startTime;
			}
			else
			{
				animations[currentAnimation].curTime = animations[currentAnimation].endTime;
				animations[currentAnimation].done = true;
			}
		}
	}
}