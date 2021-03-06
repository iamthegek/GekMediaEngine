#include "LightMesh.h"
#include <assert.h>

using namespace std;

LightMesh::LightMesh()
{
    buffers[0] = buffers[1] = 0;
}
LightMesh::~LightMesh()
{ 
	Clear();
}
void LightMesh::Clear()
{
	if (buffers[0] != 0)	
		glDeleteBuffers(2, buffers);       
}
bool LightMesh::LoadMesh(const string& Filename)
{
    Clear();
    glGenBuffers(2, buffers);
    bool Ret = false;
    Assimp::Importer Importer;
	const aiScene* pScene = Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
    if (pScene) 
	{
		Ret = InitFromScene(pScene, Filename);
	}
    else		
	{
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
	}
    return Ret;
}
bool LightMesh::InitFromScene(const aiScene* pScene, const string& Filename)
{  
    m_Entries.resize(pScene->mNumMeshes);

    vector<glm::vec3> Positions;
    vector<unsigned int> Indices;

    unsigned int NumVertices = 0;
    unsigned int NumIndices = 0;
    
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) 
	{    
        m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
        m_Entries[i].BaseVertex = NumVertices;
        m_Entries[i].BaseIndex = NumIndices;
        NumVertices += pScene->mMeshes[i]->mNumVertices;
        NumIndices  += m_Entries[i].NumIndices;
    }
    
    Positions.reserve(NumVertices);
    Indices.reserve(NumIndices);

    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) 
	{
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(paiMesh, Positions, Indices);
    }

    // Generate and populate the buffers with vertex attributes and the indices
  	glBindBuffer(GL_ARRAY_BUFFER, buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);
    
    return true;
}
void LightMesh::InitMesh(const aiMesh* paiMesh, vector<glm::vec3>& Positions, vector<unsigned int>& Indices)
{    
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
    for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) 
	{
        const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
        Positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
    }
    for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; i++) 
	{
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }
}
void LightMesh::Bind()
{
  	glBindBuffer(GL_ARRAY_BUFFER, buffers[POS_VB]);
    glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[INDEX_BUFFER]);
}
void LightMesh::Render()
{
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++)
	{
		glDrawElementsBaseVertex(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * m_Entries[i].BaseIndex), m_Entries[i].BaseVertex);
	}
}
void LightMesh::Unbind()
{
	glDisableVertexAttribArray(0);
  	glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
