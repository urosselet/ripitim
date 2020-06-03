#ifndef STEPSEQUENCER_H
#define STEPSEQUENCER_H
#include <Arduino.h>
#include <MozziGuts.h>

class StepSequencer
{
private:
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

  int baseNote = 54;
  int seqPitchLength = 8;
  int gateStep = 0;
  int gateStepIncrement = 1;
  int pitchStep = 0;
  int pitchStepIncrement = 1;
  int steppingMode = 0;
  int selectedScale = 0;
  int gatePatternIndex = 9;
  int resetLength = 64;

public:
  StepSequencer(int resetLength);
  void setSteppingMode(int steppingMode);
  void selectScale(int scale);
  void selectGatePattern(int pattern);
  int step(unsigned int globalStepNumber);
};

#endif