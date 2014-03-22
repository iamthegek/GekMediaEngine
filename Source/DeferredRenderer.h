#ifndef __DEFREND__H
#define __DEFREND__H

#include <GL\glew.h>
#include "GBuffer.h"
#include "Matrices.h"
#include "Texture.h"
#include "Shader.h"
#include "Light.h"
#include "LightMesh.h"
#include "ScreenQuadMesh.h"
#include "StaticMesh.h"
#include "BoneMesh.h"
#include "Particle.h"
#include "ShadowMap.h"
#include <vector>

typedef std::vector<GLint> UniformLocations;

enum CLEAR_TYPE
{
	GME_POINT_LIGHTS,
	GME_ALL_LIGHTS,
	GME_STATIC_MESHES,
	GME_BONE_MESHES,
	GME_ALL_MESHES,
	GME_PARTICLE_EMITTERS,
	GME_ALL
};

class DeferredRenderer
{
public:
	DeferredRenderer();
	~DeferredRenderer();

	void Clear(const int& flag);
	void Build(const GLsizei& width, 
			   const GLsizei& height, 
			   const GLfloat& fov, 
			   const GLfloat& neardist, 
			   const GLfloat& fardist);
	void SetFieldOfView(const GLfloat& fov);
	void SetShadowMapResolution(const int& resolution);

	PointLight * NewPointLight(const glm::vec3& color, const glm::vec3& position, const float& radius);
	ParticleEmitter * NewParticleEmitter(const std::string& Filename, const glm::vec3& Force);
	StaticMesh * NewStaticMesh(const std::string& Filename);
	BoneMesh * NewBoneMesh(const std::string& Filename);
	void SetGlobalLightDirection(const glm::vec3& direction);

	void LookAt(const glm::vec3& eye, 
				const glm::vec3& target, 
				const glm::vec3& up);
	Matrices& GetMatrices();

	void Update(const float& deltaTime);

	bool DoCloseShadows();
	bool DoMediumShadows();
	bool DoFarShadows();
	void DrawToShadowBuffer(StaticMesh* Mesh, const glm::mat4& objMatrix);
	void DrawToShadowBuffer(BoneMesh* Mesh, const float& time, const glm::mat4& objMatrix);

	void SetUpGBuffer();
	void DrawToGBuffer(StaticMesh* Mesh, const glm::mat4& objMatrix);
	void DrawToGBuffer(BoneMesh* Mesh, const float& time, const glm::mat4& objMatrix);
	void RenderToScreen();
	void ForwardRenderParticles();

private:
	//Deferred GBuffer 
	void DoLightStencil(PointLight* light);
	void DoLightPass(PointLight* light);
	void DoDirectionalLight();
	void DoShadowPass();
	void DoFinalPass();

	//Clear
	void ClearPointLights();
	void ClearBoneMeshes();
	void ClearStaticMeshes();
	void ClearParticleEmitters();

private:
	//Main Variables
	GBuffer * gBuffer;
	GLsizei windowWidth, windowHeight;
	GLfloat fieldOfView, nearRenderDistance, farRenderDistance, windowRatio;

	//World Variables
	Matrices worldMatrices;
	glm::mat4 worldViewMatrix;
	glm::vec3 camPosition, camDirection;

	//Point Light Variables
	LightMesh * ptLightMesh;
	std::vector<PointLight*> ptLights;
	Shader ptLightShader;
	UniformLocations ptLightUniformLocs;

	//Light Stenciling Varables
	Shader stencilShader;
	GLint uniLocStencilMVP;

	//Directional Light Variables
	DirectionalLight * dirLight;
	Shader dirLightShader;
	ScreenQuadMesh * dirLightMesh;
	UniformLocations dirLightUniformLocs;

	//Static Mesh Rendering Variables
	Shader sMeshShader;
	std::vector<StaticMesh*> sMeshes;
	UniformLocations sMeshUniformLocs;

	//Bone Mesh Rendering Variables
	Shader bnMeshShader;
	std::vector<BoneMesh*> bnMeshes;
	UniformLocations bnMeshUniformLocs;

	//Particle Variables
	std::vector<ParticleEmitter*> particleEmitters;
	Shader particleShader;
	UniformLocations particleUniformLocs;

	//Cascaded Shadow Map Variables
	Shader cShadowSMeshShader;
	Shader cShadowBnMeshShader;
	UniformLocations cShadowSMeshUniformLocs;
	UniformLocations cShadowBnMeshUniformLocs;
	CascadedShadowMap cShadowMap;
	int frameCounter, curShadowLevel;
	Shader cShadowPassShader;
	ScreenQuadMesh * cShadowPassMesh;
	UniformLocations cShadowPassUniformLocs;
};

#endif