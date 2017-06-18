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
    SquareWave
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
        generateWavetable();
        //ofDirectory directory(ofToDataPath("oscillated_wavetable"));
        //if (directory.exists())
        //{
        //    loadOscillatedWavetable();
        //} else
        //{
        //   generateOscillatedFrequencies();
        //}
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
    
    static void generateWavetable()
    {
        float phaseIncrement = TWO_PI / WAVETABLE_SIZE;
        float phase = 0;
        for (int i = 0; i < WAVETABLE_SIZE+1; i++)
        {
            //wavetables.push_back(*new vector<float>);
            wavetable[i] = sin(phase);
            phase += phaseIncrement;
        }
    }
    
    static float getWavetableValue(int index)
    {
        return wavetable[index];
    }
    
    static void generateOscillatedFrequencies()
    {
        int numSummed = 2;
        float increment;
        
        ofFile file;
        string filePath;
        for (int i = 0; i < MIDI_NOTE_NUMBER; i ++)
        {
            for (int o = 0; o < WAVETABLE_OSCILLATED_MAX; o++)
            {
                filePath = "oscillated_wavetable/"+ofToString(i)+"/"+ofToString(o)+".txt";
                if (file.doesFileExist(filePath))
                    file.removeFile(filePath);
                
                file.open(ofToDataPath(filePath), ofFile::WriteOnly);
                file.create();
                //float phase[numSummed];
                //float phaseInit[numSummed];
                //float phaseIncrements[numSummed];
                
                int numPart = 0;
                float partMult[numSummed];
                float partAmp[numSummed];
                float phaseInit[numSummed];
                float indexIncrement[numSummed];
                float index[numSummed];
                increment = (WAVETABLE_SIZE / AUDIO_SAMPLE_RATE)/*WAVETABLE_OSCILLATED_SIZE*/ * noteFrequencies[i];
                partMult[0] = 1.0;
                partAmp[0] = 1.0;
                
                for (int n = 1; n < numSummed; n++)
                {
                    partMult[n] = (2*n) -1;
                    partAmp[n] = 1 / partMult[n];
                }
                for (int p = 0; p < numSummed; p++)
                {
                    phaseInit[p] = 0;
                }
                for (int p = 0; p < numSummed; p++)
                {
                    indexIncrement[numPart] = increment * partMult[p];
                    index[numPart] = (phaseInit[p]/TWO_PI)*WAVETABLE_SIZE;
                    if (indexIncrement[numPart] < (WAVETABLE_SIZE/2))
                        numPart++;
                }
                cout << numPart << " numPart " << i << " midiNote " << o << " Oscillator number\n";
                float value = 0;
                for (int n = 1; n < WAVETABLE_OSCILLATED_SIZE+1; n++)
                {
                    value = 0; //Ignore first value because it always 1
                    for (int p = 0; p < numPart; p++)
                    {
                        value += wavetable[(int)index[p]] * partAmp[p];
                        index[p] += indexIncrement[p];
                        if (index[p] >= WAVETABLE_SIZE)
                            index[p] -= WAVETABLE_SIZE;
                    }
                    wavetableOscillated[i][o][n-1] = value;
                    file << n << " " << value << "\n"; //TEMP
                    //bufferStream << value << "\n";
                    //wavetableOscillatedBuffer[i][o].writeTo(bufferStream);
                    //bufferStream.clear();
                }
                file.close();
                filePath.clear();
            }
            cout << "Midi note finished " << i << "\n";
            numSummed++;
        }
        //saveOscillatedWavetable();
    }
    static void loadOscillatedWavetable()
    {
        ofBuffer buffer;
        for (int i = 0; i < MIDI_NOTE_NUMBER; i ++)
        {
            for (int o = 0; o < WAVETABLE_OSCILLATED_MAX; o++)
            {
                buffer = ofBufferFromFile(ofToDataPath("oscillated_wavetable/"+ofToString(i)+"/"+ofToString(o)+".txt"));
                ofBuffer::Lines lines = buffer.getLines();
                ofBuffer::Line iter = lines.begin();
                
                while (iter != lines.end())
                {
                    cout << "LINE: " << (*iter) << endl;
                    iter++;
                }
                
                buffer.clear();
            }
        }
    }
    /*static void saveOscillatedWavetable()
    {
        //Save in seperate files in case it easier to load them on run time, which I doubt because it a large number of floats
        XML.clear();
        for (int i = 0; i < MIDI_NOTE_NUMBER; i++)
        {
            for (int o = 0; o < WAVETABLE_OSCILLATED_MAX; o++)
            {
                XML.addChild("OSCILLATOR_"+ofToString(o));
                XML.setTo("OSCILLATOR_"+ofToString(o));
                for (int s = 0; s < WAVETABLE_OSCILLATED_SIZE; s++)
                {
                    XML.addValue(ofToString(s), wavetableOscillated[i][o][s]);
                }
                XML.setTo("../");
            }
            XML.save("Oscillator_note_"+ofToString(i));
            XML.clear();
        }
    }*/
    /*static void saveOscillatedWavetable()
    {
        //Save in seperate files in case it easier to load them on run time, which I doubt because it a large number of floats
        //ofFile file;
        for (int i = 0; i < MIDI_NOTE_NUMBER; i++)
        {
            for (int o = 0; o < WAVETABLE_OSCILLATED_MAX; o++)
            {
                string path = ofToDataPath("wavetable_oscillated/"+ofToString(i)+"/"+ofToString(o)+".txt");
                ofFile file(path, ofFile::ReadWrite, false);
                if (file.exists())
                {
                    file.removeFile(path);
                    file.open(path, ofFile::ReadWrite, false);
                    file.create();
                }
                //fstream file(path.c_str(), ios::out | ios::binary);
                //file.seekp(0, ios::beg);
                for (int s = 0; s < WAVETABLE_OSCILLATED_SIZE; s++)
                {
                    
                    //file.write(wavetableOscillated[i][o][s],
                }
                //XML.setTo("../");
            }
            //XML.save("Oscillator_note_"+ofToString(i));
            //XML.clear();
        }
    }*/
    static int getOscillatedWavetableIndex(float frequency)
    {
        for (int i = 0; i < MIDI_NOTE_NUMBER; i++)
        {
            if (noteFrequencies[i] == frequency)
            {
                //cout << "MIDI NOTE " << i << "\n";
                return i;
            }
        }
    }
    static float getOscillatedWavetableValue(int midiNote, int numOscillated, int sampleIndex)
    {
        //TODO: Checks
        return wavetableOscillated[midiNote][numOscillated][sampleIndex];
    }
    /*static void generateSummedWavetables()
     {
     int numSummed = 2;
     float partMult[numSummed];
     float partAmp[numSummed];
     float phase[numSummed];
     float phaseInit[numSummed];
     float phaseIncrements[numSummed];
     
     
     
     }*/
    
private:
    static float noteFrequencies[MIDI_NOTE_NUMBER];
    
    static PanningData panningLinear;
    static PanningData panningSquared;
    static PanningData panningSine;
    
    //static float wavetable[2][WAVETABLE_SIZE+1];
    static float wavetable[WAVETABLE_SIZE+1];
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
                         wavetableValue1 = zelmSynthUtil::getWavetableValue(wavetableIndex);
                         wavetableValue2 = zelmSynthUtil::getWavetableValue(wavetableIndex+1);
                         wavetableValue1 = wavetableValue1 + ((wavetableValue2 - wavetableValue1) * wavetableIndexFract);*/
                        //out[i*numChannels] = wavetableValue1 * volume * panning.left;
                        //out[i*numChannels+1] = wavetableValue1 * volume * panning.right;
                        //cout << panning.left << " " << panning.right << "\n";
                        out[i*numChannels] = zelmSynthUtil::getWavetableValue(wavetableIndex) * volume * panning.left;
                        out[i*numChannels+1] = zelmSynthUtil::getWavetableValue(wavetableIndex) * volume * panning.right;
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

class FrequencyModulation : public SoundObject
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
class AmplitudeModulation : public SoundObject
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
class RingAmplitudeModulation : public SoundObject
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
    FrequencyModulation frequencyModulation;
    AmplitudeModulation amplitudeModulation;
    RingAmplitudeModulation ringAmplitudeModulation;
    NoiseGenerator noiseGenerator;
        ofSoundStream soundStream;
    
};
