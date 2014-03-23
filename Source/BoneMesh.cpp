#include "BoneMesh.h"
#include <assert.h>
#include <iostream>

glm::mat4 AiMatrixToGlmMatrix(const aiMatrix4x4& m)
{
	glm::mat4 ret;
	ret[0] = glm::vec4(m.a1, m.a2, m.a3, m.a4); //1st column of ret = 1st row of m
	ret[1] = glm::vec4(m.b1, m.b2, m.b3, m.b4); //2nd column of ret = 2nd row of m
	ret[2] = glm::vec4(m.c1, m.c2, m.c3, m.c4); //3rd column of ret = 3rd row of m
	ret[3] = glm::vec4(m.d1, m.d2, m.d3, m.d4); //4th column of ret = 4th row of m

	return glm::transpose(ret);
}

aiMatrix4x4 GlmMatrixToAiMatrix(const glm::mat4& m)
{
	glm::vec4 c0 = m[0];
	glm::vec4 c1 = m[1];
	glm::vec4 c2 = m[2];
	glm::vec4 c3 = m[3];

	aiMatrix4x4 ret;
	ret.a1 = c0[0]; ret.a2 = c1[0]; ret.a3 = c2[0]; ret.a4 = c3[0];
	ret.b1 = c0[1]; ret.b2 = c1[1]; ret.b3 = c2[1]; ret.b4 = c3[1];
	ret.c1 = c0[2]; ret.c2 = c1[2]; ret.c3 = c2[2]; ret.c4 = c3[2];
	ret.d1 = c0[3]; ret.d2 = c1[3]; ret.d3 = c2[3]; ret.d4 = c3[3];
	return ret;
}

void BoneMesh::VertexBoneData::AddBoneData(unsigned int BoneID, float Weight)
{
    for (unsigned int i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(IDs) ; i++)
	{
        if (Weights[i] == 0.0) 
		{
            IDs[i]     = BoneID;
            Weights[i] = Weight;
            return;
        }       
	}
	//error
    assert(0);
}

BoneMesh::BoneMesh()
{
    m_VAO = 0;
    ZERO_MEM(m_Buffers);
    numBones = 0;
    assimpScene = NULL;
	textures[0] = textures[1] = 0;
}
BoneMesh::~BoneMesh()
{
	Clear();
}
void BoneMesh::Clear()
{
    if (m_Buffers[0] != 0) 
		glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
    if (m_VAO != 0) 
		glDeleteVertexArrays(1, &m_VAO);
    m_VAO = 0;

	SAFE_DELETE(textures[0]);
	SAFE_DELETE(textures[1]);
}
void BoneMesh::AddColorTexture(const std::string& Filename)
{
	if(textures[0] != 0)
		delete textures[0];
	textures[0] = new Texture(Filename, Texture::ModelTextureTypes::TYPE_COLOR);
}
void BoneMesh::AddNormalTexture(const std::string& Filename)
{
	if(textures[1] != 0)
		delete textures[1];
	textures[1] = new Texture(Filename, Texture::ModelTextureTypes::TYPE_NORMAL);
}

unsigned int BoneMesh::GetNumBones() const 
{ 
	return numBones; 
}

unsigned int BoneMesh::GetBoneIndex(std::string BoneName) 
{
	return boneMapping[BoneName];
}

glm::mat4 BoneMesh::GetBoneMatrix(unsigned int index)
{
	return AiMatrixToGlmMatrix(boneInfo[index].FinalTransformation);
}

BoneMesh::BoneInfo* BoneMesh::GetBoneInfo(unsigned int index)
{
	return &boneInfo[index];
}

bool BoneMesh::LoadMesh(const std::string& Filename)
{
	Clear();
	glGenVertexArrays(1, &m_VAO);   
	glBindVertexArray(m_VAO);
	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
	bool Ret = false;    
	assimpScene = assimpImporter.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
	if (assimpScene) {  
		globalInverseTransform = assimpScene->mRootNode->mTransformation.Inverse();
	    Ret = InitFromScene(assimpScene, Filename);
	}
	else 
	    printf("Error parsing '%s': '%s'\n", Filename.c_str(), assimpImporter.GetErrorString());
	glBindVertexArray(0);	
	return Ret;
}
bool BoneMesh::InitFromScene(const aiScene* pScene, const std::string& Filename)
{  
    meshEntries.resize(pScene->mNumMeshes);

    std::vector<glm::vec3> Positions;
    std::vector<glm::vec3> Normals;
    std::vector<glm::vec2> TexCoords;
	std::vector<glm::vec3> Tangents;
    std::vector<VertexBoneData> Bones;
    std::vector<unsigned int> Indices;
       
    unsigned int NumVertices = 0;
    unsigned int NumIndices = 0;

    for (unsigned int i = 0 ; i < meshEntries.size() ; i++) 
	{     
		meshEntries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		meshEntries[i].BaseVertex = NumVertices;
		meshEntries[i].BaseIndex  = NumIndices;
		
		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices  += meshEntries[i].NumIndices;
    }
    
    Positions.reserve(NumVertices);
    Normals.reserve(NumVertices);
    TexCoords.reserve(NumVertices);
	Tangents.reserve(NumVertices);
    Bones.resize(NumVertices);
    Indices.reserve(NumIndices);
        
    for (unsigned int i = 0 ; i < meshEntries.size() ; i++) 
	{
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(i, paiMesh, Positions, Normals, TexCoords, Tangents, Bones, Indices);
    }

  	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);    

    glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(TEX_COORD_LOCATION);
    glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

   	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(NORMAL_LOCATION);
    glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TANGENT_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Tangents[0]) * Tangents.size(), &Tangents[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TANGENT_LOCATION);
    glVertexAttribPointer(TANGENT_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

   	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(BONE_ID_LOCATION);
    glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);    
    glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

    return true;
}
void BoneMesh::InitMesh(unsigned int MeshIndex, 
						const aiMesh* paiMesh, 
						std::vector<glm::vec3>& Positions, 
						std::vector<glm::vec3>& Normals, 
						std::vector<glm::vec2>& TexCoords, 
						std::vector<glm::vec3>& Tangents, 
						std::vector<VertexBoneData>& Bones, 
						std::vector<unsigned int>& Indices)
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
    LoadBones(MeshIndex, paiMesh, Bones);
    for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; i++) 
	{
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }
}
void BoneMesh::LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones)
{
    for (unsigned int i = 0 ; i < pMesh->mNumBones ; i++) 
	{                
        unsigned int BoneIndex = 0;        
        std::string BoneName(pMesh->mBones[i]->mName.data);
        
        if (boneMapping.find(BoneName) == boneMapping.end()) 
		{
            // Allocate an index for a new bone
            BoneIndex = numBones;
            numBones++;            
	        BoneInfo bi;			
			boneInfo.push_back(bi);
            boneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;            
            boneMapping[BoneName] = BoneIndex;
        }
        else
			BoneIndex = boneMapping[BoneName];              
        
        for (unsigned int j = 0 ; j < pMesh->mBones[i]->mNumWeights ; j++) 
		{
            unsigned int VertexID = meshEntries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
            float Weight  = pMesh->mBones[i]->mWeights[j].mWeight;                   
            Bones[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }    
}
void BoneMesh::Render()
{
    glBindVertexArray(m_VAO);
    for (unsigned int i = 0 ; i < meshEntries.size() ; i++) 
	{
		glDrawElementsBaseVertex(GL_TRIANGLES, meshEntries[i].NumIndices, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * meshEntries[i].BaseIndex), meshEntries[i].BaseVertex);
	}
    glBindVertexArray(0);
}
unsigned int BoneMesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{    
    for (unsigned int i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++)
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
            return i; 
    assert(0);
    return 0;
}
unsigned int BoneMesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);
    for (unsigned int i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) 
        if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) 
            return i;
    assert(0);
    return 0;
}
unsigned int BoneMesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);
    for (unsigned int i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++)
        if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) 
            return i;
    assert(0);
    return 0;
}
void BoneMesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1) 
	{
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }
            
    unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
    unsigned int NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}
void BoneMesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumRotationKeys == 1) 
	{
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }
    
    unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    unsigned int NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;    
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}
void BoneMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1) 
	{
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
    unsigned int NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}
void BoneMesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform)
{
    std::string NodeName(pNode->mName.data);
    const aiAnimation* pAnimation = assimpScene->mAnimations[0]; 
	aiMatrix4x4 NodeTransformation = pNode->mTransformation;
    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
    
    if (pNodeAnim) 
	{
		aiVector3D Scaling;
		aiVector3D Translation;
		aiQuaternion RotationQ;
		aiMatrix4x4 ScaleMatrix, RotationMatrix, TranslationMatrix;

		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);

		aiMatrix4x4::Scaling(Scaling, ScaleMatrix);
		RotationMatrix = aiMatrix4x4(RotationQ.GetMatrix());
		aiMatrix4x4::Translation(Translation, TranslationMatrix);

		NodeTransformation = TranslationMatrix * RotationMatrix * ScaleMatrix;
    }
     
    unsigned int BoneIndex = boneMapping[NodeName];
	BoneInfo* info = &boneInfo[BoneIndex];
	aiMatrix4x4 GlobalTransformation = ParentTransform * NodeTransformation * info->BoneControl;
	info->FinalTransformation = (globalInverseTransform * GlobalTransformation * info->BoneOffset);    
	  
    for (unsigned int i = 0 ; i < pNode->mNumChildren ; i++)
        ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
}
void BoneMesh::InterpolateBones(float TimeInSeconds, std::vector<glm::mat4>& Transforms)
{
    aiMatrix4x4 Identity;
    
    float TicksPerSecond = (float)(assimpScene->mAnimations[0]->mTicksPerSecond != 0 ? assimpScene->mAnimations[0]->mTicksPerSecond : 25.0f);
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float AnimationTime = fmod(TimeInTicks, (float)assimpScene->mAnimations[0]->mDuration);

    ReadNodeHeirarchy(AnimationTime, assimpScene->mRootNode, Identity);

    Transforms.resize(numBones);

    for (unsigned int i = 0 ; i < numBones ; i++)
        Transforms[i] = AiMatrixToGlmMatrix(boneInfo[i].FinalTransformation);
}
const aiNodeAnim* BoneMesh::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
    for (unsigned int i = 0 ; i < pAnimation->mNumChannels ; i++) 
	{
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];
        if (std::string(pNodeAnim->mNodeName.data) == NodeName)
            return pNodeAnim;
    }
    return NULL;
}