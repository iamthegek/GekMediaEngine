#ifndef __GBUFFER__H
#define __GBUFFER__H

#include <GL\glew.h>
#include <cassert>

class GBuffer
{
public:
    enum GBufTextureTypes 
	{
		GBUFFER_POSITION=0,
		GBUFFER_DIFFUSE=1,
		GBUFFER_NORMAL=2,
		GBUFFER_NUM_TEXTURES=3
    };

    GBuffer();
	~GBuffer() 
	{
		Clear();
	}

	void Clear();
    bool Build(GLsizei width, GLsizei height);

    void BindForGeomPass();
    void BindForStencilPass();
    void BindForLightPass();
    void BindForFinalPass();

	GLuint GetDepthTexture() const
	{
		return depthStencilTexture;
	}
	GLuint GetTexture(const int& i) const
	{
		assert(i < GBUFFER_NUM_TEXTURES);
		return textures[i];
	}

private:
    GLuint fbo;
    GLuint textures[GBUFFER_NUM_TEXTURES];
	GLuint depthStencilTexture;
    GLuint finalTexture;
};

#endif