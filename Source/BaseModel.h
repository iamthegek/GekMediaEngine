#ifndef __BASEMODEL__H
#define __BASEMODEL__H

#include <GL\glew.h>
#include <glm\glm.hpp>

enum
{
	MAPPED_DIFFUSE = 0x01,
	MAPPED_NORMAL = 0x02
};

enum
{
	SHADOW_CLOSE = 0x01,
	SHADOW_MEDIUM = 0x02,
	SHADOW_FAR = 0x04
};

class BaseModel
{
public:
	BaseModel();
	virtual ~BaseModel() {}

	glm::mat4& GetMatrix();
	glm::mat3 GetRotation();
	glm::vec3 GetPosition();

	void SetCulling(bool FLAG = false);
	void SetShadows(const GLuint& FLAGS = 0x00);
	void SetShading(const GLuint& FLAGS = 0x00);

	GLuint GetShadowFlags();
	GLuint GetShadingFlags();
	bool GetCullingFlag();

	GLint GetType();

protected:
	glm::mat4 objectMatrix;

	bool culled;
	GLuint shadowBits;
	GLuint shadingBits;
};

#endif