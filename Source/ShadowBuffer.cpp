#include "ShadowBuffer.h"
#include <iostream>

ShadowBuffer::ShadowBuffer()
{
	width = height = 0;
	fbo = 0;
	depthTexture = 0;
}

ShadowBuffer::~ShadowBuffer()
{
	if(depthTexture != 0)
		glDeleteTextures(1, &depthTexture);
	if(fbo != 0)
		glDeleteFramebuffers(1, &fbo);
}

void ShadowBuffer::SetUp(const int& w, const int& h)
{
	width = w;
	height = h;
	if(fbo == 0)
		glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
 
	if(depthTexture == 0)
	    glGenTextures( 1, &depthTexture );
	glBindTexture( GL_TEXTURE_2D, depthTexture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE );
    glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );    
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0 );
	glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,  depthTexture, 0 );
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Shadow Buffer Error, " << status << std::endl;
    
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
    
void ShadowBuffer::Start()
{
	glBindFramebuffer(GL_FRAMEBUFFER,fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);
}

void ShadowBuffer::Stop()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint ShadowBuffer::GetDepthTexture()
{
	return depthTexture;
}