#ifndef KEWLMATION_H
#define KEWLMATION_H

#include <string>
#include "rubber1.0/Rubber.h"
#include "rubber1.0/Datafile.h"

using std::string;
using namespace RubberSpace;

typedef unsigned short uint16;

class Kewlmation
{
public:
	Kewlmation(const string filename);
	~Kewlmation();
	void renderFrame(float pos, float r, float g, float b);
	unsigned int getNumFrames() const { return m_numFrames; }

private:
	Datafile* m_dataFile;
	unsigned int m_numFrames;
	unsigned int m_dataSize;
	unsigned int* m_frameInfo;
	unsigned char* m_frames;
	static const bool SIMPLEMODE = true;
};

#endif