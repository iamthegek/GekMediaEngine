#ifndef __STATICMESH__H
#define __STATICMESH__H

#include <map>
#include <vector>
#include <GL/glew.h>
#include <assimp/Importer.hpp> 
#include <assimp/scene.h>      
#include <assimp/postprocess.h>
#include "glm/glm.hpp"
#include "Texture.h"

#include "MeshCommon.h"

class StaticMesh
{
public:
    StaticMesh();
    ~StaticMesh();
    bool LoadMesh(const std::string& Filename);
	void AddColorTexture(const std::string& Filename);
	void AddNormalTexture(const std::string& Filename);
    void Render();

	Texture * textures[2];

private:
    bool InitFromScene(const aiScene* pScene, const std::string& Filename);
    void InitMesh(	const aiMesh* paiMesh, 
					std::vector<glm::vec3>& Positions, 
					std::vector<glm::vec3>& Normals, 
					std::vector<glm::vec2>& TexCoords, 
					std::vector<glm::vec3>& Tangents, 
					std::vector<unsigned int>& Indices);
    void Clear();

	struct MeshEntry 
	{
		MeshEntry()
		{
			NumIndices = BaseVertex = BaseIndex = 0;
		}
		unsigned int NumIndices;
		unsigned int BaseVertex;
		unsigned int BaseIndex;
    };

	GLuint VAO;
	GLuint buffers[NUM_STAT_MESH_VBs];
	std::vector<MeshEntry> m_Entries;
};

#endif