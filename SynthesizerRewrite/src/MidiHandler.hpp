//
//  MidiHandler.h
//  toneGenerator3
//
//  Created by Zachary Snow on 24/06/2017.
//
//


#pragma once

struct SegmentMidi
{
    float duration;
    float volumeStart;
    float volumeEnd;
    float curve;
    //SegmentMidi * previousSegment;
};
class MidiController : public ofxMidiListener
{
public:
    void setup(int midiPort, WaveType waveType, int sampleRate, bool bRepeating, float durationTotal, float volumeMax, float durationAttack, float volumeStart, float durationDecay, float volumeDecay, float durationCutoff, float volumeCutoff)
    {
        this->midiPort = midiPort;
        this->waveType = waveType;
        this->sampleRate = sampleRate;
        this->durationTotal = durationTotal;
        this->durationAttack = durationAttack;
        this->durationDecay = durationDecay;
        this->durationCutoff = durationCutoff;
        
        midiIn.openPort(midiPort);
        midiIn.ignoreTypes(false, false, false);
        midiIn.addListener(this);
        midiIn.setVerbose(true);
        
        for (int i = 0; i < MIDI_CONTROLLER_TONE_GENERATOR_NUMBER; i++)
        {
            toneGenerators[i].setup(waveType, sampleRate);
            
        }
    }
    
    void setMidiPort(int midiPort)
    {
        this->midiPort = midiPort;
    }
    void getMidiPort()
    {
        return midiPort;
    }
    void setWaveType(WaveType waveType)
    {
        this->waveType = waveType;
    }
    void getWaveType()
    {
        return waveType;
    }
    void setDuration(float durationTotal, float volumePeak)
    {
        this->durationTotal = durationTotal;
        this->volumePeak = volumePeak;
    }
    void getDuration()
    {
        return durationTotal;
    }
    void setRepeat(bool repeat)
    {
        bRepeat = repeat;
    }
    void getRepeat()
    {
        return bRepeat;
    }
    void setAttack(float duration, float volumeStart, float curve)
    {
        durationAttack = duration;
        volumeAttackStart = volumeStart;
        curveAttack = curve;
    }
    void setDecay(float duration, float volumeEnd, float curve)
    {
        durationDecay = duration;
        volumeDecayEnd = volumeEnd;
        curveDecay = curve;
    }
    void setCutoff(float duration, float volumeEnd, float curve)
    {
        durationCutoff = duration;
        volumeCutoffEnd = volumeEnd;
        curveCutoff = curve;
    }
    void addPeakSegnent(float duration, float volumeEnd, float curve)
    {
        SegmentMidi temp;
        temp.duration = duration;
        temp.volumeEnd = volumeEnd;
        temp.curve = curve;
        if (segmentsPeak.size() == 0)
        {
            temp.volumeStart = volumePeak;
        } else
        {
            temp.volumeStart = segmentsPeak[segmentsPeak.size()-1].volumeEnd;
        }
        segmentsPeak.push_back(temp);
        
        bPeakSegmentsChanged = true;
        
    }
    void replacePeakSegment(int index, float duration, float volumeEnd, float curve)
    {
        if (segmentsPeak.size() <= index)
            return;
        segmentsPeak[index].duration = duration;
        segmentsPeak[index].volumeEnd = volumeEnd;
        segmentsPeak[index].curve = curve;
        
        bPeakSegmentsChanged = true;
    }
    void insertPeakSegment(int position, float duration, float volumeEnd, float curve)
    {
        if (segmentsPeak.size() <= position)
            return;
        
        SegmentMidi temp;
        temp.duration = duration;
        temp.volumeEnd = volumeEnd;
        temp.curve = curve;
        if (position == 0)
        {
            temp.volumeStart = volumePeak;
        } else
        {
            temp.volumeStart = segmentsPeak[position-1].volumeEnd;
        }
        segmentsPeak.insert(segmentsPeak.begin()+position, temp);
        
        bPeakSegmentsChanged = true;
    }
    void removePeakSegment(int index)
    {
        segmentsPeak.erase(segmentsPeak.begin()+index);
        
        bPeakSegmentsChanged = true;
    }
    void generateSynth()
    {
        if (bPeakSegmentsChanged)
        {
            
        }
    }
    void loadPresent(int index);
    void savePresent(string name);
    void newMidiMessage(ofxMidiMessage& message)
    {
        currentMidiMessage = message;
        midiMessages.push_back(message);
        
        if (currentMidiMessage.status == MIDI_NOTE_ON)
        {
            noteOn[currentMidiMessage.pitch] = true;
            //Do trigger or create tone generator
            
        } else if (currentMidiMessage.status == MIDI_NOTE_OFF)
        {
            //Do trigger note cut off
            noteOn[currentMidiMessage.pitch] = false;
            noteCutoff[currentMidiMessage.pitch] = true;
        }
    }
private:
    int midiPort;
    ofxMidiIn midiIn;
    ofxMidiMessage currentMidiMessage;
    vector<ofxMidiMessage> midiMessages; //Midi message is removed on note off
    
    WaveType waveType;
    int sampleRate;
    bool bRepeat;
    
    
    //bool bDurationChanged;
    float durationTotal;
    float volumePeak;
    
    //bool bAttackChanged;
    float durationAttack;
    float volumeAttackStart;
    float curveAttack;
    
    //bool bDecayChanged;
    float durationDecay;
    float volumeDecayEnd;
    float curveDecay;
    
    //bool bCutoffChanged;
    float durationCutoff;
    float volumeCutoffEnd;
    float curveCutoff;
    
    bool bPeakSegmentsChanged = false;
    vector<SegmentMidi> segmentsPeak;
    
    zelmAudioMixer audioMixer;
    //vector<ToneGenerator3> toneGenerators;
    ToneGenerator3 toneGenerators[MIDI_CONTROLLER_TONE_GENERATOR_NUMBER]; //Create 10 tone generatores for maxium number of triggers
    bool noteOn[128];
    bool noteCutoff[128];
    int noteToneGenerator[128];
    //vector<bool> bToneGeneratorsInUse;
    
};
