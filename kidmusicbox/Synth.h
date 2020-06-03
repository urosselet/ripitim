#ifndef SYNTH_H
#define SYNTH_H
#include <Arduino.h>
#include <tables/saw2048_int8.h>
#include <tables/square_no_alias512_int8.h>
#include <tables/sin2048_int8.h>
#include <tables/waveshape_compress_512_to_488_int16.h>
#include <LowPassFilter.h>
#include <Ead.h>
#include <Oscil.h>
#include <WaveShaper.h>

class Synth
{
private:
  Oscil<SAW2048_NUM_CELLS, AUDIO_RATE> vcoSaw;
  Oscil<SQUARE_NO_ALIAS512_NUM_CELLS, AUDIO_RATE> vcoSquare;
  Oscil<SIN2048_NUM_CELLS, CONTROL_RATE> filterLFO;
  LowPassFilter lpf;
  Ead kEnvelope;
  WaveShaper<int> aCompress;
  int gain;
  int vco2semitones;

public:
  Synth() : vcoSaw(SAW2048_DATA),
            vcoSquare(SQUARE_NO_ALIAS512_DATA),
            filterLFO(SIN2048_DATA),
            kEnvelope(CONTROL_RATE),
            aCompress(WAVESHAPE_COMPRESS_512_TO_488_DATA)
  {
    init();
  };
  void init();
  void controlHook();
  int audioHook();
  void setModulationFreq(float lfoFreq);
  void playNote(int note, int attack, int decay);
};

#endif