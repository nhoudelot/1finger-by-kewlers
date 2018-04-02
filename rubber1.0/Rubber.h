#ifndef RUBBER_H
#define RUBBER_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>

#ifdef __APPLE__
#include <GLUT/GLUT.h>
#include <OpenGL/gl.h>
#include <OpenGL/OpenGL.h>
#include <Carbon/Carbon.h>
#endif

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "BassRocket.h"
//#include "Shader.h"
#include "../glm/glm.hpp"

namespace RubberSpace
{

#ifdef DEBUG
	#define DEVMODE_XRES 640
	#define DEVMODE_YRES 360
#endif

#define ASPECTRATIO (16.0f/9.0f)
//#define DISKWRITE

#define LOG(fmt, ...) Rubber::getInstance()->Log(fmt, ##__VA_ARGS__)
#define ERROR(fmt, ...) Rubber::getInstance()->Error(fmt, ##__VA_ARGS__)
#define GLERR(fmt, ...)	Rubber::getInstance()->checkGLErrors(fmt, ##__VA_ARGS__);

#define PI 3.14159265358979

#define DATAPATH "/usr/share/1finger-by-kewlers/data/"

//typedef unsigned int uint;
typedef void (*fPtr)();
typedef void (*MainLoopCallBack)(double row);

enum BlendMode
{
	NO_BLEND = 0,
	ADD,
	SUBTRACT,
	ALPHA,
	INVERT,
	MULTIPLY,
	ALPHA_ADD
};

enum DepthMode
{
	NO_DEPTH = 0,
	READ_ONLY,
	ENABLED
};

enum CullMode
{
	NO_CULL = 0,
	FRONT,
	BACK
};

enum TextureSampling
{
	MIPMAP_LINEAR = 0,
	LINEAR,
	NEAREST
};

class Shader;
class Fbo;

class Rubber
{
public:
	static Rubber* getInstance();
	long long getMs();
	void init(int argc, char** argv, MainLoopCallBack mainLoop);
	void close();
	void startPlaying();	
	void setResolution(int xres, int yres);
	void initBassRocket(std::string filename, float bpm);
	int getXRes() const { return m_xRes; }
	int getYRes() const { return m_yRes; }

	MainLoopCallBack m_pMainLoop;
	BassRocket* getBassRocket() const { return m_pBassRocket; }
	
	void setBlendMode(BlendMode mode);
	void setDepthMode(DepthMode mode);
	void setCullMode(CullMode mode);
	
	void setDone() { m_done = true; }
	bool isDone() { return m_done; }
	void Error(const char *fmt, ...);
	void Log(const char *fmt, ...);
	void checkGLErrors(const char* comment);
	void clear();
	
	float getAspect() const { return float(m_xRes)/float(m_yRes); }
	void quad2D(BlendMode blendMode);
	void reloadShaders();
	void addShader(Shader* shader);
	
	void setCurrentFbo(Fbo* fbo) { m_currentFbo = fbo; }
	Fbo* getCurrentFbo() const { return m_currentFbo; }
	
	void drawElements(GLenum mode, float* vertices, float* normals, float* uvs, float* colors, unsigned int* indices, unsigned int count);

private:
	Rubber();
	Rubber(Rubber const&);
	Rubber& operator=(Rubber const&);
	static Rubber* m_pInstance;
	BassRocket* m_pBassRocket;

	BlendMode m_currentBlendMode;
	DepthMode m_currentDepthMode;
	CullMode m_currentCullMode;

	uint m_xRes;
	uint m_yRes;
	bool m_done;
	
	std::vector<Shader*> m_allShaders;
	Fbo* m_currentFbo;
};


} // RubberSpace

#endif
