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
#include "StepSequencer.h"
#include "Synth.h"

#include <MozziGuts.h>

#include <Ead.h> 
#include <EventDelay.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>

#define CONTROL_RATE 256 // Hz, powers of 2 are most reliable

Synth synth;

EventDelay stepDelay;   // for triggering steps

unsigned int globalStep = 0;
int tempoBPM = 180;
float grooveRatio = 0.54;
float quarterNoteMs;
int evenEigth;
int oddEigth;

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
StepSequencer stepSequencer(64);

void setup()
{
  randSeed(); // fresh random, MUST be called before startMozzi - wierd bug
  quarterNoteMs = 60000 / tempoBPM;
  evenEigth = quarterNoteMs * grooveRatio;
  oddEigth = quarterNoteMs * (1.0 - grooveRatio);
  stepDelay.start(quarterNoteMs);
  stepSequencer.setSteppingMode(2);
  startMozzi(CONTROL_RATE);

  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}

void updateControl()
{

  // buttonA.ledOn(vcaLevel); // >> 3 // broken because of synth encapsulation
  // buttonB.ledOn(vcaLevel);

  int rotarySwitchPosition = rotarySwitch.getPosition();
  int steppingMode = rotarySwitchPosition % 4;
  stepSequencer.setSteppingMode(steppingMode);
  int selectedScale = rotarySwitchPosition / 2;
  stepSequencer.selectScale(selectedScale);

  if (stepDelay.ready())
  {
    int wheelSpeed = wheelEncoder.getSpeed();
    float lfoFreq = 0.1f + (float)wheelSpeed / 5.f;
    synth.setModulationFreq(lfoFreq);
    stepSequencer.selectGatePattern(8);
    int note = stepSequencer.step(globalStep);
    if (note > 0)
    {
      synth.playNote(note, 5, 800);
    }

    globalStep++;
    int stepMillis;
    if (globalStep % 2 == 1)
    {
      stepMillis = oddEigth;
    }
    else
    {
      stepMillis = evenEigth;
    }
    stepDelay.start(stepMillis);
  }
  synth.controlHook();
}

int updateAudio()
{
  wheelEncoder.update();
  int output = synth.audioHook();
  return output;
}

void loop()
{
  audioHook(); // required here
}
