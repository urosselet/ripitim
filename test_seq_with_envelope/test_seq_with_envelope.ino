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
#include <Oscil.h> // oscillator template
#include <tables/brownnoise8192_int8.h> // recorded audio wavetable
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <Ead.h> // exponential attack decay
#include <EventDelay.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>


#define CONTROL_RATE 256 // Hz, powers of 2 are most reliable

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);
Oscil <BROWNNOISE8192_NUM_CELLS, AUDIO_RATE> aNoise(BROWNNOISE8192_DATA);
EventDelay kDelay; // for triggering envelope start
Ead kEnvelope(CONTROL_RATE); // resolution will be CONTROL_RATE
Ead nEnvelope(CONTROL_RATE); // resolution will be CONTROL_RATE

int gain;
int noiseGain;

int seqGate[] = {1, 1, 0, 1, 0, 0, 1};
int seqGateLength = 7;
int seqPitch[] = {0, 4, 7, 12, 19, 16, 7, -5, -1};
int seqPitchLength = 9;
int seqIndex = 0;
int pitchIndex = 0;

int baseNote = 60;

void setup() {
  // use float to set freq because it will be small and fractional
  aNoise.setFreq((float)AUDIO_RATE / BROWNNOISE8192_SAMPLERATE);
  aSin.setFreq(440); // set the frequency
  randSeed(); // fresh random, MUST be called before startMozzi - wierd bug
  startMozzi(CONTROL_RATE);
  kDelay.start(200);
}


void updateControl() {
  // jump around in audio noise table to disrupt obvious looping
  aNoise.setPhase(rand((unsigned int)BROWNNOISE8192_NUM_CELLS));

  if (kDelay.ready()) {

    if (seqGate[seqIndex % seqGateLength] == 1) { // action if gate ON
      kEnvelope.start(5, 600);
      int note = baseNote + seqPitch[pitchIndex % seqPitchLength];
      aSin.setFreq(mtof(float(note)));
      pitchIndex ++;
    } else { // action if gate OFF
    }
    seqIndex++;
    nEnvelope.start(20, 10);
    kDelay.start(200);
  }
  gain = (int) kEnvelope.next();
  noiseGain = (int) nEnvelope.next();
}


int updateAudio() {
  int noiseChannel = noiseGain * aNoise.next()>>4;
  int sineChannel = gain * aSin.next()>>1;
  return (noiseChannel + sineChannel) >> 8;
}


void loop() {
  audioHook(); // required here
}
