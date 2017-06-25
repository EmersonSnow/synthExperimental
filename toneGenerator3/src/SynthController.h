
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



class SynthController
{
public:
    SynthController()
    {
        SynthUtil::init();
        audioMixer.setup(1.0, SynthUtil::getPanning(0.5, PanningLinear));
        soundStream.setup(2, 0, AUDIO_SAMPLE_RATE, AUDIO_BUFFER_SIZE, 1);
        //cout << "Setting up soundstream \n";
    }
    void setWaveType(WaveType waveType)
    {
        this->waveType = waveType;
        generatorContainer.setWaveType(waveType);
    }
    WaveType getWaveType()
    {
        return waveType;
    }
    AudioMixer * getAudioMixer()
    {
        return &audioMixer;
    }
    void createWaveManagers()
    {
        for (int i = 0; i < WAVE_MANAGER_NUMBER; i++)
        {
            WaveManager temp;
            temp.setup(waveType, SynthUtil::getPanning(0.5, PanningLinear));
            waveManagers.push_back(temp);
            int index = waveManagers.size()-1;
            //audioMixer.AddWave();
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
    }
    
    vector<WaveManager> waveManagers;
private:
    AudioMixer audioMixer;
    WaveType waveType; //Just this for now
    
    ofSoundStream soundStream;
    
    //vector<bool> envelopeMangersInUse;
};

extern SynthController synthController;
