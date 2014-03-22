#ifndef __MATRICES__H
#define __MATRICES__H

#include <vector>
#include <map>

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtx\transform.hpp"
#include <GL\glew.h>

class Matrices
{
private:
	std::vector<glm::mat4> modelMatrix;
	std::vector<glm::mat4> viewMatrix;				//modelview matrix = view * model
	std::vector<glm::mat4> projectionMatrix;		//modelviewproj matrix = proj * view * model
	int currentMatrix;
	bool matricesReady;
	glm::mat4 modelViewMatrix, modelViewProjectionMatrix;
	glm::mat3 normalMatrix;

public:
	Matrices();

	void LoadIdentity();
	void SetMatrixMode(int m);
	void Translate(float x, float y, float z);
	void Scale(float x, float y, float z);
	void Rotate(float angle, float x, float y, float z);
	void LookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up);
	void MultMatrix(glm::mat4 mat);
	void SetPerspective(float angle, float ratio, float in_near, float in_far);
	void SetOrtho(float left, float right, float bottom, float top, float in_near, float in_far);
	void PushMatrix();
	void PopMatrix();

	glm::mat4& GetModelMatrix();
	glm::mat4& GetViewMatrix();
	glm::mat4& GetModelViewMatrix();
	glm::mat4& GetModelViewProjectionMatrix();
	glm::mat4& GetProjectionMatrix();
	glm::mat3& GetNormalMatrix();

	enum 
	{
		MODEL_MATRIX=0,
		VIEW_MATRIX,
		PROJECTION_MATRIX
	};
};

#endif