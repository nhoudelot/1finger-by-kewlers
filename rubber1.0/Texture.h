#ifndef TEXTURE_H
#define TEXTURE_H

#include "Rubber.h"
#include "Datafile.h"

using std::string;


namespace RubberSpace
{

class Texture
{
public:
	Texture(string fileName);
	void bind();
	void overlay(BlendMode blendMode = ADD, float br = 1.0f);
	
private:
	GLuint m_texNum;
	Datafile* m_file;
	int m_xRes, m_yRes;

};


}


#endif