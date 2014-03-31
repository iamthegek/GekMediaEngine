#include <assert.h>
#include "StaticMesh.h"

using namespace std;

StaticMesh::StaticMesh()
{
	ZERO_MEM(vertexBuffers);
}
StaticMesh::~StaticMesh(){ Clear();}
void StaticMesh::Clear()
{
	if (vertexBuffers[0] != 0)	
		glDeleteBuffers(NUM_STAT_MESH_VBs, vertexBuffers);       
}
bool StaticMesh::LoadMesh(const string& Filename)
{
    Clear();
    glGenBuffers(NUM_STAT_MESH_VBs, vertexBuffers);
    bool Ret = false;
    Assimp::Importer Importer;
	const aiScene* pScene = Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    if (pScene) 
		Ret = InitFromScene(pScene, Filename);
    else		
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());	
    return Ret;
}
bool StaticMesh::InitFromScene(const aiScene* pScene, const string& Filename)
{  
    meshEntries.resize(pScene->mNumMeshes);

    vector<glm::vec3> Positions;
    vector<glm::vec3> Normals;
	vector<glm::vec3> Tangents;
    vector<glm::vec2> TexCoords;
    vector<unsigned int> Indices;

    unsigned int numVerts = 0;
    unsigned int numIndices = 0;
    
    for (unsigned int i = 0 ; i < meshEntries.size() ; i++) {       
        meshEntries[i].numIndices = pScene->mMeshes[i]->mNumFaces * 3;
        meshEntries[i].baseVert = numVerts;
        meshEntries[i].baseIndex = numIndices;
        numVerts += pScene->mMeshes[i]->mNumVertices;
        numIndices  += meshEntries[i].numIndices;
    }
    
    Positions.reserve(numVerts);
    Normals.reserve(numVerts);
    TexCoords.reserve(numVerts);
	Tangents.reserve(numVerts);
    Indices.reserve(numIndices);

    for (unsigned int i = 0 ; i < meshEntries.size() ; i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(paiMesh, Positions, Normals, TexCoords, Tangents, Indices);
    }

    // Generate and populate the vertexBuffers with vertex attributes and the indices
  	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);

   	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[TANGENT_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Tangents[0]) * Tangents.size(), &Tangents[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);
    
    return true;
}
void StaticMesh::InitMesh(const aiMesh* paiMesh, vector<glm::vec3>& Positions, vector<glm::vec3>& Normals, vector<glm::vec2>& TexCoords, std::vector<glm::vec3>& Tangents, vector<unsigned int>& Indices)
{    
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
    for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) 
	{
        const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
		const aiVector3D* pTangent = &(paiMesh->mTangents[i]);

        Positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
        Normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
        TexCoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
		Tangents.push_back(glm::vec3(pTangent->x, pTangent->y, pTangent->z));
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
void StaticMesh::Render(const GLuint& FLAGS)
{
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[POS_VB]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffers[INDEX_BUFFER]);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if(FLAGS & SEND_TEXCOORDS)
	{
	    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[TEXCOORD_VB]);
	    glEnableVertexAttribArray(TEX_COORD_LOCATION);
	    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	if(FLAGS & SEND_NORMALS)
	{
   		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[NORMAL_VB]);
		glEnableVertexAttribArray(NORMAL_LOCATION);
		glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

		if(FLAGS & SEND_TANGENTS)
		{
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers[TANGENT_VB]);
			glEnableVertexAttribArray(TANGENT_LOCATION);
		    glVertexAttribPointer(TANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);
		}
	}

    for (unsigned int i = 0 ; i < meshEntries.size() ; i++)
		glDrawElementsBaseVertex(GL_TRIANGLES, meshEntries[i].numIndices, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * meshEntries[i].baseIndex), meshEntries[i].baseVert);

	glDisableVertexAttribArray(POSITION_LOCATION);
	glDisableVertexAttribArray(TEX_COORD_LOCATION);
	glDisableVertexAttribArray(NORMAL_LOCATION);
	glDisableVertexAttribArray(TANGENT_LOCATION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

