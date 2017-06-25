#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    SynthUtil::init();
    /*synthController.setWaveType(Oscillator);
    waveManager = synthController.getFreeWaveManager();
    
    waveManager->getGenerator();
    waveManager->addSegment(2.0, 0.0, 1.0, 0.2);
    waveManager->addSegment(2.0, 1.0, 0.0, 0.2);
    waveManager->start();
    
    waveManager = synthController.getFreeWaveManager();*/
    /*synthController.createWaveManagers();
    waveManager = synthController.getFreeWaveManager();
    waveManager->setup(synthController.getWaveType(), SynthUtil::getPanning(0.5, PanningLinear));
    waveManager->setInUse(true);
    waveManager->getGenerator();
    waveManager->addSegment(2.0, 0.0, 1.0, 0.2);
    waveManager->addSegment(2.0, 1.0, 0.0, 0.2);
    waveManager->start();*/
    
    //soundStream.setOutput(audioMixer);
    //synthSettings.setWaveType(FrequencyModulation);
    synthPresetManager.currentPresetInstance.waveType = Oscillator;
    int i = synthWaveManager.getWaveInstance();
    //synthWaveManager.getWaveInstanceGenerator(i);
    synthWaveManager.startWaveInstance(i);
    //WaveInstanceStruct * wave = synthWaveManager.getWaveInstance(440.0);;
    //synthWaveManager.startWaveInstance(*wave);
    //synthWaveManager.playDoAllWaveInstance(440.0);
    
    /*synthPresetManager.setWaveType(Oscillator);
    WaveInstance wave(440.0);
    wave.start();
    audioMixer.addInput(&wave);
    soundStream.setup(AUDIO_CHANNEL_NUMBER, 0, AUDIO_SAMPLE_RATE, AUDIO_BUFFER_SIZE, 1);
    soundStream.setOutput(audioMixer);*/
}

//--------------------------------------------------------------
void ofApp::update(){
    synthWaveManager.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    //cout << synthController.getAudioMixer()->mixerInputs.size() << "\n";
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    //audioMixer.saveRecording("sound1.wav");
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
