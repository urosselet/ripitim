/*  Music box using mozzi for sound generation.

		Circuit: Audio output on digital pin 9 on a Uno or similar, or
		DAC/A14 on Teensy 3.1, or
		check the README or http://sensorium.github.com/Mozzi/

		Ulysse Rosselet, 2020, CC by-nc-sa
*/



#include <MozziGuts.h>
#include <Oscil.h>                      // oscillator template
#include <tables/brownnoise8192_int8.h> // recorded audio wavetable
#include <tables/sin2048_int8.h>        // sine table for oscillator
#include <Ead.h>                        // exponential attack decay
#include <EventDelay.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <Adafruit_NeoPixel.h>


class Led {
  private:
    byte pin;
  public:
    Led(byte pin) {
      this->pin = pin;
      init();
    }
    void init() {
      pinMode(pin, OUTPUT);
      off();
    }
    void on() {
      digitalWrite(pin, HIGH);
    }
    void off() {
      digitalWrite(pin, LOW);
    }
};

class Button {
  private:
    byte pin;
    byte state;
    byte lastReading;
    unsigned long lastDebounceTime = 0;
    unsigned long debounceDelay = 20;
  public:
    Button(byte pin) {
      this->pin = pin;
      lastReading = LOW;
      init();
    }
    void init() {
      pinMode(pin, INPUT);
      update();
    }
    void update() {
      byte newReading = digitalRead(pin);

      if (newReading != lastReading) {
        lastDebounceTime = millis();
      }
      if (millis() - lastDebounceTime > debounceDelay) {
        state = newReading;
      }
      lastReading = newReading;
    }
    byte getState() {
      update();
      return state;
    }
    bool isPressed() {
      return (getState() == HIGH);
    }
};

class RotarySwitch{
  private:
  byte pin;
  byte numValue;
  public:
  RotarySwitch();
};


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
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

Led led1(3);
Button button1(2);

int scales[8][8] = {
  {0, 2, 4, 5, 7, 9, 11, 12}, // majeur
  {0, 2, 3, 5, 7, 9, 11, 12}, // mineur jazz
  {0, 1, 3, 5, 7, 8, 11, 12}, // phrygien
  {0, 3, 3, 5, 7, 10, 10, 12}, // penta mineure
  {0, 7, 12, 15, 17, 19, 22, 24}, // 2 octave penta min
  {0, 2, 3, 5, 6, 8, 9, 11} // ton demi-ton
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
Oscil<SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);
Oscil<BROWNNOISE8192_NUM_CELLS, AUDIO_RATE> aNoise(BROWNNOISE8192_DATA);
EventDelay kDelay;           // for triggering envelope start
Ead kEnvelope(CONTROL_RATE); // resolution will be CONTROL_RATE
Ead nEnvelope(CONTROL_RATE); // resolution will be CONTROL_RATE

int gain;
int noiseGain;

int seqPitchLength = 8;

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

int steppingMode = 0;
int selectedScale = 0;

int gatePatternIndex = 6;

const char ROTARY_SWITCH_INPUT_PIN = 0;
const char WHEEL_INPUT_PIN = 1;

#define NEOPIXELPIN 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);

void setup()
{
  strip.begin();
  strip.setBrightness(16);
  strip.show(); // Initialize all pixels to 'off'

  // use float to set freq because it will be small and fractional
  aNoise.setFreq((float)AUDIO_RATE / BROWNNOISE8192_SAMPLERATE);
  aSin.setFreq(440); // set the frequency
  randSeed();        // fresh random, MUST be called before startMozzi - wierd bug
  startMozzi(CONTROL_RATE);
  quarterNoteMs = 60000 / tempoBPM;
  evenEigth = quarterNoteMs * grooveRatio;
  oddEigth = quarterNoteMs * (1.0 - grooveRatio);
  kDelay.start(quarterNoteMs);
  steppingMode = 2;

  Serial.begin(9600);
  while (!Serial)
  {
    ; // wait for serial port to connect. Needed for native USB port only
  }
}


void updateControl()
{

  if (button1.isPressed()) {
    led1.on();
  }
  else {
    led1.off();
  }

  int rotarySwitchRawValue = mozziAnalogRead(ROTARY_SWITCH_INPUT_PIN);
  int rotarySwitchPosition = map(rotarySwitchRawValue, 0, 1023, 0, 11);
  steppingMode =  rotarySwitchPosition % 4;
  selectedScale = rotarySwitchPosition / 2;
  int wheelRawValue = mozziAnalogRead(WHEEL_INPUT_PIN);
  int gatePatternIndex = wheelRawValue >> 6;

  uint32_t color;
  switch (selectedScale) {
    case 0:
      color = strip.Color(255, 0, 0);
      break;
    case 1:
      color = strip.Color(0, 255, 0);
      break;
    case 2:
      color = strip.Color(0, 0, 255);
      break;
    case 3:
      color = strip.Color(255, 0, 255);
      break;
    case 4:
      color = strip.Color(255, 255, 0);
      break;
    case 5:
      color = strip.Color(0, 255, 255);
      break;

  }


  if (kDelay.ready())
  {
    //    Serial.print(steppingMode);
    //    Serial.print("\t");

    if (globalStep % 64 == 0)
    {
      pitchStep = 0;
      gateStep = 0;
    }
    boolean gateON = euclid16[gatePatternIndex][gateStep % 16] == 1;
    //    Serial.print(gateON);
    //    Serial.print("\t");

    if (gateON)
    { // action if gate ON
      strip.clear();
      kEnvelope.start(5, 600);
      int noteIndex = pitchStep % seqPitchLength;
      int note = baseNote + scales[selectedScale][noteIndex];
      //      Serial.print(noteIndex);
      //      Serial.print("\t");
      //      Serial.print(note);
      //      Serial.print("\t");
      aSin.setFreq(mtof(float(note)));


      strip.setPixelColor(noteIndex, color);
      strip.show();

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
    //    Serial.println();
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
