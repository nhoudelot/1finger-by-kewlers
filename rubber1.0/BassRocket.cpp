#include "Rubber.h"
#include "BassRocket.h"
#include "../bass/bass.h"
#include "../rocket/sync/base.h"

using namespace RubberSpace;


BassRocket::BassRocket()
	: m_pDevice(NULL)
	, m_pTimer(NULL)
{
}

void BassRocket::init(std::string filename, float bpm)
{
	m_bpm = bpm;

	//if (!BASS_Init(-1,44100,0,win,NULL)) {
	if (!BASS_Init(-1, 44100, 0, NULL, NULL)) ERROR("Failed to initialize BASS\n\n");
	
	//	BASS_STREAM_PRESCAN ?
	// | BASS_STREAM_DECODE ?
	char fname[128];
	sprintf(fname, "%s%s", DATAPATH, filename.c_str());
	HSTREAM stream = BASS_StreamCreateFile(false, fname, 0, 0, BASS_MP3_SETPOS);
	if (!stream) Rubber::getInstance()->Error("Failed to open %s\n", fname);
		
	m_pTimer = new BassTimer(stream, bpm, ROWS_PER_BEAT);

	char datafiles[128];
	sprintf(datafiles, "%ssync", DATAPATH);
	m_pDevice = Sync::createDevice(datafiles, (Sync::Timer&)*m_pTimer);
	if (!m_pDevice) ERROR("Failed to create sync device\n");

#ifndef SYNC_PLAYER
	if (m_pDevice->connect()) ERROR("Error connecting...\n");
	else LOG("Rocket is connected...\n");
#endif

	//testTrack = m_pDevice->getTrack("testgroup:test");
}


void BassRocket::close()
{
	LOG("closing bass + rocket\n");
	BASS_StreamFree(m_pTimer->getStream());
	delete m_pTimer;
	delete m_pDevice;
	BASS_Free();
}

void BassRocket::startPlaying()
{
	if (!BASS_Start()) ERROR("Error while BASS_Start: %i\n", BASS_ErrorGetCode());
	if (!BASS_ChannelPlay(m_pTimer->getStream(), false)) ERROR("BASS_ChannelPlay failed: %i\n" ,BASS_ErrorGetCode());
	LOG("Started playing\n");
}

double BassRocket::getRow()
{
#ifdef SYNC_PLAYER
	if (!m_pTimer->isPlaying()) Rubber::getInstance()->setDone(); // final version?
#endif
	float row = float(m_pTimer->getRow());
	
#ifndef SYNC_PLAYER
	if (m_pDevice->update(row, (void*)m_pTimer))
	{
		LOG("Reconnecting... ");
		if (m_pDevice->connect()) LOG("Failed\n");
		else LOG("Succeeded\n");
	}
#endif

	return row;
}

void BassRocket::setRow(double row)
{
	m_pTimer->setRow(row);
}

float BassRocket::getValue(const char* name, double row)
{
	if (m_tracks[name] == NULL) m_tracks[name] = m_pDevice->getTrack(name);
	return sync_get_val(m_tracks[name], row);	
}


void BassRocket::update()
{
	BASS_Update(0);
}


void BassRocket::getFFT(void* buffer, DWORD length)
{
	BASS_ChannelGetData(m_pTimer->getStream(), buffer, length);
}

