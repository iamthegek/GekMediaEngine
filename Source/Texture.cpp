#include "Texture.h"

#include <SFML\Graphics.hpp>
#include <iostream>

Texture::Texture(const std::string& Path, GLint Type)
{
	type = Type;
	bindTarget = GL_TEXTURE_2D;

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	
	sf::Image img;
	if(!img.loadFromFile(Path))	
	{
		std::cout << "error: failed to load (\"" << Path << "\")\n";
		textureId = 0;
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.getSize().x, img.getSize().y, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
}
Texture::Texture(const std::string& positiveX,
				const std::string& negativeX,
				const std::string& positiveY,
				const std::string& negativeY,
				const std::string& positiveZ,
				const std::string& negativeZ)
{
	type = Texture::TYPE_CUBEMAP;
	bindTarget = GL_TEXTURE_CUBE_MAP;
	sf::Image img;
	std::string paths[6] = {positiveX, negativeX, positiveY, negativeY, positiveZ, negativeZ};

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	for(int i = 0; i < 6; i++)
	{
		if(!img.loadFromFile(paths[i]))
		{
			std::cout << "error: failed to load (\"" << paths[i] << "\")\n";
			textureId = 0;
		}
		else
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
		}
	}

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);
}
Texture::~Texture()
{
	if(textureId != 0)
		glDeleteTextures(1, &textureId);
}

void Texture::Bind(GLenum ActiveTexture)
{
	glActiveTexture(ActiveTexture);
	glBindTexture(bindTarget, textureId);
}
void Texture::Unbind(GLenum ActiveTexture)
{
	glActiveTexture(ActiveTexture);
	glBindTexture(bindTarget, 0);
}
GLuint Texture::GetId()
{
	return textureId;
}
GLint Texture::GetType()
{
	return type;
}