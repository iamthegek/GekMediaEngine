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
		aiMatrix4x4 BoneControl;
	    aiMatrix4x4 BoneOffset;
	    aiMatrix4x4 FinalTransformation; 
	    BoneInfo() 	{ }
	};

	BoneMesh();
	~BoneMesh();
	
	bool LoadMesh(const std::string& Filename);
	
	unsigned int GetNumBones() const;
	unsigned int GetBoneIndex(std::string BoneName);
	BoneInfo* GetBoneInfo(unsigned int index);
	glm::mat4 GetBoneMatrix(unsigned int index);
	void InterpolateBones(float TimeInSeconds, std::vector<glm::mat4>& Transforms);

	void Render();

private:
	struct VertexBoneData 
	{        
	    unsigned int IDs[NUM_BONES_PER_VEREX];
	    float Weights[NUM_BONES_PER_VEREX];
	    VertexBoneData()
		{
			Reset();
		}
	    void Reset()
		{
	        ZERO_MEM(IDs);
	        ZERO_MEM(Weights);        
	    }
	    void AddBoneData(unsigned int BoneID, float Weight);
	};
	
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);    
	
	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);

	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform);
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
  
    GLuint m_VAO;
	GLuint m_Buffers[NUM_BONE_MESH_VBs];

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
    
    std::vector<MeshEntry> meshEntries;
     
    std::map<std::string,unsigned int> boneMapping; // maps a bone name to its index
    unsigned int numBones;
    std::vector<BoneInfo> boneInfo;
    aiMatrix4x4 globalInverseTransform;
    
    const aiScene* assimpScene;
    Assimp::Importer assimpImporter;
};

#endif