// #include <Arduino.h>
// #include <ESP8266WiFi.h>
// #include "AudioFileSourceLittleFS.h"
// #include "AudioGeneratorWAV.h"
// #include "AudioOutputI2S.h"

// AudioGeneratorWAV *wav;
// AudioFileSourceLittleFS *file;
// AudioOutputI2S *out;

// #define LRC D4
// #define BCLK D8
// #define DIN D9

// void setup() {
//   delay(1000);
//   LittleFS.begin();
//   file = new AudioFileSourceLittleFS("/horn.wav");
//   out = new AudioOutputI2S();
//   wav = new AudioGeneratorWAV();
//   //wav->begin(file, out);
//   out->begin();
// }

// int i = 0;

// void loop() {
//   if (i == 2000)
//   {
//     wav->begin(file, out);
//     i++;
//   }
//   else if (i < 2000)
//   {
//     int16_t nullSample[2];
//     nullSample[0] = 0;
//     nullSample[1] = 0;
//     out->ConsumeSample(nullSample);
//     delay(1);
//     i++;
//     return;
//   }
//   else if (wav->isRunning())
//   {
//     if (!wav->loop())
//     {
//       wav->stop(); 
//       delete(file);
//       //delete(wav);
//       file = new AudioFileSourceLittleFS("/alco-high.wav");
//       //wav = new AudioGeneratorWAV();
//       wav->begin(file, out);
//     } 
//   }
//   else
//   {
//     //Serial.printf("WAVE done\n");
    
//   }  
// }