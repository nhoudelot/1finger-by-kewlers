
#include "Shader.h"
#include "Rubber.h"
using namespace RubberSpace;

#include <GL/gl.h>


Shader::Shader(string fileName)
	: m_file(new Datafile(fileName))
	, m_shader(NULL)
	, m_vsprog(NULL)
	, m_fsprog(NULL)
	, m_maxTexUnit(0)
{
	m_vsprog = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	m_fsprog = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	m_shader = glCreateProgramObjectARB();
	
	Rubber::getInstance()->addShader(this);
	load();
}


void Shader::load()
{
	int result = 0;

	string src = string(reinterpret_cast<const char*>(m_file->getData()));
	src[m_file->getLenght()] = 0;
	string vs;
	string fs;
	
	int fspos = int(src.find("//____FRAGMENT_SHADER____", 0));
	vs = src.substr(0, fspos);
	fs = src.substr(fspos+25);
	
	const char* name =  m_file->getName()->c_str();
	const char* vschar = vs.c_str();
	const char* fschar = fs.c_str();
	compile(m_vsprog, GL_VERTEX_SHADER_ARB, name, vschar);
	compile(m_fsprog, GL_FRAGMENT_SHADER_ARB, name, fschar);
	
	glAttachObjectARB(m_shader, m_vsprog);
	glAttachObjectARB(m_shader, m_fsprog);
 
	glLinkProgramARB(m_shader);
	glGetObjectParameterivARB(m_shader, GL_OBJECT_LINK_STATUS_ARB, &result);

	if (!result)
	{   	
  		static char errmsg[1024];
		glGetInfoLogARB(m_shader, sizeof(errmsg), NULL, errmsg);
		ERROR("\tError linking shader %s\n%s", name, errmsg);
	}
 }



void Shader::compile(GLhandleARB shaderNum, GLint shaderType, const char* name, const char* code)
{
	LOG("\tCompiling %s: ", name);
	int result = 0, errorlen = 0;
	
	glShaderSourceARB(shaderNum, 1, &code, NULL);
	glCompileShaderARB(shaderNum);

	glGetObjectParameterivARB(shaderNum, GL_OBJECT_COMPILE_STATUS_ARB, &result);  
	glGetObjectParameterivARB(shaderNum, GL_OBJECT_INFO_LOG_LENGTH_ARB, &errorlen);  
  	if (!result) 
  	{
		char *errmsg = (char*)malloc(errorlen);
		int plaa;
		glGetInfoLogARB(shaderNum, errorlen, &plaa, errmsg);
		errmsg[errorlen-1] = 0;
		ERROR("\n\tError compiling shader %s [%s]: (%i)\n%s", name, shaderType==GL_VERTEX_SHADER_ARB?"VS":"FS", errorlen, errmsg);
		std::cout << name << std::endl;
		std::cout << code << std::endl;
		free(errmsg);
	} 
	else
	{
		LOG("ok\n");
	}
}


void Shader::bind()
{
	glUseProgramObjectARB(m_shader);
}

void Shader::unBind()
{
	glUseProgramObjectARB(0);

	for (GLuint i = 0; i <= m_maxTexUnit; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glDisable(GL_TEXTURE_2D);
	}
}

void Shader::uniform1f(const char* name, float val)
{
	glUniform1fARB(glGetUniformLocationARB(m_shader, name), val);
}

void Shader::uniform2f(const char* name, vec2 val)
{
	glUniform2fARB(glGetUniformLocationARB(m_shader, name), val.x, val.y);
}

void Shader::uniform3f(const char* name, vec3 val)
{
	glUniform3fARB(glGetUniformLocationARB(m_shader, name), val.x, val.y, val.z);
}

void Shader::uniform1i(const char* name, int val)
{
	glUniform1iARB(glGetUniformLocationARB(m_shader, name), val);
}


void Shader::bindTexture(const char* name, Texture* tex, GLuint texUnit)
{
	if (texUnit > m_maxTexUnit) m_maxTexUnit = texUnit;
	glActiveTexture(GL_TEXTURE0 + texUnit);
	tex->bind();
	glUniform1iARB(glGetUniformLocationARB(m_shader, name), texUnit);
}

void Shader::bindFbo(const char* name, Fbo* fbo, GLuint texUnit)
{
	if (texUnit > m_maxTexUnit) m_maxTexUnit = texUnit;
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, fbo->getTexNum());
	glUniform1iARB(glGetUniformLocationARB(m_shader, name), texUnit);
}









