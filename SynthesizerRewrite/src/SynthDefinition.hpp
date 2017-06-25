//
//  SynthDefinition.h
//  toneGenerator3
//
//  Created by Zachary Snow on 23/06/2017.
//
//

#pragma once

#include "ofMain.h"

#define ONE_DIVIDE_PI 1.0/PI
#define TWO_DIVIDE_PI 2.0/PI


#define FILE_WRITE_BUFFER_SIZE 4096


#define MIDI_NOTE_NUMBER 128


#define PANNING_TABLE_SIZE 8016


#define WAVETABLE_SIZE 65536
#define WAVETABLE_NUMBER 10

#define WAVETABLE_SINE 0
#define WAVETABLE_SQUARE 1
#define WAVETABLE_SAW 2
#define WAVETABLE_TRIANGLE 3
#define WAVETABLE_PULSE 4
#define WAVETABLE_SAW_DIRECT_CALC 5
#define WAVETABLE_TRIANGLE_DIRECT_CALC 6
#define WAVETABLE_SQUARE_DIRECT_CALC 7
#define WAVETABLE_SAW_POSITIVE 8
#define WAVETABLE_TRIANGLE_POSITIVE 9


#define AUDIO_BUFFER_SIZE 4096
#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_CHANNEL_NUMBER 2

#define WAVE_MANAGER_NUMBER 10
#define WAVE_INSTANCE_BUFFER 2


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum PanningType
{
    PanningLinear,
    PanningSquared,
    PanningSine
};
enum WaveType
{
    SineWave,
    SawWave,
    TriangleWave,
    SquareWave,
    Pulse,
    Summed,
    Oscillator,
    FrequencyModulation,
    AmplitudeModulation,
    RingModulation
};

struct PanningData
{
    float left[PANNING_TABLE_SIZE];
    float right[PANNING_TABLE_SIZE];
};
struct Panning
{
    float left;
    float right;
};

struct OscillatorData
{
    int numberPartials;
    vector<float> partials;
    vector<float> amplitude;
    bool gibbs;
};

struct ModulationData
{
    float modulationMultiplier;
    float modulationAmplitude;
};

struct SquareWaveData
{
    float dutyCycle;
    float amplitudeMin;
    float amplitudeMax;
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Segment
{
    float duration;
    float volumeStart;
    float volumeEnd;
    float exponential;
};
struct SegmentInstance : Segment
{
    int sampleDuration;
    bool bVolumeChange;
    vector<float> curveExponential;
};
struct Envelope
{
    Segment segmentDelay;
    Segment segmentAttack;
    Segment segmentDecay;
    Segment segmentSustain;
    Segment segmentRelease;
};
struct EnvelopeInstance
{
    SegmentInstance segmentDelay;
    SegmentInstance segmentAttack;
    SegmentInstance segmentDecay;
    SegmentInstance segmentSustain;
    SegmentInstance segmentRelease;
};
enum EnvelopeStage
{
    EnvelopeStart,
    EnvelopeDelay,
    EnvelopeAttack,
    EnvelopeDecay,
    EnvelopeSustain,
    EnvelopeRelease,
    EnvelopeEnd,
    EnvelopeCutoff
};
struct SynthPreset
{
    WaveType waveType;
    Panning panning;
    
    OscillatorData oscillatorData;
    ModulationData modulationData;
    SquareWaveData squareWaveData;
    
    Envelope envelope;
};

struct SynthPresetInstance
{
    WaveType waveType;
    Panning panning;
    
    OscillatorData oscillatorData;
    ModulationData modulationData;
    SquareWaveData squareWaveData;
    
    EnvelopeInstance envelopeInstance;
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SynthUtil
{
public:
    static void init()
    {
        generateNoteFrequencies();
        generatePanning();
        generateWavetables();
    }
    static void generateNoteFrequencies()
    {
        int noteCount = 0;
        double a;
        double currentFrequency = 8.1757989156;
        for (int i = 0; i < 9; i++, noteCount++)
        {
            a = (i / 12.0);
            double frequency = (currentFrequency*pow(2.0, a));
            noteFrequencies[noteCount] = frequency;
        }
        
        currentFrequency = 13.75; //Work out from A-1, 9, because it's pitch with two or less floats
        noteFrequencies[noteCount] = currentFrequency;
        for (int o = 0; o < 10; o++)
        {
            for (int i = 1; i <= 12; i++, noteCount++)
            {
                if ((o == 9) & (i == 11))
                    break;
                a = (i / 12.0);
                double frequency = (currentFrequency*pow(2.0, a));
                noteFrequencies[noteCount] = frequency;
                if (i == 12)
                {
                    currentFrequency = frequency;
                }
            }
        }
    }
    static void generatePanning()
    {
        float mappedIndex;
        for (int i = 0; i < PANNING_TABLE_SIZE; i++)
        {
            mappedIndex = (i/(((PANNING_TABLE_SIZE-1.0)/2.0))-1.0);
            panningLinear.left[i] = (1 - mappedIndex) / 2;
            panningLinear.right[i] = (1 + mappedIndex) / 2;
            
            panningSquared.left[i] = sqrt((1 - mappedIndex) / 2);
            panningSquared.right[i] = sqrt((1 + mappedIndex) / 2);
            
            panningSine.left[i] = sin((1 - mappedIndex)/2 * (PI/2));
            panningSine.right[i] = sin((1 + mappedIndex)/2 * (PI/2));
        }
    }
    
    static Panning getPanning(float value, PanningType panningType)
    {
        //float redemicalise = float((int(value*1000))/1000.0);
        int index = ofMap(value, -1.0, 1.0, 0, 2000, true);
        Panning temp;
        switch(panningType)
        {
            case PanningLinear:
            {
                temp.left = panningLinear.left[index];
                temp.right = panningLinear.right[index];
                return temp;
            }
            case PanningSquared:
            {
                temp.left = panningSquared.left[index];
                temp.right = panningSquared.right[index];
                return temp;
            }
            case PanningSine:
            {
                temp.left = panningSine.left[index];
                temp.right = panningSine.right[index];
                return temp;
            }
        }
    }
    static const float getFrequencyRadian()
    {
        return TWO_PI / (float) AUDIO_SAMPLE_RATE;
    }
    //Typed copy from BasicSynth's WaveTable.h
    static void generateWavetables()
    {
#define NUMBER_PARTS 16
        double phaseIncrement[NUMBER_PARTS];
        double phaseValue[NUMBER_PARTS];
        
        int partNumber;
        int partMax = 1;
        
        phaseIncrement[0] = TWO_PI / (double) WAVETABLE_SIZE;
        phaseValue[0] = 0.0;
        for (partNumber = 1; partNumber < NUMBER_PARTS; partNumber++)
        {
            phaseIncrement[partNumber] = phaseIncrement[0] * (partNumber+1);
            phaseValue[partNumber] = 0.0;
            if (phaseIncrement[partNumber] < PI)
                partMax++;
        }
        
        double sawValue = 0.0;
        double sawPeak = 0.0;
        
        double squareValue = 0.0;
        double squarePeak = 0.0;
        
        double triangleValue = 0.0;
        double trianglePeak = 0.0;
        
        double pulseValue = 0.0;
        double pulsePeak = 0.0;
        
        double sigK = PI / partMax;
        double sigN = 0.0;
        double sigma = 0.0;
        double amplitude = 0.0;
        
        double value = 0.0;
        double partP1 = 0.0;
        
        int i;
        
        for (i = 0; i < WAVETABLE_SIZE; i++)
        {
            value = sin(phaseValue[0]);
            wavetableSine[i] = value;
            
            if (phaseValue[0] > PI)
            {
                wavetableSawDirectCalc[i] = -1.0;
            }
            else
            {
                wavetableSawDirectCalc[i] = 1.0;
            }
            
            wavetableSawDirectCalc[i] = (phaseValue[0] / PI) - 1;
            wavetableSawPositive[i] = (phaseValue[0] / TWO_PI);
            wavetableTriangleDirectCalc[i] = 1 - (2/PI * fabs(phaseValue[0] - PI));
            wavetableTrianglePositive[i] = 1 - (fabs(phaseValue[0] - PI) / PI);
            
            phaseValue[0] += phaseIncrement[0];
            
            sawValue = value;
            squareValue = value;
            triangleValue = value;
            pulseValue = value;
            
            sigN = sigK;
            partP1 = 2;
            for (partNumber = 1; partNumber < partMax; partNumber++)
            {
                sigma = sin(sigN) / sigN;
                pulseValue += sin(phaseValue[partNumber]) * sigma;
                amplitude = sigma / partP1;
                sigN += sigK;
                value = sin(phaseValue[partNumber]) * amplitude;
                if (!(partNumber & 1))
                {
                    sawValue += value;
                    squareValue += value;
                    triangleValue += ((cos(phaseValue[partNumber]) * sigma));
                } else
                {
                    sawValue -= value;
                }
                phaseValue[partNumber] += phaseIncrement[partNumber];
                partP1 += 1.0;
            }
            if (fabs(sawValue) > sawPeak)
                sawPeak = fabs(sawValue);
            if (fabs(squareValue) > squareValue)
                squarePeak = fabs(squareValue);
            if (fabs(triangleValue) > trianglePeak)
                trianglePeak = fabs(triangleValue);
            if (fabs(pulseValue) > pulsePeak)
                pulsePeak = pulseValue;
        }
        
        for (int i = 0; i < WAVETABLE_SIZE; i++)
        {
            wavetableSaw[i] = wavetableSaw[i] / (float) sawPeak;
            wavetableSquare[i] = wavetableSquare[i] / (float) squarePeak;
            wavetableTriangle[i] = wavetableTriangle[i] / (float) trianglePeak;
            wavetablePulse[i] = wavetablePulse[i] / (float) pulsePeak;
        }
        float phaseIncrementSin = TWO_PI / WAVETABLE_SIZE;
        float phaseSin = 0;
        for (int i = 0; i < WAVETABLE_SIZE; i++)
        {
            //wavetables.push_back(*new vector<float>);
            wavetableSine[i] = sin(phaseSin);
            phaseSin += phaseIncrementSin;
        }
    }
    
    static const float getFrequencyTableIndex()
    {
        return float(WAVETABLE_SIZE) / float(AUDIO_SAMPLE_RATE);
    }
    static const float getRadianTableIndex()
    {
        return (float) WAVETABLE_SIZE / TWO_PI;
    }
    static int getWavetableIndex(WaveType waveType)
    {
        switch(waveType)
        {
            case SineWave:
            {
                return WAVETABLE_SINE;
            }
            case SawWave:
            {
                return WAVETABLE_SAW;
            }
            case TriangleWave:
            {
                return WAVETABLE_TRIANGLE;
            }
            case SquareWave:
            {
                return WAVETABLE_SQUARE;
            }
            case Pulse:
            {
                return WAVETABLE_PULSE;
            }
            case Summed:
            {
                return WAVETABLE_SINE;
            }
            case Oscillator:
            {
                return WAVETABLE_SINE;
            }
            case FrequencyModulation:
            {
                return WAVETABLE_SINE;
            }
            case AmplitudeModulation:
            {
                return WAVETABLE_SINE;
            }
            case RingModulation:
            {
                return WAVETABLE_SINE;
            }
        }
    }
    static float getWavetableValue(int wavetableIndex, int index)
    {
        if (index > WAVETABLE_SIZE || index < 0)
            return 0;
        switch(wavetableIndex)
        {
            case WAVETABLE_SINE:
                return wavetableSine[index];
            case WAVETABLE_SQUARE:
                return wavetableSquare[index];
            case WAVETABLE_SAW:
                return wavetableSaw[index];
            case WAVETABLE_TRIANGLE:
                return wavetableTriangle[index];
            case WAVETABLE_PULSE:
                return wavetablePulse[index];
            case WAVETABLE_SAW_DIRECT_CALC:
                return wavetableSawDirectCalc[index];
            case WAVETABLE_TRIANGLE_DIRECT_CALC:
                return wavetableTriangleDirectCalc[index];
            case WAVETABLE_SQUARE_DIRECT_CALC:
                return wavetableSquareDirectCalc[index];
            case WAVETABLE_SAW_POSITIVE:
                return wavetableSawPositive[index];
            case WAVETABLE_TRIANGLE_POSITIVE:
                return wavetableTrianglePositive[index];
        }
    }
    
private:
    static float noteFrequencies[MIDI_NOTE_NUMBER];
    
    static PanningData panningLinear;
    static PanningData panningSquared;
    static PanningData panningSine;
    
    static float wavetableSine[WAVETABLE_SIZE];
    static float wavetableSquare[WAVETABLE_SIZE];
    static float wavetableSaw[WAVETABLE_SIZE];
    static float wavetableTriangle[WAVETABLE_SIZE];
    static float wavetablePulse[WAVETABLE_SIZE];
    static float wavetableSawDirectCalc[WAVETABLE_SIZE];
    static float wavetableTriangleDirectCalc[WAVETABLE_SIZE];
    static float wavetableSquareDirectCalc[WAVETABLE_SIZE];
    static float wavetableSawPositive[WAVETABLE_SIZE];
    static float wavetableTrianglePositive[WAVETABLE_SIZE];
};




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


