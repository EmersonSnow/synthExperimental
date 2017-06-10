#include "ofApp.h"

float zelmSynthUtil::noteFrequencies[MIDI_NOTE_NUMBER];
PanningData zelmSynthUtil::panningLinear;
PanningData zelmSynthUtil::panningSquared;
PanningData zelmSynthUtil::panningSine;
float zelmSynthUtil::wavetable[WAVETABLE_SIZE+1];
float zelmSynthUtil::wavetableOscillated[MIDI_NOTE_NUMBER][WAVETABLE_OSCILLATED_MAX][WAVETABLE_OSCILLATED_SIZE];

//--------------------------------------------------------------
void ofApp::setup(){
    zelmSynthUtil::init();
    soundStream.setup(AUDIO_CHANNEL_NUMBER, 0, AUDIO_SAMPLE_RATE, AUDIO_BUFFER_SIZE, 1);
    
    /*storeUnalteredSoundInTable.setup(392.00, SineWave, 44100);
    storeUnalteredSoundInTable.addSegment(2.0, 0.0, 1.0, 0.2);
    storeUnalteredSoundInTable.addSegment(2.0, 1.0, 0.0, 0.2);
    storeUnalteredSoundInTable.start();
    
    storeUnalteredSoundInTable2.setup(440.0, SineWave, 44100);
    storeUnalteredSoundInTable2.addSegment(1.0, 0.0, 1.0, 0.2);
    storeUnalteredSoundInTable2.addSegment(3.0, 1.0, 0.0, 0.2);
    storeUnalteredSoundInTable2.start();
    
    storeUnalteredSoundInTable3.setup(660.0, SineWave, 44100);
    storeUnalteredSoundInTable3.addSegment(1.5, 0.0, 1.0, 0.2);
    storeUnalteredSoundInTable3.addSegment(3.0, 1.0, 0.0, 0.2);
    storeUnalteredSoundInTable3.start();
    
    storeUnalteredSoundInTable4.setup(739.99, SineWave, 44100);
    storeUnalteredSoundInTable4.addSegment(1.0, 0.0, 1.0, 0.2);
    storeUnalteredSoundInTable4.addSegment(3.0, 1.0, 0.0, 0.2);
    storeUnalteredSoundInTable4.start();
    
    storeUnalteredSoundInTable5.setup(880.00, SineWave, 44100);
    storeUnalteredSoundInTable5.addSegment(0.5, 0.0, 1.0, 0.2);
    storeUnalteredSoundInTable5.addSegment(3.5, 1.0, 0.0, 0.2);
    storeUnalteredSoundInTable5.start();
    
    audioMixer.addInput(&storeUnalteredSoundInTable);
    audioMixer.addInput(&storeUnalteredSoundInTable2);
    audioMixer.addInput(&storeUnalteredSoundInTable3);
    audioMixer.addInput(&storeUnalteredSoundInTable4);
    audioMixer.addInput(&storeUnalteredSoundInTable5);*/
    
    //audioMixer.startRecord();
    
    osciallatedWavetable.setup(440.0, 1);
    //audioMixer.addInput(&osciallatedWavetable);
    //audioMixer.setMasterVolume(1.0);
    //audioMixer.setPanning(0.0, PanningLinear);
    
    soundStream.setOutput(osciallatedWavetable);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    audioMixer.saveRecording("sound1.wav");
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
