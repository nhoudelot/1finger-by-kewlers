#include <Rubber.h>
#include <Shader.h>
#include <Model.h>
#include <Camera.h>
#include <Texture.h>
#include <BassRocket.h>
#include "Kewlmation.h"
#include <Fbo.h>

#include <string>
#include <ios>

using namespace RubberSpace;

Shader* envi;
Shader* circles;
Shader* downsample;
Shader* blur;
Shader* outlineshader;
Shader* lasershader;
Shader* depthshader;
Shader* combineshader;

Fbo* fbo;
Fbo* fbo2;
Fbo* fbo2b;
Fbo* fbo2c;
Fbo* fbo4;
Fbo* fbo4b;
Fbo* fbofx;
Fbo* fbofx2;
Fbo* laserfbo[4];

static const int numSymbols = 35;
Model* symbols[numSymbols];

Texture* envitex;
Texture* flare;

static const int numAnims = 16;
Kewlmation* rave[numAnims];

void init()
{  
	Rubber* rubber = Rubber::getInstance();

	// textures
	envitex = new Texture("envi.png");
	flare = new Texture("flare.png");

	// shaders
	envi = new Shader("envi.glsl");
	circles = new Shader("circles.glsl");
	downsample = new Shader("downsample.glsl");
	blur = new Shader("blur.glsl");
	outlineshader = new Shader("outline.glsl");
	lasershader = new Shader("laser.glsl");
	depthshader = new Shader("depth.glsl");
	combineshader = new Shader("combine.glsl");
	
	// models
	for (int i = 0; i < numSymbols; i++)
	{
		string filename = "symbol" + std::to_string((long long int)i + 1) + ".obj";
		symbols[i] = new Model(filename);
	}
	
	// animations
	for (int i = 0; i < numAnims; i++)
	{
		string filename = "rave" + std::to_string((long long int)i + 1) + ".dat";
		rave[i] = new Kewlmation(filename);
	}
	
	// framebuffers
	int xres = rubber->getXRes();
	int yres = rubber->getYRes();
	
	fbo = new Fbo(xres, yres);
	
	int fboxres = xres > 1024 ? 1024 : xres;
	int fboyres = fboxres * 9 / 16;
	fbofx = new Fbo(fboxres, fboyres);
	fbofx2 = new Fbo(fboxres, fboyres);
	fbo2 = new Fbo(xres / 2, yres / 2);
	fbo2b = new Fbo(xres / 2, yres / 2);
	fbo2c = new Fbo(xres / 2, yres / 2);
	fbo4 = new Fbo(xres / 4, yres / 4);
	fbo4b = new Fbo(xres / 4, yres / 4);
	for (int i = 0; i < 4; i++)
	{
		laserfbo[i] = new Fbo(1, 256, GL_RGBA32F_ARB, GL_RGBA, GL_FLOAT, NEAREST);
	}
	
	// init rocket stuff in right order
	float plaa;
	Camera cam;
	cam.getRocketValues(0);
	plaa = SYNC("glow:glowbr", 0);
	plaa = SYNC("glow:origbr", 0);
	plaa = SYNC("video:numframes", 0);
	plaa = SYNC("video:r", 0);
	plaa = SYNC("video:g", 0);
	plaa = SYNC("video:b", 0);
	plaa = SYNCi("video:num", 0);
	plaa = SYNC("video:pos", 0);
	plaa = SYNC("eq:br", 0);
	plaa = SYNC("eq:blend", 0);
	plaa = SYNC("eq:mode", 0);
	plaa = SYNC("circles:r", 0);
	plaa = SYNC("circles:g", 0);
	plaa = SYNC("circles:b", 0);
	plaa = SYNC("circles:blend", 0);
	plaa = SYNC("symbol:num", 0);
	plaa = SYNC("symbol:rot_x", 0);
	plaa = SYNC("symbol:rot_y", 0);
	plaa = SYNC("symbol:rot_z", 0);
	plaa = SYNC("symbol:pos_y", 0);
	plaa = SYNC("symbol:pos_z", 0);
	plaa = SYNC("symbol:scale", 0);
	plaa = SYNC("symbol:blend", 0);
	plaa = SYNC("strobo", 0);
	plaa = SYNC("laser1:pos_x", 0);
	plaa = SYNC("laser1:pos_y", 0);
	plaa = SYNC("laser1:pos_z", 0);
	plaa = SYNC("laser1:rot_x", 0);
	plaa = SYNC("laser1:rot_y", 0);
	plaa = SYNC("laser1:rot_z", 0);
	plaa = SYNC("laser1:diam", 0);
	plaa = SYNC("laser1:col_r", 0);
	plaa = SYNC("laser1:col_g", 0);
	plaa = SYNC("laser1:col_b", 0);
}


///////////////////////////////////////////////////////////////////////////////////////////////

/*
const int RGL_MAX_VERTS 128000
GLenum m_rglMode;
float* m_pRglVertices;
float* m_pRglNormals;
float* m_pRglUvs;
float* m_pRglColors;

float* m_pRglVerticesPos;
float* m_pRglNormalsPos;
float* m_pRglUvsPos;
float* m_pRglColorsPos;

static void rglBegin(GLenum mode)
{
	m_rglMode = mode;
	m_pRglVerticesPos = m_pRglVertices;
	m_pRglNormalsPos = m_pRglNormals;
	m_pRglUvsPos = m_pRglUvs;
	m_pRglColorsPos = m_pRglColors;
}

static inline void rglVertex3f(float v1, float v2, float v3)
{
	m_pRglVerticesPos[0] = v1;
	m_pRglVerticesPos[1] = v2;
	m_pRglVerticesPos[2] = v3;
	m_pRglVerticesPos += 3;
}

static inline void rglNormal3f(float n1, float n2, float n3)
{
	m_pRglNormalsPos[0] = n1;
	m_pRglNormalsPos[1] = n2;
	m_pRglNormalsPos[2] = n3;
	m_pRglNormalsPos += 3;
}

static inline void rglColor4f(float r, float g, float b, float a)
{
}

void rglEnd()
{
	drawElements(m_rglMode, m_pRglVertices, m_pRgl...)
}
*/


inline float saw(float a)
{
	float aa = fmod(a * 0.5f, 2.0f);
	if (aa < 1.0f) return aa;
	else return 2.0f - aa;
}

inline float tunnelXpos(float pos)
{
	return saw(pos * 0.04892 - sinf(pos * 0.0052837)) * 2.0f;
}

inline float tunnelYpos(float pos)
{
	return saw(pos * 0.03584 - sinf(pos * 0.0073917)) * 2.0f;
}

void equalizer(double row)
{
	static float eqtmp[128];
	static float eq[128];
	float aspect = Rubber::getInstance()->getAspect();
	Rubber::getInstance()->getBassRocket()->getFFT(eqtmp, BASS_DATA_FFT256 | BASS_DATA_FLOAT);
	float br = 	SYNC("eq:br", row);
	//float blend = SYNC("eq:blend", row);
	float rot = row / 128.0f;
		
	if (br <= 0.0f) return;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	Rubber::getInstance()->setCullMode(NO_CULL);
	Rubber::getInstance()->setBlendMode(NO_BLEND);
	Rubber::getInstance()->setDepthMode(ENABLED);
	
	int mode = SYNCi("eq:mode", row);
	
	for (int i = 0; i < 128; i++)
	{
		eqtmp[i] *= float(i);
		eq[i] = eq[i] * 0.9f;
		if (eqtmp[i] > eq[i]) eq[i] = eqtmp[i];
	}
	
	if (mode == 0)
	{
		static float verts[100 * 12];
		static float colors[100 * 16];
		static float colors2[100 * 16];
		static unsigned int indices[100 * 4];
		
		//glBegin(GL_QUADS);
		for (int i = 0; i < 100; i++)
		{
			float a1 = float(i) / 100.0 * 2.0f * PI + rot;
			float a2 = float(i + 1) / 100.0 * 2.0f * PI + rot;
			float e = glm::clamp(eq[i], 0.0f, 1.1f);
			float r = 0.45f * br + sin(a1) * e * br * 0.25f;
			float g = 0.45f * br + sin(a1 + PI) * e * br * 0.25f;
			float b = 0.75f * br;
			
			verts[i * 12 + 0] = sinf(a1) * 1.15f;
			verts[i * 12 + 1] = cosf(a1) * 1.15f * aspect;
			verts[i * 12 + 2] = 0.0f;
			verts[i * 12 + 3] = sinf(a2) * 1.15f;
			verts[i * 12 + 4] = cosf(a2) * 1.15f * aspect;
			verts[i * 12 + 5] = 0.0f;
			verts[i * 12 + 6] = sinf(a2) * (1.25f - e);
			verts[i * 12 + 7] = cosf(a2) * (1.25f - e) * aspect;
			verts[i * 12 + 8] = 0.0f;
			verts[i * 12 + 9] = sinf(a1) * (1.25f - e);
			verts[i * 12 + 10] = cosf(a1) * (1.25f - e) * aspect;
			verts[i * 12 + 11] = 0.0f;
			
			colors[i * 16 + 0] = glm::clamp(r * 0.5f, 0.0f, 1.0f);
			colors[i * 16 + 1] = glm::clamp(g * 0.5f, 0.0f, 1.0f);
			colors[i * 16 + 2] = glm::clamp(b, 0.0f, 1.0f);
			colors[i * 16 + 3] = 1.0f;
			colors[i * 16 + 4] = glm::clamp(r * 0.5f, 0.0f, 1.0f);
			colors[i * 16 + 5] = glm::clamp(g * 0.5f, 0.0f, 1.0f);
			colors[i * 16 + 6] = glm::clamp(b, 0.0f, 1.0f);
			colors[i * 16 + 7] = 1.0f;
			colors[i * 16 + 8] = glm::clamp(r * 0.5f, 0.0f, 1.0f);
			colors[i * 16 + 9] = glm::clamp(g * 0.5f, 0.0f, 1.0f);
			colors[i * 16 + 10] = glm::clamp(b, 0.0f, 1.0f);
			colors[i * 16 + 11] = 1.0f;
			colors[i * 16 + 12] = glm::clamp(r * 0.5f, 0.0f, 1.0f);
			colors[i * 16 + 13] = glm::clamp(g * 0.5f, 0.0f, 1.0f);
			colors[i * 16 + 14] = glm::clamp(b, 0.0f, 1.0f);
			colors[i * 16 + 15] = 1.0f;
			
			colors2[i * 16 + 0] = glm::clamp(r, 0.0f, 1.0f);
			colors2[i * 16 + 1] = glm::clamp(g, 0.0f, 1.0f);
			colors2[i * 16 + 2] = glm::clamp(b * 2.0f, 0.0f, 1.0f);
			colors2[i * 16 + 3] = 1.0f;
			colors2[i * 16 + 4] = glm::clamp(r, 0.0f, 1.0f);
			colors2[i * 16 + 5] = glm::clamp(g, 0.0f, 1.0f);
			colors2[i * 16 + 6] = glm::clamp(b * 2.0f, 0.0f, 1.0f);
			colors2[i * 16 + 7] = 1.0f;
			colors2[i * 16 + 8] = glm::clamp(r, 0.0f, 1.0f);
			colors2[i * 16 + 9] = glm::clamp(g, 0.0f, 1.0f);
			colors2[i * 16 + 10] = glm::clamp(b * 2.0f, 0.0f, 1.0f);
			colors2[i * 16 + 11] = 1.0f;
			colors2[i * 16 + 12] = glm::clamp(r, 0.0f, 1.0f);
			colors2[i * 16 + 13] = glm::clamp(g, 0.0f, 1.0f);
			colors2[i * 16 + 14] = glm::clamp(b * 2.0f, 0.0f, 1.0f);
			colors2[i * 16 + 15] = 1.0f;
	
			indices[i * 4 + 0] = i * 4 + 0;
			indices[i * 4 + 1] = i * 4 + 1;
			indices[i * 4 + 2] = i * 4 + 2;
			indices[i * 4 + 3] = i * 4 + 3;
												
			/*
			glColor3f(glm::clamp(r * 0.5f, 0.0f, 1.0f), glm::clamp(g * 0.5f, 0.0f, 1.0f), glm::clamp(b, 0.0f, 1.0f));
			glVertex2f(sinf(a1) * 1.15f, cosf(a1) * 1.15f * aspect);
			glVertex2f(sinf(a2) * 1.15f, cosf(a2) * 1.15f * aspect);
			glColor3f(glm::clamp(r * 0.5f, 0.0f, 1.0f), glm::clamp(g * 0.5f, 0.0f, 1.0f), glm::clamp(b, 0.0f, 1.0f));
			glVertex2f(sinf(a2) * (1.25f - e), cosf(a2) * (1.25f - e) * aspect);
			glVertex2f(sinf(a1) * (1.25f - e), cosf(a1) * (1.25f - e) * aspect);
			*/
		}
		//glEnd();
		
		Rubber::getInstance()->drawElements(GL_QUADS, verts, 0, 0, colors, indices, 400);
		
		glEnable(GL_LINE_SMOOTH);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		Rubber::getInstance()->setDepthMode(NO_DEPTH);
		Rubber::getInstance()->setBlendMode(ALPHA_ADD);
		
 		float lineWidth = Rubber::getInstance()->getXRes() / 640.0f;
		if (lineWidth < 1.0f) lineWidth = 1.0f;
		glLineWidth(lineWidth);
		
		Rubber::getInstance()->drawElements(GL_QUADS, verts, 0, 0, colors2, indices, 400);
				
		/*
		glBegin(GL_QUADS);
		for (int i = 0; i < 100; i++)
		{
			float a1 = float(i) / 100.0 * 2.0f * PI + rot;
			float a2 = float(i + 1) / 100.0 * 2.0f * PI + rot;
			float e = glm::clamp(eq[i], 0.0f, 1.1f);
			float r = 0.45f * br + sin(a1) * e * br * 0.25f;
			float g = 0.45f * br + sin(a1 + PI) * e * br * 0.25f;
			float b = 0.75f * br;
		
			glColor3f(glm::clamp(r, 0.0f, 1.0f), glm::clamp(g, 0.0f, 1.0f), glm::clamp(b * 2.0f, 0.0f, 1.0f));
			glVertex2f(sinf(a1) * 1.15f, cosf(a1) * 1.15f * aspect);
			glVertex2f(sinf(a2) * 1.15f, cosf(a2) * 1.15f * aspect);
			glColor3f(glm::clamp(r, 0.0f, 1.0f), glm::clamp(g, 0.0f, 1.0f), glm::clamp(b * 2.0f, 0.0f, 1.0f));
			glVertex2f(sinf(a2) * (1.25f - e), cosf(a2) * (1.25f - e) * aspect);
			glVertex2f(sinf(a1) * (1.25f - e), cosf(a1) * (1.25f - e) * aspect);
		}
		glEnd();*/
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 

	}
	else if (mode == 1)
	{
	
		static float vertices[128 * 128 * 12];
		static float colors[128 * 128 * 16];
		Camera cam;
		cam.getRocketValues(row);
		cam.setMatrices();
		//float tunnelpos = float(row);
		//float trans = fmod(tunnelpos, 1.0f);
		float diam = 3.0f;
					
		//glBegin(GL_QUADS);
		for (int i = 0; i < 128; i++)
		{
			//float pos = i + tunnelpos;
			float tunnelpos = row * 2.0f;
			int itunnelpos = floor(tunnelpos);
			float tunnelmod = fmod(tunnelpos, 1.0f);
			int limit = eq[((i + itunnelpos) % 90) + 4] * 128;
			
			float zpos = -(float(i) / 256.0f - 0.5f) * 100.0f + tunnelmod * 100.0f/256.0f;
			float zpos2 = -(float(i + 1) / 256.0f - 0.5f) * 100.0f + tunnelmod * 100.0f/256.0f;
			float tp1 = tunnelpos - zpos;
			float tp2 = tunnelpos - zpos2;
			float xpos = tunnelXpos(tp1) * 8.0f;
			float xpos2 = tunnelXpos(tp2) * 8.0f;
			float ypos = tunnelYpos(tp1) * 8.0f;
			float ypos2 = tunnelYpos(tp2) * 8.0f;
			
			xpos -= tunnelXpos(tunnelpos - 50.0f) * 8.0f;
			xpos2 -= tunnelXpos(tunnelpos - 50.0f) * 8.0f;
			ypos -= tunnelYpos(tunnelpos - 50.0f) * 8.0f;
			ypos2 -= tunnelYpos(tunnelpos - 50.0f) * 8.0f;


			float br2 = (1.0f - pow(float(i) / 256.0f, 0.5f)) * br;
			for (int j = 0; j < 128; j++)
			{
				int limitpos = abs(j - 64);
				/*if (limitpos > limit) glColor3f(0.025f * br2, 0.025f * br2, 0.05f * br2);
				else glColor3f(0.1f * br2, 0.04f * br2, 0.1f * br2);
				
				glVertex3f(sinf((j + 0) / 128.0f * PI * 2.0f) * diam + xpos, cosf((j + 0) / 128.0f * PI * 2.0f) * diam + ypos, zpos);
				glVertex3f(sinf((j + 1) / 128.0f * PI * 2.0f) * diam + xpos, cosf((j + 1) / 128.0f * PI * 2.0f) * diam + ypos, zpos);
				glVertex3f(sinf((j + 1) / 128.0f * PI * 2.0f) * diam + xpos2, cosf((j + 1) / 128.0f * PI * 2.0f) * diam + ypos2, zpos2);
				glVertex3f(sinf((j + 0) / 128.0f * PI * 2.0f) * diam + xpos2, cosf((j + 0) / 128.0f * PI * 2.0f) * diam + ypos2, zpos2);
				*/
				
				int index = i * 128 + j;
				
				vertices[index * 12 + 0] = sinf((j + 0) / 128.0f * PI * 2.0f) * diam + xpos;
				vertices[index * 12 + 1] = cosf((j + 0) / 128.0f * PI * 2.0f) * diam + ypos;
				vertices[index * 12 + 2] = zpos;
				vertices[index * 12 + 3] = sinf((j + 1) / 128.0f * PI * 2.0f) * diam + xpos;
				vertices[index * 12 + 4] = cosf((j + 1) / 128.0f * PI * 2.0f) * diam + ypos;
				vertices[index * 12 + 5] = zpos;
				vertices[index * 12 + 6] = sinf((j + 1) / 128.0f * PI * 2.0f) * diam + xpos2;
				vertices[index * 12 + 7] = cosf((j + 1) / 128.0f * PI * 2.0f) * diam + ypos2;
				vertices[index * 12 + 8] = zpos2;
				vertices[index * 12 + 9] = sinf((j + 0) / 128.0f * PI * 2.0f) * diam + xpos2;
				vertices[index * 12 + 10] = cosf((j + 0) / 128.0f * PI * 2.0f) * diam + ypos2;
				vertices[index * 12 + 11] = zpos2;

				if (limitpos > limit)
				{
					colors[index * 16 + 0] = 0.025f * br2;
					colors[index * 16 + 1] = 0.025f * br2;
					colors[index * 16 + 2] = 0.05f * br2;
					colors[index * 16 + 3] = 1.0f;
					colors[index * 16 + 4] = 0.025f * br2;
					colors[index * 16 + 5] = 0.025f * br2;
					colors[index * 16 + 6] = 0.05f * br2;
					colors[index * 16 + 7] = 1.0f;
					colors[index * 16 + 8] = 0.025f * br2;
					colors[index * 16 + 9] = 0.025f * br2;
					colors[index * 16 + 10] = 0.05f * br2;
					colors[index * 16 + 11] = 1.0f;
					colors[index * 16 + 12] = 0.025f * br2;
					colors[index * 16 + 13] = 0.025f * br2;
					colors[index * 16 + 14] = 0.05f * br2;
					colors[index * 16 + 15] = 1.0f;
				}
				else
				{
					colors[index * 16 + 0] = 0.1f * br2;
					colors[index * 16 + 1] = 0.04f * br2;
					colors[index * 16 + 2] = 0.1f * br2;
					colors[index * 16 + 3] = 1.0f;
					colors[index * 16 + 4] = 0.1f * br2;
					colors[index * 16 + 5] = 0.04f * br2;
					colors[index * 16 + 6] = 0.1f * br2;
					colors[index * 16 + 7] = 1.0f;
					colors[index * 16 + 8] = 0.1f * br2;
					colors[index * 16 + 9] = 0.04f * br2;
					colors[index * 16 + 10] = 0.1f * br2;
					colors[index * 16 + 11] = 1.0f;
					colors[index * 16 + 12] = 0.1f * br2;
					colors[index * 16 + 13] = 0.04f * br2;
					colors[index * 16 + 14] = 0.1f * br2;
					colors[index * 16 + 15] = 1.0f;
				}
			}
		}
		//glEnd();
/*
		float tunnelpos = row * 4.0f;
		float pos1 = tunnelpos - 50.0f;
		float pos2 = tunnelpos - 49.0f;
		float xroto = tunnelXpos(pos2) - tunnelXpos(pos1);
		float yroto = tunnelYpos(pos2) - tunnelYpos(pos1);
		
		glRotatef(-yroto * 100.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(-xroto * 100.0f, 0.0f, 1.0f, 0.0f);
*/
		Rubber::getInstance()->drawElements(GL_QUADS, vertices, 0, 0, colors, 0, 128 * 128 * 4);
		
		glEnable(GL_LINE_SMOOTH);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		Rubber::getInstance()->setDepthMode(READ_ONLY);
		Rubber::getInstance()->setBlendMode(ALPHA_ADD);

 		float lineWidth = Rubber::getInstance()->getXRes() / 640.0f;
		if (lineWidth < 1.0f) lineWidth = 1.0f;
		glLineWidth(lineWidth);
		
		Rubber::getInstance()->drawElements(GL_QUADS, vertices, 0, 0, colors, 0, 128 * 128 * 4);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////

void circlesfx(double row)
{
	float cr = SYNC("circles:r", row);
	float cg = SYNC("circles:g", row);
	float cb = SYNC("circles:b", row);
	
	if (cr + cb + cg <= 0.0f) return;
	
	circles->bind();
	circles->uniform1f("u_time", row / 64.0f);
	circles->uniform1f("u_aspect", 1.0 / Rubber::getInstance()->getAspect());
	circles->uniform3f("u_col", vec3(cr, cg, cb));
	Rubber::getInstance()->quad2D((BlendMode)SYNCi("circles:blend", row));
	circles->unBind();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void symbolsfx(int symbol, double row, bool laserfx = false, bool renderlaser = false)
{
	if (!laserfx)
	{
		Camera cam;

		cam.getRocketValues(row);
		cam.setMatrices();
	}
	
	if (laserfx) glColor3f(0.0f, 0.0f, 0.0f);
	else glColor3f(1.0f, 1.0f, 1.0f);
	
	float rotx = SYNC("symbol:rot_x", row);
	float roty = SYNC("symbol:rot_y", row);
	float rotz = SYNC("symbol:rot_z", row);
	float ypos = SYNC("symbol:pos_y", row);
	float zpos = SYNC("symbol:pos_z", row);
	float scale = SYNC("symbol:scale", row);
	
	Rubber::getInstance()->setBlendMode(BlendMode::NO_BLEND);
	Rubber::getInstance()->setDepthMode(DepthMode::ENABLED);
	Rubber::getInstance()->setCullMode(CullMode::BACK);

	if (!laserfx)
	{
		envi->bind();
		envi->bindTexture("u_envitex", envitex, 0);
	}
	else
	{
		if (!renderlaser) glUseProgramObjectARB(0);
		glDisable(GL_TEXTURE_2D);
	}
	glPushMatrix();
	glTranslatef(0.0f, ypos, zpos);
	glRotatef(rotz, 0.0f, 0.0f, 1.0f);
	glRotatef(roty, 0.0f, 1.0f, 0.0f);
	glRotatef(rotx, 1.0f, 0.0f, 0.0f);
	glScalef(scale, scale, scale);
	symbols[symbol % numSymbols]->render();
	glPopMatrix();
	//if (!laserfx) glUseProgramObjectARB(0);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void downsampler(Fbo* source)
{
	downsample->bind();
	downsample->bindFbo("u_source", source, 0);
	downsample->uniform2f("u_pixel", vec2(1.0f / float(source->getXRes()), 1.0f / float(source->getYRes())));
	Rubber::getInstance()->quad2D(NO_BLEND);
	downsample->unBind();
}

void blurfilter(Fbo* source, vec2 dir, int radius)
{
	blur->bind();
	blur->bindFbo("u_source", source, 0);
	blur->uniform2f("u_pixel", vec2(1.0f / float(source->getXRes()), 1.0f / float(source->getYRes())) * dir);
	blur->uniform1i("u_radius", radius);
	Rubber::getInstance()->quad2D(NO_BLEND);
	blur->unBind();
}

void outlinefilter(Fbo* source, BlendMode blendmode)
{
	outlineshader->bind();
	outlineshader->bindFbo("u_source", source, 0);
	outlineshader->uniform2f("u_pixel", vec2(1.0f / float(source->getXRes()), 1.0f / float(source->getYRes())));
	Rubber::getInstance()->quad2D(blendmode);
	outlineshader->unBind();
}

///////////////////////////////////////////////////////////////////////////////////////////////


void videos(double row)
{
	int numframes = SYNCi("video:numframes", row);
	Rubber::getInstance()->setBlendMode(ADD);
	Rubber::getInstance()->setDepthMode(NO_DEPTH);
	Rubber::getInstance()->setCullMode(NO_CULL);
	float r, g, b;
	r = SYNC("video:r", row);
	g = SYNC("video:g", row);
	b = SYNC("video:b", row);
	int vnum = SYNCi("video:num", row) % numAnims;
	float pos = SYNC("video:pos", row) ;
	
	for (int i = 0; i < numframes; i++)
	{
		float pos2 = pos + float(i) / float(rave[vnum]->getNumFrames());
		rave[vnum]->renderFrame(pos2, r, g, b);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////

void lazers(double row)
{
	Rubber* rubber = Rubber::getInstance();
	int numLasers = SYNCi("laser1:numlasers", row);
	vec3 flarepos[4];
	float flarebri[4];
	if (numLasers < 1) return;
	Camera cam;
	float r = SYNC("laser1:col_r", row);
	float g = SYNC("laser1:col_g", row);
	float b = SYNC("laser1:col_b", row);
	if (r + g + b <= 0.0f) return;
	vec3 laser1pos(SYNC("laser1:pos_x", row), SYNC("laser1:pos_y", row), SYNC("laser1:pos_z", row));
//	vec3 laser1rot(SYNC("laser1:rot_x", row), SYNC("laser1:rot_y", row), SYNC("laser1:rot_z", row));
	float diam = SYNC("laser1:diam", row);
	int symbol = SYNCi("symbol:num", row);
	float xrot[4], yrot[4], zrot[4], xpos[4];

	// calc positions and render fbos
	for (int i = 0; i < 4; i++)
	{		
		xrot[i] = SYNC("laser1:rot_x", row + double(i) * 16.0);
		yrot[i] = SYNC("laser1:rot_y", row + double(i) * 16.0);
		zrot[i] = SYNC("laser1:rot_z", row + double(i) * 16.0);
		xpos[i] = (float(i) - 1.5f) * 32.0f;
				
		//xpos *= 32.0f;
		
		flarepos[i] = vec3(xpos[i] + laser1pos.x, laser1pos.y, laser1pos.z);
		vec3 lasertgt = vec3(xpos[i] + laser1pos.x + xrot[i], laser1pos.y + yrot[i], -laser1pos.z);
		vec3 laserdir = lasertgt - flarepos[i];
		//LOG("laserdir = (%6.3f, %6.3f, %6.3f)\n", laserdir.x, laserdir.y, laserdir.z);
		float laserlen = glm::length(laserdir);
		float hypo = sqrt(laserlen * laserlen + (diam) * (diam));
		laserdir = glm::normalize(laserdir);
		flarebri[i] = glm::clamp(glm::dot(laserdir, glm::normalize(cam.getPos() - flarepos[i])), 0.0f, 1.0f);
		flarebri[i] = powf(flarebri[i], 512.0f) * 0.5f + 0.25f;
	
		if (symbol > -1)
		{
			laserfbo[i]->bind();
			Camera lasercam;
			lasercam.setDefaults();
			Rubber::getInstance()->clear();
			cam.setPos(flarepos[i]);
			cam.setTgt(lasertgt);
			float fov = glm::degrees(acos(laserlen/hypo));
			cam.setFov(fov);
			cam.setRoll(-glm::degrees(zrot[i]));
			cam.setMatrices(1.0f / 256.0f);
			//LOG("laserlen: %6.3f diameterx2: %6.3f, hypo: %6.3f angle:%6.3f\n", laserlen, diam*2.0f, hypo, glm::degrees(acos(laserlen/hypo)));
			//cam.setMatrices(1.0f);
			//glRotatef(glm::degrees(zrot[i]) + 90.0f, 0.0f, 0.0f, 1.0f);
			depthshader->bind();
			symbolsfx(symbol, row, true, true);
			depthshader->unBind();
			laserfbo[i]->unBind();
		}
	}
	
	cam.getRocketValues(row);
	cam.setMatrices();

	if (symbol > -1) symbolsfx(symbol, row, true);
	
	rubber->setBlendMode(ADD);
	rubber->setCullMode(NO_CULL);
	rubber->setDepthMode(READ_ONLY);
	
	lasershader->bind();
		
	for (int i = 0; i < 4; i++)
	{
		lasershader->bindFbo("u_depthtex", laserfbo[i], 0);
		//lasershader->bindFbo("u_depthtex", fbo2, 0);
	
		//float xpos = (float(i) - 1.5f);
		//float xrot, yrot, zrot;
		
		//xrot = SYNC("laser1:rot_x", row + double(i) * 16.0);
		//yrot = SYNC("laser1:rot_y", row + double(i) * 16.0);
		//zrot = SYNC("laser1:rot_z", row + double(i) * 16.0);
		
				/*
		xpos *= 32.0f;
		flarepos[i] = vec3(xpos + laser1pos.x, laser1pos.y, laser1pos.z);
		vec3 laserdir = vec3(xpos + laser1pos.x + xrot, laser1pos.y + yrot , -laser1pos.z) - flarepos[i];
		laserdir = glm::normalize(laserdir);
		flarebri[i] = glm::clamp(glm::dot(laserdir, glm::normalize(cam.getPos() - flarepos[i])), 0.0f, 1.0f);
		flarebri[i] = powf(flarebri[i], 512.0f) * 0.5f + 0.25f; */
		
		float br = 128.0f / diam;
		//float step = 1.0f / (float(numLasers) * 2.0f + 1.0f);
		float step = 1.0f / 256.0;
		float plaa1x, plaa1y, plaa2x, plaa2y;
		
		vec3 lasertgt = vec3(xpos[i] + laser1pos.x + xrot[i], laser1pos.y + yrot[i], -laser1pos.z);
		vec3 laserdir = lasertgt - flarepos[i];
		float laserlen = glm::length(laserdir);
		laserdir = glm::normalize(laserdir);

		vec3 right = glm::cross(laserdir, vec3(0.0f, 1.0f, 0.0f));
		right = glm::normalize(right);
		vec3 up = glm::cross(laserdir, right);
		up = glm::normalize(up);
		
		right *= diam * 0.5f;
		up *= diam * 0.5f;
		
		plaa1x = sinf(zrot[i]);
		plaa1y = cosf(zrot[i]);
		plaa2x = -plaa1x; //sinf(zrot[i] + PI);
		plaa2y = -plaa1y;//cosf(zrot[i] + PI);
		
		glBegin(GL_QUADS);

		for (int j = 0; j < 256; j++)
		{
			float ipol = float(j) * step;
			float x1, y1, x2, y2;
			x1 = glm::mix(plaa1x, plaa2x, ipol);
			x2 = glm::mix(plaa1x, plaa2x, ipol + step);// * (numLasers == 1 ? 1.0 : 0.5));
			y1 = glm::mix(plaa1y, plaa2y, ipol);
			y2 = glm::mix(plaa1y, plaa2y, ipol + step);// * (numLasers == 1 ? 1.0 : 0.5));
			float tc1 = float(j) / 256.0f;
			float tc2 = float(j+1) / 256.0f;
			vec3 asdf1 = right * x1 + up * y1;
			vec3 asdf2 = right * x2 + up * y2;
			
			if (numLasers > 1 && ((j/numLasers)&1) == 0) continue;
			
			float xx, yy, zz;
			xx = xpos[i] + laser1pos.x + asdf1.x + xrot[i];
			yy = laser1pos.y + asdf1.y + yrot[i];
			zz = -laser1pos.z + asdf1.z;
			float dist = glm::length(vec3(xx, yy, zz) - vec3(xpos[i] + laser1pos.x, laser1pos.y, laser1pos.z));
			
			glTexCoord2f(0.0f, tc1);
			glColor3f(r * br, g * br, b * br);
			glVertex3f(xpos[i] + laser1pos.x, laser1pos.y, laser1pos.z);
			glVertex3f(xpos[i] + laser1pos.x, laser1pos.y, laser1pos.z);

			glTexCoord2f(dist, tc2);
			glColor3f(r / diam * br, g / diam * br, b / diam * br);
			//vec3 asdf1 = right * plaa1x + up * plaa1y;
			glVertex3f(xx, yy, zz);
			//vec3 asdf2 = right * plaa2x + up * plaa2y;
			glVertex3f(xpos[i] + laser1pos.x + asdf2.x + xrot[i], laser1pos.y + asdf2.y + yrot[i], -laser1pos.z + asdf2.z);

/*			glColor3f(r * br, g * br, b * br);
			glVertex3f(xpos[i] + laser1pos.x + x1 * 0.01f, laser1pos.y + y1 * 0.01f, laser1pos.z);
			glColor3f(r * br, g * br, b * br);
			glVertex3f(xpos[i] + laser1pos.x + x2 * 0.01f, laser1pos.y + y2 * 0.01f, laser1pos.z);
			glColor3f(r / diam * br, g / diam * br, b / diam * br);
			glVertex3f(xpos[i] + laser1pos.x + xrot[i] + x2, laser1pos.y + yrot[i] + y2, -laser1pos.z);
			glColor3f(r / diam * br, g / diam * br, b / diam * br);
			glVertex3f(xpos[i] + laser1pos.x + xrot[i] + x1, laser1pos.y + yrot[i] + y1, -laser1pos.z);
		*/
		}

		glEnd();

	}
	lasershader->unBind();
	
	flare->bind();
	float flaresize = 128.0f;
	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++)
	{
		glColor3f(flarebri[i], flarebri[i], flarebri[i]);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(flarepos[i].x - flaresize, flarepos[i].y - flaresize * 0.5f, flarepos[i].z);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(flarepos[i].x + flaresize, flarepos[i].y - flaresize * 0.5f, flarepos[i].z);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(flarepos[i].x + flaresize, flarepos[i].y + flaresize * 0.5f, flarepos[i].z);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(flarepos[i].x - flaresize, flarepos[i].y + flaresize * 0.5f, flarepos[i].z);
	}
	glEnd();
}


void mainloop(double row)
{
	Rubber* rubber = Rubber::getInstance();

	int symbol = SYNCi("symbol:num", row);
	if (symbol > -1)
	{
		fbofx->bind();
		rubber->clear();
		symbolsfx(symbol, row);
		fbofx->unBind();
	}
	
	rubber->checkGLErrors("symbols");

	float eqbr = SYNC("eq:br", row);

	if (eqbr > 0.0f)
	{
		fbofx2->bind();
		rubber->clear();
		equalizer(row);
		rubber->checkGLErrors("eq");
		fbofx2->unBind();
	}
	
	fbo->bind();
	rubber->clear();
	
	videos(row);
	rubber->checkGLErrors("videos");
	
	if (eqbr > 0.0f) fbofx2->overlay(BlendMode(SYNCi("eq:blend", row)), eqbr);
	
	lazers(row);
	rubber->checkGLErrors("lazers");
	circlesfx(row);
	rubber->checkGLErrors("circles");
	
	if (symbol > 0) outlinefilter(fbofx, (BlendMode)SYNCi("symbol:blend", row));
	rubber->checkGLErrors("outlines");

	float strobo = SYNC("strobo", row) * 0.5f;
	glColor3f(strobo, strobo, strobo);
	glDisable(GL_TEXTURE_2D);
	rubber->quad2D(ADD);

	fbo->unBind();
	
	// glow
	fbo2->bind();
	rubber->clear();
	downsampler(fbo);
	fbo2->unBind();

	float rot = row / 128.0f * PI;
	vec2 xdir(sinf(rot), cosf(rot));
	vec2 ydir(sinf(rot + PI / 2.0f), cosf(rot + PI / 2.0f));
	
	fbo2b->bind();
	rubber->clear();
	blurfilter(fbo2, xdir, rubber->getXRes() / 80);
	fbo2b->unBind();

	fbo2c->bind();
	rubber->clear();
	blurfilter(fbo2, ydir, rubber->getXRes() / 80);
	fbo2c->unBind();

	fbo4->bind();
	rubber->clear();
	downsampler(fbo2);
	fbo4->unBind();
	
	fbo4b->bind();
	rubber->clear();
	blurfilter(fbo4, vec2(0.0, 1.0), rubber->getYRes() / 160);
	fbo4b->unBind();
	
	fbo4->bind();
	rubber->clear();
	blurfilter(fbo4b, vec2(1.0, 0.0), rubber->getXRes() / 40);
	fbo4->unBind();


	rubber->clear();
	float glowbr = SYNC("glow:glowbr", row);
	combineshader->bind();
	combineshader->bindFbo("u_source1", fbo, 0);
	combineshader->bindFbo("u_source2", fbo2b, 1);
	combineshader->bindFbo("u_source3", fbo2c, 2);
	combineshader->bindFbo("u_source4", fbo4, 3);
	combineshader->uniform1f("u_glowbr", glowbr);
	Rubber::getInstance()->quad2D(NO_BLEND);
	combineshader->unBind();

/*
	rubber->clear();
	float glowbr = SYNC("glow:glowbr", row);
	fbo->overlay(NO_BLEND, glowbr);
	fbo2b->overlay(ADD, glowbr);
	fbo2c->overlay(ADD, glowbr);
	fbo4->overlay(ADD, glowbr);
*/

/*
	fbo2b->bind();
	blurfilter(fbo2, vec2(1.0, 0.0), rubber->getXRes() / 160);
	fbo2b->unBind();
	
	fbo2->bind();
	blurfilter(fbo2b, vec2(0.0, 1.0), rubber->getYRes() / 160);
	fbo2->unBind();
	
	fbo4->bind();
	rubber->clear();
	downsampler(fbo2);
	fbo4->unBind();
	
	fbo4b->bind();
	rubber->clear();
	blurfilter(fbo4, vec2(0.0, 1.0), rubber->getYRes() / 160);
	fbo4b->unBind();
	
	fbo4->bind();
	rubber->clear();
	blurfilter(fbo4b, vec2(1.0, 0.0), rubber->getXRes() / 20);
	fbo4->unBind();

	rubber->clear();
	float glowbr = SYNC("glow:origbr", row);
	fbo4->overlay(NO_BLEND, SYNC("glow:glowbr", row));
	fbo->overlay(ADD, glowbr);
	fbo2->overlay(ADD, glowbr);
*/

}


int main(int argc, char** argv) 
{
	Rubber* rubber = Rubber::getInstance();
	rubber->init(argc, argv, (MainLoopCallBack)&mainloop);
	rubber->initBassRocket("Religion_Demo_Edit.mp3", 162);
	init();
	rubber->startPlaying();
	rubber->close();
}
