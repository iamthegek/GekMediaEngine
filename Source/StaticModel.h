#ifndef __STATICMODEL__H
#define __STATICMODEL__H

#include "BaseModel.h"
#include "StaticMesh.h"

class StaticModel : public BaseModel
{
public:
	StaticModel();
	~StaticModel();

	void AddLevelOfDetail(StaticMesh * model, const GLfloat& maxDistance);
	void DecideLevelOfDetail(const glm::vec3& cameraPosition);

	void AddColorTexture(const std::string& Filename);
	void AddNormalTexture(const std::string& Filename);

	StaticMesh * GetMesh();
	Texture * GetTexture(const GLint& index)
	{
		return textures[index];
	}

protected:
	StaticMesh * meshes[3];
	Texture * textures[2];

	GLint maxLOD;
	GLint curLOD;
	GLfloat cutoffLOD[2];
};

#endif