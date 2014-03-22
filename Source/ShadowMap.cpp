#include "ShadowMap.h"

CascadedShadowMap::CascadedShadowMap()
{
	lightProjectionMatrix[0] = glm::ortho(-35.0f,  35.0f,  -35.0f,  35.0f,  250.0f, 800.0f);
	lightProjectionMatrix[1] = glm::ortho(-200.0f, 200.0f, -200.0f, 200.0f, 100.0f, 2200.0f);
	lightProjectionMatrix[2] = glm::ortho(-800.0f, 800.0f, -800.0f, 800.0f, 100.0f, 4000.0f);
}

CascadedShadowMap::~CascadedShadowMap()
{
}

void CascadedShadowMap::SetUp(const int& resolution)
{
	switch(resolution)
	{
		case ShadowResolutions::S_2048x2048:
			buf[0].SetUp(2048, 2048);
			buf[1].SetUp(2048, 2048);
			buf[2].SetUp(2048, 2048);
			break;

		case ShadowResolutions::S_1024x1024:
			buf[0].SetUp(1024, 1024);
			buf[1].SetUp(1024, 1024);
			buf[2].SetUp(1024, 1024);
			break;

		case ShadowResolutions::S_512x512:
			buf[0].SetUp(512, 512);
			buf[1].SetUp(512, 512);
			buf[2].SetUp(512, 512);
			break;
	}
}

void CascadedShadowMap::Start(const int& index)
{
	buf[index].Start();
}

void CascadedShadowMap::Stop(const int& index)
{
	buf[index].Stop();
}

GLuint CascadedShadowMap::GetDepthTexture(const int& index)
{
	return buf[index].GetDepthTexture();
}

glm::mat4& CascadedShadowMap::GetLightMatrix(const int& index)
{
	return lightMatrix[index];
}

glm::mat4& CascadedShadowMap::GetLightProjectionMatrix(const int& index)
{
	return lightProjectionMatrix[index];
}

glm::mat4& CascadedShadowMap::GetLightViewMatrix(const int& index)
{
	return lightViewMatrix[index];
}

void CascadedShadowMap::SetLightDirection(const glm::vec3& dir)
{
	lightDirection = glm::normalize(dir);
}

void CascadedShadowMap::UpdateMatrices(	const glm::mat4& cView,
										const glm::vec3& cDir,
										const glm::vec3& cPos)
{
	glm::mat4 camInv = glm::inverse(cView);
	glm::vec3 camDirection  = glm::normalize(glm::vec3(cDir.x, 0, cDir.z));
	glm::vec3 up = glm::vec3(0,1,0);

	//Shadow 0
	glm::vec3 eye = lightDirection * -500.0f + cPos - camDirection * 35.0f;
	glm::vec3 lookAt = cPos - camDirection * 35.0f;
	lightViewMatrix[0] = glm::lookAt(eye, lookAt, up);

	//Shadow 1
	eye = lightDirection * -500.0f + cPos - camDirection * 200.0f;
	lookAt = cPos - camDirection * 200.0f;
	lightViewMatrix[1] = glm::lookAt(eye, lookAt, up);

	//Shadow 2
	eye = lightDirection * -2000.0f + cPos - camDirection * 1000.0f;
	lookAt = cPos - camDirection * 1000.0f;
	lightViewMatrix[2] = glm::lookAt(eye, lookAt, up);

	//Shadow Matrices
	lightMatrix[0] = lightProjectionMatrix[0] * lightViewMatrix[0] * camInv; //ORDER COULD BE WRONG
	lightMatrix[1] = lightProjectionMatrix[1] * lightViewMatrix[1] * camInv; //ORDER COULD BE WRONG
	lightMatrix[2] = lightProjectionMatrix[2] * lightViewMatrix[2] * camInv; //ORDER COULD BE WRONG
}