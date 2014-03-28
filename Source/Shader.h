#ifndef __SHADER__H
#define __SHADER__H

#include <string>
#include <vector>
#include <glm\glm.hpp>

class Shader
{
private:
	unsigned int program, vert, frag;
	std::vector<unsigned int> uniformLocations;

public:
	Shader();
	~Shader();
	
	bool Load(const char* in_vert, const char* in_frag);
	void Bind();
	void Unbind();
	unsigned int GetProgram();

	void Locate(const char* n);
	unsigned int GetLocation(const unsigned int& index);
	void Send(const unsigned int& i, const int& var);
	void Send(const unsigned int& i, const float& var);
	void Send(const unsigned int& i, const glm::vec2& var);
	void Send(const unsigned int& i, const glm::vec3& var);
	void Send(const unsigned int& i, const glm::mat4& var);
	void Send(const unsigned int& i, std::vector<glm::mat4>& var);
};

#endif