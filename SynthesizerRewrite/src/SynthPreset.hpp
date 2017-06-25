//
//  SynthPreset.hpp
//  SynthesizerRewrite
//
//  Created by Zachary Snow on 25/06/2017.
//
//

#pragma once

#include "SynthDefinition.hpp"
#include "SynthCurveGenerator.hpp"

struct SynthPresetStruct
{
    WaveType waveType;
    Panning panning;
    
    OscillatorData oscillatorData;
    ModulationData modulationData;
    SquareWaveData squareWaveData;
    
    Envelope envelope;
};

class SynthPresetBase
{
public:
    SynthPresetBase()
    {
        
    }
    inline void setWaveType(WaveType waveType)
    {
        this->waveType = waveType;
    }
    inline WaveType getWaveType()
    {
        return waveType;
    }
    inline void setPanning(Panning panning)
    {
        this->panning = panning;
    }
    inline void getPanning()
    {
        return this->panning;
    }
    inline void setSquareWaveData(SquareWaveData squareWaveData)
    {
        this->squareWaveData = squareWaveData;
    }
    inline void setSquareWaveData(float dutyCycle, float amplitudeMin, float amplitudeMax)
    {
        this->squareWaveData.dutyCycle = dutyCycle;
        this->squareWaveData.amplitudeMin = amplitudeMin;
        this->squareWaveData.amplitudeMax = amplitudeMax;
    }
    inline SquareWaveData getSquareWaveData()
    {
        return this->squareWaveData;
    }
    inline void setOscillatorData(OscillatorData oscillatorData)
    {
        this->oscillatorData = oscillatorData;
    }
    inline void setOscillatorData(int numberPartials, float *partials, float *amplitude, bool gibbs)
    {
        //oscillatorWavetable.setup(frequency, numberPartials, partials, amplitude, gibbs);
        this->oscillatorData.numberPartials = numberPartials;
        this->oscillatorData.partials.clear();
        this->oscillatorData.amplitude.clear();
        for (int i = 0; i < numberPartials; i++)
        {
            this->oscillatorData.partials.push_back(partials[i]);
            this->oscillatorData.amplitude.push_back(amplitude[i]);
        }
        this->oscillatorData.gibbs = gibbs;
    }
    inline OscillatorData getOscillatorData()
    {
        return this->oscillatorData;
    }
    inline void setFrequencyModulationData(ModulationData modulationData)
    {
        this->modulationData = modulationData;
    }
    inline void setFrequencyModulationData(float modulationMultiplier, float modulationIndexAmplitude)
    {
        this->modulationData.modulationMultiplier = modulationMultiplier;
        this->modulationData.modulationAmplitude = modulationIndexAmplitude;
    }
    inline ModulationData getFrequencyModulationData()
    {
        return this->modulationData;
    }
    inline void setAmplitudeModulationData(ModulationData modulationData)
    {
        this->modulationData = modulationData;
    }
    inline void setAmplitudeModulationData(float modulationMultiplier, float modulationAmplitude)
    {
        this->modulationData.modulationMultiplier = modulationMultiplier;
        this->modulationData.modulationAmplitude = modulationAmplitude;
    }
    inline ModulationData getAmplitudeModulationData()
    {
        return this->modulationData;
    }
    inline void setRingModulationData(ModulationData modulationData)
    {
        this->modulationData = modulationData;
    }
    inline void setRingModulationData(float modulationMultiplier, float modulationAmplitude)
    {
        this->modulationData.modulationMultiplier = modulationMultiplier;
        this->modulationData.modulationAmplitude = modulationAmplitude;
    }
    inline ModulationData getRingModulationData()
    {
        return this->modulationData;
    }
private:
    WaveType waveType;
    Panning panning;
    
    OscillatorData oscillatorData;
    ModulationData modulationData;
    SquareWaveData squareWaveData;
    
};
class SynthPresetClass : public SynthPresetBase
{
public:
    SynthPresetClass()
    {
        
    }
    
    void setEnvelopeSegmentData(EnvelopeStage stage, float duration, float volumeStart, float volumeEnd, float exponential)
    {
        Segment * segment;
        segment->duration = duration;
        segment->volumeStart = volumeStart;
        segment->volumeEnd = volumeEnd;
        segment->exponential = exponential;

        if (stage == EnvelopeDelay)
        {
            envelope.segmentDelay = *segment;
        } else if (stage == EnvelopeAttack)
        {
            envelope.segmentAttack = *segment;
        } else if (stage == EnvelopeDecay)
        {
            envelope.segmentDecay = *segment;
        } else if (stage == EnvelopeSustain)
        {
            envelope.segmentSustain = *segment;

        } else if (stage == EnvelopeRelease)
        {
            envelope.segmentRelease = *segment;
        } else if (stage == EnvelopeCutoff)
        {
            //TODO
            //envelope.segmentCutoff = *segment;
        }
    }
    Segment & getEnvelopeSegmentData(EnvelopeStage stage)
    {
        if (stage == EnvelopeDelay)
        {
            return envelope.segmentDelay;
        } else if (stage == EnvelopeAttack)
        {
            return envelope.segmentAttack;
        } else if (stage == EnvelopeDecay)
        {
            return envelope.segmentDecay;
        } else if (stage == EnvelopeSustain)
        {
            return envelope.segmentSustain;
            
        } else if (stage == EnvelopeRelease)
        {
            return envelope.segmentRelease;
        }
    }
    
private:
    
    Envelope envelope;
};

class SynthPresetInstanceClass : public SynthPresetBase
{
    void setEnvelopeSegmentData(EnvelopeStage stage, float duration, float volumeStart, float volumeEnd, float exponential)
    {
        SegmentInstance * segment;
        segment->duration = duration;
        segment->volumeStart = volumeStart;
        segment->volumeEnd = volumeEnd;
        segment->exponential = exponential;
        
        if (stage == EnvelopeDelay)
        {
            envelope.segmentDelay = *segment;
        } else if (stage == EnvelopeAttack)
        {
            envelope.segmentAttack = *segment;
        } else if (stage == EnvelopeDecay)
        {
            envelope.segmentDecay = *segment;
        } else if (stage == EnvelopeSustain)
        {
            envelope.segmentSustain = *segment;
            
        } else if (stage == EnvelopeRelease)
        {
            envelope.segmentRelease = *segment;
        } else if (stage == EnvelopeCutoff)
        {
            //TODO
            //envelope.segmentCutoff = *segment;
        }
    }
    SegmentInstance & getEnvelopeSegmentData(EnvelopeStage stage)
    {
        if (stage == EnvelopeDelay)
        {
            return envelope.segmentDelay;
        } else if (stage == EnvelopeAttack)
        {
            return envelope.segmentAttack;
        } else if (stage == EnvelopeDecay)
        {
            return envelope.segmentDecay;
        } else if (stage == EnvelopeSustain)
        {
            return envelope.segmentSustain;
            
        } else if (stage == EnvelopeRelease)
        {
            return envelope.segmentRelease;
        }
    }
private:
    EnvelopeInstance envelope;
};
