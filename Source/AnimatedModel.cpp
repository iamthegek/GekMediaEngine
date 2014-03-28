#include "AnimatedModel.h"

AnimatedModel::AnimatedModel() : BaseModel()
{
	mesh = 0;
	curAnim = -1;
	type = ANIMATED_MODEL;
	textures[0] = textures[1] = 0;
}

AnimatedModel::~AnimatedModel()
{
	if(mesh != 0)
		delete mesh;
	if(textures[0] != 0)
		delete textures[0];
	if(textures[1] != 0)
		delete textures[1];
}

void AnimatedModel::AddAnimation(const GLfloat& start, const GLfloat& end, const char* n, bool loop)
{
	Animation a;
	a.startTime = a.curTime = start;
	a.endTime = end;
	a.done = false;
	a.loop = loop;
	a.name = n;
	animations.push_back(a);
}

void AnimatedModel::SetAnimation(const char* n)
{
	for(unsigned int i = 0; i < animations.size(); i++)
	{
		if(strcmp(n, animations[i].name) == 0)
		{
			curAnim = i;
			return;
		}
	}
}

void AnimatedModel::SetMesh(BoneMesh * m)
{
	mesh = m;
}

BoneMesh * AnimatedModel::GetMesh()
{
	return mesh;
}

std::vector<glm::mat4>& AnimatedModel::GetBoneTransforms()
{
	return boneTransforms;
}

void AnimatedModel::Update(const float& deltaTime)
{
	if(mesh == 0 || animations.size() == 0)
		return;

	if(!animations[curAnim].done)
	{
		animations[curAnim].curTime += deltaTime;
		if(animations[curAnim].curTime > animations[curAnim].endTime)
		{
			if(animations[curAnim].loop)
			{
				animations[curAnim].curTime = animations[curAnim].startTime;
			}
			else
			{
				animations[curAnim].curTime = animations[curAnim].endTime;
				animations[curAnim].done = true;
			}
		}
	}

	mesh->InterpolateBones(animations[curAnim].curTime, boneTransforms);
}

void AnimatedModel::AddColorTexture(const std::string& Filename)
{
	if(textures[0] != 0)
		delete textures[0];
	textures[0] = new Texture(Filename, Texture::ModelTextureTypes::TYPE_COLOR);
}
void AnimatedModel::AddNormalTexture(const std::string& Filename)
{
	if(textures[1] != 0)
		delete textures[1];
	textures[1] = new Texture(Filename, Texture::ModelTextureTypes::TYPE_NORMAL);
}