#include "Texture.h"

#include <SFML\Graphics.hpp>
#include <iostream>

Texture::Texture(const std::string& Path, GLint Type)
{
	type = Type;
	sf::Image img;
	if(!img.loadFromFile(Path))	
	{
		std::cout << "error: failed to load (\"" << Path << "\")\n";
		textureId = 0;
	}
	else
	{
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
Texture::~Texture()
{
	if(textureId != 0)
		glDeleteTextures(1, &textureId);
}

void Texture::Bind(GLenum ActiveTexture)
{
	glActiveTexture(ActiveTexture);
	glBindTexture(GL_TEXTURE_2D, textureId);
}
void Texture::Unbind(GLenum ActiveTexture)
{
	glActiveTexture(ActiveTexture);
	glBindTexture(GL_TEXTURE_2D, 0);
}
GLuint Texture::GetId()
{
	return textureId;
}
GLint Texture::GetType()
{
	return type;
}