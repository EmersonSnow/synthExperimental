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
            case SawWave:
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
            case SawWave:
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
            case SawWave:
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
                    case SawWave:
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
        bInUse = false;
        bIsSetup = false;
        //bGeneratorIsSetup = false;
        bStart = false;
        bCutoff = false;
        bGotGenerator = false;
        
        waveGeneratorIndex = 0;
        currentSampleCount = 0;
        segmentIndex = 0;
        wavetableIndex = 0;
        state = 0;
        
        frequency = 440.0;
        duration = 0.0;
        phase = 0.0;
        phaseIncrement = 0.0;
        
        volume = 1.0;

        triangleValue = 0.0;
        cout << "Called the initialiser \n";
    }
    
    void setup(WaveType waveType, Panning panning)
    {
        this->panning = panning;
        this->setWaveType(waveType);
        
        /*switch(waveType)
        {
            case SineWave:
            {
                phase = 0;
                float freTI = WAVETABLE_SIZE / (float)AUDIO_SAMPLE_RATE;
                phaseIncrement = freTI * frequency;
                break;
            }
            case SawWave:
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
        }*/
    }
    void setup(float frequency, WaveType waveType, Panning panning)
    {
        this->setFrequency(frequency);
        this->setWaveType(waveType);
        this->panning = panning;
    }
    void setWaveType(WaveType waveType)
    {
        this->waveType = waveType;
        if (bInUse)
        {
            getGenerator();
        }
        
    }
    void setFrequency(float frequency)
    {
        this->frequency = frequency;
        if (bGotGenerator)
        {
            waveGenerator->setFrequency(frequency);
            waveGenerator->reset();
        }
        /*switch(waveType)
        {
            case SineWave:
            {
                //wavetableGenerator.generate(AUDIO_SAMPLE_RATE);
                phase = 0;
                float freTI = WAVETABLE_SIZE / (float)AUDIO_SAMPLE_RATE;
                phaseIncrement = freTI * frequency;
                break;
            }
            case SawWave:
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
        }*/
    }
    inline void setInUse(bool b)
    {
        bInUse = b;
    }
    inline bool getInUse()
    {
        return bInUse;
    }
    bool checkIsSetup()
    {
        if (segments.size() == 0)
            return false;
        
        if (!bGotGenerator)
            return false;
        
        if (!bFrequencyIsSet)
            return false;
    }
    
    void getGenerator()
    {
        switch(waveType)
        {
            case SineWave:
            {
                
                break;
            }
            case SawWave:
            {
                waveGeneratorIndex = generatorContainer.getAvailableGenerator();
                waveGenerator = generatorContainer.getSawWaveGenerator(waveGeneratorIndex);
                break;
            }
            case TriangleWave:
            {
                waveGeneratorIndex = generatorContainer.getAvailableGenerator();
                waveGenerator = generatorContainer.getTriangleWaveGenerator(waveGeneratorIndex);
                break;
            }
            case SquareWave:
            {
                waveGeneratorIndex = generatorContainer.getAvailableGenerator();
                waveGenerator = generatorContainer.getSquareWaveGenerator(waveGeneratorIndex);
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
                waveGeneratorIndex = generatorContainer.getAvailableGenerator();
                waveGenerator = generatorContainer.getOscillatorGenerator(waveGeneratorIndex);
                break;
            }
            case FrequencyModulation:
            {
                waveGeneratorIndex = generatorContainer.getAvailableGenerator();
                waveGenerator = generatorContainer.getFrequencyModulationGenerator(waveGeneratorIndex);
                break;
            }
            case AmplitudeModulation:
            {
                waveGeneratorIndex = generatorContainer.getAvailableGenerator();
                waveGenerator = generatorContainer.getAmplitudeModulationGenerator(waveGeneratorIndex);
                break;
            }
            case RingModulation:
            {
                waveGeneratorIndex = generatorContainer.getAvailableGenerator();
                waveGenerator = generatorContainer.getRingModulationGenerator(waveGeneratorIndex);
                break;
            }
        }
        if (bFrequencyIsSet)
        {
            waveGenerator->setFrequency(frequency);
        }
        waveGenerator->reset();
        
        bGotGenerator = true;
    }
    GeneratorBase * getGeneratorPointer()
    {
        switch(waveType)
        {
            case SineWave:
            {
                
                break;
            }
            case SawWave:
            {
                return waveGenerator = generatorContainer.getSawWaveGenerator(waveGeneratorIndex);
            }
            case TriangleWave:
            {
                return generatorContainer.getTriangleWaveGenerator(waveGeneratorIndex);
            }
            case SquareWave:
            {
                return generatorContainer.getSquareWaveGenerator(waveGeneratorIndex);
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
                return generatorContainer.getOscillatorGenerator(waveGeneratorIndex);
            }
            case FrequencyModulation:
            {
                return generatorContainer.getFrequencyModulationGenerator(waveGeneratorIndex);
            }
            case AmplitudeModulation:
            {
                return generatorContainer.getAmplitudeModulationGenerator(waveGeneratorIndex);
            }
            case RingModulation:
            {
                return generatorContainer.getRingModulationGenerator(waveGeneratorIndex);
            }
        }
    }
    /*void setSawWave()
    {
        waveGeneratorIndex = generatorContainer.getAvailableGenerator();
        waveGenerator = generatorContainer.getSawWaveGenerator(waveGeneratorIndex);
        if (bFrequencyIsSet)
        {
            waveGenerator->setFrequency(frequency);
        }
        waveGenerator->reset();
    }
    void setTriangleWave()
    {
        waveGeneratorIndex = generatorContainer.getAvailableGenerator();
        waveGenerator = generatorContainer.getTriangleWaveGenerator(waveGeneratorIndex);
        if (bFrequencyIsSet)
        {
            waveGenerator->setFrequency(frequency);
        }
        waveGenerator->reset();
    }
    void setSquareWave()
    {
        waveGeneratorIndex = generatorContainer.getAvailableGenerator();
        waveGenerator = generatorContainer.getSquareWaveGenerator(waveGeneratorIndex);
        if (bFrequencyIsSet)
        {
            waveGenerator->setFrequency(frequency);
        }
        waveGenerator->reset();
    }
    void setOscillator()
    {
        waveGeneratorIndex = generatorContainer.getAvailableGenerator();
        waveGenerator = generatorContainer.getOscillatorGenerator(waveGeneratorIndex);
        if (bFrequencyIsSet)
        {
            waveGenerator->setFrequency(frequency);
        }
        waveGenerator->reset();
    }
    void setFrequencyModulation()
    {
        waveGeneratorIndex = generatorContainer.getAvailableGenerator();
        waveGenerator = generatorContainer.getFrequencyModulationGenerator(waveGeneratorIndex);
        if (bFrequencyIsSet)
        {
            waveGenerator->setFrequency(frequency);
        }
        waveGenerator->reset();
    }
    void setAmplitudeModulation()
    {
        waveGeneratorIndex = generatorContainer.getAvailableGenerator();
        waveGenerator = generatorContainer.getAmplitudeModulationGenerator(waveGeneratorIndex);
        if (bFrequencyIsSet)
        {
            waveGenerator->setFrequency(frequency);
        }
        waveGenerator->reset();
    }
    void setRingModulation()
    {
        waveGeneratorIndex = generatorContainer.getAvailableGenerator();
        waveGenerator = generatorContainer.getRingModulationGenerator(waveGeneratorIndex);
        if (bFrequencyIsSet)
        {
            waveGenerator->setFrequency(frequency);
        }
        waveGenerator->reset();
    }*/
    void freeGenerator()
    {
        waveGenerator->setInUse(false);
        bGotGenerator = false;
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
        
        cout << "Calling \n";
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
                case SawWave:
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
                case Summed:
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
    bool bInUse;
    
    bool bCutoff;
    bool bStart;
    bool bIsSetup;
    bool bFrequencyIsSet;
    
    int state;
    
    Segment cutoff;
    
    WaveType waveType;
    
    float volume;
    Panning panning;
    
    
    
    int currentSampleCount;
    int segmentIndex;
    
    float frequency;
    float duration;
    float phase;
    float phaseIncrement;
    
    
    
    bool bGotGenerator;
    int waveGeneratorIndex;
    
    //GeneratorBase & waveGenerator;
    GeneratorBase * waveGenerator;
    
    /*SawWaveGenerator * sawWaveGenerator;
    TriangleWaveGenerator * triangleWaveGenerator;
    SquareWaveGenerator squareWaveGenerator;
    OscillatorWavetable * oscillatorWavetable;
    FrequencyModulationWavetable * frequencyModulationWavetable;
    AmplitudeModulationWavetable * amplitudeModulationWavetable;
    RingModulationWavetable * ringModulationWavetable;*/
    
    
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

class AudioMixer : public SoundObject
{
public:
    void setup(float masterVolume, Panning panning)
    {
        this->masterVolume = masterVolume;
        this->panning = panning;
    }
    void addInput(WaveManager * waveManager)
    {
        mixerInputs.push_back(waveManager);
    }
    void removeInput(WaveManager * waveManager)
    {
        for (int i = 0; i < mixerInputs.size(); i++)
        {
            if (mixerInputs[i] == waveManager)
            {
                mixerInputs.erase(mixerInputs.begin()+i);
                return;
            }
        }
    }
    void audioOut(ofSoundBuffer &out)
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
                        /*if ((out.getBuffer()[b] > 1.0) || (out.getBuffer()[b] < -1.0))
                         {
                         cout << "Audio not noramlised " << out.getBuffer()[b] << "\n";
                         }*/
                    }
                }
            }
        }
        out *= masterVolume;
        
        if (bRecord)
            record(out);
    }
    
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
    vector<WaveManager*> mixerInputs;
};

