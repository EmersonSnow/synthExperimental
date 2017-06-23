#pragma once

#include "ofMain.h"
#include "SynthDefinition.h"
#include "SynthAudio.h"
#include "WavetableGenerators.h"
#include "EnvelopeGenerator.h"
#include "ofxXmlSettings.h"


















//Old classes

class ToneGenerator3 : public SoundObject
{
public:
    void setup(/*float frequency,*/ WaveType waveType, int sampleRate)
    {
        this->frequency = 440.0; //Set 440.0 by as a placeholder
        this->waveType = waveType;
        this->sampleRate = AUDIO_SAMPLE_RATE;
        
        panning.left = 1.0;
        panning.right = 1.0;
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
                phaseIncrement = (2 * frequency) / AUDIO_SAMPLE_RATE;
                break;
            }
            case TriangleWave:
            {
                phase = 0;
                phaseIncrement = (TWO_PI / AUDIO_SAMPLE_RATE) * frequency;
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
        }
        bAudioChanged = true;
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
    
    int sampleRate;
    
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
    /*int wavetableIndexBase;
     float wavetableIndexFract;
     float wavetableValue1;
     float wavetableValue2;*/
    
    float triangleValue;
    //float wavetableIncrement;
    
    vector<Segment> segments;
    
    
};

class OscillatedWavetableBook : public SoundObject
{
public:
    void setup(float frequency, int numberPartials, float *partials, float *amplitude, int gibbs = 0, float volume = 1.0)
    {
        this->maxMultiplier = 1;
        this->frequency = frequency;
        this->numberPartials = numberPartials;
        this->gibbs = gibbs;
        
        this->index = 0;
        this->indexIncrement = SynthUtil::getFrequencyTableIndex() * frequency;
        

        for (int i = 0; i < numberPartials; i++)
        {
            SumPart part;
            part.index = 0;
            part.increment = 0;
            part.multiplier = partials[i];
            part.amplitude = amplitude[i];
            part.sigma = amplitude[i];
            if (part.multiplier > maxMultiplier)
                maxMultiplier = part.multiplier;
            parts.push_back(part);
        }
        calcParts();;
    }
    
    void calcParts()
    {
        float tableDivideTwo = (float) WAVETABLE_SIZE / 2.0;
        scale = 0;
        countPartials = 0;
        float sigK = 0;
        float sigN = 0;
        float sigTL = 0;
        if (gibbs)
        {
            sigK = PI / maxMultiplier;
            sigTL = tableDivideTwo;
        }
        for (int i = 0; i < parts.size(); i++)
        {
            parts[i].increment = indexIncrement * parts[i].multiplier;
            if (parts[i].increment < tableDivideTwo)
            {
                if (gibbs && (parts[i].multiplier > 0))
                {
                    sigN = sigK * parts[i].multiplier;
                    parts[i].sigma = parts[i].amplitude * SynthUtil::getWavetableValue(WAVETABLE_SINE, sigN*sigTL) / sigN;
                } else
                {
                    parts[i].sigma = parts[i].amplitude;
                }
                scale += (float) fabs((double)parts[i].sigma);
            } else
            {
                parts[i].sigma = 0;
            }
        }
        
    }
    inline float IndexCheck(float index)
    {
        if (index >= WAVETABLE_SIZE)
        {
            do
                index -= WAVETABLE_SIZE;
            while (index >= WAVETABLE_SIZE);
        }
        else if (index < 0)
        {
            do
                index += WAVETABLE_SIZE;
            while (index < 0);
        }
        return index;
    }
    void processAudio(ofSoundBuffer &in, ofSoundBuffer &out)
    {
        int numFrames = out.getNumFrames();
        int numChannels = out.getNumChannels();
        float v = 0;
        for (int i = 0; i < numFrames; i++)
        {
            v = 0;
            for (int p = 0; p < parts.size(); p++)
            {
                /*if (parts[p].index >= WAVETABLE_SIZE)
                {
                    while (parts[p].index >= WAVETABLE_SIZE)
                    {
                        parts[p].index -= WAVETABLE_SIZE;
                    }
                } else if (parts[p].index < 0)
                {
                    while (parts[p].index < 0)
                    {
                        parts[p].index += WAVETABLE_SIZE;
                    }
                }*/
                parts[p].index = IndexCheck(parts[p].index);
                v += SynthUtil::getWavetableValue(WAVETABLE_SINE, parts[p].index);
                parts[p].index += parts[p].increment;
            }
            v /= scale;
            
            index = IndexCheck(index);
            /*if (index >= WAVETABLE_SIZE)
            {
                while (index >= WAVETABLE_SIZE)
                {
                    index -= WAVETABLE_SIZE;
                }
            } else if (index < 0)
            {
                while (index < 0)
                {
                    index += WAVETABLE_SIZE;
                }
            }*/
            v += SynthUtil::getWavetableValue(WAVETABLE_SINE, index);
            index += indexIncrement;
            
            
            for (int c = 0; c < numChannels; c++)
            {
                out[i*numChannels +c] = v * 1.0;
            }
            //cout << SynthUtil::getOscillatedWavetableValue(SynthUtil::getOscillatedWavetableIndex(frequency), 4, sampleIndex) << "\n";
        }
    }
private:
    int numberPartials;
    int countPartials;
    bool gibbs;
    float frequency;
    float volume;
    float index;
    float indexIncrement;
    float maxMultiplier;
    float scale;
    struct SumPart
    {
        float index;
        float increment;
        float multiplier;
        float amplitude;
        float sigma;
    };
    vector<SumPart> parts;
    
    
    
};
class FrequencyModulation : public SoundObject
{
public:
    void setup(float carrierFrequency, float modulationFrequencyMultiplier, float volume = 1.0, float phase = 0)
    {
        this->carrierFrequency = carrierFrequency;
        this->modulationFrequency = carrierFrequency*modulationFrequencyMultiplier;
        this->volume = volume;
        //this->phase = phase;
        modulationPhase = 0;
        carrierPhase = 0;
        
    }
    
    void processAudio(ofSoundBuffer &in, ofSoundBuffer &out)
    {
        float frequencyRadian = TWO_PI / in.getSampleRate();
        modulationIncrement = frequencyRadian * modulationFrequency;
        modulationAmplitude = 100;
        int numFrames = out.getNumFrames();
        int numChannels = out.getNumChannels();
        for (int i = 0; i < numFrames; i++)
        {
            for (int c = 0; c < numChannels; c++)
            {
                out[i*numChannels +c] = sin(carrierPhase) * volume;
            }
            modulationValue = modulationAmplitude * sin(modulationPhase);
            carrierIncrement = frequencyRadian * (carrierFrequency + modulationValue);
            if ((carrierPhase += carrierIncrement) >= TWO_PI)
                carrierPhase -= TWO_PI;
            
            if((modulationPhase += modulationIncrement) >= TWO_PI)
                modulationPhase -= TWO_PI;
        }
    }
    //float frequency;
    float modulationFrequency;
    float volume;
private:
    float currentPhase;
    float modulationValue;
    float modulationIncrement;
    float modulationPhase;
    float modulationAmplitude;
    float carrierPhase;
    float carrierIncrement;
    float carrierFrequency;
};
class FrequencyModulationRefactored : public SoundObject
{
public:
    void setup(float carrierFrequency, float modulationFrequencyMultiplier, float volume = 1.0)
    {
        this->carrierFrequency = carrierFrequency;
        this->modulationFrequency = carrierFrequency*modulationFrequencyMultiplier;
        this->volume = volume;
        
        float frequencyRadian = (float)TWO_PI / (float)AUDIO_SAMPLE_RATE;
        modulationIncrement = frequencyRadian * modulationFrequency;
        modulationAmplitude = frequencyRadian * 100;
        modulationPhase = 0;
        
        carrierIncrement = frequencyRadian * modulationFrequency;
        carrierPhase = 0;
    }
    
    void processAudio(ofSoundBuffer &in, ofSoundBuffer &out)
    {
        int numFrames = out.getNumFrames();
        int numChannels = out.getNumChannels();
        for (int i = 0; i < numFrames; i++)
        {
            for (int c = 0; c < numChannels; c++)
            {
                out[i*numChannels +c] = sin(carrierPhase) * volume;
            }
            modulationValue = modulationAmplitude * sin(modulationPhase);
            carrierPhase += carrierIncrement + modulationValue;
            if (carrierPhase >= TWO_PI)
                carrierPhase -= TWO_PI;
            
            if((modulationPhase += modulationIncrement) >= TWO_PI)
                modulationPhase -= TWO_PI;
        }
    }
    //float frequency;
    float modulationFrequency;
    float volume;
private:
    float currentPhase;
    float modulationValue;
    float modulationIncrement;
    float modulationPhase;
    float modulationAmplitude;
    float carrierPhase;
    float carrierIncrement;
    float carrierFrequency;
};
class FrequencyModulationRefactoredWavetable : public SoundObject
{
public:
    void setup(float carrierFrequency, float modulationFrequencyMultiplier, float modulationIndexAmplitude, float volume = 1.0)
    {
        this->carrierFrequency = carrierFrequency;
        this->modulationFrequency = carrierFrequency*modulationFrequencyMultiplier;
        this->volume = volume;
        this->indexOfModulation = modulationIndexAmplitude;
        float freTI = (float)WAVETABLE_SIZE / (float)AUDIO_SAMPLE_RATE;
        modulationIncrement = freTI * modulationFrequency;
        modulationAmplitude = freTI * indexOfModulation * modulationFrequency;
        modulationIndex = 0;
        
        carrierIncrement = freTI * carrierFrequency;
        carrierIndex = 0;
    }
    
    void processAudio(ofSoundBuffer &in, ofSoundBuffer &out)
    {
        int numFrames = out.getNumFrames();
        int numChannels = out.getNumChannels();
        for (int i = 0; i < numFrames; i++)
        {
            for (int c = 0; c < numChannels; c++)
            {
                out[i*numChannels +c] = SynthUtil::getWavetableValue(WAVETABLE_SINE, carrierIndex) * volume;
            }
            modulationValue = modulationAmplitude * SynthUtil::getWavetableValue(WAVETABLE_SINE, modulationIndex);
            
            carrierIndex += carrierIncrement + modulationValue;
            if (carrierIndex >= WAVETABLE_SIZE)
            {
                carrierIndex -= WAVETABLE_SIZE;
            } else if (carrierIndex < 0)
            {
                carrierIndex += WAVETABLE_SIZE;
            }
            
            if((modulationIndex += modulationIncrement) >= WAVETABLE_SIZE)
                modulationIndex -= WAVETABLE_SIZE;
        }
    }
    //float frequency;
    float modulationFrequency;
    float volume;
private:
    float currentPhase;
    float modulationValue;
    float modulationIncrement;
    float modulationIndex;
    float modulationAmplitude;
    float indexOfModulation;
    float carrierIndex;
    float carrierIncrement;
    float carrierFrequency;
};
class AmplitudeModulation : public SoundObject
{
public:
    void setup(float carrierFrequency, float modulationFrequencyMultiplier, float modulationAmplitude,
               float volume = 1.0, float phase = 0)
    {
        this->carrierFrequency = carrierFrequency;
        this->modulationFrequency = carrierFrequency*modulationFrequencyMultiplier;
        this->modulationAmplitude = modulationAmplitude;
        this->volume = volume;
        //this->phase = phase;
        modulationPhase = 0;
        carrierPhase = 0;
        
    }
    
    void processAudio(ofSoundBuffer &in, ofSoundBuffer &out)
    {
        float frequencyRadian = TWO_PI / in.getSampleRate();
        carrierIncrement = frequencyRadian * carrierFrequency;
        modulationIncrement = frequencyRadian * modulationFrequency;
        modulationScale = 1.0 / (1.0 + modulationAmplitude);
        int numFrames = out.getNumFrames();
        int numChannels = out.getNumChannels();
        for (int i = 0; i < numFrames; i++)
        {
            modulationValue = 1.0 + (modulationAmplitude * sin(modulationPhase));
            carrierValue = volume * sin(carrierPhase);
            for (int c = 0; c < numChannels; c++)
            {
                out[i*numChannels +c] = carrierValue * modulationValue * modulationScale;
            }
            if ((carrierPhase += carrierIncrement) >= TWO_PI)
                carrierPhase -= TWO_PI;
            
            if((modulationPhase += modulationIncrement) >= TWO_PI)
                modulationPhase -= TWO_PI;
        }
    }
    //float frequency;
    float modulationFrequency;
    float volume;
private:
    float currentPhase;
    float modulationValue;
    float modulationIncrement;
    float modulationPhase;
    float modulationAmplitude;
    float modulationScale;
    float carrierPhase;
    float carrierIncrement;
    float carrierFrequency;
    float carrierValue;
};
//It does work, the original was written wrong
class AmplitudeModulationWavetableOld : public SoundObject
{
public:
    void setup(float carrierFrequency, float modulationFrequencyMultiplier, float modulationAmplitude,
               float volume = 1.0)
    {
        this->carrierFrequency = carrierFrequency;
        this->modulationFrequency = carrierFrequency*modulationFrequencyMultiplier;
        this->modulationAmplitude = modulationAmplitude;
        this->volume = volume;
        //this->phase = phase;
        modulationIndex = 0;
        carrierIndex = 0;
        float freTI = (float)WAVETABLE_SIZE / (float)AUDIO_SAMPLE_RATE;
        carrierIncrement = freTI * carrierFrequency;
        modulationIncrement = freTI * modulationFrequency;
        
        modulationScale = 1.0 / (1.0 + modulationAmplitude);
        
    }
    
    void processAudio(ofSoundBuffer &in, ofSoundBuffer &out)
    {
        int numFrames = out.getNumFrames();
        int numChannels = out.getNumChannels();
        for (int i = 0; i < numFrames; i++)
        {
            modulationValue = 1.0 + (modulationAmplitude * SynthUtil::getWavetableValue(WAVETABLE_SINE, modulationIndex));
            carrierValue = volume * SynthUtil::getWavetableValue(WAVETABLE_SINE, carrierIndex);
            for (int c = 0; c < numChannels; c++)
            {
                out[i*numChannels +c] = carrierValue * modulationValue * modulationScale;
            }
            carrierIndex += carrierIncrement;
            if(carrierIndex >= WAVETABLE_SIZE)
                carrierIndex -= WAVETABLE_SIZE;
            
            modulationIndex += modulationIncrement;
            if(modulationIndex >= WAVETABLE_SIZE)
                modulationIndex -= WAVETABLE_SIZE;
        }
    }
    //float frequency;
    float modulationFrequency;
    float volume;
private:
    float modulationValue;
    float modulationIncrement;
    float modulationIndex;
    float modulationAmplitude;
    float modulationScale;
    float carrierIndex;
    float carrierIncrement;
    float carrierFrequency;
    float carrierValue;
};
class AmplitudeModulationWavetableExample : public SoundObject
{
public:
    void setup(float carrierFrequency, float modulationFrequency, float modulationAmplitude, float volume = 1.0)
    {
        this->carrierFrequency = carrierFrequency;
        this->modulationFrequency = modulationFrequency;
        this->modulationAmplitude = modulationAmplitude;
        
        modulationScale = 1.0 / (1.0 + modulationAmplitude);
        
        carrierIncrement = carrierFrequency * SynthUtil::getFrequencyTableIndex();
        modulationIncrement = modulationFrequency * SynthUtil::getFrequencyTableIndex();
    }
    inline float IndexCheck(float index)
    {
        if (index >= WAVETABLE_SIZE)
        {
            do
                index -= WAVETABLE_SIZE;
            while (index >= WAVETABLE_SIZE);
        }
        else if (index < 0)
        {
            do
                index += WAVETABLE_SIZE;
            while (index < 0);
        }
        return index;
    }
    void processAudio(ofSoundBuffer &in, ofSoundBuffer &out)
    {
        int numFrames = out.getNumFrames();
        int numChannels = out.getNumChannels();
        for (int i = 0; i < numFrames; i++)
        {
            carrierIndex = IndexCheck(carrierIndex);
            modulationIndex = IndexCheck(modulationIndex);
            
            modulationValue = 1.0 + (modulationAmplitude * SynthUtil::getWavetableValue(WAVETABLE_SINE, modulationIndex));
            for (int c = 0; c < numChannels; c++)
            {
                
                out[i*numChannels +c] = SynthUtil::getWavetableValue(WAVETABLE_SINE, carrierIndex) * modulationValue * modulationScale;
            }
            carrierIndex += carrierIncrement;
            modulationIndex += modulationIncrement;
        }
    }
    //float frequency;
    float modulationFrequency;
    float volume;
private:
    float modulationValue;
    float modulationIncrement;
    float modulationIndex;
    float modulationAmplitude;
    float modulationScale;
    float carrierIndex;
    float carrierIncrement;
    float carrierFrequency;
    float carrierValue;
};
class RingAmplitudeModulation : public SoundObject
{
public:
    void setup(float carrierFrequency, float modulationFrequencyMultiplier, float modulationAmplitude,
               float volume = 1.0, float phase = 0)
    {
        this->carrierFrequency = carrierFrequency;
        this->modulationFrequency = carrierFrequency*modulationFrequencyMultiplier;
        this->modulationAmplitude = modulationAmplitude;
        this->volume = volume;
        //this->phase = phase;
        modulationPhase = 0;
        carrierPhase = 0;
        
    }
    
    void processAudio(ofSoundBuffer &in, ofSoundBuffer &out)
    {
        float frequencyRadian = TWO_PI / in.getSampleRate();
        carrierIncrement = frequencyRadian * modulationValue;
        modulationIncrement = frequencyRadian * modulationFrequency;
        modulationScale = 1.0 / (1.0 + modulationAmplitude);
        int numFrames = out.getNumFrames();
        int numChannels = out.getNumChannels();
        for (int i = 0; i < numFrames; i++)
        {
            modulationValue = modulationAmplitude * sin(modulationPhase);
            carrierValue = volume * sin(carrierPhase);
            for (int c = 0; c < numChannels; c++)
            {
                out[i*numChannels +c] = carrierValue * modulationValue;
            }
            if ((carrierPhase += carrierIncrement) >= TWO_PI)
                carrierPhase -= TWO_PI;
            
            if((modulationPhase += modulationIncrement) >= TWO_PI)
                modulationPhase -= TWO_PI;
        }
    }
    //float frequency;
    float modulationFrequency;
    float volume;
private:
    float currentPhase;
    float modulationValue;
    float modulationIncrement;
    float modulationPhase;
    float modulationAmplitude;
    float modulationScale;
    float carrierPhase;
    float carrierIncrement;
    float carrierFrequency;
    float carrierValue;
};

class NoiseGenerator : public SoundObject
{
public:
    void setup(float volume = 1.0)
    {
        this->volume = volume;
        
    }
    
    void processAudio(ofSoundBuffer &in, ofSoundBuffer &out)
    {
        int numFrames = out.getNumFrames();
        int numChannels = out.getNumChannels();
        for (int i = 0; i < numFrames; i++)
        {
            float value  = ofRandom(-1.0, 1.0);
            for (int c = 0; c < numChannels; c++)
            {
                out[i*numChannels +c] = value;
            }
        }
    }
    //float frequency;
    float volume;
private:
};


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
        ToneGenerator3 storeUnalteredSoundInTable;
    ToneGenerator3 storeUnalteredSoundInTable2;
    ToneGenerator3 storeUnalteredSoundInTable3;
    ToneGenerator3 storeUnalteredSoundInTable4;
    ToneGenerator3 storeUnalteredSoundInTable5;
    AudioMixer audioMixer;
    
    //FrequencyModulation frequencyModulation;
    //AmplitudeModulation amplitudeModulation;
    RingAmplitudeModulation ringAmplitudeModulation;
    NoiseGenerator noiseGenerator;
    AmplitudeModulationWavetable amplitudeModulationWavetable;
    FrequencyModulationRefactored frequencyModulationRefactored;
    FrequencyModulationRefactoredWavetable frequencyModulationRefactoredWavetable;
    OscillatedWavetableBook oscillatedWavetableBook;
    AmplitudeModulationWavetableExample
    amplitudeModulationWavetableExample;
    EnvelopeManager envelopeManager;
        ofSoundStream soundStream;
    
};
