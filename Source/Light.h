#ifndef __LIGHT__H
#define __LIGHT__H

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\transform.hpp"
#include "glm\gtc\type_ptr.hpp"

#include <GL\glew.h>
#include "Shader.h"
#include "Matrices.h"

class Light
{
public:
	Light(GLuint t) : type(t) 
	{
	}
	virtual ~Light() 
	{
	}

	enum
	{
		POINT_LIGHT,
		DIRECTIONAL_LIGHT
	};

	GLuint GetType() 
	{ 
		return type; 
	}

protected:
	GLuint type;
};

class PointLight : public Light
{
public:
	PointLight(Shader& shdr);
	~PointLight();

	PointLight* SetPosition(const glm::vec3& p);
	PointLight* SetRadius(const float& r);
	PointLight* SetColor(const glm::vec3& c);

	void SendToShader(Matrices& in_mat, const glm::vec3& eyePosition);

	GLfloat radius;
	glm::vec3 color;
	glm::vec3 position;

private:
	GLuint uniLocMvpMatrix;
	GLuint uniLocColor, uniLocRadius, uniLocPosition, uniLocEyePosition;
};

class DirectionalLight : public Light
{
public:
	DirectionalLight(Shader& shdr);
	~DirectionalLight();

	DirectionalLight* SetDirection(const glm::vec3& dir);
	DirectionalLight* SetColor(const glm::vec3& col);

	void SendToShader(Matrices& in_mat, glm::vec3 eyePosition);

	glm::vec3 color;
	glm::vec3 direction;

private:
	GLuint uniLocColor, uniLocDir, uniLocEyePosition,uniLocMvpMatrix;
};

#endif