#ifndef __SHADOW_BUFFER__H
#define __SHADOW_BUFFER__H

#include <GL\glew.h>

class ShadowBuffer
{
public:
	ShadowBuffer();
	~ShadowBuffer();

	void SetUp(const int& w, const int& h);
	void Start();
	void Stop();
	GLuint GetDepthTexture();

private:
	GLuint fbo;
	GLuint depthTexture;
	GLint width, height;
};

#endif