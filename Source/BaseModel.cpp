#include "BaseModel.h"

BaseModel::BaseModel()
{
	cullStatus = CULL_DISABLED;
	shadingStatus = SHADING_DIFFUSE_AND_NORMAL;
	shadowStatus = SHADOW_FAR;
	type = -1;

	objectMatrix = glm::mat4(1.0f);
}
glm::mat4& BaseModel::GetMatrix()
{
	return objectMatrix;
}
glm::mat3 BaseModel::GetRotation()
{
	return glm::mat3(objectMatrix);
}
glm::vec3 BaseModel::GetPosition()
{
	return glm::vec3(objectMatrix[3]);
}
void BaseModel::SetStatus(const GLint& which, const GLint& status)
{
	switch(which)
	{
		case STATUS_CULLING: cullStatus = status; return;
		case STATUS_SHADING: shadingStatus = status; return;
		case STATUS_SHADOW:	shadowStatus = status; return;
	}
}
GLint BaseModel::GetStatus(const GLint& which)
{
	switch(which)
	{
		case STATUS_CULLING: return cullStatus;
		case STATUS_SHADING: return shadingStatus;
		case STATUS_SHADOW:	return shadowStatus;
	}
	return -1;
}
GLint BaseModel::GetType()
{
	return type;
}