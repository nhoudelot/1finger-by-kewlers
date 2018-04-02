#include <sys/time.h>
#ifdef __APPLE__
#include "CoreFoundation/CoreFoundation.h"
#endif

#include <cstdarg>

#include "Rubber.h"
#include "Shader.h"
#include "BmpWriter.h"


using namespace RubberSpace;


Rubber::Rubber() : m_done(false), m_currentFbo(0) {}
Rubber* Rubber::m_pInstance = NULL;


void Rubber::Error(const char *fmt, ...)
{
	static char buf[8000];
	va_list ap;

	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	fprintf(stderr,buf);

#ifndef DEBUG
	exit(1);
#endif
}

void Rubber::Log(const char *fmt, ...)
{
#ifdef DEBUG
	static char buf[8000];
	va_list ap;

	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	printf(buf);
#endif
}

long long Rubber::getMs()
{
	timeval time;
	gettimeofday(&time, NULL);
	long long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
	return millis;
}


Rubber* Rubber::getInstance()
{
	if (!m_pInstance)
	{
		m_pInstance = new Rubber();
	}
   return m_pInstance;
}

namespace RubberSpace
{

void rubberGlutResizeCallback(int x, int y)
{
    Rubber::getInstance()->setResolution(x, y);
}

void glutRenderBitmapString(float x, float y, char* text)
{
	char *c;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glRasterPos2f(x, y);
	glDisable(GL_TEXTURE_2D);
//	Rubber::getInstance()->setDepthMode(NO_DEPTH);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    
	for (c = text; *c != '\0'; c++)
	{
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
	}

}

void rubberGlutMainLoopCallback()
{
	static long long prevMs = 0;
	Rubber* rubber = Rubber::getInstance();
	long ms = rubber->getMs();
	static long long framecnt = 0;
	static long long fps = 0.0f;

	if (rubber->isDone()) rubber->close();

#ifdef DEBUG
	if (ms >= prevMs + 1000)
	{
        printf("%d\n",framecnt);
		fps = framecnt;
		framecnt = 0;
		prevMs = ms;

		rubber->reloadShaders();
	}
	framecnt++;
#endif

#ifdef DISKWRITE
	static int frame = 0;
	double dwfps = 25.0;
	double row = frame / dwfps / 60.0 * rubber->getBassRocket()->getBpm() * float(rubber->getBassRocket()->getRowsPerBeat());
	rubber->getBassRocket()->setRow(row);
#else
	double row = rubber->getBassRocket()->getRow();
#endif

    //printf("%lf\n",rubber->getBassRocket()->getRow());

	rubber->m_pMainLoop(row);

#ifdef DEBUG
	char debugtxt[128];
	sprintf(debugtxt, "time = %6.2f fps = %i", row / 64.0f, fps);
	float yp = 26.0f / rubber->getYRes();
	glColor3f(1.0f, 1.0f, 1.0f);
	glutRenderBitmapString(-1.0f, 1.0f - yp, debugtxt);
#endif

//	glFlush(); // Probably not needed
	
#ifdef DISKWRITE
	char filename[128];
	sprintf(filename, "kwl_%05i.bmp", frame);
	BmpWriter::writeBmp(filename);
	frame++;
#endif

    glutSwapBuffers();
   	glutPostRedisplay();
	
	rubber->checkGLErrors("after glutPostRedisplay");
   	rubber->getBassRocket()->update();
#ifdef DEBUG
	if (!rubber->getBassRocket()->isPlaying()) usleep(100000);
#endif

}

void rubberGlutKeyboardCallback(unsigned char key, int x, int y)
{
	switch(key) 
	{
		case 27:
    		RubberSpace::Rubber::getInstance()->setDone();
    		break;
    	default:
    		break;
	}
}
} // namespace

void Rubber::init(int argc, char** argv, MainLoopCallBack mainLoop)
{
#ifdef __APPLE__
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path[PATH_MAX];
    if (!CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path, PATH_MAX))
    {
        ERROR("chdir erreur");
    }
    CFRelease(resourcesURL);

    chdir(path);
	LOG("Changing working directory to %s\n", path);
#endif

	m_pMainLoop = mainLoop;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);

    if(argc==2)
    {
        glutInitWindowSize(1280,720);
        glutCreateWindow("I am rubber, you are glue");
    }
    else
    {
    	int w = glutGet(GLUT_SCREEN_WIDTH);
    	int h = glutGet(GLUT_SCREEN_HEIGHT);
    	char gameModeString[128];
    	sprintf(gameModeString, "%ix%i:32", w, h);
    	glutGameModeString(gameModeString);
    	if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)) glutEnterGameMode();
    	else exit(1);
    	glutSetCursor(GLUT_CURSOR_NONE);
    }

#ifdef __APPLE__
    GLint VBL=1;
    CGLSetParameter(CGLGetCurrentContext(), kCGLCPSwapInterval, &VBL);
#endif

    glutDisplayFunc(rubberGlutMainLoopCallback);
    glutReshapeFunc(rubberGlutResizeCallback);
    glutKeyboardFunc(rubberGlutKeyboardCallback);

    m_xRes = glutGet(GLUT_WINDOW_WIDTH);
    m_yRes = glutGet(GLUT_WINDOW_HEIGHT);
    
    setBlendMode(NO_BLEND);
    setDepthMode(ENABLED);
    setCullMode(BACK);
}

void Rubber::initBassRocket(std::string filename, float bpm)
{
	m_pBassRocket = new BassRocket;
	m_pBassRocket->init(filename, bpm);
}

void Rubber::startPlaying()
{
  	m_pBassRocket->startPlaying();
	glutMainLoop(); 
}

void Rubber::close()
{
	m_pBassRocket->close();
	delete m_pBassRocket;
	exit(0);
}


void Rubber::setBlendMode(BlendMode mode)
{
	if (m_currentBlendMode == mode) return;

	switch(mode)
	{
		case NO_BLEND:
			glDisable(GL_BLEND);
			break;
		case ADD:
	    	glEnable(GL_BLEND);
		    glBlendFunc(GL_ONE, GL_ONE);
		    break;
		case SUBTRACT:
		    glEnable(GL_BLEND);
		    glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
			break;
		case ALPHA:
		    glEnable(GL_BLEND);
		    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
			break;
		case INVERT:
		    glEnable(GL_BLEND);
		    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);	
			break;
		case MULTIPLY:
		    glEnable(GL_BLEND);
		    glBlendFunc(GL_ZERO, GL_SRC_COLOR);
			break;
		case ALPHA_ADD:
		    glEnable(GL_BLEND);
		    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			break;
		default:
			break;					    
	} 
	
	m_currentBlendMode = mode;
}


void Rubber::setDepthMode(DepthMode mode)
{
	if (m_currentDepthMode == mode) return;
	
	switch(mode)
	{
		case NO_DEPTH:
		    glDisable(GL_DEPTH_TEST);
			break;
		case READ_ONLY:
		    glEnable(GL_DEPTH_TEST);
		    glDepthFunc(GL_LESS);
		    glDepthMask(GL_FALSE);
			break;
		case ENABLED:
		    glEnable(GL_DEPTH_TEST);
		    glDepthFunc(GL_LESS);
		    glDepthMask(GL_TRUE);
			break;
	}

	m_currentDepthMode = mode;
}

void Rubber::setCullMode(CullMode mode)
{
	if (m_currentCullMode == mode) return;
	
	switch(mode)
	{
		case NO_CULL:
		    glDisable(GL_CULL_FACE);
			break;
		case FRONT:
		    glEnable(GL_CULL_FACE);
		    glCullFace(GL_FRONT);
			break;
		case BACK:
		    glEnable(GL_CULL_FACE);
		    glCullFace(GL_BACK);
			break;
	}
	
	m_currentCullMode = mode;
}


void Rubber::checkGLErrors(const char* comment)
{
#ifdef DEBUG
	GLenum err = glGetError();

	if (err != GL_NO_ERROR)
	{
		switch(err)
		{
			case GL_INVALID_ENUM:
				ERROR("GL_INVALID_ENUM (%s)\n", comment);
				break;
			case GL_INVALID_VALUE:
				ERROR("GL_INVALID_VALUE (%s)\n", comment);
				break;
			case GL_INVALID_OPERATION:
				ERROR("GL_INVALID_OPERATION (%s)\n", comment);
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				ERROR("GL_INVALID_FRAMEBUFFER_OPERATION (%s)\n", comment);
				break;
			case GL_OUT_OF_MEMORY:
				ERROR("GL_OUT_OF_MEMORY (%s)\n", comment);
				break;
			case GL_STACK_UNDERFLOW:
				ERROR("GL_STACK_UNDERFLOW (%s)\n", comment);
				break;
			case GL_STACK_OVERFLOW:
				ERROR("GL_STACK_OVERFLOW (%s)\n", comment);
				break;
			default:
				break;
		}
	}
#endif
}

void Rubber::clear()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	setBlendMode(NO_BLEND);
	setCullMode(BACK);
	setDepthMode(ENABLED);
	
	glDisable(GL_TEXTURE_2D);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glUseProgramObjectARB(0);
}


void Rubber::setResolution(int xres, int yres)
{
	m_xRes = xres;
	m_yRes = yres;
	float crop = getAspect() / ASPECTRATIO;

	if (getAspect() > ASPECTRATIO) // narrowscreen
	{
		glViewport(int(((m_xRes - (m_xRes / crop)) * 0.5)), 0, int(m_xRes / crop), m_yRes);
	}
	else // widescreen
	{
		glViewport(0, int((m_yRes * (1.0 - crop) * 0.5)), m_xRes, int((m_yRes * crop)));
	}
}

void Rubber::quad2D(BlendMode blendMode)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	Rubber::getInstance()->setDepthMode(NO_DEPTH);
	Rubber::getInstance()->setBlendMode(blendMode);
	Rubber::getInstance()->setCullMode(NO_CULL);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f,  1.0f);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f,  1.0f);
	glEnd();

}

void Rubber::addShader(Shader *shader)
{
	m_allShaders.push_back(shader);
}

void Rubber::reloadShaders()
{
	for (int i = 0; i < m_allShaders.size(); i++)
	{
		bool changed = m_allShaders[i]->getFile()->reloadIfChanged();
		if (changed) m_allShaders[i]->load();
	}
}

void Rubber::drawElements(GLenum mode, float* vertices, float* normals, float* uvs, float* colors, unsigned int* indices, unsigned int count)
{

	assert(vertices != NULL);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	if (normals)
	{
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT, 0, normals);
	}

	if (uvs)
	{
		glClientActiveTexture(GL_TEXTURE0_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(3, GL_FLOAT, 0, uvs);
	}
	
	if (colors)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, 0, colors);
	}
	
	if (indices) glDrawElements(mode, count, GL_UNSIGNED_INT, indices);
	else glDrawArrays(mode, 0, count);

	glDisableClientState(GL_VERTEX_ARRAY);
	if (normals) glDisableClientState(GL_NORMAL_ARRAY);
	if (uvs) glDisableClientState(GL_NORMAL_ARRAY);
	if (colors) glDisableClientState(GL_COLOR_ARRAY);

}




