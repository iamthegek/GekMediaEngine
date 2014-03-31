#include "StaticModel.h"

StaticModel::StaticModel() : BaseModel()
{
	meshes[0] = 0;
	meshes[1] = 0;
	meshes[2] = 0;
	maxLOD = curLOD = -1;
	cutoffLOD[0] = cutoffLOD[1] = 0.0f;
	textures[0] = textures[1] = 0;
}

StaticModel::~StaticModel()
{
	for(int i = 0; i < 3; i++)
		SAFE_DELETE(meshes[i]);

	SAFE_DELETE(textures[0]);
	SAFE_DELETE(textures[1]);
}

void StaticModel::AddLevelOfDetail(StaticMesh * model, const GLfloat& maxDistance)
{
	maxLOD++;
	if(maxLOD > 2)
		return;
	meshes[maxLOD] = model;
	if(maxLOD < 2)
		cutoffLOD[maxLOD] = maxDistance * maxDistance;
	curLOD = 0;
}

StaticMesh * StaticModel::GetMesh()
{
	return meshes[curLOD];
}

void StaticModel::DecideLevelOfDetail(const glm::vec3& cameraPosition)
{
	if(maxLOD < 0)
		return;

	glm::vec3 toVector = cameraPosition - GetPosition();
	GLfloat dist = glm::dot(toVector, toVector);

	if(dist < cutoffLOD[0]|| maxLOD == 0) //if we are close or do not have lesser level of detail, set to zero
	{
		curLOD = 0;
		if(textures[1] != 0)
			SetShading(MAPPED_DIFFUSE | MAPPED_NORMAL);
		return;
	}

	SetShading(MAPPED_DIFFUSE);
	if(dist < cutoffLOD[1] || maxLOD == 1)
	{
		curLOD = 1;
		return;
	}

	curLOD = 2;
	return;

}

void StaticModel::AddColorTexture(const std::string& Filename)
{
	SAFE_DELETE(textures[0]);
	textures[0] = new Texture(Filename, Texture::ModelTextureTypes::TYPE_COLOR);
}
void StaticModel::AddNormalTexture(const std::string& Filename)
{
	SAFE_DELETE(textures[1]);
	textures[1] = new Texture(Filename, Texture::ModelTextureTypes::TYPE_NORMAL);
}