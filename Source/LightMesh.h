#ifndef __LIGHTMESH__H
#define __LIGHTMESH__H

#include <map>
#include <vector>
#include <GL/glew.h>
#include <assimp/Importer.hpp> 
#include <assimp/scene.h>       
#include <assimp/postprocess.h> 

#include "glm/glm.hpp"

class LightMesh
{
public:
    LightMesh();
    ~LightMesh();
    bool LoadMesh(const std::string& Filename);
	void Bind();
    void Render();
	void Unbind();

private:
    bool InitFromScene(const aiScene* pScene, const std::string& Filename);
    void InitMesh(const aiMesh* paiMesh, std::vector<glm::vec3>& Positions, std::vector<unsigned int>& Indices);
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

	enum
	{
		INDEX_BUFFER=0, 
		POS_VB
	};

	GLuint VAO;
	GLuint buffers[2];
	std::vector<MeshEntry> m_Entries;
};

#endif