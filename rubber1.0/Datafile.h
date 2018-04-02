#ifndef DATAFILE_H
#define DATAFILE_H

#include <stdio.h>
#include <string>
#include <time.h>

namespace RubberSpace {

using std::string;

class Datafile
{
public:
	Datafile(string fileName);
	~Datafile();
	int getLenght() const { return m_length; }
	unsigned char* getData() const { return m_data; }
	bool reloadIfChanged();
	const string* getName() const { return &m_fileName; }
	
private:
	int fileLength();
	time_t fileChanged();
	
	FILE* m_file;
	const string m_fileName;
	int m_length;
	time_t m_timeStamp;
	unsigned char* m_data;
};

} // RubberSpace

#endif