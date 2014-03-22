#ifndef __TEXTURE__H
#define __TEXTURE__H

#include <GL\glew.h>
#include <string>

class Texture
{
public:
	enum ModelTextureTypes
	{
		TYPE_COLOR=0,
		TYPE_NORMAL
	};

	Texture(const std::string& Path, GLint Type = ModelTextureTypes::TYPE_COLOR);
	~Texture();

	void Bind(GLenum ActiveTexture = GL_TEXTURE0);
	static void Unbind(GLenum ActiveTexture = GL_TEXTURE0);
	GLuint GetId();
	GLint GetType();

protected:
	GLuint textureId;
	GLint type;
};

#endif