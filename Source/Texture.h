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
		TYPE_NORMAL,
		TYPE_CUBEMAP
	};

	Texture(const std::string& Path, GLint Type = ModelTextureTypes::TYPE_COLOR);
	Texture(const std::string& positveX,
			const std::string& negativeX,
			const std::string& positveY,
			const std::string& negativeY,
			const std::string& positveZ,
			const std::string& negativeZ); //cubemap constructer

	~Texture();

	void Bind(GLenum ActiveTexture = GL_TEXTURE0);
	void Unbind(GLenum ActiveTexture = GL_TEXTURE0);
	GLuint GetId();
	GLint GetType();

protected:
	GLuint textureId;
	GLenum bindTarget;
	GLint type;
};

#endif