#ifndef SHADER_H
#define SHADER_H

#include <string>

#include "Rubber.h"
#include "Datafile.h"
#include "Texture.h"
#include "Fbo.h"

using std::string;
using glm::vec2;
using glm::vec3;

namespace RubberSpace
{

class Shader
{
public:
	Shader(string fileName);
	void bind();
	void unBind();
	void uniform1f(const char* name, float val);
	void uniform2f(const char* name, vec2 val);
	void uniform3f(const char* name, vec3 val);
	void uniform1i(const char* name, int val);
	void bindTexture(const char* name, Texture* tex, GLuint texUnit);
	Datafile* getFile() const { return m_file; }
	void load();
	void bindFbo(const char* name, Fbo* fbo, GLuint texUnit);
	
private:
	Datafile* m_file;
	GLhandleARB m_shader, m_vsprog, m_fsprog;
	GLuint m_maxTexUnit;
	
	void compile(GLhandleARB shaderNum, GLint shaderType, const char* name, const char* code);
};

} // RubberSpace

#endif
