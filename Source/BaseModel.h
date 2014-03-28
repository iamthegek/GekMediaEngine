#ifndef __BASEMODEL__H
#define __BASEMODEL__H

#include <GL\glew.h>
#include <glm\glm.hpp>

#define STATUS_CULLING 0
#define CULL_DISABLED 1
#define CULL_ENABLED 2

#define STATUS_SHADING 10
#define SHADING_DIFFUSE 11
#define SHADING_DIFFUSE_AND_NORMAL 12

#define STATUS_SHADOW 30
#define SHADOW_NONE 31
#define SHADOW_CLOSE 32
#define SHADOW_MEDIUM 32
#define SHADOW_FAR 34

#define STATIC_MODEL 100
#define ANIMATED_MODEL 101

class BaseModel
{
public:
	BaseModel();
	virtual ~BaseModel() {}

	glm::mat4& GetMatrix();
	glm::mat3 GetRotation();
	glm::vec3 GetPosition();

	void SetStatus(const GLint& which, const GLint& status);
	GLint GetStatus(const GLint& which);

	GLint GetType();

protected:
	GLint cullStatus;		//Is this even drawn?
	GLint shadingStatus;	//How do we shade this?
	GLint shadowStatus;		//How far out do we render shadows?

	glm::mat4 objectMatrix;

	GLint type;
};

#endif