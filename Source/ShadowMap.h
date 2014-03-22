#ifndef __SHADOW_MAP__H
#define __SHADOW_MAP__H

#include "ShadowBuffer.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

enum ShadowResolutions
{
	S_2048x2048,
	S_1024x1024,
	S_512x512
};

class CascadedShadowMap
{
public:
	CascadedShadowMap();
	~CascadedShadowMap();

	void SetUp(const int& resolution);
	void Start(const int& index);
	void Stop(const int& index);
	GLuint GetDepthTexture(const int& index);
	glm::mat4& GetLightMatrix(const int& index);
	glm::mat4& GetLightProjectionMatrix(const int& index);
	glm::mat4& GetLightViewMatrix(const int& index);
	void SetLightDirection(const glm::vec3& dir);
	void UpdateMatrices(const glm::mat4& cmView,
						const glm::vec3& cDir,
						const glm::vec3& cPos);

private:
	ShadowBuffer buf[3];

	glm::vec3 lightDirection;
	glm::mat4 lightMatrix[3];			//MVP
	glm::mat4 lightProjectionMatrix[3];	//P
	glm::mat4 lightViewMatrix[3];		//V
};

#endif