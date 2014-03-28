#ifndef __ANIMMODEL__H
#define __ANIMMODEL__H

#include "BaseModel.h"
#include "BoneMesh.h"

struct Animation
{
	GLfloat startTime, endTime, curTime;
	bool loop, done;
	const char* name;
};

class AnimatedModel : public BaseModel
{
public:
	AnimatedModel();
	~AnimatedModel();

	void AddAnimation(const GLfloat& start, const GLfloat& end, const char* n, bool loop);
	void SetAnimation(const char* n);
	void SetMesh(BoneMesh * m);
	BoneMesh * GetMesh();
	std::vector<glm::mat4>& GetBoneTransforms();

	void AddColorTexture(const std::string& Filename);
	void AddNormalTexture(const std::string& Filename);
	Texture * GetTexture(const GLint& index)
	{
		return textures[index];
	}

	void Update(const float& deltaTime);

protected:
	BoneMesh * mesh;
	Texture * textures[2];
	std::vector<glm::mat4> boneTransforms;
	std::vector<Animation> animations;
	GLint curAnim;
};

#endif