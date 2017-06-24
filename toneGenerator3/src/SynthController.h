
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



class SynthController //Store global values in here
{
public:
    SynthController()
    {
        audioMixer.setup(1.0, SynthUtil::getPanning(0.5, PanningLinear));;
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
    WaveManager & getFreeWaveManagers()
    {
        int index;
        for (index = 0; index < waveManagers.size(); index++)
        {
            if (!waveManagers[index].getInUse())
            {
                switch(waveType)
                {
                    case Oscillator:
                    {
                        waveManagers[index].setOscillator();
                    }
                    case FrequencyModulation:
                    {
                        waveManagers[index].setFrequencyModulation();
                    }
                    case AmplitudeModulation:
                    {
                        waveManagers[index].setAmplitudeModulation();
                    }
                    case RingModulation:
                    {
                        waveManagers[index].setRingModulation();
                    }
                }
                return waveManagers[index];
            }
        }
        index = waveManagers.size();
        waveManagers.push_back(*new WaveManager);
        waveManagers[index].setup(waveType, SynthUtil::getPanning(0.5, PanningLinear));
        
        switch(waveType)
        {
            case Oscillator:
            {
                waveManagers[index].setOscillator();
            }
            case FrequencyModulation:
            {
                waveManagers[index].setFrequencyModulation();
            }
            case AmplitudeModulation:
            {
                waveManagers[index].setAmplitudeModulation();
            }
            case RingModulation:
            {
                waveManagers[index].setRingModulation();
            }
        }

        audioMixer.addInput(&waveManagers[index]);
        return waveManagers[index];
    }

    
private:
    AudioMixer audioMixer;
    WaveType waveType; //Just this for now
    vector<WaveManager> waveManagers;
    //vector<bool> envelopeMangersInUse;
    
    OscillatorData oscillatorData;
    ModulationData frequencyModulationData;
    ModulationData amplitudeModulationData;
    ModulationData ringModulationData;
};

extern SynthController synthController;
