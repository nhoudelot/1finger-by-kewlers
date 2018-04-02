#ifndef BASSROCKET_H
#define BASSROCKET_H

#include <cstdio>
#include <string.h>
#include <map>
#include "../bass/bass.h"
#include "../rocket/sync-cpp.h"

namespace RubberSpace
{

#define SYNC(a,b) Rubber::getInstance()->getBassRocket()->getValue((a),(b))
#define SYNCi(a,b) (int)floor(Rubber::getInstance()->getBassRocket()->getValue((a),(b)))

class BassTimer : public Sync::Timer {
public:
	BassTimer(HSTREAM stream, float bpm, int rowsPerBeat) : stream(stream)
	{
		rowRate = (double(bpm) / 60) * rowsPerBeat;
	}

	// BASS hooks
	void  pause()           { BASS_ChannelPause(stream); }
	void  play()            { BASS_ChannelPlay(stream, false); }
	float getTime()         { return float(BASS_ChannelBytes2Seconds(stream, BASS_ChannelGetPosition(stream, BASS_POS_BYTE))); }
	float getRow()          { return float(getTime() * rowRate); }
	void  setRow(float row) { BASS_ChannelSetPosition(stream, BASS_ChannelSeconds2Bytes(stream, float(row / rowRate)), BASS_POS_BYTE); }
	bool  isPlaying()       { return (BASS_ChannelIsActive(stream) == BASS_ACTIVE_PLAYING); }

	HSTREAM getStream() { return stream; }
	
private:
	HSTREAM stream;
	double rowRate;
};

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};


class BassRocket
{
public:
	BassRocket();	
	void init(std::string filename, float bpm);
	void close();
	void startPlaying();
	double getRow();
	void setRow(double row);
	float getValue(const char* name, double row);
	void update();
	bool isPlaying() const { return m_pTimer->isPlaying(); }
	void getFFT(void* buffer, DWORD length);
	float getBpm() const { return m_bpm; }
	int getRowsPerBeat() const { return ROWS_PER_BEAT; }

private:
	BassTimer* m_pTimer;
	Sync::Device* m_pDevice;
	std::map<const char*, sync_track*, ltstr> m_tracks;
	float m_bpm;
	static const int ROWS_PER_BEAT = 8;
};

} // RubberSpace

#endif
