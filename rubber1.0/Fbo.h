#ifndef FBO_H
#define FBO_H

#include "Rubber.h"

using glm::vec2;

namespace RubberSpace {

class Fbo
{
public:
	Fbo(int xres, int yres, GLenum internalFormat = GL_RGBA8, GLenum format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE, TextureSampling sampling = LINEAR);
	~Fbo();
	void bind();
	void unBind();
	void overlay(BlendMode blendmode, float br, vec2 scale = vec2(1.0f, 1.0f), vec2 position = vec2(0.0f, 0.0f));
	GLuint getTexNum() const { return m_texNum; }
	int getXRes() const { return m_xRes; }
	int getYRes() const { return m_yRes; }
	GLuint getNum() const { return m_fboNum; }

private:
	GLuint m_fboNum;
	GLuint m_texNum;
	GLuint m_dtexNum;
	int m_xRes, m_yRes;
	Fbo* m_previousFbo;
};

} // namespace

#endif
