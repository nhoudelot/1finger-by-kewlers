#include "Fbo.h"

using namespace RubberSpace;

Fbo::Fbo(int xres, int yres, GLenum internalFormat, GLenum format, GLenum type, TextureSampling sampling)
	: m_xRes(xres)
	, m_yRes(yres)
	, m_fboNum(0)
	, m_texNum(0)
	, m_dtexNum(0)
{
	glGenFramebuffers(1, &m_fboNum);

	LOG("creating FBO (%i,%i) ... ", m_xRes, m_yRes);

	// color
    glGenTextures(1, &m_texNum);
    glBindTexture(GL_TEXTURE_2D, m_texNum);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_xRes, m_yRes, 0, format, GL_UNSIGNED_BYTE, NULL);

	if (sampling == LINEAR)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else if (sampling == NEAREST)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else LOG("Impossibru sampring for flamebuffel");
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	Rubber::getInstance()->checkGLErrors("\tColor attachment\n");
    glBindTexture(GL_TEXTURE_2D, 0);
	
	// depth
    glGenRenderbuffers(1, &m_dtexNum);
    glBindRenderbuffer(GL_RENDERBUFFER, m_dtexNum);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_xRes, m_yRes);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
	Rubber::getInstance()->checkGLErrors("\tDepth attachment\n");

    glBindFramebuffer(GL_FRAMEBUFFER, m_fboNum);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texNum, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_dtexNum);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
	{
    	ERROR("Framebuffer not complete\n");
    	if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) ERROR("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
		if (status == GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT) ERROR("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS");
		if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) ERROR("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
		if (status == GL_FRAMEBUFFER_UNSUPPORTED) ERROR("GL_FRAMEBUFFER_UNSUPPORTED");
		if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) ERROR("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
		if (status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) ERROR("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE");
		if (status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER) ERROR("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER");
    }
    
	Rubber::getInstance()->checkGLErrors("\tFramebuffer\n");
	LOG("\tdone\n");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Fbo::bind()
{
	m_previousFbo = Rubber::getInstance()->getCurrentFbo();
	Rubber::getInstance()->setCurrentFbo(this);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fboNum);
	glViewport(0, 0, m_xRes, m_yRes);
}


void Fbo::unBind()
{
	if (m_previousFbo == 0)
	{
		Rubber::getInstance()->setCurrentFbo(0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		Rubber::getInstance()->setResolution(Rubber::getInstance()->getXRes(), Rubber::getInstance()->getYRes());
	}
	else
	{
		Rubber::getInstance()->setCurrentFbo(m_previousFbo);
		glBindFramebuffer(GL_FRAMEBUFFER, m_previousFbo->getNum());
		glViewport(0, 0, m_previousFbo->getXRes(), m_previousFbo->getYRes());
	}
}


void Fbo::overlay(BlendMode blendMode, float br, vec2 scale, vec2 position)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	if (blendMode == ALPHA) glColor4f(1.0f, 1.0f, 1.0f, br);
	else glColor4f(br, br, br, 1.0f);
	
	Rubber::getInstance()->setDepthMode(NO_DEPTH);
	Rubber::getInstance()->setBlendMode(blendMode);
	Rubber::getInstance()->setCullMode(NO_CULL);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texNum);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f * scale.x + position.x,  1.0f * scale.y + position.y);
	glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f * scale.x + position.x,  1.0f * scale.y + position.y);
	glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f * scale.x + position.x, -1.0f * scale.y + position.y);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f * scale.x + position.x, -1.0f * scale.y + position.y);
	glEnd();
}
