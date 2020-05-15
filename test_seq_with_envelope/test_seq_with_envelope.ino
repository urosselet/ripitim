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
#include <Ead.h> // exponential attack decay
#include <EventDelay.h>
#include <mozzi_rand.h>

#define CONTROL_RATE 256 // Hz, powers of 2 are most reliable

Oscil<BROWNNOISE8192_NUM_CELLS, AUDIO_RATE> aNoise(BROWNNOISE8192_DATA);
EventDelay kDelay; // for triggering envelope start
Ead kEnvelope(CONTROL_RATE); // resolution will be CONTROL_RATE

int gain;

int seqGate[]={1,1,0,1,0};
int seqIndex = 0;

void setup(){
  // use float to set freq because it will be small and fractional
  aNoise.setFreq((float)AUDIO_RATE/BROWNNOISE8192_SAMPLERATE);
  randSeed(); // fresh random, MUST be called before startMozzi - wierd bug
  startMozzi(CONTROL_RATE);
  kDelay.start(500);
}


void updateControl(){
  // jump around in audio noise table to disrupt obvious looping
  aNoise.setPhase(rand((unsigned int)BROWNNOISE8192_NUM_CELLS));

  if(kDelay.ready()){
    if(seqGate[seqIndex % sizeof(seqGate)] == 1){ 
          kEnvelope.start(5,800);
    } else {
    }
    kDelay.start(500);
    // move to next step
    seqIndex++;
  }
  gain = (int) kEnvelope.next();
}


int updateAudio(){
  return (gain*aNoise.next())>>8;
}


void loop(){
  audioHook(); // required here
}
