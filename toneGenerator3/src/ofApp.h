#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

#define TWO_DIVIDE_PI 2.0/PI
#define FILE_WRITE_BUFFER_SIZE 4096
#define MIDI_NOTE_NUMBER 128
#define PANNING_TABLE_SIZE 8016
#define WAVETABLE_SIZE 65536
//#define WAVETABLE_OSCILLATED_SIZE 2621440
#define WAVETABLE_OSCILLATED_SIZE 4809
#define WAVETABLE_OSCILLATED_MAX 6
#define AUDIO_BUFFER_SIZE 4096
#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_CHANNEL_NUMBER 2

enum PanningType
{
    PanningLinear,
    PanningSquared,
    PanningSine
};
enum WaveType
{
    SineWave,
    SawtoothWave,
    TriangleWave,
    SquareWave,
    FrequencyModulation,
    AmplitudeModulation
};

struct PanningData
{
    float left[PANNING_TABLE_SIZE];
    float right[PANNING_TABLE_SIZE];
};
struct Panning
{
    float left;
    float right;
};

class zelmSynthUtil
{
public:
    static void init()
    {
        generateNoteFrequencies();
        generatePanning();
        generateSinWavetable();
    }
    static void generateNoteFrequencies()
    {
        int noteCount = 0;
        double a;
        double currentFrequency = 8.1757989156;
        for (int i = 0; i < 9; i++, noteCount++)
        {
            a = (i / 12.0);
            double frequency = (currentFrequency*pow(2.0, a));
            noteFrequencies[noteCount] = frequency;
        }
        
        currentFrequency = 13.75; //Work out from A-1, 9, because it's pitch with two or less floats
        noteFrequencies[noteCount] = currentFrequency;
        for (int o = 0; o < 10; o++)
        {
            for (int i = 1; i <= 12; i++, noteCount++)
            {
                if ((o == 9) & (i == 11))
                    break;
                a = (i / 12.0);
                double frequency = (currentFrequency*pow(2.0, a));
                noteFrequencies[noteCount] = frequency;
                if (i == 12)
                {
                    currentFrequency = frequency;
                }
            }
        }
    }
    static void generatePanning()
    {
        float mappedIndex;
        for (int i = 0; i < PANNING_TABLE_SIZE; i++)
        {
            mappedIndex = (i/(((PANNING_TABLE_SIZE-1.0)/2.0))-1.0);
            panningLinear.left[i] = (1 - mappedIndex) / 2;
            panningLinear.right[i] = (1 + mappedIndex) / 2;
            
            panningSquared.left[i] = sqrt((1 - mappedIndex) / 2);
            panningSquared.right[i] = sqrt((1 + mappedIndex) / 2);
            
            panningSine.left[i] = sin((1 - mappedIndex)/2 * (PI/2));
            panningSine.right[i] = sin((1 + mappedIndex)/2 * (PI/2));
        }
    }
    
    static Panning getPanning(float value, PanningType panningType)
    {
        //float redemicalise = float((int(value*1000))/1000.0);
        int index = ofMap(value, -1.0, 1.0, 0, 2000, true);
        Panning temp;
        switch(panningType)
        {
            case PanningLinear:
            {
                temp.left = panningLinear.left[index];
                temp.right = panningLinear.right[index];
                return temp;
            }
            case PanningSquared:
            {
                temp.left = panningSquared.left[index];
                temp.right = panningSquared.right[index];
                return temp;
            }
            case PanningSine:
            {
                temp.left = panningSine.left[index];
                temp.right = panningSine.right[index];
                return temp;
            }
        }
    }
    //static void generateSinWavetable;
    static void generateSinWavetable()
    {
        float phaseIncrement = TWO_PI / WAVETABLE_SIZE;
        float phase = 0;
        for (int i = 0; i < WAVETABLE_SIZE+1; i++)
        {
            //wavetables.push_back(*new vector<float>);
            wavetableSin[i] = sin(phase);
            phase += phaseIncrement;
        }
    }
    
    static const float getFrequencyTableIndex()
    {
        return float(WAVETABLE_SIZE) / float(AUDIO_SAMPLE_RATE);
    }
    
    static float getWavetableSinValue(int index)
    {
        if (index > WAVETABLE_SIZE || index < 0)
            return 0;
        return wavetableSin[index];
    }
    
private:
    static float noteFrequencies[MIDI_NOTE_NUMBER];
    
    static PanningData panningLinear;
    static PanningData panningSquared;
    static PanningData panningSine;
    
    //static float wavetable[2][WAVETABLE_SIZE+1];
    static float wavetableSin[WAVETABLE_SIZE+1];
    static float wavetableOscillated[MIDI_NOTE_NUMBER][WAVETABLE_OSCILLATED_MAX][WAVETABLE_OSCILLATED_SIZE];
    //static ofBuffer wavetableOscillatedBuffer[MIDI_NOTE_NUMBER][WAVETABLE_OSCILLATED_MAX];
    //static ofXml XML;
    //static vector<vector<float>> wavetables;
    
    
};
class SoundObject : public ofBaseSoundOutput
{
public:
    //SoundObject();
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
        
        if (bRecord)
            record(out);
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
        int samplerate = AUDIO_SAMPLE_RATE;
        
        file.seekp (0, ios::beg);
        file.write ("RIFF", 4);
        file.write ((char*) &myChunkSize, 4);
        file.write ("WAVE", 4);
        file.write ("fmt ", 4);
        file.write ((char*) &mySubChunk1Size, 4);
        file.write ((char*) &myFormat, 2); // should be 1 for PCM
        file.write ((char*) &channels, 2); // # channels (1 or 2)
        file.write ((char*) &samplerate, 4); // 44100
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
    
    
    
    bool bRecord = false;
    vector<float> recordData;
    ofSoundBuffer workingBuffer;
    SoundObject *inputObject;
    
private:
    
    
};

class zelmAudioMixer : public SoundObject
{
public:
    
    void addInput(SoundObject * soundObject)
    {
        mixerInputs.push_back(soundObject);
    }
    
    void audioOut(ofSoundBuffer &out)
    {
        if (mixerInputs.size() > 0)
        {
            for (int i = 0; i < mixerInputs.size(); i++)
            {
                if (mixerInputs[i] != NULL)
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
        this->panning = zelmSynthUtil::getPanning(panning, panningType);
    }
private:
    float masterVolume = 1.0;
    vector<SoundObject*> mixerInputs;
    Panning panning;
    
};

class CurveExponentGenerator
{
public:
    void generate(float duration, float curvature, bool bAscending, int sampleRate)
    {
        durationSample = duration * (float)sampleRate;
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

/*class PlayFromWavetable : public SoundObject
 {
 public:
 void setWavetable(WavetableGenerator * _wavetableGenetator)
 {
 this->wavetableGenerator = _wavetableGenetator;
 }
 void processAudio(ofSoundBuffer &in, ofSoundBuffer &out)
 {
 int numFrames = out.getNumFrames();
 int numChannels = out.getNumChannels();
 for (int i = 0; i < numFrames; i++, wavetableIndex++)
 {
 for (int c = 0; c < numChannels; c++)
 {
 out[i*numChannels +c] = wavetableGenerator->wavetable[wavetableIndex];
 }
 if (wavetableIndex == (wavetableGenerator->wavetable.size()-1))
 {
 wavetableIndex = -1;
 }
 }
 }
 
 private:
 WavetableGenerator * wavetableGenerator;
 int wavetableIndex = 0;
 };*/

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

class ToneGenerator3 : public SoundObject
{
public:
    void setup(/*float frequency,*/ WaveType waveType, int sampleRate)
    {
        this->frequency = 440.0; //Set 440.0 by as a placeholder
        this->waveType = waveType;
        this->sampleRate = sampleRate;
        
        panning.left = 1.0;
        panning.right = 1.0;
        switch(waveType)
        {
            case SineWave:
            {
                //wavetableGenerator.generate(sampleRate);
                phase = 0;
                float freTI = WAVETABLE_SIZE / (float)sampleRate;
                phaseIncrement = freTI * frequency;
                break;
            }
            case SawtoothWave:
            {
                phase = -1;
                phaseIncrement = (2 * frequency) / sampleRate;
                break;
            }
            case TriangleWave:
            {
                phase = 0;
                phaseIncrement = (TWO_PI / sampleRate) * frequency;
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
                //wavetableGenerator.generate(sampleRate);
                phase = 0;
                float freTI = WAVETABLE_SIZE / (float)sampleRate;
                phaseIncrement = freTI * frequency;
                break;
            }
            case SawtoothWave:
            {
                phase = -1;
                phaseIncrement = (2 * frequency) / sampleRate;
                break;
            }
            case TriangleWave:
            {
                phase = 0;
                phaseIncrement = (TWO_PI / sampleRate) * frequency;
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
                //wavetableGenerator.generate(sampleRate);
                phase = 0;
                float freTI = WAVETABLE_SIZE / (float)sampleRate;
                phaseIncrement = freTI * frequency;
                break;
            }
            case SawtoothWave:
            {
                phase = -1;
                phaseIncrement = (2 * frequency) / sampleRate;
                break;
            }
            case TriangleWave:
            {
                phase = 0;
                phaseIncrement = (TWO_PI / sampleRate) * frequency;
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
                                                 (volumeStart < volumeEnd) ? true : false, AUDIO_SAMPLE_RATE);
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
                                                 (volumeStart < volumeEnd) ? true : false, AUDIO_SAMPLE_RATE);
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
                                               (volumeStart < volumeEnd) ? true : false, AUDIO_SAMPLE_RATE);
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
        this->panning = zelmSynthUtil::getPanning(panning, panningType);
        
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
                         wavetableValue1 = zelmSynthUtil::getWavetableSinValue(wavetableIndex);
                         wavetableValue2 = zelmSynthUtil::getWavetableSinValue(wavetableIndex+1);
                         wavetableValue1 = wavetableValue1 + ((wavetableValue2 - wavetableValue1) * wavetableIndexFract);*/
                        //out[i*numChannels] = wavetableValue1 * volume * panning.left;
                        //out[i*numChannels+1] = wavetableValue1 * volume * panning.right;
                        //cout << panning.left << " " << panning.right << "\n";
                        out[i*numChannels] = zelmSynthUtil::getWavetableSinValue(wavetableIndex) * volume * panning.left;
                        out[i*numChannels+1] = zelmSynthUtil::getWavetableSinValue(wavetableIndex) * volume * panning.right;
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


class OscillatedWavetable : public SoundObject
{
public:
    void setup(float frequency, int numOsc)
    {
        this->frequency = frequency;
        this->numOsc = numOsc;
        panning.left = 1.0;
        panning.right = 1.0;
    }
    
    void processAudio(ofSoundBuffer &in, ofSoundBuffer &out)
    {
        int numFrames = out.getNumFrames();
        int numChannels = out.getNumChannels();
        for (int i = 0; i < numFrames; i++)
        {
            //cout << zelmSynthUtil::getOscillatedWavetableValue(zelmSynthUtil::getOscillatedWavetableIndex(frequency), 4, sampleIndex) << "\n";
            out[i*numChannels] = zelmSynthUtil::getOscillatedWavetableValue(zelmSynthUtil::getOscillatedWavetableIndex(frequency), numOsc, sampleIndex) * 1.0 * panning.left;
            out[i*numChannels+1] = zelmSynthUtil::getOscillatedWavetableValue(zelmSynthUtil::getOscillatedWavetableIndex(frequency), numOsc, sampleIndex) * 1.0 * panning.right;
            if (++sampleIndex == (WAVETABLE_OSCILLATED_SIZE))
            {
                sampleIndex = 0;
            }
        }
        if (!bGotSoundBuffer)
        {
            drawBuffer = out;
            bGotSoundBuffer = true;
        }
    }
    void draw()
    {
        //if (buffer.size() >0) {
        int x = 0;
        int y = 0;
            vector<ofMesh>meshes;
            int chans = drawBuffer.getNumChannels();
            for (int i = 0; i < chans; i++) {
                meshes.push_back(ofMesh());
                meshes.back().setMode(OF_PRIMITIVE_LINE_STRIP);
            }
            
            float h = ofGetWindowHeight() / float(chans);
            //float h2 = h * 0.5f;
            //            float factor= this->width / buffer.getNumFrames();
            
            float xInc = ofGetWindowWidth()/(float)(drawBuffer.getNumFrames() -1);
            ofVec3f v;
            v.x = x;
            for(int i=0; i<drawBuffer.getNumFrames(); i++){
                
                //v.x = ofMap(i, 0, buffer.getNumFrames() -1 , x, getMaxX());
                for (int j = 0; j < chans; j++) {
                    v.y = ofMap(drawBuffer[i*chans + j], -1, 1, h*(j+1) + y, h*j + y );
                    //     meshes[j].addColor(ofFloatColor::pink);
                    meshes[j].addVertex(v);
                    if (ofGetMouseX() == (int)v.x)
                    {
                        cout << i*chans + j << "\n";
                    }
                }
                v.x += xInc;
            }
            for (int i = 0; i < chans; i++) {
                meshes[i].draw();
            }
        //}
    }
private:
    int sampleIndex = 0;
    int numOsc;
    float frequency;
    Panning panning;
    
    ofSoundBuffer drawBuffer;
    
    bool bGotSoundBuffer;
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
        this->indexIncrement = zelmSynthUtil::getFrequencyTableIndex() * frequency;
        

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
                    parts[i].sigma = parts[i].amplitude * zelmSynthUtil::getWavetableSinValue(sigN*sigTL) / sigN;
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
                v += zelmSynthUtil::getWavetableSinValue(parts[p].index);
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
            v += zelmSynthUtil::getWavetableSinValue((int)index);
            index += indexIncrement;
            
            
            for (int c = 0; c < numChannels; c++)
            {
                out[i*numChannels +c] = v * 1.0;
            }
            //cout << zelmSynthUtil::getOscillatedWavetableValue(zelmSynthUtil::getOscillatedWavetableIndex(frequency), 4, sampleIndex) << "\n";
        }
    }
private:
    int numberPartials;
    int countPartials;
    int gibbs;
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
class FrequencyModulator : public SoundObject
{
public:
    void setup(float carrierFrequency, float modulatorFrequencyMultiplier, float volume = 1.0, float phase = 0)
    {
        this->carrierFrequency = carrierFrequency;
        this->modulatorFrequency = carrierFrequency*modulatorFrequencyMultiplier;
        this->volume = volume;
        //this->phase = phase;
        modulatorPhase = 0;
        carrierPhase = 0;
        
    }
    
    void processAudio(ofSoundBuffer &in, ofSoundBuffer &out)
    {
        float frequencyRadian = TWO_PI / in.getSampleRate();
        modulatorIncrement = frequencyRadian * modulatorFrequency;
        modulatorAmplitude = 100;
        int numFrames = out.getNumFrames();
        int numChannels = out.getNumChannels();
        for (int i = 0; i < numFrames; i++)
        {
            for (int c = 0; c < numChannels; c++)
            {
                out[i*numChannels +c] = sin(carrierPhase) * volume;
            }
            modulatorValue = modulatorAmplitude * sin(modulatorPhase);
            carrierIncrement = frequencyRadian * (carrierFrequency + modulatorValue);
            if ((carrierPhase += carrierIncrement) >= TWO_PI)
                carrierPhase -= TWO_PI;
            
            if((modulatorPhase += modulatorIncrement) >= TWO_PI)
                modulatorPhase -= TWO_PI;
        }
    }
    //float frequency;
    float modulatorFrequency;
    float volume;
private:
    float currentPhase;
    float modulatorValue;
    float modulatorIncrement;
    float modulatorPhase;
    float modulatorAmplitude;
    float carrierPhase;
    float carrierIncrement;
    float carrierFrequency;
};
class FrequencyModulatorRefactored : public SoundObject
{
public:
    void setup(float carrierFrequency, float modulatorFrequencyMultiplier, float volume = 1.0)
    {
        this->carrierFrequency = carrierFrequency;
        this->modulatorFrequency = carrierFrequency*modulatorFrequencyMultiplier;
        this->volume = volume;
        
        float frequencyRadian = (float)TWO_PI / (float)AUDIO_SAMPLE_RATE;
        modulatorIncrement = frequencyRadian * modulatorFrequency;
        modulatorAmplitude = frequencyRadian * 100;
        modulatorPhase = 0;
        
        carrierIncrement = frequencyRadian * modulatorFrequency;
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
            modulatorValue = modulatorAmplitude * sin(modulatorPhase);
            carrierPhase += carrierIncrement + modulatorValue;
            if (carrierPhase >= TWO_PI)
                carrierPhase -= TWO_PI;
            
            if((modulatorPhase += modulatorIncrement) >= TWO_PI)
                modulatorPhase -= TWO_PI;
        }
    }
    //float frequency;
    float modulatorFrequency;
    float volume;
private:
    float currentPhase;
    float modulatorValue;
    float modulatorIncrement;
    float modulatorPhase;
    float modulatorAmplitude;
    float carrierPhase;
    float carrierIncrement;
    float carrierFrequency;
};
class FrequencyModulatorRefactoredWavetable : public SoundObject
{
public:
    void setup(float carrierFrequency, float modulatorFrequencyMultiplier, float modulatorIndexAmplitude, float volume = 1.0)
    {
        this->carrierFrequency = carrierFrequency;
        this->modulatorFrequency = carrierFrequency*modulatorFrequencyMultiplier;
        this->volume = volume;
        this->indexOfModulator = modulatorIndexAmplitude;
        float freTI = (float)WAVETABLE_SIZE / (float)AUDIO_SAMPLE_RATE;
        modulatorIncrement = freTI * modulatorFrequency;
        modulatorAmplitude = freTI * indexOfModulator * modulatorFrequency;
        modulatorIndex = 0;
        
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
                out[i*numChannels +c] = zelmSynthUtil::getWavetableSinValue(carrierIndex) * volume;
            }
            modulatorValue = modulatorAmplitude * zelmSynthUtil::getWavetableSinValue(modulatorIndex);
            
            carrierIndex += carrierIncrement + modulatorValue;
            if (carrierIndex >= WAVETABLE_SIZE)
            {
                carrierIndex -= WAVETABLE_SIZE;
            } else if (carrierIndex < 0)
            {
                carrierIndex += WAVETABLE_SIZE;
            }
            
            if((modulatorIndex += modulatorIncrement) >= WAVETABLE_SIZE)
                modulatorIndex -= WAVETABLE_SIZE;
        }
    }
    //float frequency;
    float modulatorFrequency;
    float volume;
private:
    float currentPhase;
    float modulatorValue;
    float modulatorIncrement;
    float modulatorIndex;
    float modulatorAmplitude;
    float indexOfModulator;
    float carrierIndex;
    float carrierIncrement;
    float carrierFrequency;
};
class AmplitudeModulator : public SoundObject
{
public:
    void setup(float carrierFrequency, float modulatorFrequencyMultiplier, float modulatorAmplitude,
               float volume = 1.0, float phase = 0)
    {
        this->carrierFrequency = carrierFrequency;
        this->modulatorFrequency = carrierFrequency*modulatorFrequencyMultiplier;
        this->modulatorAmplitude = modulatorAmplitude;
        this->volume = volume;
        //this->phase = phase;
        modulatorPhase = 0;
        carrierPhase = 0;
        
    }
    
    void processAudio(ofSoundBuffer &in, ofSoundBuffer &out)
    {
        float frequencyRadian = TWO_PI / in.getSampleRate();
        carrierIncrement = frequencyRadian * carrierFrequency;
        modulatorIncrement = frequencyRadian * modulatorFrequency;
        modulatorScale = 1.0 / (1.0 + modulatorAmplitude);
        int numFrames = out.getNumFrames();
        int numChannels = out.getNumChannels();
        for (int i = 0; i < numFrames; i++)
        {
            modulatorValue = 1.0 + (modulatorAmplitude * sin(modulatorPhase));
            carrierValue = volume * sin(carrierPhase);
            for (int c = 0; c < numChannels; c++)
            {
                out[i*numChannels +c] = carrierValue * modulatorValue * modulatorScale;
            }
            if ((carrierPhase += carrierIncrement) >= TWO_PI)
                carrierPhase -= TWO_PI;
            
            if((modulatorPhase += modulatorIncrement) >= TWO_PI)
                modulatorPhase -= TWO_PI;
        }
    }
    //float frequency;
    float modulatorFrequency;
    float volume;
private:
    float currentPhase;
    float modulatorValue;
    float modulatorIncrement;
    float modulatorPhase;
    float modulatorAmplitude;
    float modulatorScale;
    float carrierPhase;
    float carrierIncrement;
    float carrierFrequency;
    float carrierValue;
};
//It does work, the original was written wrong
class AmplitudeModulatorWavetable : public SoundObject
{
public:
    void setup(float carrierFrequency, float modulatorFrequencyMultiplier, float modulatorAmplitude,
               float volume = 1.0)
    {
        this->carrierFrequency = carrierFrequency;
        this->modulatorFrequency = carrierFrequency*modulatorFrequencyMultiplier;
        this->modulatorAmplitude = modulatorAmplitude;
        this->volume = volume;
        //this->phase = phase;
        modulatorIndex = 0;
        carrierIndex = 0;
        float freTI = (float)WAVETABLE_SIZE / (float)AUDIO_SAMPLE_RATE;
        carrierIncrement = freTI * carrierFrequency;
        modulatorIncrement = freTI * modulatorFrequency;
        
        modulatorScale = 1.0 / (1.0 + modulatorAmplitude);
        
    }
    
    void processAudio(ofSoundBuffer &in, ofSoundBuffer &out)
    {
        int numFrames = out.getNumFrames();
        int numChannels = out.getNumChannels();
        for (int i = 0; i < numFrames; i++)
        {
            modulatorValue = 1.0 + (modulatorAmplitude * zelmSynthUtil::getWavetableSinValue(modulatorIndex));
            carrierValue = volume * zelmSynthUtil::getWavetableSinValue(carrierIndex);
            for (int c = 0; c < numChannels; c++)
            {
                out[i*numChannels +c] = carrierValue * modulatorValue * modulatorScale;
            }
            carrierIndex += carrierIncrement;
            if(carrierIndex >= WAVETABLE_SIZE)
                carrierIndex -= WAVETABLE_SIZE;
            
            modulatorIndex += modulatorIncrement;
            if(modulatorIndex >= WAVETABLE_SIZE)
                modulatorIndex -= WAVETABLE_SIZE;
        }
    }
    //float frequency;
    float modulatorFrequency;
    float volume;
private:
    float modulatorValue;
    float modulatorIncrement;
    float modulatorIndex;
    float modulatorAmplitude;
    float modulatorScale;
    float carrierIndex;
    float carrierIncrement;
    float carrierFrequency;
    float carrierValue;
};
class AmplitudeModulatorWavetableExample : public SoundObject
{
public:
    void setup(float carrierFrequency, float modulatorFrequency, float modulatorAmplitude, float volume = 1.0)
    {
        this->carrierFrequency = carrierFrequency;
        this->modulatorFrequency = modulatorFrequency;
        this->modulatorAmplitude = modulatorAmplitude;
        
        modulatorScale = 1.0 / (1.0 + modulatorAmplitude);
        
        carrierIncrement = carrierFrequency * zelmSynthUtil::getFrequencyTableIndex();
        modulatorIncrement = modulatorFrequency * zelmSynthUtil::getFrequencyTableIndex();
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
            modulatorIndex = IndexCheck(modulatorIndex);
            
            modulatorValue = 1.0 + (modulatorAmplitude * zelmSynthUtil::getWavetableSinValue(modulatorIndex));
            for (int c = 0; c < numChannels; c++)
            {
                
                out[i*numChannels +c] = zelmSynthUtil::getWavetableSinValue(carrierIndex) * modulatorValue * modulatorScale;
            }
            carrierIndex += carrierIncrement;
            modulatorIndex += modulatorIncrement;
        }
    }
    //float frequency;
    float modulatorFrequency;
    float volume;
private:
    float modulatorValue;
    float modulatorIncrement;
    float modulatorIndex;
    float modulatorAmplitude;
    float modulatorScale;
    float carrierIndex;
    float carrierIncrement;
    float carrierFrequency;
    float carrierValue;
};
class RingAmplitudeModulator : public SoundObject
{
public:
    void setup(float carrierFrequency, float modulatorFrequencyMultiplier, float modulatorAmplitude,
               float volume = 1.0, float phase = 0)
    {
        this->carrierFrequency = carrierFrequency;
        this->modulatorFrequency = carrierFrequency*modulatorFrequencyMultiplier;
        this->modulatorAmplitude = modulatorAmplitude;
        this->volume = volume;
        //this->phase = phase;
        modulatorPhase = 0;
        carrierPhase = 0;
        
    }
    
    void processAudio(ofSoundBuffer &in, ofSoundBuffer &out)
    {
        float frequencyRadian = TWO_PI / in.getSampleRate();
        carrierIncrement = frequencyRadian * modulatorValue;
        modulatorIncrement = frequencyRadian * modulatorFrequency;
        modulatorScale = 1.0 / (1.0 + modulatorAmplitude);
        int numFrames = out.getNumFrames();
        int numChannels = out.getNumChannels();
        for (int i = 0; i < numFrames; i++)
        {
            modulatorValue = modulatorAmplitude * sin(modulatorPhase);
            carrierValue = volume * sin(carrierPhase);
            for (int c = 0; c < numChannels; c++)
            {
                out[i*numChannels +c] = carrierValue * modulatorValue;
            }
            if ((carrierPhase += carrierIncrement) >= TWO_PI)
                carrierPhase -= TWO_PI;
            
            if((modulatorPhase += modulatorIncrement) >= TWO_PI)
                modulatorPhase -= TWO_PI;
        }
    }
    //float frequency;
    float modulatorFrequency;
    float volume;
private:
    float currentPhase;
    float modulatorValue;
    float modulatorIncrement;
    float modulatorPhase;
    float modulatorAmplitude;
    float modulatorScale;
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
    zelmAudioMixer audioMixer;
    
    OscillatedWavetable osciallatedWavetable;
    FrequencyModulator frequencyModulator;
    AmplitudeModulator amplitudeModulator;
    RingAmplitudeModulator ringAmplitudeModulator;
    NoiseGenerator noiseGenerator;
    AmplitudeModulatorWavetable amplitudeModulatorWavetable;
    FrequencyModulatorRefactored frequencyModulatorRefactored;
    FrequencyModulatorRefactoredWavetable frequencyModulatorRefactoredWavetable;
    OscillatedWavetableBook oscillatedWavetableBook;
    AmplitudeModulatorWavetableExample
    amplitudeModulatorWavetableExample;
        ofSoundStream soundStream;
    
};
