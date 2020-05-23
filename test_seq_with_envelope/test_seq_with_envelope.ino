/*  Example playing an enveloped noise source
		using Mozzi sonification library.

		Demonstrates Ead (exponential attack decay).

		Circuit: Audio output on digital pin 9 on a Uno or similar, or
		DAC/A14 on Teensy 3.1, or
		check the README or http://sensorium.github.com/Mozzi/

		Mozzi help/discussion/announcements:
		https://groups.google.com/forum/#!forum/mozzi-users

		Tim Barrass 2012, CC by-nc-sa
*/

#include <MozziGuts.h>
#include <Oscil.h>                      // oscillator template
#include <tables/brownnoise8192_int8.h> // recorded audio wavetable
#include <tables/sin2048_int8.h>        // sine table for oscillator
#include <Ead.h>                        // exponential attack decay
#include <EventDelay.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>

#define CONTROL_RATE 256 // Hz, powers of 2 are most reliable

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);
Oscil<BROWNNOISE8192_NUM_CELLS, AUDIO_RATE> aNoise(BROWNNOISE8192_DATA);
EventDelay kDelay;           // for triggering envelope start
Ead kEnvelope(CONTROL_RATE); // resolution will be CONTROL_RATE
Ead nEnvelope(CONTROL_RATE); // resolution will be CONTROL_RATE

int gain;
int noiseGain;

//int seqGate[] = {1,1, 1, 1, 1, 1, 1, 1};
int seqGate[] = {1, 0, 0, 1, 1, 0, 1, 0};
int seqGateLength = 8;
int seqPitch[] = {0, 2, 4, 5, 7};
//int seqPitch[] = {0, 4, 7, 12, 19, 16, 7, -5, -1};
int seqPitchLength = 5;

int gateStep = 0;
int gateStepIncrement = 1;
int pitchStep = 0;
int pitchStepIncrement = 1;
int globalStep = 0;
int globalStepIncrement = 1;
int tempoBPM = 160;
float grooveRatio = 0.56;
float quarterNoteMs;
int evenEigth;
int oddEigth;

int baseNote = 60;

char mode = 'A';

void setup()
{
  // use float to set freq because it will be small and fractional
  aNoise.setFreq((float)AUDIO_RATE / BROWNNOISE8192_SAMPLERATE);
  aSin.setFreq(440); // set the frequency
  randSeed();        // fresh random, MUST be called before startMozzi - wierd bug
  startMozzi(CONTROL_RATE);
  quarterNoteMs = 60000 / tempoBPM;
  evenEigth = quarterNoteMs * grooveRatio;
  oddEigth = quarterNoteMs * (1.0 - grooveRatio);
  kDelay.start(quarterNoteMs);
  mode = 'C';

  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}


void updateControl()
{
  if (Serial.available() > 0)
  {
    mode = Serial.read();
  }
  if (kDelay.ready())
  {
    Serial.print(mode);
    Serial.print("\t");

    if (globalStep % 64 == 0)
    {
      pitchStep = 0;
      gateStep = 0;
      //if(mode == 'A'){ mode = 'B';} else {mode = 'A';}
    }
    boolean gateON = seqGate[gateStep % seqGateLength] == 1;
    Serial.print(gateON);
    Serial.print("\t");

    if (gateON)
    { // action if gate ON
      kEnvelope.start(5, 600);
      int noteIndex = pitchStep % seqPitchLength;
      Serial.print(noteIndex);
      Serial.print("\t");

      int note = baseNote + seqPitch[noteIndex];
      Serial.print(note);
      Serial.print("\t");
      aSin.setFreq(mtof(float(note)));
      switch (mode)
      {
      case 'A':
        pitchStep += 2;
        break;
      case 'B':
        pitchStep += 1 + globalStep % 16;
        break;
      case 'C':
        pitchStep += 1 + globalStep % 8;
        break;
      case 'D':
        pitchStep = seqPitchLength - (globalStep % seqPitchLength);
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
        //        pitchStep --;
      }
    }
    gateStep += gateStepIncrement;
    globalStep += globalStepIncrement;
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
    Serial.println();
  }
  aNoise.setPhase(rand((unsigned int)BROWNNOISE8192_NUM_CELLS)); // jump around in audio noise table to disrupt obvious looping
  gain = (int)kEnvelope.next();
  noiseGain = (int)nEnvelope.next();
}

int updateAudio()
{
  int noiseChannel = noiseGain * aNoise.next() >> 4;
  int sineChannel = gain * aSin.next() >> 1;
  return (noiseChannel + sineChannel) >> 8;
}

void loop()
{
  audioHook(); // required here
}
