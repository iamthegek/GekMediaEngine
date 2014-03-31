#include "BaseModel.h"

BaseModel::BaseModel()
{
	SetCulling(false);
	SetShadows(SHADOW_CLOSE | SHADOW_MEDIUM | SHADOW_FAR);
	SetShading(0x00);

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
void BaseModel::SetCulling(bool FLAG)
{
	culled = FLAG;
}
void BaseModel::SetShadows(const GLuint& FLAGS)
{
	shadowBits = FLAGS;
}
void BaseModel::SetShading(const GLuint& FLAGS)
{
	shadingBits = FLAGS;
}
GLuint BaseModel::GetShadowFlags()
{
	return shadowBits;
}
GLuint BaseModel::GetShadingFlags()
{
	return shadingBits;
}
bool BaseModel::GetCullingFlag()
{
	return culled;
}