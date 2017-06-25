
//
//  SynthController.h
//  toneGenerator3
//
//  Created by Zachary Snow on 24/06/2017.
//
//

#pragma once

#include "WaveManager.h"
#include "Includes.h"



class SynthController : public ofBaseSoundOutput
{
public:
    SynthController()
    {
        
        this->masterVolume = 1.0;
        this->panning = SynthUtil::getPanning(0.5, PanningLinear);
        soundStream.setup(2, 0, AUDIO_SAMPLE_RATE, AUDIO_BUFFER_SIZE, 1);
        //cout << "Setting up soundstream \n";
    }
    void setWaveType(WaveType waveType)
    {
        this->waveType = waveType;
        //generatorContainer.setWaveType(waveType);
    }
    WaveType getWaveType()
    {
        return waveType;
    }
    /*AudioMixer * getAudioMixer()
    {
        return &audioMixer;
    }*/
    /*void createWaveManagers()
    {
        for (int i = 0; i < WAVE_MANAGER_NUMBER; i++)
        {
            WaveManager temp;
            temp.setup(waveType, SynthUtil::getPanning(0.5, PanningLinear));
            waveManagers.push_back(*new WaveManager());
            int index = waveManagers.size()-1;
            audioMixer.addInput(&waveManagers[index]);
        }
        soundStream.setOutput(audioMixer);
    }
    WaveManager * getFreeWaveManager()
    {
        int index;
        for (index = 0; index < waveManagers.size(); index++)
        {
            if (!waveManagers[index].getInUse())
            {
                waveManagers[index].setInUse(true);
                return &waveManagers[index];
            }
        }
        waveManagers.push_back(*new WaveManager);
        index = waveManagers.size()-1;
        waveManagers[index].setup(waveType, SynthUtil::getPanning(0.5, PanningLinear));
        waveManagers[index].getGenerator();
        
        audioMixer.addInput(&waveManagers[index]);
        soundStream.setOutput(audioMixer);
        return &waveManagers[index];
    }*/
    
    /*void audioOut(ofSoundBuffer &out)
    {
        if (mixerInputs.size() > 0)
        {
            for (int i = 0; i < mixerInputs.size(); i++)
            {
                if (mixerInputs[i]->getInUse())
                    cout << i << " Index, " << mixerInputs[i]->getInUse() << "\n";
                if (mixerInputs[i] != NULL && mixerInputs[i]->getInUse())
                {
                    ofSoundBuffer temp;
                    temp.resize(out.size());
                    temp.setNumChannels(out.getNumChannels());
                    temp.setSampleRate(out.getSampleRate());
                    
                    mixerInputs[i]->audioOut(temp);
                    
                    int left = 0;
                    for (int right = 1; right < temp.size(); right+=2, left+=2)
                    {
                        out.getBuffer()[left] += temp.getBuffer()[left] * (1.0/mixerInputs.size()) * panning.left;
                        out.getBuffer()[right] += temp.getBuffer()[right] * (1.0/mixerInputs.size()) * panning.right;
                        if ((out.getBuffer()[b] > 1.0) || (out.getBuffer()[b] < -1.0))
                        {
                            cout << "Audio not noramlised " << out.getBuffer()[b] << "\n";
                        }
                    }
                }
            }
        }
        out *= masterVolume;
        
        if (bRecord)
            record(out);
    }*/
    
    void setMasterVolume(float masterVolume)
    {
        this->masterVolume = masterVolume;
    }
    
    void setPanning(float panning, PanningType panningType)
    {
        this->panning = SynthUtil::getPanning(panning, panningType);
    }
    
    virtual void record(ofSoundBuffer &out)
    {
        for (int i = 0; i < out.size(); i++)
        {
            recordData.push_back(out.getBuffer()[i]);
        }
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

private:
    float masterVolume = 1.0;
    Panning panning;
    bool bRecord = false;
    vector<float> recordData;
    
    WaveType waveType;
    
    ofSoundStream soundStream;
    
    //vector<WaveManager> waveManagers;
    //vector<bool> envelopeMangersInUse;
};

