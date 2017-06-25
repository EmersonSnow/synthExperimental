//
//  SynthPreset.h
//  SynthesiserRewrite
//
//  Created by Zachary Snow on 24/06/2017.
//
//

#pragma once

#include "SynthDefinition.h"
#include "SynthCurveGenerator.h"


class SynthPresetManager
{
public:
    SynthPresetInstance inialisesynthPresetInstance(SynthPreset & synthPreset)
    {
        SynthPresetInstance * synthPresetInstance;
        synthPresetInstance->panning = synthPreset.panning;
        inialiseSegmentInstance(synthPreset.envelope.segmentDelay,
                                synthPresetInstance->envelopeInstance.segmentDelay);
        
        inialiseSegmentInstance(synthPreset.envelope.segmentAttack,
                                synthPresetInstance->envelopeInstance.segmentAttack);
        
        inialiseSegmentInstance(synthPreset.envelope.segmentDecay,
                                synthPresetInstance->envelopeInstance.segmentDecay);
        
        
        synthPresetInstance->envelopeInstance.segmentsSustain.clear();
        for (int i = 0; i < synthPreset.envelope.segmentsSustain.size(); i++)
        {
            synthPresetInstance->envelopeInstance.segmentsSustain.push_back(*new SegmentInstance);
            inialiseSegmentInstance(synthPreset.envelope.segmentsSustain[i],
                                    synthPresetInstance->envelopeInstance.segmentsSustain[i]);
        }
        
        inialiseSegmentInstance(synthPreset.envelope.segmentDecay,
                                synthPresetInstance->envelopeInstance.segmentDecay);
        
        inialiseSegmentInstance(synthPreset.envelope.segmentRelease,
                                synthPresetInstance->envelopeInstance.segmentRelease);
        return *synthPresetInstance;
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
    
private:
    CurveExponentGenerator curveExponentGenerator;
};
extern synthPreset;
