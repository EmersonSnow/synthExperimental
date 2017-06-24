//
//  SynthAudio.h
//  toneGenerator3
//
//  Created by Zachary Snow on 23/06/2017.
//
//

#pragma once

#include "SynthDefinition.h"

class SoundObject : public ofBaseSoundOutput
{
public:
    virtual ~SoundObject() {}
    
    virtual void processAudio(ofSoundBuffer &input, ofSoundBuffer &output)
    {
        input.copyTo(output);
    }
    
    virtual void audioOut(ofSoundBuffer &out)
    {
        if (workingBuffer.size() != out.size())
        {
            workingBuffer.resize(out.size());
            workingBuffer.setNumChannels(out.getNumChannels());
            workingBuffer.setSampleRate(out.getSampleRate());
        }
        //inputObject->audioOut(workingBuffer);
        processAudio(workingBuffer, out);
    }
    
protected:
    
    ofSoundBuffer workingBuffer;
    SoundObject *inputObject;
};
