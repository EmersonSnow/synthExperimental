//
//  WavetableGenerator.h
//  toneGenerator3
//
//  Created by Zachary Snow on 23/06/2017.
//
//

#pragma once

#include "SynthDefinition.h"

class WavetableBase
{
public:
    void setupBase(float frequency, WaveType waveType, float index = 0)
    {
        bInUse = false;
        setFrequency(frequency);
        setWaveType(waveType);
        reset(index);
    }
    virtual void reset(float index = 0)
    {
        this->index = index;
        indexIncrement = frequency * SynthUtil::getFrequencyTableIndex();
    }
    inline bool setIsInUse(bool b)
    {
        bInUse = b;
    }
    inline bool getIsInUse()
    {
        return bInUse;
    }
    inline void setFrequency(float frequency)
    {
        this->frequency = frequency;
    }
    inline float getFrequency()
    {
        return frequency;
    }
    virtual void modulate(float frequencyValue)
    {
        indexIncrement = (frequency + frequencyValue) * SynthUtil::getFrequencyTableIndex();
    }
    virtual void phaseModulation(float phase)
    {
        phaseModulateWavetable(phase * SynthUtil::getRadianTableIndex());
    }
    virtual void phaseModulateWavetable(float index)
    {
        this->index += index;
    }
    virtual void setWaveType(WaveType waveType)
    {
        this->waveType = waveType;
        wavetableIndex = SynthUtil::getWavetableIndex(waveType);
    }
    virtual void getWaveType()
    {
        return waveType;
    }
    inline float IndexWrapWavetable(float index)
    {
        if (index >= WAVETABLE_SIZE)
        {
            while (index >= WAVETABLE_SIZE)
            {
                index -= WAVETABLE_SIZE;
            }
        } else if (index < 0)
        {
            while (index < 0)
            {
                index += WAVETABLE_SIZE;
            }
        }
        return index;
    }
    virtual float generateSample()
    {
        index += indexIncrement;
        index = IndexWrapWavetable(index);
        return SynthUtil::getWavetableValue(wavetableIndex, index);
    }
protected:
    bool bInUse;
    WaveType waveType;
    int wavetableIndex;
    float index;
    float indexIncrement;
    float frequency;
    
};
class OscillatorWavetable : public WavetableBase
{
public:
    OscillatorWavetable()
    {
        scale = 1;
        gibbs = false;
        maxMultiplier = 1;
    }
    void setup(float frequency, int numberPartials, float *partials, float *amplitude, bool gibbs = false, float index = 0.0)
    {
        this->maxMultiplier = 1;
        this->numberPartials = numberPartials;
        this->gibbs = gibbs;
        
        for (int i = 0; i < numberPartials; i++)
        {
            addPart(partials[i], amplitude[i]);
        }
        
        WavetableBase::setupBase(frequency, Oscillator, index);
        calcParts();
    }
    virtual void reset(float index)
    {
        WavetableBase::reset(index);
        calcParts();
        if (index >= 0)
        {
            for (int i = 0; i < parts.size(); i++)
            {
                parts[i].index = 0;
            }
        }
    }
    void addPart(float partial, float amplitude)
    {
        SumPart part;
        part.index = 0;
        part.increment = 0;
        part.multiplier = partial;
        part.amplitude = amplitude;
        part.sigma = amplitude;
        if (part.multiplier > maxMultiplier)
            maxMultiplier = part.multiplier;
        parts.push_back(part);
    }
    void setGibbs(bool gibbs)
    {
        this->gibbs = gibbs;
    }
    virtual void modulate(float frequencyValue)
    {
        WavetableBase::modulate(frequencyValue);
        calcParts();
    }
    virtual void phaseModulateWavetable(float index)
    {
        for (int i = 0; i < parts.size(); i++)
        {
            parts[i].index += index * parts[i].multiplier;
        }
    }
    void calcParts()
    {
        float tableDivideTwo = (float) WAVETABLE_SIZE / 2.0;
        scale = 0;
        countPartials = 0;
        float sigK = 0;
        float sigN = 0;
        float sigTL = 0;
        if (gibbs)
        {
            sigK = PI / maxMultiplier;
            sigTL = tableDivideTwo;
        }
        for (int i = 0; i < parts.size(); i++)
        {
            parts[i].increment = indexIncrement * parts[i].multiplier;
            if (parts[i].increment < tableDivideTwo)
            {
                if (gibbs && (parts[i].multiplier > 0))
                {
                    sigN = sigK * parts[i].multiplier;
                    parts[i].sigma = parts[i].amplitude * SynthUtil::getWavetableValue(WAVETABLE_SINE, sigN*sigTL) / sigN;
                } else
                {
                    parts[i].sigma = parts[i].amplitude;
                }
                scale += (float) fabs((double)parts[i].sigma);
            } else
            {
                parts[i].sigma = 0;
            }
        }
        
    }
    virtual float generateSample()
    {
        float value = 0;
        for (int p = 0; p < parts.size(); p++)
        {
            value += SynthUtil::getWavetableValue(WAVETABLE_SINE, parts[p].index);
            parts[p].index += parts[p].increment;
            parts[p].index = IndexWrapWavetable(parts[p].index);
        }
        value /= scale;
        value += SynthUtil::getWavetableValue(WAVETABLE_SINE, index);
        index += indexIncrement;
        index = IndexWrapWavetable(index);
        return value;
    }
private:
    int numberPartials;
    int countPartials;
    bool gibbs;
    float maxMultiplier;
    float scale;
    struct SumPart
    {
        float index;
        float increment;
        float multiplier;
        float amplitude;
        float sigma;
    };
    vector<SumPart> parts;
};
class FrequencyModulationWavetable : public WavetableBase
{
public:
    FrequencyModulationWavetable()
    {
        modulationMultiplier = 1.0;
        modulationAmplitude = 0.0;
        modulationIncrement = 0.0;
        modulationIndex = 0.0;
        indexOfModulation = 1;
    }
    void setup(float frequency, float modulationMultiplier, float modulationIndexAmplitude, float index = 0.0)
    {
        this->modulationMultiplier = modulationMultiplier;
        this->modulationFrequency = frequency*modulationMultiplier;
        this->indexOfModulation = modulationIndexAmplitude;
        
        modulationIncrement = SynthUtil::getFrequencyTableIndex() * modulationFrequency;
        modulationAmplitude = SynthUtil::getFrequencyTableIndex() * indexOfModulation * modulationFrequency;
        modulationIndex = 0.0;
        
        WavetableBase::setupBase(frequency, FrequencyModulation, index);
    }
    virtual void reset(float index = 0)
    {
        WavetableBase::reset(index);
        {
            if (index >= 0)
                modulationIndex = index * SynthUtil::getRadianTableIndex();
            
            calcModulationIncrement();
        }
    }
    inline void calcModulationAmplitude()
    {
        modulationAmplitude = indexOfModulation * modulationIncrement;
    }
    inline void calcModulationIncrement()
    {
        modulationIncrement = indexIncrement * modulationMultiplier;
        calcModulationAmplitude();
    }
    void setModulationIndex(float modulationIndex)
    {
        indexOfModulation = modulationIndex;
        calcModulationAmplitude();
    }
    void getModulationIndex()
    {
        return indexOfModulation;
    }
    void setModulationMultiplier(float frequencyValue)
    {
        modulationMultiplier = frequencyValue;
    }
    float getModulationMultiplier()
    {
        return modulationMultiplier;
    }
    virtual void modulation(float frequencyValue)
    {
        WavetableBase::modulate(frequencyValue);
        calcModulationIncrement();
    }
    virtual void phaseModulateWavetable(float index)
    {
        WavetableBase::phaseModulateWavetable(index);
        modulationIndex += index * modulationMultiplier;
    }
    virtual float generateSample()
    {
        float value = SynthUtil::getWavetableValue(WAVETABLE_SINE, index);
        modulationValue = modulationAmplitude * SynthUtil::getWavetableValue(WAVETABLE_SINE, modulationIndex);
        
        index += indexIncrement + modulationValue;
        modulationIndex += modulationIncrement;
        index = IndexWrapWavetable(index);
        modulationIndex = IndexWrapWavetable(modulationIndex);
        
        return value;
    }
private:
    float currentPhase;
    float modulationFrequency;
    float modulationMultiplier;
    float modulationValue;
    float modulationIncrement;
    float modulationIndex;
    float modulationAmplitude;
    float indexOfModulation;
};
class AmplitudeModulationWavetable : public WavetableBase
{
public:
    AmplitudeModulationWavetable()
    {
        modulationFrequency = 0.0;
        modulationAmplitude = 0.0;
        modulationIncrement = 0.0;
        modulationIndex = 0.0;
        modulationScale = 0.0;
    }
    virtual void setup(float frequency, float modulationMultiplier, float modulationAmplitude, float index = 0.0)
    {
        this->modulationFrequency = frequency * modulationMultiplier;
        this->modulationAmplitude = modulationAmplitude;
        
        modulationIndex = 0;
        modulationIncrement = SynthUtil::getRadianTableIndex() * modulationFrequency;
        
        modulationScale = 1.0 / (1.0 + modulationAmplitude);
        
        WavetableBase::setupBase(frequency, AmplitudeModulation, index);
    }
    virtual void reset(float index = 0)
    {
        WavetableBase::reset(index);
        modulationIncrement = SynthUtil::getFrequencyTableIndex() * modulationFrequency;
        if (index >= 0)
            modulationIndex = index * SynthUtil::getRadianTableIndex();
    }
    virtual void modulate(float frequencyValue)
    {
        WavetableBase::modulate(frequencyValue);
        modulationIncrement = SynthUtil::getFrequencyTableIndex() * (modulationFrequency+frequencyValue);
    }
    virtual void phaseModulateWavetable(float index)
    {
        this->index += index;
        modulationIndex += (index * modulationAmplitude / modulationFrequency);
    }
    virtual float generateSample()
    {
        float modulationValue = 1.0 + (modulationAmplitude * SynthUtil::getWavetableValue(WAVETABLE_SINE, modulationIndex));
        float value = SynthUtil::getWavetableValue(WAVETABLE_SINE, index) * modulationValue * modulationScale;
        
        index += indexIncrement;
        modulationIndex += modulationIncrement;
        index = IndexWrapWavetable(index);
        modulationIndex = IndexWrapWavetable(index);
        
        return value;
    }
protected:
    float modulationFrequency;
    float modulationIncrement;
    float modulationIndex;
    float modulationAmplitude;
    float modulationScale;
};
class RingModulationWavetable : public AmplitudeModulationWavetable
{
public:
    virtual float generateSample()
    {
        float value = SynthUtil::getWavetableValue(WAVETABLE_SINE, index) * modulationAmplitude * SynthUtil::getWavetableValue(WAVETABLE_SINE, modulationIndex);
        index += indexIncrement;
        modulationIndex += modulationIncrement;
        index = IndexWrapWavetable(index);
        modulationIndex = IndexWrapWavetable(modulationIndex);
        
        return value;
    }
};

//TODO: Set up a way so immediary after a wave switch, the sound carries on.
class GeneratorContainer
{
public:
    GeneratorContainer()
    {
        float partials[4] = {2.0, 3.0, 4.0, 5.0};
        float amplitudes[4] = {0.80, .60, 0.40, 0.20};
        setOscillatorData(4, partials, amplitudes, false);
        setFrequencyModulationData(2.0, 10);
        setAmplitudeModulationData(2.0, 0.75);
        setRingModulationData(2.0, 0.75);
    }
    void setWaveType(WaveType waveType)
    {
        if (this->waveType == waveType)
            return;
        
        this->waveType = waveType;
        
        /*wavetableInUse.clear();
        switch(waveType)
        {
            case Oscillator:
            {
                for (int i = 0; i < oscillatorWavetables.size(); i++)
                {
                    oscillatorWavetables.setIsInUse(false);
                }
                return;
            }
            case FrequencyModulation:
            {
                for (int i = 0; i < frequencyModulationWavetables.size(); i++)
                {
                    //wavetableInUse.push_back(false);
                }
                return;
            }
            case AmplitudeModulation:
            {
                for (int i = 0; i < amplitudeModulationWavetables.size(); i++)
                {
                    //wavetableInUse.push_back(false);
                }
                return;
            }
            case RingModulation:
            {
                for (int i = 0; i < ringModulationWavetables.size(); i++)
                {
                    //wavetableInUse.push_back(false);
                }
                return;
            }
        }*/
        //Maybe keep the already created classes, as they don't use much resources
        //oscillatorWavetables.clear();
        //frequencyModulationWavetables.clear();
        //amplitudeModulationWavetables.clear();
        //ringModulationWavetables.clear();
    }
    int getAvailableGenerator()
    {
        switch(waveType)
        {
            case Oscillator:
            {
                for (int i = 0; i < oscillatorWavetables.size(); i++)
                {
                    if (!oscillatorWavetables[i].getIsInUse())
                    {
                        return i;
                    }
                }
                oscillatorWavetables.push_back(*new OscillatorWavetable);
                int index = oscillatorWavetables.size()-1;
                oscillatorWavetables[index].setIsInUse(true);
                oscillatorWavetables[index].setup(440.0, oscillatorData.numberPartials, &oscillatorData.partials[0], &oscillatorData.amplitude[0], oscillatorData.gibbs);
                return index;
            }
            case FrequencyModulation:
            {
                for (int i = 0; i < frequencyModulationWavetables.size(); i++)
                {
                    if (!frequencyModulationWavetables[i].getIsInUse())
                    {
                        return i;
                    }
                }
                frequencyModulationWavetables.push_back(*new FrequencyModulationWavetable);
                int index = frequencyModulationWavetables.size()-1;
                frequencyModulationWavetables[index].setIsInUse(true);
                frequencyModulationWavetables[index].setup(440.0, frequencyModulationData.modulationMultiplier, frequencyModulationData.modulationAmplitude);
                return index;
            }
            case AmplitudeModulation:
            {
                for (int i = 0; i < amplitudeModulationWavetables.size(); i++)
                {
                    if (!amplitudeModulationWavetables[i].getIsInUse())
                    {
                        return i;
                    }
                }
                amplitudeModulationWavetables.push_back(*new AmplitudeModulationWavetable);
                int index = amplitudeModulationWavetables.size()-1;
                amplitudeModulationWavetables[index].setIsInUse(true);
                amplitudeModulationWavetables[index].setup(440.0, amplitudeModulationData.modulationMultiplier, amplitudeModulationData.modulationAmplitude);
                return index;
            }
            case RingModulation:
            {
                for (int i = 0; i < ringModulationWavetables.size(); i++)
                {
                    if (!ringModulationWavetables[i].getIsInUse())
                    {
                        return i;
                    }
                }
                ringModulationWavetables.push_back(*new RingModulationWavetable);
                int index = ringModulationWavetables.size()-1;
                ringModulationWavetables[index].setIsInUse(true);
                ringModulationWavetables[index].setup(440.0, ringModulationData.modulationMultiplier, amplitudeModulationData.modulationAmplitude);
                return index;

            }
        }
    }
    
    OscillatorWavetable * getOscillatorGenerator(int index)
    {
        return &oscillatorWavetables[index];
    }
    FrequencyModulationWavetable * getFrequencyModulationGenerator(int index)
    {
        return &frequencyModulationWavetables[index];
    }
    AmplitudeModulationWavetable * getAmplitudeModulationGenerator(int index)
    {
        return &amplitudeModulationWavetables[index];
    }
    RingModulationWavetable * getRingModulationGenerator(int index)
    {
        return &ringModulationWavetables[index];
    }
    void setFreeGenerator(int index)
    {
        switch(waveType)
        {
            case Oscillator:
            {
                oscillatorWavetables[index].setIsInUse(false);
                return;
            }
            case FrequencyModulation:
            {
                frequencyModulationWavetables[index].setIsInUse(false);
                return;
            }
            case AmplitudeModulation:
            {
                amplitudeModulationWavetables[index].setIsInUse(false);
                return;
            }
            case RingModulation:
            {
                ringModulationWavetables[index].setIsInUse(false);
                return;
            }
        }
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
        
        for (int i = 0; oscillatorWavetables.size(); i++)
        {
            oscillatorWavetables[i].setup(440.0, numberPartials, &oscillatorData.partials[0], &oscillatorData.amplitude[0], gibbs);
        }
    }
    OscillatorData getOscillatorData()
    {
        return oscillatorData;
    }
    
    void setFrequencyModulationData(float modulationMultiplier, float modulationIndexAmplitude)
    {
        frequencyModulationData.modulationMultiplier = modulationMultiplier;
        frequencyModulationData.modulationAmplitude = modulationIndexAmplitude;
        
        for (int i = 0; i < frequencyModulationWavetables.size(); i++)
        {
            frequencyModulationWavetables[i].setup(440.0, modulationMultiplier, modulationIndexAmplitude);
        }
    }
    ModulationData getFrequencyModulationData()
    {
        return frequencyModulationData;
    }
    
    void setAmplitudeModulationData(float modulationMultiplier, float modulationAmplitude)
    {
        amplitudeModulationData.modulationMultiplier = modulationMultiplier;
        amplitudeModulationData.modulationAmplitude;
        for (int i = 0 ; amplitudeModulationWavetables.size(); i++)
        {
            amplitudeModulationWavetables[i].setup(440.0, modulationMultiplier, modulationAmplitude);
        }
    }
    ModulationData getAmplitudeModulationData()
    {
        return amplitudeModulationData;
    }
    
    void setRingModulationData(float modulationMultiplier, float modulationAmplitude)
    {
        ringModulationData.modulationMultiplier = modulationMultiplier;
        ringModulationData.modulationAmplitude;
        
        for (int i = 0 ; ringModulationWavetables.size(); i++)
        {
            ringModulationWavetables[i].setup(440.0, modulationMultiplier, modulationAmplitude);
        }
    }
    ModulationData getRIngModulationData()
    {
        return ringModulationData;
    }
    
private:
    WaveType waveType;
    
    OscillatorData oscillatorData;
    ModulationData frequencyModulationData;
    ModulationData amplitudeModulationData;
    ModulationData ringModulationData;
    
    vector<OscillatorWavetable> oscillatorWavetables;
    vector<FrequencyModulationWavetable> frequencyModulationWavetables;
    vector<AmplitudeModulationWavetable> amplitudeModulationWavetables;
    vector<RingModulationWavetable> ringModulationWavetables;
};
extern GeneratorContainer generatorContainer;
