#include "config.h"

#include "AudioFileSourceLittleFS.h"
#include "AudioGeneratorWAV.h"
#include "AudioOutputI2S.h"
#include "AudioOutputMixer.h"


#ifndef AUDIODRIVER_H
#define AUDIODRIVER_H

#define ENGINE_WAV_LOW            "/alco-low.wav"
#define ENGINE_WAV_MID            "/alco-mid.wav"
#define ENGINE_WAV_HIGH           "/alco-high.wav"
#define HORN_WAV                  "/horn.wav"
#define BELL_WAV                  "/bell.wav"

#define ENGINE_CHANNEL      0
#define HORN_CHANNEL        1
#define BELL_CHANNEL        2



class AudioDriver
{
    float engineRpms;
    float filteredEngineRpms;

    bool bellOn;
    bool hornOn;

    AudioGeneratorWAV *wavs[3];
    AudioFileSourceLittleFS *files[3];
    AudioOutputI2S *out;
    AudioOutputMixer *mixer;
    AudioOutputMixerStub *stubs[3];

public:
    AudioDriver();

    void Setup();
    void Loop();

    void SetBell(bool on);
    void SetHorn(bool on);
    void setEngineRpms(float rpms);
};





#endif