#include "Kewlmation.h"
#include <stdio.h>

Kewlmation::Kewlmation(const string filename)
	: m_dataFile(NULL)
	, m_numFrames(0)
	, m_dataSize(0)
	, m_frameInfo(NULL)
	, m_frames(NULL)
{
	m_dataFile = new Datafile(filename);
	unsigned char* data = m_dataFile->getData();
	memcpy(&m_numFrames, data, 4);
	memcpy(&m_dataSize, data + 4, 4);
	m_frameInfo = (unsigned int*)(data + 8);
	m_frames = data + 8 + sizeof(unsigned int) * m_numFrames * 2;
}

Kewlmation::~Kewlmation()
{
	delete m_dataFile;
}

void Kewlmation::renderFrame(float pos, float r, float g, float b)
{
	unsigned int frame = pos * m_numFrames;
	frame = frame % m_numFrames;
	unsigned int numTriangles = m_frameInfo[frame * 2 + 1];
	unsigned char* data = m_frames + m_frameInfo[frame * 2 + 0];
	unsigned int i, j;
	//Rubber* rubber = Rubber::getInstance();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
		
	if (SIMPLEMODE)
	{
		glBegin(GL_TRIANGLES);
		for (i = 0; i < numTriangles; i++)
		{
			unsigned char* d = data + i * 7;
			float br = (float)(d[0]) / 255.0f;
			glColor3f(r * br, g * br, b * br);
			float v[6];
			
			for (j = 0; j < 6 ; j++) v[j] = (float)(d[j + 1] / 255.0f) * 2.0f - 1.0f;
			
			glVertex2f(v[0], v[1]);
			glVertex2f(v[2], v[3]);
			glVertex2f(v[4], v[5]);
		}
		glEnd();
		

		Rubber::getInstance()->setBlendMode(ALPHA_ADD);

 		float lineWidth = Rubber::getInstance()->getXRes() / 640.0f;
		if (lineWidth < 1.0f) lineWidth = 1.0f;
		glLineWidth(lineWidth);
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_LINE_SMOOTH);
		glBegin(GL_TRIANGLES);
		for (i = 0; i < numTriangles; i++)
		{
			unsigned char* d = data + i * 7;
			float br = (float)(d[0]) / 255.0f * 0.5f;
			glColor3f(r * br, g * br, b * br);
			float v[6];
			
			for (j = 0; j < 6 ; j++) v[j] = (float)(d[j + 1] / 255.0f) * 2.0f - 1.0f;
			
			glVertex2f(v[0], v[1]);
			glVertex2f(v[2], v[3]);
			glVertex2f(v[4], v[5]);
		}
		glEnd();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else
	{
		glBegin(GL_TRIANGLES);
		for (i = 0; i < numTriangles; i++)
		{
			for (j = 0; j < 3; j++)
			{
				unsigned char* v = data + i * 12 + j * 4;
				uint16 col = v[2] * 256 + v[3];
				float rr, gg, bb;
				rr = (float)(col >> 11) / 31.0;
				gg = (float)((col >> 5) & 63) / 63.0;
				bb = (float)(col & 31) / 31.0;
				glColor3f(r * rr, g * gg, b * bb);
				glVertex2f((float)(v[0] / 255.0) * 2.0 - 1.0, (float)(v[1] / 255.0) * 2.0 - 1.0);
			}
		}
		glEnd();
	}

}
