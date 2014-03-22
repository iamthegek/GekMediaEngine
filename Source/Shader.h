#ifndef __SHADER__H
#define __SHADER__H

#include <string>
#include <vector>

class Shader
{
private:
	unsigned int program, vert, frag;

public:
	Shader();
	~Shader();
	bool Load(const char* in_vert, const char* in_frag);
	void Bind();
	void Unbind();
	unsigned int GetProgram();
};

#endif