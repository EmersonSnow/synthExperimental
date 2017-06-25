//
//  SynthWaveManager.h
//  SynthesiserRewrite
//
//  Created by Zachary Snow on 24/06/2017.
//
//

#pragma once

#include "SynthDefinition.h"
#include "SynthGenerators.h"
#include "SynthPreset.h"
class CurveExponentGenerator
{
public:
    void generate(float durationSample, float curvature, bool bAscending)
    {
        if (bAscending)
        {
            exponentMul = pow((curvature+1.0)/curvature, 1.0/float(durationSample));
            exponentNow = curvature;
            
        } else
        {
            exponentMul = pow(curvature/(1.0+curvature), 1.0/durationSample);
            exponentNow = curvature+1.0;
        }
        for (int i = 0; i < durationSample; i++)
        {
            exponentNow *= exponentMul;
            curve.push_back((exponentNow - curvature) * 1.0);
        }
        bGenerated = true;
    }
    void generateMappedCurve(float volumeStart, float volumeEnd)
    {
        if (!bGenerated)
            return;
        if (volumeStart < volumeEnd)
        {
            for (int i = 0; i < curve.size(); i++)
            {
                curveMapped.push_back(ofMap(curve[i], 0.0, 1.0, volumeStart, volumeEnd, true));
            }
        } else
        {
            for (int i = 0; i < curve.size(); i++)
            {
                curveMapped.push_back(ofMap(curve[i], 1.0, 0.0, volumeStart, volumeEnd, true));
            }
        }
    }
    void clear()
    {
        curve.clear();
        curveMapped.clear();
    }
    vector<float> curve;
    vector<float> curveMapped;
    
private:
    bool bGenerated = false;
    int durationSample;
    float exponentMin;
    float exponentMul;
    float exponentNow;
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

struct WaveInstance
{
    bool bFree;
    bool bStart;
    bool bGenerator;
    bool bFinished;
    EnvelopeStage stage;
    int currentSampleCount;
    int currentSegmentIndex;
    int curveIndex;
    int generatorIndex;
    float frequency;
    float volume;
    float samples[AUDIO_BUFFER_SIZE*2];
};


class SynthWaveManager : public ofBaseSoundOutput
{
private:
    ofSoundBuffer workingBuffer;
    ofSoundStream soundStream;
    
    struct GeneratorContainer
    {
        vector<SawWaveGenerator> sawWaveGenerators;
        vector<TriangleWaveGenerator> triangleWaveGenerators;
        vector<SquareWaveGenerator> squareWaveGenerators;
        
        vector<OscillatorWavetable> oscillatorGenerators;
        vector<FrequencyModulationWavetable> frequencyModulationGenerators;
        vector<AmplitudeModulationWavetable> amplitudeModulationGenerators;
        vector<RingModulationWavetable> ringModulationGenerators;
        
    } generatorContainer;
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool bSynthPresetLoaded;
    bool bSynthPresetInialised;
    SynthPreset synthPreset;
    SynthPresetInstance synthPresetInstance;
    CurveExponentGenerator curveExponentGenerator;
    
    int activeWaveInstanceCount;
    vector<WaveInstance> waveInstances;
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    float mixerMasterVolume;
    Panning mixerPanning;
    
    bool bRecord;
    vector<float> recordData;
public:
    SynthWaveManager()
    {
        bSynthPresetLoaded = false;
        bSynthPresetInialised = false;
        bRecord = false;
        
        activeWaveInstanceCount = 0;
        
        setMixerMasterVolume(1.0);
        mixerPanning.left = 1.0;
        mixerPanning.right = 1.0;
        
        soundStream.setup(AUDIO_CHANNEL_NUMBER, 0, AUDIO_SAMPLE_RATE, AUDIO_BUFFER_SIZE, 1);
        soundStream.setOutput(this);
        
        loadSynthPreset(synthSettings.defaultPreset);
        inialiseSynthPreset();
    }
    
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void loadSynthPreset(SynthPreset synthPreset)
    {
        bSynthPresetLoaded = true;
        this->synthPreset = synthPreset;
    }
    void inialiseSynthPreset()
    {
        if (!bSynthPresetLoaded)
            return;
        
        synthPresetInstance.duration = synthPreset.duration;
        //TEMP
        //synthPresetInstance.panning = synthPreset.panning;
        synthPresetInstance.panning.left = 1.0;
        synthPresetInstance.panning.right = 1.0;
        inialiseSegmentInstance(synthPreset.envelope.segmentDelay,
                                synthPresetInstance.envelopeInstance.segmentDelay);
        
        inialiseSegmentInstance(synthPreset.envelope.segmentAttack,
                                synthPresetInstance.envelopeInstance.segmentAttack);
        
        inialiseSegmentInstance(synthPreset.envelope.segmentDecay,
                                synthPresetInstance.envelopeInstance.segmentDecay);
        
        
        synthPresetInstance.envelopeInstance.segmentsSustain.clear();
        for (int i = 0; i < synthPreset.envelope.segmentsSustain.size(); i++)
        {
            synthPresetInstance.envelopeInstance.segmentsSustain.push_back(*new SegmentInstance);
            inialiseSegmentInstance(synthPreset.envelope.segmentsSustain[i],
                                    synthPresetInstance.envelopeInstance.segmentsSustain[i]);
        }
        
        inialiseSegmentInstance(synthPreset.envelope.segmentDecay,
                                synthPresetInstance.envelopeInstance.segmentDecay);
        
        inialiseSegmentInstance(synthPreset.envelope.segmentRelease,
                                synthPresetInstance.envelopeInstance.segmentRelease);
        
        bSynthPresetInialised = true;
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
    
    
    
    
    
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void playDoAllWaveInstance(float frequency)
    {
        WaveInstance * current = getWaveInstance(frequency);
        getAvailableGenerator(*current);
        startWaveInstance(*current);
    }
    
    WaveInstance * getWaveInstance(float frequency)
    {
        //Look for a free wave instance
        bool bCreateNewInstance = true;
        int index;
        for (int i = 0; i < waveInstances.size(); i++)
        {
            if (waveInstances[i].bFree)
            {
                index = i;
                bCreateNewInstance = false;
                break;
            }
        }
        if (bCreateNewInstance)
        {
            waveInstances.push_back(*new WaveInstance);
            index = waveInstances.size()-1;
            resetWaveInstance(waveInstances[index], false, frequency);
            activeWaveInstanceCount++;
            return &waveInstances[index];
        } else
        {
            waveInstances[index].frequency = frequency;
            activeWaveInstanceCount++;
            return &waveInstances[index];
        }
    }
    void resetWaveInstance(WaveInstance & waveInstance, bool bFree = true, float frequency = 440.0)
    {
        waveInstance.bFree = bFree;
        waveInstance.bStart = false;
        waveInstance.bFinished = false;
        waveInstance.bGenerator = false;
        waveInstance.stage = EnvelopeStart;
        waveInstance.currentSampleCount = 0;
        waveInstance.currentSegmentIndex = 0;
        waveInstance.generatorIndex = 0;
        waveInstance.frequency = frequency;
        waveInstance.volume = 0.0;
    }
    void setFree(WaveInstance & waveInstance)
    {
        switch(synthSettings.getWaveType())
        {
            case SineWave:
            {
                break;
            }
            case SawWave:
            {
                generatorContainer.sawWaveGenerators[waveInstance.generatorIndex].setInUse(false);
                break;
            }
            case TriangleWave:
            {
                generatorContainer.triangleWaveGenerators[waveInstance.generatorIndex].setInUse(false);
                break;
            }
            case SquareWave:
            {
                generatorContainer.squareWaveGenerators[waveInstance.generatorIndex].setInUse(false);
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
                generatorContainer.oscillatorGenerators[waveInstance.generatorIndex].setInUse(false);
                break;
            }
            case FrequencyModulation:
            {
                generatorContainer.frequencyModulationGenerators[waveInstance.generatorIndex].setInUse(false);
                break;
            }
            case AmplitudeModulation:
            {
                generatorContainer.amplitudeModulationGenerators[waveInstance.generatorIndex].setInUse(false);
                break;
            }
            case RingModulation:
            {
                generatorContainer.ringModulationGenerators[waveInstance.generatorIndex].setInUse(false);
                break;
            }
        }
        resetWaveInstance(waveInstance);
    }
    void startWaveInstance(WaveInstance & waveInstance)
    {
        getAvailableGenerator(waveInstance);
        waveInstance.bStart = true;
    }
    int getNumberActiveWaveInstances()
    {
        int count = 0;
        for (int i = 0; i < waveInstances.size(); i++)
        {
            if (!waveInstances[i].bFree)
                count++;
        }
        return count;
    }
    void getAvailableGenerator(WaveInstance & waveInstance)
    {
        switch(synthSettings.getWaveType())
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
                        generatorContainer.sawWaveGenerators[i].setup(waveInstance.frequency);
                        waveInstance.bGenerator = true;
                        waveInstance.generatorIndex = i;
                        return;
                    }
                }
                generatorContainer.sawWaveGenerators.push_back(*new SawWaveGenerator);
                int index = generatorContainer.sawWaveGenerators.size()-1;
                generatorContainer.sawWaveGenerators[index].setInUse(true);
                generatorContainer.sawWaveGenerators[index].setup(waveInstance.frequency);
                waveInstance.bGenerator = true;
                waveInstance.generatorIndex = index;
                return;
            }
            case TriangleWave:
            {
                for (int i = 0; i < generatorContainer.triangleWaveGenerators.size(); i++)
                {
                    if (!generatorContainer.triangleWaveGenerators[i].getInUse())
                    {
                        waveInstance.bGenerator = true;
                        waveInstance.generatorIndex = i;
                        return;
                    }
                }
                generatorContainer.triangleWaveGenerators.push_back(*new TriangleWaveGenerator);
                int index = generatorContainer.triangleWaveGenerators.size()-1;
                generatorContainer.triangleWaveGenerators[index].setInUse(true);
                generatorContainer.triangleWaveGenerators[index].setup(waveInstance.frequency);
                waveInstance.bGenerator = true;
                waveInstance.generatorIndex = index;
                return;
            }
            case SquareWave:
            {
                for (int i = 0; i < generatorContainer.squareWaveGenerators.size(); i++)
                {
                    if (!generatorContainer.squareWaveGenerators[i].getInUse())
                    {
                        waveInstance.bGenerator = true;
                        waveInstance.generatorIndex = i;
                        return;
                    }
                }
                generatorContainer.squareWaveGenerators.push_back(*new SquareWaveGenerator);
                int index = generatorContainer.squareWaveGenerators.size()-1;
                generatorContainer.squareWaveGenerators[index].setInUse(true);
                SquareWaveData squareWaveData = synthSettings.getSquareWaveData();
                generatorContainer.squareWaveGenerators[index].setup(waveInstance.frequency, squareWaveData.dutyCycle, squareWaveData.amplitudeMin, squareWaveData.amplitudeMax);
                cout << "Got here \n";
                waveInstance.bGenerator = true;
                waveInstance.generatorIndex = index;
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
                        generatorContainer.oscillatorGenerators[i].setFrequency(waveInstance.frequency);
                        waveInstance.bGenerator = true;
                        waveInstance.generatorIndex = i;
                        return;
                    }
                }
                generatorContainer.oscillatorGenerators.push_back(*new OscillatorWavetable);
                int index = generatorContainer.oscillatorGenerators.size()-1;
                generatorContainer.oscillatorGenerators[index].setInUse(true);
                OscillatorData oscillatorData = synthSettings.getOscillatorData();
                generatorContainer.oscillatorGenerators[index].setup(waveInstance.frequency, oscillatorData.numberPartials, &oscillatorData.partials[0], &oscillatorData.amplitude[0], oscillatorData.gibbs);
                waveInstance.bGenerator = true;
                waveInstance.generatorIndex = index;
                return;
            }
            case FrequencyModulation:
            {
                for (int i = 0; i < generatorContainer.frequencyModulationGenerators.size(); i++)
                {
                    if (!generatorContainer.frequencyModulationGenerators[i].getInUse())
                    {
                        waveInstance.bGenerator = true;
                        waveInstance.generatorIndex = i;
                        return ;
                    }
                }
                generatorContainer.frequencyModulationGenerators.push_back(*new FrequencyModulationWavetable);
                int index = generatorContainer.frequencyModulationGenerators.size()-1;
                generatorContainer.frequencyModulationGenerators[index].setInUse(true);
                ModulationData frequencyModulationData = synthSettings.getFrequencyModulationData();
                generatorContainer.frequencyModulationGenerators[index].setup(waveInstance.frequency, frequencyModulationData.modulationMultiplier, frequencyModulationData.modulationAmplitude);
                waveInstance.bGenerator = true;
                waveInstance.generatorIndex = index;
                return;
            }
            case AmplitudeModulation:
            {
                for (int i = 0; i < generatorContainer.amplitudeModulationGenerators.size(); i++)
                {
                    if (!generatorContainer.amplitudeModulationGenerators[i].getInUse())
                    {
                        waveInstance.bGenerator = true;
                        waveInstance.generatorIndex = i;
                        return;
                    }
                }
                generatorContainer.amplitudeModulationGenerators.push_back(*new AmplitudeModulationWavetable);
                int index = generatorContainer.amplitudeModulationGenerators.size()-1;
                generatorContainer.amplitudeModulationGenerators[index].setInUse(true);
                ModulationData amplitudeModulationData = synthSettings.getAmplitudeModulationData();
                generatorContainer.amplitudeModulationGenerators[index].setup(waveInstance.frequency, amplitudeModulationData.modulationMultiplier, amplitudeModulationData.modulationAmplitude);
                waveInstance.bGenerator = true;
                waveInstance.generatorIndex = index;
                return;
            }
            case RingModulation:
            {
                for (int i = 0; i < generatorContainer.ringModulationGenerators.size(); i++)
                {
                    if (!generatorContainer.ringModulationGenerators[i].getInUse())
                    {
                        waveInstance.bGenerator = true;
                        waveInstance.generatorIndex = i;
                        return;
                    }
                }
                generatorContainer.ringModulationGenerators.push_back(*new RingModulationWavetable);
                int index = generatorContainer.ringModulationGenerators.size()-1;
                generatorContainer.ringModulationGenerators[index].setInUse(true);
                ModulationData ringModulationData = synthSettings.getRingModulationData();
                generatorContainer.ringModulationGenerators[index].setup(waveInstance.frequency, ringModulationData.modulationMultiplier, ringModulationData.modulationAmplitude);
                waveInstance.bGenerator = true;
                waveInstance.generatorIndex = index;
                return;
            }
        }
    }
    /*SawWaveGenerator * getSawWaveGenerator(int index)
    {
        return &generatorContainer.sawWaveGenerators[index];
    }
    TriangleWaveGenerator * getTriangleWaveGenerator(int index)
    {
        return &generatorContainer.triangleWaveGenerators[index];
    }
    SquareWaveGenerator * getSquareWaveGenerator(int index)
    {
        return &generatorContainer.squareWaveGenerators[index];
    }
    OscillatorWavetable * getOscillatorGenerator(int index)
    {
        return &generatorContainer.oscillatorGenerators[index];
    }
    FrequencyModulationWavetable * getFrequencyModulationGenerator(int index)
    {
        return &generatorContainer.frequencyModulationGenerators[index];
    }
    AmplitudeModulationWavetable * getAmplitudeModulationGenerator(int index)
    {
        return &generatorContainer.amplitudeModulationGenerators[index];
    }
    RingModulationWavetable * getRingModulationGenerator(int index)
    {
        return &generatorContainer.ringModulationGenerators[index];
    }*/
    
    void setFreeGenerator(int index)
    {
        switch(synthSettings.getWaveType())
        {
            case SineWave:
            {
                
            }
            case SawWave:
            {
                generatorContainer.sawWaveGenerators[index].setInUse(false);
                return;
            }
            case TriangleWave:
            {
                generatorContainer.triangleWaveGenerators[index].setInUse(false);
            }
            case SquareWave:
            {
                generatorContainer.squareWaveGenerators[index].setInUse(false);
            }
            case Pulse:
            {
                
            }
            case Summed:
            {
                
            }
            case Oscillator:
            {
                generatorContainer.oscillatorGenerators[index].setInUse(false);
                return;
            }
            case FrequencyModulation:
            {
                generatorContainer.frequencyModulationGenerators[index].setInUse(false);
                return;
            }
            case AmplitudeModulation:
            {
                generatorContainer.amplitudeModulationGenerators[index].setInUse(false);
                return;
            }
            case RingModulation:
            {
                generatorContainer.ringModulationGenerators[index].setInUse(false);
                return;
            }
        }
    }


    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void calcEnvelopeStage(WaveInstance & waveInstance, SegmentInstance & segmentInstance)
    {
        if (!segmentInstance.bVolumeChange)
        {
            waveInstance.volume = segmentInstance.volumeEnd;
            if (segmentInstance.sampleDuration <= waveInstance.currentSampleCount)
            {
                waveInstance.currentSampleCount = 0;
                waveInstance.stage = getNextStage(waveInstance.stage);
            }
        } else
        {
            if (segmentInstance.sampleDuration >= waveInstance.currentSampleCount)
            {
                waveInstance.volume = segmentInstance.curveExponential[waveInstance.curveIndex];
                waveInstance.curveIndex++;
            } else
            {
                waveInstance.curveIndex = 0;
                waveInstance.currentSampleCount = 0;
                waveInstance.stage = getNextStage(waveInstance.stage);
            }
        }
    }
    void calcEnvelopeRelease(WaveInstance & waveInstance, SegmentInstance & segmentInstance)
    {
        
    }
    EnvelopeStage getNextStage(EnvelopeStage stage)
    {
        if (stage == EnvelopeDelay)
        {
            return EnvelopeAttack;
        } else if (stage == EnvelopeAttack)
        {
            return EnvelopeDecay;
        } else if (stage == EnvelopeDecay)
        {
            return EnvelopeSustain;
        } else if (stage == EnvelopeSustain)
        {
            return EnvelopeRelease;
        } else if (stage == EnvelopeRelease)
        {
            return EnvelopeEnd;
        }
    }
    void processAudio(ofSoundBuffer &in, ofSoundBuffer &out)
    {
        int numFrames = out.getNumFrames();
        int numChannels = out.getNumChannels();
        float sample = 0.0;
        for (int w = 0; w < waveInstances.size(); w++)
        {
            if (!waveInstances[w].bFree && waveInstances[w].bStart)
            {
                for (int i = 0; i < numFrames; i++)
                {
                    switch(waveInstances[w].stage)
                    {
                        case EnvelopeStart:
                        {
                            waveInstances[w].curveIndex = 0;
                            waveInstances[w].stage = EnvelopeDelay;
                            break;
                        }
                        case EnvelopeDelay:
                        {
                            calcEnvelopeStage(waveInstances[w], synthPresetInstance.envelopeInstance.segmentDelay);
                            break;
                        }
                        case EnvelopeAttack:
                        {
                            calcEnvelopeStage(waveInstances[w], synthPresetInstance.envelopeInstance.segmentAttack);
                            break;
                        }
                        case EnvelopeDecay:
                        {
                            calcEnvelopeStage(waveInstances[w], synthPresetInstance.envelopeInstance.segmentDecay);
                            break;
                        }
                        case EnvelopeSustain:
                        {
                            calcEnvelopeStage(waveInstances[w], synthPresetInstance.envelopeInstance.segmentsSustain[0]);
                            break;
                        }
                        case EnvelopeRelease:
                        {
                            calcEnvelopeStage(waveInstances[w], synthPresetInstance.envelopeInstance.segmentRelease);
                            break;
                        }
                        case EnvelopeEnd:
                        {
                            setFree(waveInstances[w]);
                            break;
                        }
                        case EnvelopeCutoff:
                        {
                            //Do cutoff
                        }
                    }
                    switch (synthSettings.getWaveType())
                    {
                        case SineWave:
                        {
                            
                        }
                        case SawWave:
                        {
                            sample = generatorContainer.sawWaveGenerators[waveInstances[w].generatorIndex].generateSample() * waveInstances[w].volume;
                            waveInstances[w].samples[i*numChannels] = sample;
                            waveInstances[w].samples[i*numChannels+1] = sample;
                            break;
                        }
                        case TriangleWave:
                        {
                            sample = generatorContainer.triangleWaveGenerators[waveInstances[w].generatorIndex].generateSample() * waveInstances[w].volume;
                            waveInstances[w].samples[i*numChannels] = sample;
                            waveInstances[w].samples[i*numChannels+1] = sample;
                            break;
                        }
                        case SquareWave:
                        {
                            sample = generatorContainer.squareWaveGenerators[waveInstances[w].generatorIndex].generateSample() * waveInstances[w].volume;
                            waveInstances[w].samples[i*numChannels] = sample;
                            waveInstances[w].samples[i*numChannels+1] = sample;
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
                            sample = generatorContainer.oscillatorGenerators[waveInstances[w].generatorIndex].generateSample() * 1.0;
                            waveInstances[w].samples[i*numChannels] = sample;
                            waveInstances[w].samples[i*numChannels+1] = sample;
                            break;
                        }
                        case FrequencyModulation:
                        {
                            sample = generatorContainer.frequencyModulationGenerators[waveInstances[w].generatorIndex].generateSample() * waveInstances[w].volume;
                            waveInstances[w].samples[i*numChannels] = sample;
                            waveInstances[w].samples[i*numChannels+1] = sample;
                            break;
                        }
                        case AmplitudeModulation:
                        {
                            sample = generatorContainer.amplitudeModulationGenerators[waveInstances[w].generatorIndex].generateSample() * waveInstances[w].volume;
                            waveInstances[w].samples[i*numChannels] = sample;
                            waveInstances[w].samples[i*numChannels+1] = sample;
                            break;
                        }
                        case RingModulation:
                        {
                            sample = generatorContainer.ringModulationGenerators[waveInstances[w].generatorIndex].generateSample() * waveInstances[w].volume;
                            waveInstances[w].samples[i*numChannels] = sample;
                            waveInstances[w].samples[i*numChannels+1] = sample;
                            break;
                        }
                    }
                    waveInstances[w].currentSampleCount++;
                }
            }
        }
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    void setMixerMasterVolume(float masterVolume)
    {
        this->mixerMasterVolume = masterVolume;
    }
    
    void setMixerPanning(Panning panning)
    {
        this->mixerPanning = panning;
    }
    
    virtual void record(ofSoundBuffer &out)
    {
        for (int i = 0; i < out.size(); i++)
        {
            recordData.push_back(out.getBuffer()[i]);
        }
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void audioOut(ofSoundBuffer &out)
    {
        if (workingBuffer.size() != out.size())
        {
            workingBuffer.resize(out.size());
            workingBuffer.setNumChannels(out.getNumChannels());
            workingBuffer.setSampleRate(out.getSampleRate());
        };
        processAudio(workingBuffer, out);
        
        if (waveInstances.size() > 0)
        {
            for (int i = 0; i < waveInstances.size(); i++)
            {
                if (!waveInstances[i].bFree)
                {
                    int left = 0;
                    for (int right = 1; right < AUDIO_BUFFER_SIZE; right+=2, left+=2)
                    {
                        
                        out.getBuffer()[left] += waveInstances[i].samples[left] * (1.0/activeWaveInstanceCount) * mixerPanning.left;
                        out.getBuffer()[right] += waveInstances[i].samples[right] * (1.0/activeWaveInstanceCount) * mixerPanning.right;
                        //cout << mixerPanning.left  << "\n";
                        //if ((out.getBuffer()[b] > 1.0) || (out.getBuffer()[b] < -1.0))
                        //{
                        //    cout << "Audio not noramlised " << out.getBuffer()[b] << "\n";
                        //}
                    }
                }
            }
        }
        out *= mixerMasterVolume;
        
        if (bRecord)
            record(out);
    }
    void startRecord()
    {
        recordData = *new vector<float>;
        bRecord = true;
    }
    
    void saveRecording(string path)
    {
        ofFile f(path);
        if(ofToLower(f.getExtension())!="wav") {
            path += ".wav";
            ofLogWarning() << "Can only write wav files - will save file as " << path;
        }
        
        fstream file(ofToDataPath(path).c_str(), ios::out | ios::binary);
        if(!file.is_open()) {
            ofLogError() << "Error opening sound file '" << path << "' for writing";
            return false;
        }
        
        // write a wav header
        short myFormat = 1; // for pcm
        int mySubChunk1Size = 16;
        int bitsPerSample = 16; // assume 16 bit pcm
        int myByteRate = AUDIO_SAMPLE_RATE * 2 * bitsPerSample/8;
        short myBlockAlign = 2 * bitsPerSample/8;
        int myChunkSize = 36 + recordData.size()*bitsPerSample/8;
        int myDataSize = recordData.size()*bitsPerSample/8;
        cout << "myDataSize " << myDataSize << "\n";
        int channels = 2;
        int sampleRate = AUDIO_SAMPLE_RATE;
        
        file.seekp (0, ios::beg);
        file.write ("RIFF", 4);
        file.write ((char*) &myChunkSize, 4);
        file.write ("WAVE", 4);
        file.write ("fmt ", 4);
        file.write ((char*) &mySubChunk1Size, 4);
        file.write ((char*) &myFormat, 2); // should be 1 for PCM
        file.write ((char*) &channels, 2); // # channels (1 or 2)
        file.write ((char*) &sampleRate, 4); // 44100
        file.write ((char*) &myByteRate, 4); //
        file.write ((char*) &myBlockAlign, 2);
        file.write ((char*) &bitsPerSample, 2); //16
        file.write ("data", 4);
        file.write ((char*) &myDataSize, 4);
        
        
        
        short writeBuff[FILE_WRITE_BUFFER_SIZE];
        int pos = 0;
        while(pos<recordData.size()) {
            int len = MIN(FILE_WRITE_BUFFER_SIZE, recordData.size()-pos);
            for(int i = 0; i < len; i++) {
                writeBuff[i] = (int)(recordData[pos]*32767.f);
                pos++;
            }
            file.write((char*)writeBuff, len*bitsPerSample/8);
        }
        
        file.close();
    }
};
