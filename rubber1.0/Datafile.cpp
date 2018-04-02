#include "Datafile.h"
#include "Rubber.h"
#include <sys/types.h>
#include <sys/stat.h>


using namespace RubberSpace;
using std::string;


Datafile::Datafile(string fileName)
	: m_fileName(DATAPATH + fileName)
{
	m_file = fopen(m_fileName.c_str(), "rb");

	if (!m_file)
	{
		ERROR("Error opening file: %s\n", m_fileName.c_str());
	}
	
	m_length = fileLength();
	m_data = new unsigned char[m_length];
	fread(m_data, m_length, 1, m_file);
	fclose(m_file);
	
	LOG("Loaded %s, %i bytes\n", fileName.c_str(), m_length);
}

Datafile::~Datafile()
{
	delete[] m_data;
}

int Datafile::fileLength()
{
	fseek(m_file, 0, SEEK_END);
	long sz = ftell(m_file);
	fseek(m_file, 0, SEEK_SET);
	return (int)sz;
}

time_t Datafile::fileChanged()
{
	struct stat fst;
	bzero(&fst, sizeof(fst));

	if (stat(m_fileName.c_str(), &fst) != 0) ERROR("stat() failed for %s", m_fileName.c_str());

	return fst.st_mtime;
}

bool Datafile::reloadIfChanged()
{
	time_t changed = fileChanged();
	
	if (m_timeStamp != changed)
	{
		m_file = fopen(m_fileName.c_str(), "rb");
		if (!m_file)
		{
			ERROR("Error opening file: %s\n", m_fileName.c_str());
		}
	
		m_length = fileLength();
		delete[] m_data;
		m_data = new unsigned char[m_length];
		fread(m_data, m_length, 1, m_file);
		fclose(m_file);
		m_timeStamp = changed;
		return true;
	}
	
	return false;
}

