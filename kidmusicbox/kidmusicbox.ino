/*  Music box using mozzi for sound generation.

		Circuit: Audio output on digital pin 9 on a Uno or similar, or
		DAC/A14 on Teensy 3.1, or
		check the README or http://sensorium.github.com/Mozzi/

		Ulysse Rosselet, 2020, CC by-nc-sa
*/

#include "IlluminatedButton.h"
#include "RotarySwitch.h"
#include "MozziAnalogSensor.h"
#include "Wheel.h"
// #include "Synth.h"

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <WaveShaper.h>
#include <tables/brownnoise8192_int8.h> // recorded audio wavetable
#include <tables/sin2048_int8.h>        // sine table for oscillator
#include <tables/saw2048_int8.h>
#include <tables/square_no_alias512_int8.h>
#include <tables/sin256_int8.h> // sine table for wheel lfo
#include <LowPassFilter.h>

#include <tables/waveshape_compress_512_to_488_int16.h>

#include <Ead.h> // exponential attack decay
#include <EventDelay.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Adafruit_NeoPixel.h>

#define CONTROL_RATE 256 // Hz, powers of 2 are most reliable

boolean euclid16[16][16] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    {1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0},
    {1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0},
    {1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0},
    {1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0},
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
    {1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0},
    {1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0},
    {1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1},
    {1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1},
    {1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1},
    {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

int scales[8][8] = {
    {0, 2, 4, 5, 7, 9, 11, 12},     // majeur
    {0, 2, 3, 5, 7, 9, 11, 12},     // mineur jazz
    {0, 1, 3, 5, 7, 8, 11, 12},     // phrygien
    {0, 3, 3, 5, 7, 10, 10, 12},    // penta mineure
    {0, 7, 12, 15, 17, 19, 22, 24}, // 2 octave penta min
    {0, 2, 3, 5, 6, 8, 9, 11}       // ton demi-ton
};

int scalesModes[8][8] = {
    {0, 2, 4, 5, 7, 9, 11, 12}, // ionien
    {0, 2, 3, 5, 7, 9, 10, 12}, // dorien
    {0, 1, 3, 5, 7, 8, 10, 12}, // phrygien
    {0, 2, 4, 6, 7, 9, 11, 12}, // lydien
    {0, 2, 4, 5, 7, 9, 10, 12}, // mixolydien
    {0, 2, 3, 5, 7, 8, 10, 12}, // aeolien
    {0, 1, 3, 5, 6, 8, 10, 12}, // locrien
    {0, 2, 4, 5, 7, 9, 11, 12}  // ionien
};

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil<BROWNNOISE8192_NUM_CELLS, AUDIO_RATE> aNoise(BROWNNOISE8192_DATA);
Oscil<SAW2048_NUM_CELLS, AUDIO_RATE> vcoSaw(SAW2048_DATA);
Oscil<SQUARE_NO_ALIAS512_NUM_CELLS, AUDIO_RATE> vcoSquare(SQUARE_NO_ALIAS512_DATA);
Oscil<SIN2048_NUM_CELLS, CONTROL_RATE> kFilterMod(SIN2048_DATA);
Ead kEnvelope(CONTROL_RATE);
WaveShaper<int> aCompress(WAVESHAPE_COMPRESS_512_TO_488_DATA);
LowPassFilter lpf;
int gain;
int vco2semitones;

EventDelay kDelay;           // for triggering envelope start
Ead nEnvelope(CONTROL_RATE); // resolution will be CONTROL_RATE

int noiseGain;

int seqPitchLength = 8;

int gateStep = 0;
int gateStepIncrement = 1;
int pitchStep = 0;
int pitchStepIncrement = 1;
unsigned int globalStep = 0;
int tempoBPM = 180;
float grooveRatio = 0.54;
float quarterNoteMs;
int evenEigth;
int oddEigth;

int baseNote = 54;

int steppingMode = 0;
int selectedScale = 0;

int gatePatternIndex = 9;

const byte ROTARYSWITCH_PIN = 0;
const byte BUTTON_1_SWITCH_PIN = 3;
const byte BUTTON_1_LED_PIN = 5;
const byte BUTTON_2_SWITCH_PIN = 4;
const byte BUTTON_2_LED_PIN = 6;
const byte REED_SWITCH_1_PIN = 11;
const byte REED_SWITCH_2_PIN = 10;
const byte WHEEL_ENCODER_PIN1 = 7;
const byte WHEEL_ENCODER_PIN2 = 8;

IlluminatedButton buttonA(BUTTON_1_SWITCH_PIN, BUTTON_1_LED_PIN, false);
IlluminatedButton buttonB(BUTTON_2_SWITCH_PIN, BUTTON_2_LED_PIN, false);
RotarySwitch rotarySwitch(ROTARYSWITCH_PIN, 12);
Wheel wheelEncoder(WHEEL_ENCODER_PIN1, WHEEL_ENCODER_PIN2);
Button reedSwitchA(REED_SWITCH_1_PIN, false);
Button reedSwitchB(REED_SWITCH_2_PIN, false);

void setup()
{

  // use float to set freq because it will be small and fractional
  aNoise.setFreq((float)AUDIO_RATE / BROWNNOISE8192_SAMPLERATE);
  randSeed(); // fresh random, MUST be called before startMozzi - wierd bug
  lpf.setResonance(240);
  kFilterMod.setFreq(1.3f);
  vco2semitones = -24;
  quarterNoteMs = 60000 / tempoBPM;
  evenEigth = quarterNoteMs * grooveRatio;
  oddEigth = quarterNoteMs * (1.0 - grooveRatio);
  kDelay.start(quarterNoteMs);
  steppingMode = 2;
  startMozzi(CONTROL_RATE);

  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void updateControl()
{

  // buttonA.ledOn(gain); // >> 3 // broken because of synth encapsulation
  // buttonB.ledOn(gain);

  int rotarySwitchPosition = rotarySwitch.getPosition();
  steppingMode = rotarySwitchPosition % 4;
  selectedScale = rotarySwitchPosition / 2;

  if (kDelay.ready())
  {
    int wheelSpeed = wheelEncoder.getSpeed();
    float lfoFreq = 0.1f + (float)wheelSpeed / 5.f;
    kFilterMod.setFreq(0.1f + lfoFreq);
    gatePatternIndex = 8;

    if (globalStep % 64 == 0)
    {
      pitchStep = 0;
      gateStep = 0;
    }
    boolean gateON = euclid16[gatePatternIndex][gateStep % 16] == 1;
    if (gateON)
    { // action if gate ON
      int noteIndex = pitchStep % seqPitchLength;
      int note = baseNote + scales[selectedScale][noteIndex];
      vcoSaw.setFreq(mtof(note));
      vcoSquare.setFreq(mtof(note + vco2semitones));
      kEnvelope.start(5, 800);
      switch (steppingMode)
      {
      case 0:
        pitchStep += 2;
        break;
      case 1:
        pitchStep += 1 + globalStep % 16;
        break;
      case 2:
        pitchStep += 1 + globalStep % 8;
        break;
      case 3:
        pitchStep = seqPitchLength - (globalStep % seqPitchLength);
        break;
      case 4:
        if (globalStep % 3 == 0 || globalStep % 4 == 0)
        {
          pitchStep++;
        }
        break;
      default:
        pitchStep += 1;
        break;
      }
    }
    else
    { // action if gate OFF
      if (globalStep % 7 == 0)
      {
        pitchStep--;
      }
    }
    gateStep += gateStepIncrement;
    globalStep++;
    nEnvelope.start(5, 10);
    int stepMillis;
    if (globalStep % 2 == 1)
    {
      stepMillis = oddEigth;
    }
    else
    {
      stepMillis = evenEigth;
    }
    kDelay.start(stepMillis);
  }
  aNoise.setPhase(rand((unsigned int)BROWNNOISE8192_NUM_CELLS)); // jump around in audio noise table to disrupt obvious looping
  noiseGain = (int)nEnvelope.next();
  byte cutoff_freq = map(kFilterMod.next(), -127, 127, 10, 255);
  lpf.setCutoffFreq(cutoff_freq);
  gain = (int)kEnvelope.next();
}

int updateAudio()
{
  wheelEncoder.update();
  int noiseChannel = (noiseGain * aNoise.next()) >> 9;
  char asig0 = vcoSaw.next();
  char asig1 = vcoSquare.next();
  // use a waveshaping table to squeeze 2 summed 8 bit signals into the range -244 to 243
  int awaveshaped3 = aCompress.next(256u + asig0 + asig1) >> 2;
  int filtered = lpf.next(awaveshaped3);
  int filtered2 = lpf.next(filtered);
  int output = (gain * filtered2) >> 7;
  return output;
}

void loop()
{
  audioHook(); // required here
}
