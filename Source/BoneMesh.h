#ifndef __BONEMESH__H
#define __BONEMESH__H

//adapted from http://ogldev.atspace.co.uk/www/tutorial38/tutorial38.html

#include <map>
#include <vector>
#include <assert.h>
#include <GL/glew.h>
#include <assimp/Importer.hpp>  
#include <assimp/scene.h>
#include <assimp/postprocess.h> 
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include <string>

#include "MeshCommon.h"
#include "Texture.h"

extern glm::mat4 AiMatrixToGlmMatrix(const aiMatrix4x4& m);
extern aiMatrix4x4 GlmMatrixToAiMatrix(const glm::mat4& m);

class BoneMesh
{
public:
	struct BoneInfo 
	{
		aiMatrix4x4 BoneControl, BoneOffset, FinalTransformation; 
	    BoneInfo() 	{ }
	};

	BoneMesh();
	~BoneMesh();
	
	bool LoadMesh(const std::string& Filename);
	
	unsigned int GetNumBones() const;
	unsigned int GetBoneIndex(const std::string& BoneName);
	BoneInfo* GetBoneInfo(const GLuint& index);
	glm::mat4 GetBoneMatrix(const GLuint& index);
	void InterpolateBones(const GLfloat& TimeInSeconds, std::vector<glm::mat4>& Transforms);

	void Render(const GLuint& FLAGS = 0x00);

private:
	struct VertexBoneData 
	{        
	    GLuint IDs[NUM_BONES_PER_VEREX];
	    GLfloat Weights[NUM_BONES_PER_VEREX];
	    VertexBoneData()
		{
			Reset();
		}
	    void Reset()
		{
	        ZERO_MEM(IDs);
	        ZERO_MEM(Weights);        
	    }
	    void AddBoneData(const GLuint& BoneID, const GLfloat& Weight);
	};
	
	void CalcInterpolatedScaling(aiVector3D& Out, const GLfloat& AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, const GLfloat& AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, const GLfloat& AnimationTime, const aiNodeAnim* pNodeAnim);    
	
	unsigned int FindScaling(const GLfloat& AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindRotation(const GLfloat& AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindPosition(const GLfloat& AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);

	void ReadNodeHeirarchy(const GLfloat& AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform);
	bool InitFromScene(const aiScene* pScene, const std::string& Filename);
	void InitMesh(unsigned int MeshIndex, 
				  const aiMesh* paiMesh, 
				  std::vector<glm::vec3>& Positions, 
				  std::vector<glm::vec3>& Normals, 
				  std::vector<glm::vec2>& TexCoords, 
				  std::vector<glm::vec3>& Tangents,
				  std::vector<VertexBoneData>& Bones, 
				  std::vector<unsigned int>& Indices);
	void LoadBones(unsigned int MeshIndex, const aiMesh* paiMesh, std::vector<VertexBoneData>& Bones);
	void Clear();
 
	GLuint vertexBuffers[NUM_BONE_MESH_VBs];

    struct MeshEntry 
	{
		MeshEntry() : numIndices(0), baseIndex(0), baseVert(0) {}
        GLuint numIndices, baseVert, baseIndex;
    };
    
    std::vector<MeshEntry> meshEntries;
     
    std::map<std::string,GLuint> boneMapping; // maps a bone name to its index
    GLuint numBones;
    std::vector<BoneInfo> boneInfo;
    aiMatrix4x4 globalInverseTransform;
    
    const aiScene* assimpScene;
    Assimp::Importer assimpImporter;
};

#endif