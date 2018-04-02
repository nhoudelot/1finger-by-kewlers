#include "Texture.h"

using namespace RubberSpace;
#include "../stb_image/stb_image.c"

Texture::Texture(string fileName)
	: m_file(new Datafile(fileName))
	, m_xRes(0)
	, m_yRes(0)
{
	int bpp;
	unsigned char* data = stbi_load_from_memory(m_file->getData(), m_file->getLenght(), &m_xRes, &m_yRes, &bpp, 0);
	GLenum format;

	LOG("\tTexture: %ix%i, %i components\n", m_xRes, m_yRes, bpp);
	if (data == NULL) ERROR("\tsbt_image failed to process image\n");
	
	switch (bpp)
	{
		case 1:
			format = GL_LUMINANCE8;
			break;
		case 3:
			format = GL_RGB;
			break;
		case 4:
			format = GL_RGBA;
			break;
		default:
			format = 0;
			ERROR("\tUnregonized number of components: %i\n", bpp);
			break;
	}

	glGenTextures(1, &m_texNum);
	glBindTexture(GL_TEXTURE_2D, m_texNum);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, format == GL_RGBA ? GL_RGBA : GL_RGB, m_xRes, m_yRes, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	stbi_image_free(data);
}


void Texture::bind()
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texNum);
}


void Texture::overlay(BlendMode blendMode, float br)
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

	bind();
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f,  1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f,  1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f);
	glEnd();
}


