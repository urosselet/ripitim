#include "StepSequencer.h"
#include <Arduino.h>

StepSequencer::StepSequencer(int resetLength)
{
    this->resetLength = resetLength;
}

int StepSequencer::step(unsigned int globalStep)
{
    int note = 0;
    if (globalStep % resetLength == 0)
    {
        pitchStep = 0;
        gateStep = 0;
    }
    boolean gateON = euclid16[gatePatternIndex][gateStep % 16] == 1;
    if (gateON)
    { // action if gate ON
        int noteIndex = pitchStep % seqPitchLength;
        note = baseNote + scales[selectedScale][noteIndex];

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
    return note;
}

void StepSequencer::setSteppingMode(int steppingMode)
{
    this->steppingMode = steppingMode;
}
void StepSequencer::selectScale(int scale)
{
    this->selectedScale = scale;
}
void StepSequencer::selectGatePattern(int pattern)
{
    this->gatePatternIndex = pattern;
}