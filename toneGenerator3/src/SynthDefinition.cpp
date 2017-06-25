//
//  SynthDefinition.cpp
//  toneGenerator3
//
//  Created by Zachary Snow on 24/06/2017.
//
//

#include <SynthDefinition.h>

SynthController synthController;
GeneratorContainer generatorContainer;

float SynthUtil::noteFrequencies[MIDI_NOTE_NUMBER];
PanningData SynthUtil::panningLinear;
PanningData SynthUtil::panningSquared;
PanningData SynthUtil::panningSine;
float SynthUtil::wavetableSine[WAVETABLE_SIZE];
float SynthUtil::wavetableSquare[WAVETABLE_SIZE];
float SynthUtil::wavetableSaw[WAVETABLE_SIZE];
float SynthUtil::wavetableTriangle[WAVETABLE_SIZE];
float SynthUtil::wavetablePulse[WAVETABLE_SIZE];
float SynthUtil::wavetableSawDirectCalc[WAVETABLE_SIZE];
float SynthUtil::wavetableTriangleDirectCalc[WAVETABLE_SIZE];
float SynthUtil::wavetableSquareDirectCalc[WAVETABLE_SIZE];
float SynthUtil::wavetableSawPositive[WAVETABLE_SIZE];
float SynthUtil::wavetableTrianglePositive[WAVETABLE_SIZE];
