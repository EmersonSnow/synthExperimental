//
//  SynthWaveManager.h
//  SynthesiserRewrite
//
//  Created by Zachary Snow on 24/06/2017.
//
//

#pragma once

#include "SynthDefinition.hpp"
#include "SynthPresetManager.hpp"
#include "SynthWaveInstance.hpp"
#include "SynthEnvelopeGenerator.hpp"

struct WaveDevices
{
    
};

class SynthWaveManager : public ofBaseSoundOutput
{
private:
    ofSoundBuffer workingBuffer;
    ofSoundStream soundStream;
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool bSynthPresetLoaded;
    bool bSynthPresetInialised;
    SynthPreset synthPreset;
    SynthPresetInstance synthPresetInstance;
    
    //int activewaveInstanceStructCount;
    //vector<WaveInstanceStruct> waveInstanceStructs;
    vector<WaveInstance> waveInstances;
    vector<EnvelopeGenerator> envelopeGenerators;
    vector<ofSoundBuffer> soundBuffers;
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
        
        
        setMixerMasterVolume(1.0);
        mixerPanning.left = 1.0;
        mixerPanning.right = 1.0;
        
        soundStream.setup(AUDIO_CHANNEL_NUMBER, 0, AUDIO_SAMPLE_RATE, AUDIO_BUFFER_SIZE, 1);
        soundStream.setOutput(this);
        
        //loadSynthPreset(synthPresetManager.currentPreset);
        //inialiseSynthPreset();
    }
    
    void update()
    {
        /*for (int i = 0; i < waveInstances.size(); i++)
        {
            waveInstances[i].update();
        }*/
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void loadSynthPreset(SynthPreset synthPreset)
    {
        bSynthPresetLoaded = true;
        this->synthPreset = synthPreset;
        this->synthPresetInstance = synthPresetManager.inialiseSynthPresetInstance(synthPreset);
    }
    
    
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    int getWaveInstance(float frequency = 440.0)
    {
        //Look for a free wave instance
        bool bCreateNewInstance = true;
        int index;
        for (int i = 0; i < waveInstances.size(); i++)
        {
            if (!waveInstances[i].getInUse())
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
            waveInstances[index].setEnvelopeIndex(index);
            return index;
        } else
        {
            waveInstances[index].setFrequency(frequency);
            waveInstances[index].setEnvelopeIndex(index);
            return index;
        }
    }
    void startWaveInstance(int index)
    {
        waveInstances[index].start();
    }
    int getEnevelopGenerator()
    {
        for (int i = 0; i < waveInstances.size(); i++)
        {
            if (!envelopeGenerators[i].getInUse())
            {
                return i;
            }
        }
        envelopeGenerators.push_back(*new EnvelopeGenerator);
        return waveInstances.size()-1;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void processAudio(ofSoundBuffer &in, ofSoundBuffer &out)
    {
        int numFrames = out.getNumFrames();
        int numChannels = out.getNumChannels();
        float sample = 0.0;
        for (int w = 0; w < waveInstances.size(); w++)
        {
            if (waveInstances[w].getInUse())
            {
                //TODO enevlope
                for (int i = 0; i < numFrames; i++)
                {
                    sample = waveInstances[w].generate() * envelopeGenerators[waveInstances[w].getEnvelopeIndex()].generate();
                    out[i*numChannels] = sample ;
                    out[i*numChannels+1] = sample;
                    /*switch(waveInstanceStructs[w].stage)
                    {
                        case EnvelopeStart:
                        {
                            waveInstanceStructs[w].curveIndex = 0;
                            waveInstanceStructs[w].stage = EnvelopeDelay;
                            break;
                        }
                        case EnvelopeDelay:
                        {
                            calcEnvelopeStage(waveInstanceStructs[w], synthPresetInstance.envelopeInstance.segmentDelay);
                            break;
                        }
                        case EnvelopeAttack:
                        {
                            calcEnvelopeStage(waveInstanceStructs[w], synthPresetInstance.envelopeInstance.segmentAttack);
                            break;
                        }
                        case EnvelopeDecay:
                        {
                            calcEnvelopeStage(waveInstanceStructs[w], synthPresetInstance.envelopeInstance.segmentDecay);
                            break;
                        }
                        case EnvelopeSustain:
                        {
                            calcEnvelopeStage(waveInstanceStructs[w], synthPresetInstance.envelopeInstance.segmentsSustain[0]);
                            break;
                        }
                        case EnvelopeRelease:
                        {
                            calcEnvelopeStage(waveInstanceStructs[w], synthPresetInstance.envelopeInstance.segmentRelease);
                            break;
                        }
                        case EnvelopeEnd:
                        {
                            setFree(waveInstanceStructs[w]);
                            break;
                        }
                        case EnvelopeCutoff:
                        {
                            //Do cutoff
                        }
                    }*/
                    
                    /*switch (synthSettings.getWaveType())
                    {
                        case SineWave:
                        {
                            
                        }
                        case SawWave:
                        {
                            sample = waveInstances[w].samples[i]; // envelopeGenerator.volume[i];
                            samples[i*numChannels] = sample;
                            samples[i*numChannels+1] = sample;
                            break;
                        }
                        case TriangleWave:
                        {
                            sample = generatorContainerClass.triangleWaveGenerators[waveInstanceStructs[w].generatorIndex].generateSample() * waveInstanceStructs[w].volume;
                            samples[i*numChannels] = sample;
                            samples[i*numChannels+1] = sample;
                            break;
                        }
                        case SquareWave:
                        {
                            sample = generatorContainerClass.squareWaveGenerators[waveInstanceStructs[w].generatorIndex].generateSample() * waveInstanceStructs[w].volume;
                            waveInstanceStructs[w].samples[i*numChannels] = sample;
                            waveInstanceStructs[w].samples[i*numChannels+1] = sample;
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
                            sample = generatorContainerClass.oscillatorGenerators[waveInstanceStructs[w].generatorIndex].generateSample() * 1.0;
                            waveInstanceStructs[w].samples[i*numChannels] = sample;
                            waveInstanceStructs[w].samples[i*numChannels+1] = sample;
                            break;
                        }
                        case FrequencyModulation:
                        {
                            sample = generatorContainerClass.frequencyModulationGenerators[waveInstanceStructs[w].generatorIndex].generateSample() * waveInstanceStructs[w].volume;
                            waveInstanceStructs[w].samples[i*numChannels] = sample;
                            waveInstanceStructs[w].samples[i*numChannels+1] = sample;
                            break;
                        }
                        case AmplitudeModulation:
                        {
                            sample = generatorContainerClass.amplitudeModulationGenerators[waveInstanceStructs[w].generatorIndex].generateSample() * waveInstanceStructs[w].volume;
                            waveInstanceStructs[w].samples[i*numChannels] = sample;
                            waveInstanceStructs[w].samples[i*numChannels+1] = sample;
                            break;
                        }
                        case RingModulation:
                        {
                            sample = generatorContainerClass.ringModulationGenerators[waveInstanceStructs[w].generatorIndex].generateSample() * waveInstanceStructs[w].volume;
                            waveInstanceStructs[w].samples[i*numChannels] = sample;
                            waveInstanceStructs[w].samples[i*numChannels+1] = sample;
                            break;
                        }
                    }
                    waveInstanceStructs[w].stageSampleCount++;*/
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
            int n = WaveInstance::getActivateWaveInstances();
            for (int i = 0; i < waveInstances.size(); i++)
            {
                if (!waveInstances[i].getInUse())
                {
                    int left = 0;
                    for (int right = 1; right < AUDIO_BUFFER_SIZE; right+=2, left+=2)
                    {
                        out.getBuffer()[left] += waveInstances[i].samples[left] * (1.0/n) * mixerPanning.left;
                        out.getBuffer()[right] += waveInstances[i].samples[right] * (1.0/n) * mixerPanning.right;
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
