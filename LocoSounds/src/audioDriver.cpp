#include "audioDriver.h"

AudioDriver::AudioDriver()
{
    engineRpms = 0.0;

    bellOn = false;
    hornOn = false;
}


void AudioDriver::SetBell(bool on)
{
    bellOn = on;

    // Trigger to start the bell.
    if (bellOn && !wavs[BELL_CHANNEL]->isRunning())
    {
        files[BELL_CHANNEL] = new AudioFileSourceLittleFS(BELL_WAV);
        wavs[BELL_CHANNEL]->begin(files[BELL_CHANNEL], stubs[BELL_CHANNEL]);
    }
}


void AudioDriver::SetHorn(bool on)
{
    hornOn = on;

    // Trigger to start the horn sound.
    if (hornOn && !wavs[HORN_CHANNEL]->isRunning())
    {
        files[HORN_CHANNEL] = new AudioFileSourceLittleFS(HORN_WAV);
        wavs[HORN_CHANNEL]->begin(files[HORN_CHANNEL], stubs[HORN_CHANNEL]);
    }
}


void AudioDriver::setEngineRpms(float rpms)
{
    engineRpms = rpms;
}


void AudioDriver::Setup()
{
  // Set up an I2S output and a mixer.
  out = new AudioOutputI2S();
  mixer = new AudioOutputMixer(32, out);

  // Use channel 0 for the looped engine sounds.
  stubs[ENGINE_CHANNEL] = mixer->NewInput();
  // Gain is set by engine RPMs
  wavs[ENGINE_CHANNEL] = new AudioGeneratorWAV();

  // Use channel 1 for the periodic horn sound.
  stubs[HORN_CHANNEL] = mixer->NewInput();
  stubs[HORN_CHANNEL]->SetGain(1.0);        // 0.8 when running on desktop
  wavs[HORN_CHANNEL] = new AudioGeneratorWAV();

  // Use channel 2 for the bell sound
  stubs[BELL_CHANNEL] = mixer->NewInput();
  stubs[BELL_CHANNEL]->SetGain(0.9);        // 0.6 when running on desktop
  wavs[BELL_CHANNEL] = new AudioGeneratorWAV();
}


void AudioDriver::Loop()
{    
    bool engineOn = engineRpms >= ENGINE_RPM_IDLE - 1.0;

    // Start the engine noise if necessary.
    if (engineOn && !wavs[ENGINE_CHANNEL]->isRunning())
    {
        files[ENGINE_CHANNEL] = new AudioFileSourceLittleFS(ENGINE_WAV_LOW);
        wavs[ENGINE_CHANNEL]->begin(files[ENGINE_CHANNEL], stubs[ENGINE_CHANNEL]);
    }

    // Set engine volume based on rpms
    float enginePercent = (engineRpms - ENGINE_RPM_IDLE) / (ENGINE_RPM_MAX - ENGINE_RPM_IDLE) * 100.0;
    if (enginePercent < 0.0)
    { enginePercent = 0.0; }
    if (enginePercent > 100.0)
    { enginePercent = 100.0; }

    float engine_gain = ((enginePercent / 100.0) * 0.8) + 0.3;
    if (engine_gain > 1.0) {
        engine_gain = 1.0;
    }
    stubs[ENGINE_CHANNEL]->SetGain(engine_gain);  

    // Send the engine noise to the mixer.
    if (wavs[ENGINE_CHANNEL]->isRunning())
    {
        if (!wavs[ENGINE_CHANNEL]->loop())
        {
            // Dispose of resources when it finishes.
            wavs[ENGINE_CHANNEL]->stop(); 
            delete(files[ENGINE_CHANNEL]);

            // Restart the engine noise if the engine is still on.
            if (engineOn)
            {
                // Set the engine wav file based on rpms
                if (engineRpms < ENGINE_RPM_1) {
                    files[ENGINE_CHANNEL] = new AudioFileSourceLittleFS(ENGINE_WAV_LOW);
                }
                else if (engineRpms < ENGINE_RPM_2) {
                    files[ENGINE_CHANNEL] = new AudioFileSourceLittleFS(ENGINE_WAV_MID);
                }
                else {
                    files[ENGINE_CHANNEL] = new AudioFileSourceLittleFS(ENGINE_WAV_HIGH);
                }

                wavs[ENGINE_CHANNEL]->begin(files[ENGINE_CHANNEL], stubs[ENGINE_CHANNEL]);
            }
        } 
    }

    // Send the horn noise to the mixer.
    if (wavs[HORN_CHANNEL]->isRunning())
    {
        if (!wavs[HORN_CHANNEL]->loop())
        {
            // Dispose of resources when it finishes.
            wavs[HORN_CHANNEL]->stop(); 
            delete(files[HORN_CHANNEL]);
        } 
    }

    // Send the bell noise to the mixer.
    if (wavs[BELL_CHANNEL]->isRunning())
    {
        if (!wavs[BELL_CHANNEL]->loop())
        {
            // Dispose of resources when it finishes.
            wavs[BELL_CHANNEL]->stop();
            delete(files[BELL_CHANNEL]);

            // Restart it if the bell should still be on.
            if (bellOn)
            {
                files[BELL_CHANNEL] = new AudioFileSourceLittleFS(BELL_WAV);
                wavs[BELL_CHANNEL]->begin(files[BELL_CHANNEL], stubs[BELL_CHANNEL]);
            }
        }
    }
}