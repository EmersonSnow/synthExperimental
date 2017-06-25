//
//  SynthPreset.h
//  SynthesiserRewrite
//
//  Created by Zachary Snow on 24/06/2017.
//
//

#pragma once

#include "SynthDefinition.hpp"
#include "SynthPreset.hpp"


class SynthPresetManager
{
public:
    SynthPresetManager()
    {
        bPresetInstanced = false;
        
        currentPreset = buildPreset();
        inialiseSynthPresetInstance(currentPreset);
    }
    void loadPreset(){}
    void savePreset(){}
    SynthPresetInstance & getCurrentPreset()
    {
        return currentPresetInstance;
    }
    SynthPresetInstance inialiseSynthPresetInstance(SynthPreset & synthPreset)
    {
        SynthPresetInstance synthPresetInstance;
        synthPresetInstance.waveType = synthPreset.waveType;
        synthPresetInstance.panning = synthPreset.panning;
        
        if (synthPresetInstance.waveType == SquareWave)
        {
            synthPresetInstance.squareWaveData = synthPreset.squareWaveData;
        } else if (synthPresetInstance.waveType == Oscillator)
        {
            synthPresetInstance.oscillatorData = synthPreset.oscillatorData;
        } else if((synthPresetInstance.waveType == FrequencyModulation) || (synthPresetInstance.waveType == AmplitudeModulation) || (synthPresetInstance.waveType == RingModulation))
        {
            synthPresetInstance.modulationData = synthPreset.modulationData;
        }
        
        inialiseSegmentInstance(synthPreset.envelope.segmentDelay,
                                synthPresetInstance.envelopeInstance.segmentDelay);
        
        inialiseSegmentInstance(synthPreset.envelope.segmentAttack,
                                synthPresetInstance.envelopeInstance.segmentAttack);
        
        inialiseSegmentInstance(synthPreset.envelope.segmentDecay,
                                synthPresetInstance.envelopeInstance.segmentDecay);
        inialiseSegmentInstance(synthPreset.envelope.segmentSustain,
                                    synthPresetInstance.envelopeInstance.segmentSustain);
        inialiseSegmentInstance(synthPreset.envelope.segmentDecay,
                                synthPresetInstance.envelopeInstance.segmentDecay);
        
        inialiseSegmentInstance(synthPreset.envelope.segmentRelease,
                                synthPresetInstance.envelopeInstance.segmentRelease);
        
        bPresetInstanced = true;
        return synthPresetInstance;
    }
    void inialiseSegmentInstance(Segment &segment, SegmentInstance &segmentInstance)
    {
        segmentInstance.duration = segment.duration;
        segmentInstance.sampleDuration = segment.duration * (float) AUDIO_SAMPLE_RATE;
        segmentInstance.volumeStart = segment.volumeStart;
        segmentInstance.volumeEnd = segment.volumeEnd;
        if (segmentInstance.volumeEnd == segment.volumeStart)
        {
            segmentInstance.bVolumeChange = false;
        } else
        {
            segmentInstance.bVolumeChange = true;
        }
        segmentInstance.exponential = segment.exponential;
        bool bAscending = (segmentInstance.volumeStart < segmentInstance.volumeEnd) ? true : false;
        
        curveExponentGenerator.generate(segmentInstance.sampleDuration, segmentInstance.exponential, bAscending);
        curveExponentGenerator.generateMappedCurve(segment.volumeStart, segment.volumeEnd);
        if (segmentInstance.bVolumeChange)
            segmentInstance.curveExponential = curveExponentGenerator.curveMapped;
        curveExponentGenerator.clear();
    }
    SynthPreset buildPreset()
    {
        SynthPreset defaultPreset;
        defaultPreset.waveType = Oscillator;
        defaultPreset.panning.left = 1.0;
        defaultPreset.panning.right = 1.0;
        
        float partials[10] = {2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0};
        float amplitudes[10] = {1.0, .9, 0.8, 0.7, .6, .5, .4, .3, .2, .1};
        //setFrequencyModulationData(2.0, 10);
        //setAmplitudeModulationData(2.0, 0.75);
        //setRingModulationData(2.0, 0.75);
        
        defaultPreset.oscillatorData.numberPartials = 10;
        for (int i = 0; i < defaultPreset.oscillatorData.numberPartials; i++)
        {
            defaultPreset.oscillatorData.partials.push_back(partials[i]);
            defaultPreset.oscillatorData.amplitude.push_back(amplitudes[i]);
        };
        defaultPreset.oscillatorData.gibbs = false;
        
        defaultPreset.envelope.segmentDelay.duration = 0.25;
        defaultPreset.envelope.segmentDelay.volumeStart = 0;
        defaultPreset.envelope.segmentDelay.volumeEnd = 0;
        defaultPreset.envelope.segmentDelay.exponential = 0;
        
        defaultPreset.envelope.segmentAttack.duration = 0.5;
        defaultPreset.envelope.segmentAttack.volumeStart = 0;
        defaultPreset.envelope.segmentAttack.volumeEnd = 1.0;
        defaultPreset.envelope.segmentAttack.exponential = 0.2;
        
        defaultPreset.envelope.segmentDecay.duration = 0.25;
        defaultPreset.envelope.segmentDecay.volumeStart = 1.0;
        defaultPreset.envelope.segmentDecay.volumeEnd = 0.75;
        defaultPreset.envelope.segmentDecay.exponential = 0.2;
        
        defaultPreset.envelope.segmentSustain.duration = 2.0;
        defaultPreset.envelope.segmentSustain.volumeStart = 0.75;
        defaultPreset.envelope.segmentSustain.volumeEnd = 0.5;
        defaultPreset.envelope.segmentSustain.exponential = 0.4;
        
        defaultPreset.envelope.segmentRelease.duration = 10;
        defaultPreset.envelope.segmentRelease.volumeStart = 0.5;
        defaultPreset.envelope.segmentRelease.volumeEnd = 0;
        defaultPreset.envelope.segmentRelease.exponential = 0.2;
        
        return defaultPreset;
    }
    void setWaveType(WaveType waveType)
    {
        currentPresetInstance.waveType = waveType;
    }
    WaveType getWaveType()
    {
        return currentPresetInstance.waveType;
    }
    void alterPreset(Segment & segment)
    {
        
    }
    void setSquareWaveData(float dutyCycle, float amplitudeMin, float amplitudeMax)
    {
        currentPreset.squareWaveData.dutyCycle = dutyCycle;
        currentPreset.squareWaveData.amplitudeMin = amplitudeMin;
        currentPreset.squareWaveData.amplitudeMax = amplitudeMax;
        
        if (bPresetInstanced)
        {
            currentPresetInstance.squareWaveData.dutyCycle = dutyCycle;
            currentPresetInstance.squareWaveData.amplitudeMin = amplitudeMin;
            currentPresetInstance.squareWaveData.amplitudeMax = amplitudeMax;
        }
    }
    SquareWaveData getSquareWaveData()
    {
        return currentPreset.squareWaveData;
    }
    void setOscillatorData(int numberPartials, float *partials, float *amplitude, bool gibbs)
    {
        //oscillatorWavetable.setup(frequency, numberPartials, partials, amplitude, gibbs);
        currentPreset.oscillatorData.numberPartials = numberPartials;
        currentPreset.oscillatorData.partials.clear();
        currentPreset.oscillatorData.amplitude.clear();
        for (int i = 0; i < numberPartials; i++)
        {
            currentPreset.oscillatorData.partials.push_back(partials[i]);
            currentPreset.oscillatorData.amplitude.push_back(amplitude[i]);
        }
        currentPreset.oscillatorData.gibbs = gibbs;
        if (bPresetInstanced)
        {
            currentPresetInstance.oscillatorData.numberPartials = numberPartials;
            currentPresetInstance.oscillatorData.partials.clear();
            currentPresetInstance.oscillatorData.amplitude.clear();
            for (int i = 0; i < numberPartials; i++)
            {
                currentPresetInstance.oscillatorData.partials.push_back(partials[i]);
                currentPresetInstance.oscillatorData.amplitude.push_back(amplitude[i]);
            }
            currentPresetInstance.oscillatorData.gibbs = gibbs;
        }
    }
    OscillatorData getOscillatorData()
    {
        return currentPreset.oscillatorData;
    }
    
    void setFrequencyModulationData(float modulationMultiplier, float modulationIndexAmplitude)
    {
        currentPreset.modulationData.modulationMultiplier = modulationMultiplier;
        currentPreset.modulationData.modulationAmplitude = modulationIndexAmplitude;
        if (bPresetInstanced)
        {
            currentPresetInstance.modulationData.modulationMultiplier = modulationMultiplier;
            currentPresetInstance.modulationData.modulationAmplitude = modulationIndexAmplitude;
        }
    }
    ModulationData getFrequencyModulationData()
    {
        return currentPreset.modulationData;
    }
    
    void setAmplitudeModulationData(float modulationMultiplier, float modulationAmplitude)
    {
        currentPreset.modulationData.modulationMultiplier = modulationMultiplier;
        currentPreset.modulationData.modulationAmplitude = modulationAmplitude;
        if (bPresetInstanced)
        {
            currentPresetInstance.modulationData.modulationMultiplier = modulationMultiplier;
            currentPresetInstance.modulationData.modulationAmplitude = modulationAmplitude;
        }

    }
    ModulationData getAmplitudeModulationData()
    {
        return currentPreset.modulationData;
    }
    
    void setRingModulationData(float modulationMultiplier, float modulationAmplitude)
    {
        currentPreset.modulationData.modulationMultiplier = modulationMultiplier;
        currentPreset.modulationData.modulationAmplitude = modulationAmplitude;
        if (bPresetInstanced)
        {
            currentPresetInstance.modulationData.modulationMultiplier = modulationMultiplier;
            currentPresetInstance.modulationData.modulationAmplitude = modulationAmplitude;
        }
    }
    ModulationData getRingModulationData()
    {
        return currentPreset.modulationData;
    }
    
    SynthPresetInstance currentPresetInstance;
private:
    bool bPresetInstanced;
    SynthPreset currentPreset;
    
    CurveExponentGenerator curveExponentGenerator;
};
extern SynthPresetManager synthPresetManager;
