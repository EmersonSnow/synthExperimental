//
//  Synthh
//  SynthesizerRewrite
//
//  Created by Zachary Snow on 25/06/2017.
//
//

#pragma once

#include "SynthPresetManager.hpp"
#include "SynthGenerators.hpp"

class WaveInstance
{
public:
    WaveInstance(float frequency = 440.0)
    {
        bInUse = false;
        bGenerator = false;
        generatorIndex = 0;
        this->frequency = frequency;
        
    }
    
    void reset(bool bInUse = false, float frequency = 440.0)
    {
        if (!this->bInUse && bInUse)
            activeWaveInstance--;
        
        setInUse(bInUse);
        //bStart = false;
        bGenerator = false;
        //stage = EnvelopeStart;
        //stageSampleCount = 0;
        generatorIndex = 0;
        frequency = frequency;
    }
    void setInUse(bool b)
    {
        bInUse = b;
    }
    bool getInUse()
    {
        return bInUse;
    }
    void setFrequency(float f)
    {
        frequency = f;
    }
    void setFree()
    {
        if (!bGenerator)
            return;
        switch(synthPresetManager.getWaveType())
        {
            case SineWave:
            {
                break;
            }
            case SawWave:
            {
                generatorContainer.sawWaveGenerators[generatorIndex].setInUse(false);
                break;
            }
            case TriangleWave:
            {
                generatorContainer.triangleWaveGenerators[generatorIndex].setInUse(false);
                break;
            }
            case SquareWave:
            {
                generatorContainer.squareWaveGenerators[generatorIndex].setInUse(false);
                break;
            }
            case Pulse:
            {
                break;
            }
            case Summed:
            {
                break;
            }
            case Oscillator:
            {
                generatorContainer.oscillatorGenerators[generatorIndex].setInUse(false);
                break;
            }
            case FrequencyModulation:
            {
                generatorContainer.frequencyModulationGenerators[generatorIndex].setInUse(false);
                break;
            }
            case AmplitudeModulation:
            {
                generatorContainer.amplitudeModulationGenerators[generatorIndex].setInUse(false);
                break;
            }
            case RingModulation:
            {
                generatorContainer.ringModulationGenerators[generatorIndex].setInUse(false);
                break;
            }
        }
        reset();
    }
    void getGenerator()
    {
        switch(synthPresetManager.getWaveType())
        {
            case SineWave:
            {
                break;
            }
            case SawWave:
            {
                for (int i = 0; i < generatorContainer.sawWaveGenerators.size(); i++)
                {
                    if (!generatorContainer.sawWaveGenerators[i].getInUse())
                    {
                        generatorContainer.sawWaveGenerators[i].setup(frequency);
                        bGenerator = true;
                        generatorIndex = i;
                        return;
                    }
                }
                generatorContainer.sawWaveGenerators.push_back(*new SawWaveGenerator);
                int index = generatorContainer.sawWaveGenerators.size()-1;
                generatorContainer.sawWaveGenerators[index].setInUse(true);
                generatorContainer.sawWaveGenerators[index].setup(frequency);
                bGenerator = true;
                generatorIndex = index;
                return;
            }
            case TriangleWave:
            {
                for (int i = 0; i < generatorContainer.triangleWaveGenerators.size(); i++)
                {
                    if (!generatorContainer.triangleWaveGenerators[i].getInUse())
                    {
                        bGenerator = true;
                        generatorIndex = i;
                        return;
                    }
                }
                generatorContainer.triangleWaveGenerators.push_back(*new TriangleWaveGenerator);
                int index = generatorContainer.triangleWaveGenerators.size()-1;
                generatorContainer.triangleWaveGenerators[index].setInUse(true);
                generatorContainer.triangleWaveGenerators[index].setup(frequency);
                bGenerator = true;
                generatorIndex = index;
                return;
            }
            case SquareWave:
            {
                for (int i = 0; i < generatorContainer.squareWaveGenerators.size(); i++)
                {
                    if (!generatorContainer.squareWaveGenerators[i].getInUse())
                    {
                        bGenerator = true;
                        generatorIndex = i;
                        return;
                    }
                }
                generatorContainer.squareWaveGenerators.push_back(*new SquareWaveGenerator);
                int index = generatorContainer.squareWaveGenerators.size()-1;
                generatorContainer.squareWaveGenerators[index].setInUse(true);
                SquareWaveData squareWaveData = synthPresetManager.getSquareWaveData();
                generatorContainer.squareWaveGenerators[index].setup(frequency, squareWaveData.dutyCycle, squareWaveData.amplitudeMin, squareWaveData.amplitudeMax);
                bGenerator = true;
                generatorIndex = index;
                return;
            }
            case Pulse:
            {
                break;
            }
            case Summed:
            {
                break;
            }
            case Oscillator:
            {
                for (int i = 0; i < generatorContainer.oscillatorGenerators.size(); i++)
                {
                    if (!generatorContainer.oscillatorGenerators[i].getInUse())
                    {
                        generatorContainer.oscillatorGenerators[i].setFrequency(frequency);
                        bGenerator = true;
                        generatorIndex = i;
                        return;
                    }
                }
                generatorContainer.oscillatorGenerators.push_back(*new OscillatorWavetable);
                int index = generatorContainer.oscillatorGenerators.size()-1;
                cout << "Index " << index << "\n";
                generatorContainer.oscillatorGenerators[index].setInUse(true);
                OscillatorData oscillatorData = synthPresetManager.getOscillatorData();
                generatorContainer.oscillatorGenerators[index].setup(frequency, oscillatorData.numberPartials, &oscillatorData.partials[0], &oscillatorData.amplitude[0], oscillatorData.gibbs);
                bGenerator = true;
                generatorIndex = index;
                return;
            }
            case FrequencyModulation:
            {
                for (int i = 0; i < generatorContainer.frequencyModulationGenerators.size(); i++)
                {
                    if (!generatorContainer.frequencyModulationGenerators[i].getInUse())
                    {
                        bGenerator = true;
                        generatorIndex = i;
                        return ;
                    }
                }
                generatorContainer.frequencyModulationGenerators.push_back(*new FrequencyModulationWavetable);
                int index = generatorContainer.frequencyModulationGenerators.size()-1;
                generatorContainer.frequencyModulationGenerators[index].setInUse(true);
                ModulationData frequencyModulationData = synthPresetManager.getFrequencyModulationData();
                generatorContainer.frequencyModulationGenerators[index].setup(frequency, frequencyModulationData.modulationMultiplier, frequencyModulationData.modulationAmplitude);
                bGenerator = true;
                generatorIndex = index;
                return;
            }
            case AmplitudeModulation:
            {
                for (int i = 0; i < generatorContainer.amplitudeModulationGenerators.size(); i++)
                {
                    if (!generatorContainer.amplitudeModulationGenerators[i].getInUse())
                    {
                        bGenerator = true;
                        generatorIndex = i;
                        return;
                    }
                }
                generatorContainer.amplitudeModulationGenerators.push_back(*new AmplitudeModulationWavetable);
                int index = generatorContainer.amplitudeModulationGenerators.size()-1;
                generatorContainer.amplitudeModulationGenerators[index].setInUse(true);
                ModulationData amplitudeModulationData = synthPresetManager.getAmplitudeModulationData();
                generatorContainer.amplitudeModulationGenerators[index].setup(frequency, amplitudeModulationData.modulationMultiplier, amplitudeModulationData.modulationAmplitude);
                bGenerator = true;
                generatorIndex = index;
                return;
            }
            case RingModulation:
            {
                for (int i = 0; i < generatorContainer.ringModulationGenerators.size(); i++)
                {
                    if (!generatorContainer.ringModulationGenerators[i].getInUse())
                    {
                        bGenerator = true;
                        generatorIndex = i;
                        return;
                    }
                }
                generatorContainer.ringModulationGenerators.push_back(*new RingModulationWavetable);
                int index = generatorContainer.ringModulationGenerators.size()-1;
                generatorContainer.ringModulationGenerators[index].setInUse(true);
                ModulationData ringModulationData = synthPresetManager.getRingModulationData();
                generatorContainer.ringModulationGenerators[index].setup(frequency, ringModulationData.modulationMultiplier, ringModulationData.modulationAmplitude);
                bGenerator = true;
                generatorIndex = index;
                return;
            }
        }
    }

    void start()
    {
        if (!bGenerator)
            getGenerator();
        
        bInUse = true;
        activeWaveInstance++;
    }
    
    static int getActivateWaveInstances()
    {
        return activeWaveInstance;
    }
    
    /*int getReadBufferIndex()
    {
        return bufferReadIndex;
    }
    int getWriteBufferIndex();
    {
        return 0;
    }
    void readBuffer()
    {
        if (++bufferReadIndex == WAVE_INSTANCE_BUFFER)
        {
            bufferReadIndex = 0;
        }
        bufferReadCount++;
    }
    void writeBuffer()
    {
        if (!(bufferReadCount > (bufferWriteCount-(WAVE_INSTANCE_BUFFER-1))))
            return;
        
        if (++0 == WAVE_INSTANCE_BUFFER)
        {
            0 = 0;
        }
        generateBuffer();
        bufferWriteCount++;
    }
    void update()
    {
        writeBuffer();
    }*/
    /*void generate()
    {
        float sample;
        for (int i = 0; i < AUDIO_BUFFER_SIZE; i++)
        {
            switch (synthPresetManager.getWaveType())
            {
                case SineWave:
                {
                    
                }
                case SawWave:
                {
                    sample = generatorContainer.sawWaveGenerators[generatorIndex].generateSample();
                    samples[i*AUDIO_CHANNEL_NUMBER] = sample;
                    samples[i*AUDIO_CHANNEL_NUMBER+1] = sample;
                    break;
                }
                case TriangleWave:
                {
                    sample = generatorContainer.triangleWaveGenerators[generatorIndex].generateSample();
                    samples[i*AUDIO_CHANNEL_NUMBER] = sample;
                    samples[i*AUDIO_CHANNEL_NUMBER+1] = sample;
                    break;
                }
                case SquareWave:
                {
                    sample = generatorContainer.squareWaveGenerators[generatorIndex].generateSample();
                    samples[i*AUDIO_CHANNEL_NUMBER] = sample;
                    samples[i*AUDIO_CHANNEL_NUMBER+1] = sample;
                }
                case Pulse:
                {
                    break;
                }
                case Summed:
                {
                    break;
                }
                case Oscillator:
                {
                    sample = generatorContainer.oscillatorGenerators[generatorIndex].generateSample();
                    samples[i*AUDIO_CHANNEL_NUMBER] = sample;
                    samples[i*AUDIO_CHANNEL_NUMBER+1] = sample;
                    break;
                }
                case FrequencyModulation:
                {
                    sample = generatorContainer.frequencyModulationGenerators[generatorIndex].generateSample();
                    samples[i*AUDIO_CHANNEL_NUMBER] = sample;
                    samples[i*AUDIO_CHANNEL_NUMBER+1] = sample;
                    break;
                }
                case AmplitudeModulation:
                {
                    sample = generatorContainer.amplitudeModulationGenerators[generatorIndex].generateSample();
                    samples[i*AUDIO_CHANNEL_NUMBER] = sample;
                    samples[i*AUDIO_CHANNEL_NUMBER+1] = sample;
                    break;
                }
                case RingModulation:
                {
                    sample = generatorContainer.ringModulationGenerators[generatorIndex].generateSample();
                    samples[i*AUDIO_CHANNEL_NUMBER] = sample;
                    samples[i*AUDIO_CHANNEL_NUMBER+1] = sample;
                    break;
                }
            }
        }
    }*/
    float generate()
    {
        switch (synthPresetManager.getWaveType())
        {
            case SineWave:
            {
                
            }
            case SawWave:
            {
                return generatorContainer.sawWaveGenerators[generatorIndex].generateSample();
                break;
            }
            case TriangleWave:
            {
                return generatorContainer.triangleWaveGenerators[generatorIndex].generateSample();
                break;
            }
            case SquareWave:
            {
                return generatorContainer.squareWaveGenerators[generatorIndex].generateSample();
            }
            case Pulse:
            {
                break;
            }
            case Summed:
            {
                break;
            }
            case Oscillator:
            {
                return generatorContainer.oscillatorGenerators[0].generateSample();
                break;
            }
            case FrequencyModulation:
            {
                return generatorContainer.frequencyModulationGenerators[generatorIndex].generateSample();
                break;
            }
            case AmplitudeModulation:
            {
                return  generatorContainer.amplitudeModulationGenerators[generatorIndex].generateSample();
                break;
            }
            case RingModulation:
            {
                return generatorContainer.ringModulationGenerators[generatorIndex].generateSample();
                break;
            }
        }

    }
    float samples[AUDIO_BUFFER_SIZE*2];
    
private:
    bool bInUse;
    bool bGenerator;
    int generatorIndex;
    float frequency;
    
    //int bufferWriteIndex;
    //int bufferReadIndex;
    //int bufferWriteCount;
    //int bufferReadCount;
    //float volume;
    
    struct GeneratorContainer
    {
        vector<SawWaveGenerator> sawWaveGenerators;
        vector<TriangleWaveGenerator> triangleWaveGenerators;
        vector<SquareWaveGenerator> squareWaveGenerators;
        
        vector<OscillatorWavetable> oscillatorGenerators;
        vector<FrequencyModulationWavetable> frequencyModulationGenerators;
        vector<AmplitudeModulationWavetable> amplitudeModulationGenerators;
        vector<RingModulationWavetable> ringModulationGenerators;
        
    };
    
    static GeneratorContainer generatorContainer;
    static int activeWaveInstance;
};
