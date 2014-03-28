#ifndef __DEFREND__H
#define __DEFREND__H

#include <GL\glew.h>
#include "GBuffer.h"
#include "Texture.h"
#include "Shader.h"
#include "Light.h"
#include "LightMesh.h"
#include "ScreenQuadMesh.h"
#include "StaticMesh.h"
#include "BoneMesh.h"
#include "ShadowMap.h"
#include <vector>

class DeferredBuffers
{
public:
	DeferredBuffers();
	~DeferredBuffers();

	void ClearPointLights();
	void Build(const GLsizei& width, 
			   const GLsizei& height, 
			   const GLfloat& fov, 
			   const GLfloat& neardist, 
			   const GLfloat& fardist);
	void SetFieldOfView(const GLfloat& fov);
	void SetShadowMapResolution(const int& resolution);

	PointLight * NewPointLight(const glm::vec3& color, const glm::vec3& position, const float& radius);
	void SetGlobalLightDirection(const glm::vec3& direction);
	void SetGlobalLightColor(const glm::vec3& color);
	void LoadSkybox(const std::string& positiveX, const std::string& negativeX,
					const std::string& positiveY, const std::string& negativeY,
					const std::string& positiveZ, const std::string& negativeZ);

	void LookAt(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up);
	glm::mat4& GetProjectionMatrix() { return projectionMatrix; }
	glm::mat4& GetViewMatrix() { return viewMatrix; }
	CascadedShadowMap * GetShadowMap();

	bool DoShadows(const GLint& which);
	void SetUpGBuffer();
	void RenderToScreen();

	glm::vec3 camPosition, camDirection;

private:
	//Deferred
	void DoLightStencil(PointLight* light);
	void DoLightPass(PointLight* light);
	void DoDirectionalLight();
	void DoShadowPass();
	void DoSkybox();
	void DoFinalPass();

private:
	//Main Variables
	GBuffer * gBuffer;
	GLsizei windowWidth, windowHeight;
	GLfloat fieldOfView, nearRenderDistance, farRenderDistance, windowRatio;

	//World Variables
	glm::mat4 projectionMatrix, viewMatrix;

	//Point Light Variables
	LightMesh * ptLightMesh;
	std::vector<PointLight*> ptLights;
	Shader ptLightShader;

	//Light Stenciling Varables
	Shader stencilShader;

	//Directional Light Variables
	DirectionalLight * dirLight;
	Shader dirLightShader;
	ScreenQuadMesh * dirLightMesh;

	//Cascaded Shadow Map Variables
	CascadedShadowMap cShadowMap;
	int frameCounter;
	Shader cShadowPassShader;
	ScreenQuadMesh * cShadowPassMesh;

	//Skybox variables
	Shader skyboxShader;
	LightMesh * skyboxMesh;
	Texture * skyboxCubemap;
};

#endif