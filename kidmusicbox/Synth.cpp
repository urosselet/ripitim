#include "Synth.h"
#include <Arduino.h>
#include <MozziGuts.h>
#include <mozzi_midi.h>

void Synth::init()
{
    lpf.setResonance(240);
    kFilterMod.setFreq(1.3f);
    vco2semitones = -24;
}

int Synth::audioHook()
{
    char asig0 = vcoSaw.next();
    char asig1 = vcoSquare.next();
    // use a waveshaping table to squeeze 2 summed 8 bit signals into the range -244 to 243
    int awaveshaped3 = aCompress.next(256u + asig0 + asig1) >> 2;
    int filtered = lpf.next(awaveshaped3);
    int filtered2 = lpf.next(filtered);
    int output = (gain * filtered2) >> 7;
    return output;
}

void Synth::controlHook()
{
    byte cutoff_freq = map(kFilterMod.next(), -127, 127, 10, 255);
    lpf.setCutoffFreq(cutoff_freq);
    gain = (int)kEnvelope.next();
}

void Synth::setModulationFreq(float lfoFreq)
{
    kFilterMod.setFreq(0.1f + lfoFreq);
}

void Synth::playNote(int note, int attack, int decay)
{
    vcoSaw.setFreq(mtof(note));
    vcoSquare.setFreq(mtof(note + vco2semitones));
    kEnvelope.start(attack, decay);
}
