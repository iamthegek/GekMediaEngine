#include "Light.h"

PointLight::PointLight(Shader& shdr) : Light(Light::POINT_LIGHT)
{
	uniLocColor			= glGetUniformLocation(shdr.GetProgram(), "LightColor");
	uniLocRadius		= glGetUniformLocation(shdr.GetProgram(), "LightRadius");
	uniLocPosition		= glGetUniformLocation(shdr.GetProgram(), "LightPosition");
	uniLocEyePosition	= glGetUniformLocation(shdr.GetProgram(), "EyeWorldPosition");
	uniLocMvpMatrix		= glGetUniformLocation(shdr.GetProgram(), "ModelViewProjectionMatrix");

	radius = 10.0f;
	color = glm::vec3(1,1,1);
	position = glm::vec3(0,0,0);
}
PointLight::~PointLight(){}
PointLight* PointLight::SetPosition(const glm::vec3& p)
{
	position = p;
	return this;
}
PointLight* PointLight::SetRadius(const float& r)
{
	radius = r;
	return this;
}
PointLight* PointLight::SetColor(const glm::vec3& c)
{
	color = c;
	return this;
}
void PointLight::SendToShader(Matrices& in_mat, const glm::vec3& eyePosition)
{
	in_mat.SetMatrixMode(Matrices::MODEL_MATRIX);
	in_mat.PushMatrix();
	in_mat.Translate(position.x, position.y, position.z);
	in_mat.Scale(radius,radius,radius);
	glUniformMatrix4fv(uniLocMvpMatrix, 1, GL_FALSE, &in_mat.GetModelViewProjectionMatrix()[0][0]);		//Send MVP Matrix
	glUniform1f(uniLocRadius, radius);																	//Send Light Radius
	glUniform3f(uniLocColor, color.x, color.y, color.z);												//Send Light Color
	glUniform3f(uniLocEyePosition, eyePosition.x, eyePosition.y, eyePosition.z);						//Send Eye Position
	glUniform3f(uniLocPosition, position.x, position.y, position.z);									//Send Light Position
	in_mat.PopMatrix();
}

DirectionalLight::DirectionalLight(Shader& shdr) : Light(DIRECTIONAL_LIGHT)
{
	uniLocMvpMatrix		= glGetUniformLocation(shdr.GetProgram(), "ModelViewProjectionMatrix");
	uniLocColor			= glGetUniformLocation(shdr.GetProgram(), "LightColor");
	uniLocDir			= glGetUniformLocation(shdr.GetProgram(), "LightDirection");
	uniLocEyePosition	= glGetUniformLocation(shdr.GetProgram(), "EyeWorldPosition");

	color = glm::vec3(1,1,1);
	direction = glm::normalize(glm::vec3(1,-2,0));
}
DirectionalLight::~DirectionalLight(){}
DirectionalLight* DirectionalLight::SetDirection(const glm::vec3& dir)
{
	direction = glm::normalize(dir);
	return this;
}
DirectionalLight* DirectionalLight::SetColor(const glm::vec3& col)
{
	color = col;
	return this;
}
void DirectionalLight::SendToShader(Matrices& in_mat, glm::vec3 eyePosition)
{
	in_mat.SetMatrixMode(Matrices::VIEW_MATRIX);
	in_mat.PushMatrix();
	in_mat.LoadIdentity();

	in_mat.SetMatrixMode(Matrices::MODEL_MATRIX);
	in_mat.PushMatrix();
	in_mat.LoadIdentity();

	in_mat.SetOrtho(-1,1,-1,1,0,1);

	glUniformMatrix4fv(uniLocMvpMatrix, 1, GL_FALSE, &in_mat.GetModelViewProjectionMatrix()[0][0]);
	glUniform3f(uniLocColor, color.x, color.y, color.z);
	glUniform3f(uniLocDir, direction.x, direction.y, direction.z);
	glUniform3f(uniLocEyePosition, eyePosition.x, eyePosition.y, eyePosition.z);

	//in_mat.SetPerspective(90.0, 16.0/9.0, 1.0, 1000.0); not necessary due to UpdateCamera() function

	in_mat.PopMatrix(); //model

	in_mat.SetMatrixMode(Matrices::VIEW_MATRIX);
	in_mat.PopMatrix(); //view

	in_mat.SetMatrixMode(Matrices::MODEL_MATRIX);
}