#ifndef __LIGHT__H
#define __LIGHT__H

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\transform.hpp"
#include "glm\gtc\type_ptr.hpp"

#include <GL\glew.h>
#include "Shader.h"

struct PointLight
{
	GLfloat radius;
	glm::vec3 color;
	glm::vec3 position;
};

struct DirectionalLight
{
	glm::vec3 color;
	glm::vec3 direction;

};

#endif