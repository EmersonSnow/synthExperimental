//
//  EnvelopeGenerator.h
//  toneGenerator3
//
//  Created by Zachary Snow on 23/06/2017.
//
//

#pragma once

#include "Includes.h"

class CurveExponentGenerator
{
public:
    void generate(float duration, float curvature, bool bAscending)
    {
        durationSample = duration * (float)AUDIO_SAMPLE_RATE;
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
                curveMapped.push_back(ofMap(curve[i], 0.0, 1.0, volumeStart, volumeEnd, false));
            }
        } else
        {
            for (int i = 0; i < curve.size(); i++)
            {
                curveMapped.push_back(ofMap(curve[i], 1.0, 0.0, volumeStart, volumeEnd, false));
            }
        }
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

struct Segment
{
    bool bVolumeChange;
    int sampleDuration;
    float duration;
    float volumeStart;
    float volumeEnd;
    CurveExponentGenerator curveExponentGenerator;
    
    int curveIndex;
};

//Volume will be handled by the ToneGenerator3 class (it will also be replaced)

/*class EnvelopeManager : public SoundObject
{
public:
    void setup(float frequency, WaveType waveType, Panning panning)
    {
        this->frequency = frequency;
        this->waveType = waveType;
        this->panning = panning;
        
        switch(waveType)
        {
            case SineWave:
            {
                //wavetableGenerator.generate(AUDIO_SAMPLE_RATE);
                phase = 0;
                float freTI = WAVETABLE_SIZE / (float)AUDIO_SAMPLE_RATE;
                phaseIncrement = freTI * frequency;
                break;
            }
            case SawtoothWave:
            {
                phase = -1;
                phaseIncrement = (2 * frequency) / (float)AUDIO_SAMPLE_RATE;
                break;
            }
            case TriangleWave:
            {
                phase = 0;
                phaseIncrement = (TWO_PI / (float)AUDIO_SAMPLE_RATE) * frequency;
                break;
            }
            case SquareWave:
            {
                
            }
            case Pulse:
            {
                
            }
            case Oscillator:
            {
                oscillatorWavetable = *new OscillatorWavetable();
            }
            case FrequencyModulation:
            {
                
            }
            case AmplitudeModulation:
            {
                
            }
        }
        
        bAudioChanged = true;
        bSoundSamplePeriodFinished = false;
    }
    void setWaveType(WaveType waveType)
    {
        this->waveType = waveType;
        switch(waveType)
        {
            case SineWave:
            {
                //wavetableGenerator.generate(AUDIO_SAMPLE_RATE);
                phase = 0;
                float freTI = WAVETABLE_SIZE / (float)AUDIO_SAMPLE_RATE;
                phaseIncrement = freTI * frequency;
                break;
            }
            case SawtoothWave:
            {
                phase = -1;
                phaseIncrement = (2 * frequency) / (float)AUDIO_SAMPLE_RATE;
                break;
            }
            case TriangleWave:
            {
                phase = 0;
                phaseIncrement = (TWO_PI / (float)AUDIO_SAMPLE_RATE) * frequency;
                break;
            }
            case SquareWave:
            {
                
            }
            case Pulse:
            {
                
            }
            case Oscillator:
            {
                oscillatorWavetable = *new OscillatorWavetable();
            }
            case FrequencyModulation:
            {
                
            }
            case AmplitudeModulation:
            {
                
            }
        }
        
        bAudioChanged = true;
    }
    void setFrequency(float frequency)
    {
        this->frequency = frequency;
        switch(waveType)
        {
            case SineWave:
            {
                //wavetableGenerator.generate(AUDIO_SAMPLE_RATE);
                phase = 0;
                float freTI = WAVETABLE_SIZE / (float)AUDIO_SAMPLE_RATE;
                phaseIncrement = freTI * frequency;
                break;
            }
            case SawtoothWave:
            {
                phase = -1;
                phaseIncrement = (2 * frequency) / AUDIO_SAMPLE_RATE;
                break;
            }
            case TriangleWave:
            {
                phase = 0;
                phaseIncrement = (TWO_PI / AUDIO_SAMPLE_RATE) * frequency;
            }
            case SquareWave:
            {
                
            }
            case Pulse:
            {
                
            }
            case Oscillator:
            {
                oscillatorWavetable.setFrequency(frequency);
            }
            case FrequencyModulation:
            {
                
            }
            case AmplitudeModulation:
            {
                
            }
        }
        bAudioChanged = true;
    }
    void setOscillatorWavetable(int numberPartials, float *partials, float *amplitude, bool gibbs)
    {
        oscillatorWavetable.setup(frequency, numberPartials, partials, amplitude, gibbs);
    }
    void addSegment(float duration, float volumeStart, float volumeEnd, float exponential, bool bSineWaveOscillator = false, int numberOscillator = 0)
    {
        Segment temp;
        temp.duration = duration;
        temp.volumeStart = volumeStart;
        temp.volumeEnd = volumeEnd;
        temp.sampleDuration = duration * AUDIO_SAMPLE_RATE;
        if (volumeStart == volumeEnd)
        {
            temp.bVolumeChange = false;
        } else
        {
            temp.curveExponentGenerator.generate(duration, exponential,
                                                 (volumeStart < volumeEnd) ? true : false);
            temp.curveExponentGenerator.generateMappedCurve(volumeStart, volumeEnd);
            temp.bVolumeChange = true;
        }
        segments.push_back(temp);
        
        bAudioChanged = true;
    }
    void insertSegment(int position, float duration, float volumeStart, float volumeEnd, float exponential, bool bSineWaveOscillator = false, int numberOscillator = 0)
    {
        if (segments.size() <= position)
            return;
        
        Segment temp;
        temp.duration = duration;
        temp.volumeStart = volumeStart;
        temp.volumeEnd = volumeEnd;
        temp.sampleDuration = duration * AUDIO_SAMPLE_RATE;
        if (volumeStart == volumeEnd)
        {
            temp.bVolumeChange = false;
        } else
        {
            temp.curveExponentGenerator.generate(duration, exponential,
                                                 (volumeStart < volumeEnd) ? true : false);
            temp.curveExponentGenerator.generateMappedCurve(volumeStart, volumeEnd);
            temp.bVolumeChange = true;
        }
        segments.insert(segments.begin()+position, temp);
        
        bAudioChanged = true;
    }
    void addCutoff(float duration, float volumeStart, float volumeEnd, float exponential)
    {
        cutoff.duration = duration;
        cutoff.volumeStart = volumeStart;
        cutoff.volumeEnd = volumeEnd;
        cutoff.sampleDuration = duration * AUDIO_SAMPLE_RATE;
        
        cutoff.curveExponentGenerator.generate(duration, exponential,
                                               (volumeStart < volumeEnd) ? true : false);
        cutoff.curveExponentGenerator.generateMappedCurve(volumeStart, volumeEnd);
        cutoff.bVolumeChange = true;
        
    }
    void removeSegment(int index)
    {
        if (segments.size() <= index)
            return;
        segments.erase(segments.begin()+index);
        
        bAudioChanged = true;
    }
    
    void setPanning(float panning, PanningType panningType)
    {
        this->panning = SynthUtil::getPanning(panning, panningType);
        
        bAudioChanged = true;
    }
    
    void start()
    {
        segmentIndex = 0;
        bStart = true;
    }
    void stopCutoff()
    {
        bCutoff = true;
    }
    void stop()
    {
        bStart = false;
    }
    //void allocateWavetableGenerator(WaveType waveType)
    //{
    
    //}
    void processAudio(ofSoundBuffer &in, ofSoundBuffer &out)
    {
        if (!bStart)
            return;
        
        float frequencyRadian = TWO_PI / in.getSampleRate();
        int numFrames = out.getNumFrames();
        int numChannels = out.getNumChannels();
        
        for (int i = 0; i < numFrames; i++, currentSampleCount++)
        {
            if (!bAudioChanged && bSoundSamplePeriodFinished)
            {
                out[i*numChannels] = audioPeriod[audioPeriodCount];
                out[i*numChannels+1] = audioPeriod[audioPeriodCount+1];
                if ((audioPeriodCount += 2) >= audioPeriod.size())
                {
                    audioPeriodCount = 0;
                }
            } else
            {
                if (bSoundSamplePeriodFinished)
                {
                    bSoundSamplePeriodFinished = false;
                }
                if (bAudioChanged)
                {
                    audioPeriod.clear();
                    bAudioChanged = false;
                }
                switch(state)
                {
                    case 0:
                        segments[segmentIndex].curveIndex = 0;
                        if (!segments[segmentIndex].bVolumeChange)
                        {
                            volume = segments[segmentIndex].volumeEnd;
                            state = 2;
                        } else
                        {
                            state = 1;
                        }
                        break;
                    case 1:
                        if (segments[segmentIndex].sampleDuration >= currentSampleCount)
                        {
                            volume = segments[segmentIndex].curveExponentGenerator.curveMapped[segments[segmentIndex].curveIndex];
                            segments[segmentIndex].curveIndex++;
                            
                        } else
                        {
                            state = 3;
                        }
                        break;
                    case 2:
                        if (segments[segmentIndex].sampleDuration <= currentSampleCount)
                        {
                            state = 3;
                        }
                        break;
                    case 3:
                        volume = segments[segmentIndex].volumeEnd;
                        currentSampleCount = -1;
                        if (++segmentIndex == segments.size())
                        {
                            //segmentIndex = 0;
                            audioPeriodCount = 0;
                            //bAudioChanged = false;
                            bSoundSamplePeriodFinished = true;
                        }
                        state = 0;
                        break;
                    case 4:
                    {
                        //TODO: work out cutoff
                        break;
                    }
                        
                }
                
                switch (waveType)
                {
                    case SineWave:
                    {
                        //Increased wavetable size by 4, to elimate this calcation;
                        //wavetableIndexBase = floor(wavetableIndex);
                         //wavetableIndexFract = wavetableIndex - wavetableIndexBase;
                         //wavetableValue1 = SynthUtil::getWavetableValue(WAVETABLE_SINE, wavetableIndex);
                        // wavetableValue2 = SynthUtil::getWavetableValue(WAVETABLE_SINE, wavetableIndex+1);
                         //wavetableValue1 = wavetableValue1 + ((wavetableValue2 - wavetableValue1) * wavetableIndexFract);
                        //out[i*numChannels] = wavetableValue1 * volume * panning.left;
                        //out[i*numChannels+1] = wavetableValue1 * volume * panning.right;
                        //cout << panning.left << " " << panning.right << "\n";
                        out[i*numChannels] = SynthUtil::getWavetableValue(WAVETABLE_SINE, wavetableIndex) * volume * panning.left;
                        out[i*numChannels+1] = SynthUtil::getWavetableValue(WAVETABLE_SINE, wavetableIndex) * volume * panning.right;
                        if ((wavetableIndex += phaseIncrement) >= (WAVETABLE_SIZE))
                        {
                            wavetableIndex -= WAVETABLE_SIZE;
                        }
                        break;
                    }
                    case SawtoothWave:
                    {
                        out[i*numChannels] = phase * volume * panning.left;
                        out[i*numChannels+1] = phase * volume * panning.right;
                        phase += phaseIncrement;
                        if (phase >= 1)
                        {
                            phase -= 2;
                        }
                        break;
                    }
                    case TriangleWave:
                    {
                        triangleValue = (phase * TWO_DIVIDE_PI);
                        if (phase < 0)
                        {
                            triangleValue = 1.0 + triangleValue;
                        } else
                        {
                            triangleValue = 1.0 - triangleValue;
                        }
                        out[i*numChannels] = triangleValue * volume * panning.left;
                        out[i*numChannels+1] = triangleValue * volume * panning.right;
                        
                        if ((phase += phaseIncrement) >= PI)
                        {
                            phase -= TWO_PI;
                        }
                    }
                    case SquareWave:
                    {
                        
                    }
                    case Pulse:
                    {
                        
                    }
                    case Oscillator:
                    {
                        float value = oscillatorWavetable.generateSample();
                        out[i*numChannels] = value * volume * panning.left;
                        out[i*numChannels+1] = value * volume * panning.right;
                    }
                    case FrequencyModulation:
                    {
                        
                    }
                    case AmplitudeModulation:
                    {
                        
                    }
                }
                audioPeriod.push_back(out[i*numChannels]);
                audioPeriod.push_back(out[i*numChannels+1]);
            }
        }
    }
    
    void getInUse()
    {
        return bInUse;
    }
private:
    //New values for MidiController
    bool bInUse = false;
    bool bCutoff = false;
    
    Segment cutoff;
    //End of new values
    
    
    bool bAudioChanged = false;
    bool bSoundSamplePeriodFinished = false;
    //bool bPlayingFromSavedTable = false;
    
    
    int audioPeriodCount;
    vector<float> audioPeriod;
    
    bool bStart = false;
    
    WaveType waveType;
    
    int state;
    int currentSampleCount;
    int segmentIndex;
    
    float frequency;
    float duration;
    float phase;
    float phaseIncrement;
    
    float volume;
    Panning panning;
    //WavetableGenerator wavetableGenerator;
    float wavetableIndex = 0;
    //int wavetableIndexBase;
     //float wavetableIndexFract;
     //float wavetableValue1;
     //float wavetableValue2;
    
    float triangleValue;
    //float wavetableIncrement;
    
    vector<Segment> segments;
    
    OscillatorWavetable oscillatorWavetable;
};*/
class WaveManager : public SoundObject
{
public:
    WaveManager()
    {
    }
    void setup(WaveType waveType, Panning panning)
    {
        this->frequency = 440.0;
        this->waveType = waveType;
        this->panning = panning;
        
        switch(waveType)
        {
            case SineWave:
            {
                phase = 0;
                float freTI = WAVETABLE_SIZE / (float)AUDIO_SAMPLE_RATE;
                phaseIncrement = freTI * frequency;
                break;
            }
            case SawtoothWave:
            {
                phase = -1;
                phaseIncrement = (2 * frequency) / (float)AUDIO_SAMPLE_RATE;
                break;
            }
            case TriangleWave:
            {
                phase = 0;
                phaseIncrement = (TWO_PI / (float)AUDIO_SAMPLE_RATE) * frequency;
                break;
            }
            case SquareWave:
            {
                
            }
            case Pulse:
            {
                
            }
            case Oscillator:
            {
                //oscillatorWavetable = *new OscillatorWavetable();
            }
            case FrequencyModulation:
            {
                
            }
            case AmplitudeModulation:
            {
                
            }
            case RingModulation:
            {
                
            }
        }
    }
    void setup(float frequency, WaveType waveType, Panning panning)
    {
        this->frequency = frequency;
        this->waveType = waveType;
        this->panning = panning;
        
        switch(waveType)
        {
            case SineWave:
            {
                phase = 0;
                float freTI = WAVETABLE_SIZE / (float)AUDIO_SAMPLE_RATE;
                phaseIncrement = freTI * frequency;
                break;
            }
            case SawtoothWave:
            {
                phase = -1;
                phaseIncrement = (2 * frequency) / (float)AUDIO_SAMPLE_RATE;
                break;
            }
            case TriangleWave:
            {
                phase = 0;
                phaseIncrement = (TWO_PI / (float)AUDIO_SAMPLE_RATE) * frequency;
                break;
            }
            case SquareWave:
            {
                
            }
            case Pulse:
            {
                
            }
            case Oscillator:
            {
                //oscillatorWavetable = *new OscillatorWavetable();
            }
            case FrequencyModulation:
            {
                
            }
            case AmplitudeModulation:
            {
                
            }
        }
    }
    void setWaveType(WaveType waveType)
    {
        this->waveType = waveType;
        switch(waveType)
        {
            case SineWave:
            {
                //wavetableGenerator.generate(AUDIO_SAMPLE_RATE);
                phase = 0;
                float freTI = WAVETABLE_SIZE / (float)AUDIO_SAMPLE_RATE;
                phaseIncrement = freTI * frequency;
                break;
            }
            case SawtoothWave:
            {
                phase = -1;
                phaseIncrement = (2 * frequency) / (float)AUDIO_SAMPLE_RATE;
                break;
            }
            case TriangleWave:
            {
                phase = 0;
                phaseIncrement = (TWO_PI / (float)AUDIO_SAMPLE_RATE) * frequency;
                break;
            }
            case SquareWave:
            {
                
            }
            case Pulse:
            {
                
            }
            case Oscillator:
            {
                //oscillatorWavetable = *new OscillatorWavetable();
            }
            case FrequencyModulation:
            {
                
            }
            case AmplitudeModulation:
            {
                
            }
        }
    }
    void setFrequency(float frequency)
    {
        this->frequency = frequency;
        switch(waveType)
        {
            case SineWave:
            {
                //wavetableGenerator.generate(AUDIO_SAMPLE_RATE);
                phase = 0;
                float freTI = WAVETABLE_SIZE / (float)AUDIO_SAMPLE_RATE;
                phaseIncrement = freTI * frequency;
                break;
            }
            case SawtoothWave:
            {
                phase = -1;
                phaseIncrement = (2 * frequency) / AUDIO_SAMPLE_RATE;
                break;
            }
            case TriangleWave:
            {
                phase = 0;
                phaseIncrement = (TWO_PI / AUDIO_SAMPLE_RATE) * frequency;
            }
            case SquareWave:
            {
                
            }
            case Pulse:
            {
                
            }
            case Oscillator:
            {
                //oscillatorWavetable.setFrequency(frequency);
            }
            case FrequencyModulation:
            {
                
            }
            case AmplitudeModulation:
            {
                
            }
            case RingModulation:
            {
                
            }
        }
    }
    
    void setOscillator()
    {
        waveGeneratorIndex = generatorContainer.getAvailableGenerator();
        oscillatorWavetable = generatorContainer.getOscillatorGenerator(waveGeneratorIndex);
    }
    void setFrequencyModulation()
    {
        waveGeneratorIndex = generatorContainer.getAvailableGenerator();        frequencyModulationWavetable = generatorContainer.getFrequencyModulationGenerator(waveGeneratorIndex);
    }
    void setAmplitudeModulation()
    {
        waveGeneratorIndex = generatorContainer.getAvailableGenerator();
        amplitudeModulationWavetable = generatorContainer.getAmplitudeModulationGenerator(waveGeneratorIndex);
    }
    void setRingModulation()
    {
        waveGeneratorIndex = generatorContainer.getAvailableGenerator();
        ringModulationWavetable = generatorContainer.getRingModulationGenerator(waveGeneratorIndex);
    }
    
    void addSegment(float duration, float volumeStart, float volumeEnd, float exponential, bool bSineWaveOscillator = false, int numberOscillator = 0)
    {
        Segment temp;
        temp.duration = duration;
        temp.volumeStart = volumeStart;
        temp.volumeEnd = volumeEnd;
        temp.sampleDuration = duration * AUDIO_SAMPLE_RATE;
        if (volumeStart == volumeEnd)
        {
            temp.bVolumeChange = false;
        } else
        {
            temp.curveExponentGenerator.generate(duration, exponential,
                                                 (volumeStart < volumeEnd) ? true : false);
            temp.curveExponentGenerator.generateMappedCurve(volumeStart, volumeEnd);
            temp.bVolumeChange = true;
        }
        segments.push_back(temp);
    }
    bool getInUse()
    {
        return bInUse;
    }
    void insertSegment(int position, float duration, float volumeStart, float volumeEnd, float exponential, bool bSineWaveOscillator = false, int numberOscillator = 0)
    {
        if (segments.size() <= position)
            return;
        
        Segment temp;
        temp.duration = duration;
        temp.volumeStart = volumeStart;
        temp.volumeEnd = volumeEnd;
        temp.sampleDuration = duration * AUDIO_SAMPLE_RATE;
        if (volumeStart == volumeEnd)
        {
            temp.bVolumeChange = false;
        } else
        {
            temp.curveExponentGenerator.generate(duration, exponential,
                                                 (volumeStart < volumeEnd) ? true : false);
            temp.curveExponentGenerator.generateMappedCurve(volumeStart, volumeEnd);
            temp.bVolumeChange = true;
        }
        segments.insert(segments.begin()+position, temp);
    }
    void addCutoff(float duration, float volumeStart, float volumeEnd, float exponential)
    {
        cutoff.duration = duration;
        cutoff.volumeStart = volumeStart;
        cutoff.volumeEnd = volumeEnd;
        cutoff.sampleDuration = duration * AUDIO_SAMPLE_RATE;
        
        cutoff.curveExponentGenerator.generate(duration, exponential,
                                               (volumeStart < volumeEnd) ? true : false);
        cutoff.curveExponentGenerator.generateMappedCurve(volumeStart, volumeEnd);
        cutoff.bVolumeChange = true;
        
    }
    void removeSegment(int index)
    {
        if (segments.size() <= index)
            return;
        segments.erase(segments.begin()+index);
    }
    
    void setPanning(float panning, PanningType panningType)
    {
        this->panning = SynthUtil::getPanning(panning, panningType);
    }
    
    void start()
    {
        segmentIndex = 0;
        bStart = true;
    }
    void stopCutoff()
    {
        bCutoff = true;
    }
    void stop()
    {
        bStart = false;
    }
    //void allocateWavetableGenerator(WaveType waveType)
    //{
    
    //}
    void processAudio(ofSoundBuffer &in, ofSoundBuffer &out)
    {
        if (!bStart)
            return;
        
        int numFrames = out.getNumFrames();
        int numChannels = out.getNumChannels();
        
        for (int i = 0; i < numFrames; i++, currentSampleCount++)
        {
            switch(state)
            {
                case 0:
                    segments[segmentIndex].curveIndex = 0;
                    if (!segments[segmentIndex].bVolumeChange)
                    {
                        volume = segments[segmentIndex].volumeEnd;
                        state = 2;
                    } else
                    {
                        state = 1;
                    }
                    break;
                case 1:
                    if (segments[segmentIndex].sampleDuration >= currentSampleCount)
                    {
                        volume = segments[segmentIndex].curveExponentGenerator.curveMapped[segments[segmentIndex].curveIndex];
                            segments[segmentIndex].curveIndex++;
                            
                    } else
                    {
                        state = 3;
                    }
                    break;
                case 2:
                    if (segments[segmentIndex].sampleDuration <= currentSampleCount)
                    {
                        state = 3;
                    }
                    break;
                case 3:
                    volume = segments[segmentIndex].volumeEnd;
                    currentSampleCount = -1;
                    if (++segmentIndex == segments.size())
                    {
                        segmentIndex = 0;
                    }
                    state = 0;
                    break;
                case 4:
                {
                    //TODO: work out cutoff
                    break;
                }
            }
                
            switch (waveType)
            {
                case SineWave:
                {
                    //Increased wavetable size by 4, to elimate this calcation;
                    /*wavetableIndexBase = floor(wavetableIndex);
                    wavetableIndexFract = wavetableIndex - wavetableIndexBase;
                    wavetableValue1 = SynthUtil::getWavetableValue(WAVETABLE_SINE, wavetableIndex);
                    wavetableValue2 = SynthUtil::getWavetableValue(WAVETABLE_SINE, wavetableIndex+1);
                    wavetableValue1 = wavetableValue1 + ((wavetableValue2 - wavetableValue1) * wavetableIndexFract);*/
                    //out[i*numChannels] = wavetableValue1 * volume * panning.left;
                    //out[i*numChannels+1] = wavetableValue1 * volume * panning.right;
                    //cout << panning.left << " " << panning.right << "\n";
                    out[i*numChannels] = SynthUtil::getWavetableValue(WAVETABLE_SINE, wavetableIndex) * volume * panning.left;
                    out[i*numChannels+1] = SynthUtil::getWavetableValue(WAVETABLE_SINE, wavetableIndex) * volume * panning.right;
                    if ((wavetableIndex += phaseIncrement) >= (WAVETABLE_SIZE))
                    {
                        wavetableIndex -= WAVETABLE_SIZE;
                    }
                    break;
                }
                case SawtoothWave:
                {
                    out[i*numChannels] = phase * volume * panning.left;
                    out[i*numChannels+1] = phase * volume * panning.right;
                    phase += phaseIncrement;
                    if (phase >= 1)
                    {
                        phase -= 2;
                    }
                    break;
                }
                case TriangleWave:
                {
                    triangleValue = (phase * TWO_DIVIDE_PI);
                    if (phase < 0)
                    {
                        triangleValue = 1.0 + triangleValue;
                    } else
                    {
                        triangleValue = 1.0 - triangleValue;
                    }
                    out[i*numChannels] = triangleValue * volume * panning.left;
                    out[i*numChannels+1] = triangleValue * volume * panning.right;
                    
                    if ((phase += phaseIncrement) >= PI)
                    {
                        phase -= TWO_PI;
                    }
                }
                case SquareWave:
                {
                    
                }
                case Pulse:
                {
                        
                }
                case Oscillator:
                {
                    //float value = oscillatorWavetable.generateSample();
                    //out[i*numChannels] = value * volume * panning.left;
                    //out[i*numChannels+1] = value * volume * panning.right;
                }
                case FrequencyModulation:
                {
                    
                }
                case AmplitudeModulation:
                {
                        
                }
                case RingModulation:
                {
                    
                }
            }
        }
    }
    
    
private:
    //New values for MidiController
    bool bInUse = false;
    bool bCutoff = false;
    
    Segment cutoff;
    
    bool bStart = false;
    
    WaveType waveType;
    
    float volume;
    Panning panning;
    
    int state;
    
    int currentSampleCount;
    int segmentIndex;
    
    float frequency;
    float duration;
    float phase;
    float phaseIncrement;
    
    int waveGeneratorIndex;
    OscillatorWavetable * oscillatorWavetable;
    FrequencyModulationWavetable * frequencyModulationWavetable;
    AmplitudeModulationWavetable * amplitudeModulationWavetable;
    RingModulationWavetable * ringModulationWavetable;
    
    
    //WavetableGenerator wavetableGenerator;
    float wavetableIndex = 0;
    /*int wavetableIndexBase;
     float wavetableIndexFract;
     float wavetableValue1;
     float wavetableValue2;*/
    
    float triangleValue;
    //float wavetableIncrement;
    
    vector<Segment> segments;
    
};
