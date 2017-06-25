#pragma once

#include "ofMain.h"
#include "SynthWaveManager.hpp"

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

class AudioMixer : public ofBaseSoundOutput
{
public:
    AudioMixer()
    {
        masterVolume = 1.0;
        panning.left = 1.0;
        panning.right = 1.0;
        
    }
    void addInput(WaveInstance * soundObject)
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
                {;
                    
                    int left = 0;
                    for (int right = 1; right < out.size(); right+=2, left+=2)
                    {
                        float v = mixerInputs[i]->generate();
                        out.getBuffer()[left] += v * (1.0/mixerInputs.size()) * panning.left;
                        out.getBuffer()[right] += v * (1.0/mixerInputs.size()) * panning.right;
                        /*if ((out.getBuffer()[b] > 1.0) || (out.getBuffer()[b] < -1.0))
                         {
                         cout << "Audio not noramlised " << out.getBuffer()[b] << "\n";
                         }*/
                    }
                }
            }
        }
        out *= masterVolume;
    }
    
    void setMasterVolume(float masterVolume)
    {
        this->masterVolume = masterVolume;
    }
    
    void setPanning(float panning, PanningType panningType)
    {
        this->panning = SynthUtil::getPanning(panning, panningType);
    }
    
private:
    float masterVolume;
    vector<WaveInstance*> mixerInputs;
    Panning panning;
    
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
		
    //WaveManager * waveManager;
    SynthWaveManager synthWaveManager;
    AudioMixer audioMixer;
    ofSoundStream soundStream;
};



