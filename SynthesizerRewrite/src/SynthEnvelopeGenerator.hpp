//
//  SynthEnvelopeGenerator.h
//  SynthesizerRewrite
//
//  Created by Zachary Snow on 25/06/2017.
//
//

#pragma once

#include "SynthDefinition.hpp"
#include "SynthPresetManager.hpp"
#include "SynthClassBase.hpp"

class EnvelopeGenerator : public SynthClassBase
{
public:
    
    EnvelopeGenerator()
    {
        bInUse = false;
        stageSampleCount = 0;
        index = 0;
        stage = EnvelopeStart;
    }
    void reset()
    {
        stageSampleCount = 0;
        index = 0;
        stage = EnvelopeStart;
    }
    void getStage()
    {
        return stage;
    }
    float generate()
    {
        SynthPresetInstance & currentPreset = synthPresetManager.getCurrentPreset();
        switch(stage)
        {
            case EnvelopeStart:
            {
                index = 0;
                stage = EnvelopeDelay;
                break;
            }
            case EnvelopeDelay:
            {
                return calcEnvelopeStage(currentPreset.envelopeInstance.segmentDelay);
            }
            case EnvelopeAttack:
            {
                return calcEnvelopeStage(currentPreset.envelopeInstance.segmentAttack);
            }
            case EnvelopeDecay:
            {
                return calcEnvelopeStage(currentPreset.envelopeInstance.segmentDecay);
            }
            case EnvelopeSustain:
            {
                return calcEnvelopeStage(currentPreset.envelopeInstance.segmentSustain);
            }
            case EnvelopeRelease:
            {
                return calcEnvelopeStage(currentPreset.envelopeInstance.segmentRelease);
            }
            case EnvelopeEnd:
            {
                reset();
                break;
            }
            case EnvelopeCutoff:
            {
                //Do cutoff
                break;
            }
        }
    }
    
    float calcEnvelopeStage(SegmentInstance & segmentInstance)
    {
        float volume;
        if (!segmentInstance.bVolumeChange)
        {
            volume = segmentInstance.volumeEnd;
            if (segmentInstance.sampleDuration <= stageSampleCount)
            {
                volume = segmentInstance.volumeEnd;
                stageSampleCount = 0;
                stage = getNextStage(stage);
            }
            return volume;
        } else
        {
            if (segmentInstance.sampleDuration >= stageSampleCount)
            {
                volume = segmentInstance.curveExponential[index];
                index++;
            } else
            {
                volume = segmentInstance.volumeEnd;
                index = 0;
                stageSampleCount = 0;
                stage = getNextStage(stage);
            }
            return volume;
        }
    }
    void calcEnvelopeCutoff(SegmentInstance & segmentInstance)
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
private:
    int stageSampleCount;
    int index;
    EnvelopeStage stage;
};
