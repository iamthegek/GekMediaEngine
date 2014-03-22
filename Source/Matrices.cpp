#include "Matrices.h"
#include <iostream>

Matrices::Matrices()
{
	modelMatrix.push_back(glm::mat4(1.0));
	viewMatrix.push_back(glm::mat4(1.0));
	projectionMatrix.push_back(glm::mat4(1.0));
	currentMatrix = MODEL_MATRIX;
	matricesReady = true;
	modelViewMatrix = glm::mat4(1.0); 
	modelViewProjectionMatrix = glm::mat4(1.0);
	normalMatrix = glm::mat3(1.0);
}
void Matrices::LoadIdentity()
{
	switch(currentMatrix)
	{
		case MODEL_MATRIX: 
			modelMatrix[modelMatrix.size()-1] = glm::mat4(1.0);
			break;
		case VIEW_MATRIX:
			viewMatrix[viewMatrix.size()-1] = glm::mat4(1.0);
			break;
		case PROJECTION_MATRIX:
			projectionMatrix[projectionMatrix.size()-1] = glm::mat4(1.0);
			break;
	}
	matricesReady = false;
}
void Matrices::SetMatrixMode(int m)
{
	currentMatrix = m;
}
void Matrices::Translate(float x, float y, float z)
{
	switch(currentMatrix)	
	{
		case MODEL_MATRIX:
			modelMatrix[modelMatrix.size()-1] *= glm::translate(x,y,z);
			break;
		case VIEW_MATRIX:
			viewMatrix[viewMatrix.size()-1] *= glm::translate(x,y,z);
			break;
	}
	matricesReady = false;
}
void Matrices::Scale(float x, float y, float z)
{
	switch(currentMatrix)
	{
		case MODEL_MATRIX:
			modelMatrix[modelMatrix.size()-1] *= glm::scale(x,y,z);
			break;
		case VIEW_MATRIX:
			viewMatrix[viewMatrix.size()-1] *= glm::scale(x,y,z);
				break;
	}
	matricesReady = false;
}
void Matrices::Rotate(float angle, float x, float y, float z)
{
	switch(currentMatrix)
	{
		case MODEL_MATRIX:
			modelMatrix[modelMatrix.size()-1] *= glm::rotate(angle,x,y,z);
			break;
		case VIEW_MATRIX:
			viewMatrix[viewMatrix.size()-1] *= glm::rotate(angle,x,y,z);
			break;
	}
	matricesReady = false;
}
void Matrices::LookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 up)
{
	switch(currentMatrix)
	{
		case MODEL_MATRIX:	
			modelMatrix[modelMatrix.size()-1] *= glm::lookAt(eye, center, up);
			break;
		case VIEW_MATRIX:	
			viewMatrix[viewMatrix.size()-1] *= glm::lookAt(eye, center, up);
			break;
	}
	matricesReady = false;
}
void Matrices::MultMatrix(glm::mat4 mat)
{
	switch(currentMatrix)
	{
	case MODEL_MATRIX:	
		modelMatrix[modelMatrix.size()-1] *= mat;
		break;
	case VIEW_MATRIX:	
		viewMatrix[viewMatrix.size()-1] *= mat;
		break;
	}
	matricesReady = false;
}
void Matrices::SetPerspective(float angle, float ratio, float in_near, float in_far)
{
	projectionMatrix[projectionMatrix.size()-1] = glm::perspective(angle, ratio, in_near, in_far);
	matricesReady = false;
}
void Matrices::SetOrtho(float left, float right, float bottom, float top, float in_near, float in_far)
{
	projectionMatrix[projectionMatrix.size()-1] = glm::ortho(left, right, bottom, top, in_near, in_far);
	matricesReady = false;
}

void Matrices::PushMatrix()
{
	glm::mat4 temp;
	switch(currentMatrix)
	{
		case MODEL_MATRIX:
			temp = modelMatrix[modelMatrix.size()-1];
			modelMatrix.push_back(temp);
			break;
		case VIEW_MATRIX:
			temp = viewMatrix[viewMatrix.size()-1];
			viewMatrix.push_back(temp);
			break;
		case PROJECTION_MATRIX:
			temp = projectionMatrix[projectionMatrix.size()-1];
			projectionMatrix.push_back(temp);
			break;
	}
	matricesReady = false;
}
void Matrices::PopMatrix()
{
	switch(currentMatrix)
	{
		case MODEL_MATRIX:
			if(modelMatrix.size() > 1)
				modelMatrix.pop_back();
			break;
		case VIEW_MATRIX:
			if(viewMatrix.size() > 1)
				viewMatrix.pop_back();
			break;
		case PROJECTION_MATRIX:
			if(projectionMatrix.size() > 1)
				projectionMatrix.pop_back();
			break;
	}
	matricesReady = false;
}

glm::mat4& Matrices::GetModelMatrix()
{
	return modelMatrix[modelMatrix.size()-1];
}
glm::mat4& Matrices::GetViewMatrix()
{
	return viewMatrix[viewMatrix.size()-1];
}
glm::mat4& Matrices::GetModelViewMatrix()
{
	if(!matricesReady)
		modelViewMatrix = viewMatrix[viewMatrix.size()-1] * modelMatrix[modelMatrix.size()-1];
	return modelViewMatrix;
}
glm::mat4& Matrices::GetModelViewProjectionMatrix()
{
	if(!matricesReady)
		modelViewProjectionMatrix = projectionMatrix[projectionMatrix.size()-1] * viewMatrix[viewMatrix.size()-1] * modelMatrix[modelMatrix.size()-1];
	return modelViewProjectionMatrix;
}
glm::mat4& Matrices::GetProjectionMatrix()
{	
	return projectionMatrix[projectionMatrix.size()-1];
}
glm::mat3& Matrices::GetNormalMatrix()
{
	if(!matricesReady)
		normalMatrix = glm::mat3(GetModelViewMatrix());
	return normalMatrix;
}