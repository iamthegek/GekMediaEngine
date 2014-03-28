#include "Shader.h"
#include <GL\glew.h>
#include <fstream>
#include <iostream>

void LoadShaderSource(const char* path, std::string& source){
	std::ifstream in(path);
	if(!in.is_open())
	{
		std::cout << "Error opening: " << path << std::endl;
		return;
	}
	char tmp[300];
	while(!in.eof())
	{
		in.getline(tmp, 300);
		source += tmp;
		source += '\n';
	}
}
unsigned int CreateShader(std::string& source, unsigned int which, const char* logname){
	unsigned int id = glCreateShader(which);
	const char* csource = source.c_str();

	glShaderSource(id,1,&csource,NULL);
	glCompileShader(id);

	char log[1024];
	glGetShaderInfoLog(id,1024,NULL,log);
	std::cout << '\"' << logname << "\": " << log << std::endl;

	return id;
}

Shader::Shader()
{
	program = 0;
}
Shader::~Shader()
{
	glDetachShader(program, frag);
	glDetachShader(program, vert);
	glDeleteShader(frag);
	glDeleteShader(vert);
	glDeleteProgram(program);
}
bool Shader::Load(const char* in_vert, const char* in_frag)
{
	if(program != 0)
		return false;

	std::string source;
	LoadShaderSource(in_vert, source);
	vert = CreateShader(source, GL_VERTEX_SHADER, in_vert);

	source = "";
	LoadShaderSource(in_frag, source);
	frag = CreateShader(source, GL_FRAGMENT_SHADER, in_frag);

	program = glCreateProgram();
	glAttachShader(program, vert);
	glAttachShader(program, frag);

	glBindFragDataLocation(program, 0, "Color");
	glBindFragDataLocation(program, 1, "Normal");

	glLinkProgram(program);
	glUseProgram(program);
	return true;
}
void Shader::Bind()
{
	glUseProgram(program);
}
void Shader::Unbind()
{
	glUseProgram(0);
}
unsigned int Shader::GetProgram()
{
	return program;
}
void Shader::Locate(const char* n)
{
	uniformLocations.push_back(glGetUniformLocation(program, n));
}
unsigned int Shader::GetLocation(const unsigned int& index)
{
	return uniformLocations[index];
}
void Shader::Send(const unsigned int& i, const int& var)
{
	glUniform1i(uniformLocations[i], var);
}
void Shader::Send(const unsigned int& i, const float& var)
{
	glUniform1f(uniformLocations[i], var);
}
void Shader::Send(const unsigned int& i, const glm::vec2& var)
{
	glUniform2f(uniformLocations[i], var.x, var.y);
}
void Shader::Send(const unsigned int& i, const glm::vec3& var)
{
	glUniform3f(uniformLocations[i], var.x, var.y, var.z);
}
void Shader::Send(const unsigned int& i, const glm::mat4& var)
{
	glUniformMatrix4fv(uniformLocations[i], 1, GL_FALSE, &var[0][0]);
}
void Shader::Send(const unsigned int& i, std::vector<glm::mat4>& var)
{
	glUniformMatrix4fv(uniformLocations[i], var.size(), GL_FALSE, &var[0][0][0]);
}