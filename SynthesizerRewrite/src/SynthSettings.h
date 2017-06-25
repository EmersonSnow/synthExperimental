//
//  SynthSettings.h
//  SynthesizerRewrite
//
//  Created by Zachary Snow on 25/06/2017.
//
//

#pragma once
#include "SynthDefinition.h"
#include "SynthPreset.h"

class SynthSettings
{
public:
    SynthSettings()
    {
        setWaveType(SineWave);
        float partials[4] = {2.0, 3.0, 4.0, 5.0};
        float amplitudes[4] = {0.80, .60, 0.40, 0.20};
        setOscillatorData(4, partials, amplitudes, false);
        setFrequencyModulationData(2.0, 10);
        setAmplitudeModulationData(2.0, 0.75);
        setRingModulationData(2.0, 0.75);
        
        buildPreset();
    }
    void setWaveType(WaveType waveType)
    {
        this->waveType = waveType;
    }
    WaveType getWaveType()
    {
        return waveType;
    }
    void setSquareWaveData(float dutyCycle, float amplitudeMin, float amplitudeMax)
    {
        squareWaveData.dutyCycle = dutyCycle;
        squareWaveData.amplitudeMin = amplitudeMin;
        squareWaveData.amplitudeMax = amplitudeMax;
    }
    SquareWaveData getSquareWaveData()
    {
        return squareWaveData;
    }
    void setOscillatorData(int numberPartials, float *partials, float *amplitude, bool gibbs)
    {
        //oscillatorWavetable.setup(frequency, numberPartials, partials, amplitude, gibbs);
        oscillatorData.numberPartials = numberPartials;
        oscillatorData.partials.clear();
        oscillatorData.amplitude.clear();
        for (int i = 0; i < numberPartials; i++)
        {
            oscillatorData.partials.push_back(partials[i]);
            oscillatorData.amplitude.push_back(amplitude[i]);
        }
        oscillatorData.gibbs = gibbs;
    }
    OscillatorData getOscillatorData()
    {
        return oscillatorData;
    }
    
    void setFrequencyModulationData(float modulationMultiplier, float modulationIndexAmplitude)
    {
        frequencyModulationData.modulationMultiplier = modulationMultiplier;
        frequencyModulationData.modulationAmplitude = modulationIndexAmplitude;
    }
    ModulationData getFrequencyModulationData()
    {
        return frequencyModulationData;
    }
    
    void setAmplitudeModulationData(float modulationMultiplier, float modulationAmplitude)
    {
        amplitudeModulationData.modulationMultiplier = modulationMultiplier;
        amplitudeModulationData.modulationAmplitude;
    }
    ModulationData getAmplitudeModulationData()
    {
        return amplitudeModulationData;
    }
    
    void setRingModulationData(float modulationMultiplier, float modulationAmplitude)
    {
        ringModulationData.modulationMultiplier = modulationMultiplier;
        ringModulationData.modulationAmplitude;
    }
    ModulationData getRingModulationData()
    {
        return ringModulationData;
    }
    
    void buildPreset()
    {
        currentPreset.waveType = SquareWave;
        currentPreset.panning = SynthUtil::getPanning(0.5, PanningLinear);
        currentPreset.envelope.segmentDelay.duration = 0.25;
        currentPreset.envelope.segmentDelay.volumeStart = 0;
        currentPreset.envelope.segmentDelay.volumeEnd = 0;
        currentPreset.envelope.segmentDelay.exponential = 0;
        
        currentPreset.envelope.segmentAttack.duration = 0.5;
        currentPreset.envelope.segmentAttack.volumeStart = 0;
        currentPreset.envelope.segmentAttack.volumeEnd = 1.0;
        currentPreset.envelope.segmentAttack.exponential = 0.2;
        
        currentPreset.envelope.segmentDecay.duration = 0.25;
        currentPreset.envelope.segmentDecay.volumeStart = 1.0;
        currentPreset.envelope.segmentDecay.volumeEnd = 0.75;
        currentPreset.envelope.segmentDecay.exponential = 0.2;
        
        currentPreset.envelope.segmentsSustain.push_back(currentPreset.envelope.segmentDelay);
        currentPreset.envelope.segmentsSustain[0].duration = 2.0;
        currentPreset.envelope.segmentsSustain[0].volumeStart = 0.75;
        currentPreset.envelope.segmentsSustain[0].volumeEnd = 0.5;
        currentPreset.envelope.segmentsSustain[0].exponential = 0.4;
        
        currentPreset.envelope.segmentRelease.duration = 10;
        currentPreset.envelope.segmentRelease.volumeStart = 0.5;
        currentPreset.envelope.segmentRelease.volumeEnd = 0;
        currentPreset.envelope.segmentRelease.exponential = 0.2;
    }
    SynthPreset currentPreset;
    SynthPresetInstance currentPresetInstance;
private:
    WaveType waveType;
    
    SquareWaveData squareWaveData;
    OscillatorData oscillatorData;
    ModulationData frequencyModulationData;
    ModulationData amplitudeModulationData;
    ModulationData ringModulationData;
};
extern SynthSettings synthSettings;
