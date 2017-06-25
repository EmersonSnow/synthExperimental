//
//  SynthMidiDefinitions.h
//  SynthesizerRewrite
//
//  Created by Zachary Snow on 25/06/2017.
//
//

#ifndef SynthMidiDefinitions_h
#define SynthMidiDefinitions_h


#endif /* SynthMidiDefinitions_h */

class LaunchKeyController
{
    //All these are in channel 1
    int getSliderControlIndex(int n)
    {
        if (n < 9 && n > 0)
        {
            return n+40;
        } else if (n == 9)
        {
            return 7;
        }
    }
    int getButtonControlIndex(int n)
    {
        if (n < 10 && n > 0)
        {
            return 50+n;
        }
    }
    int getPotentiometerControlIndex(int n)
    {
        if (n < 9 && n > 0)
        {
            return 20+n;
        }
    }
    
    float slidersControl = {41, 42, 43, 44, 45, 46, 47, 48, 7};
    float slidersButton =  {51, 52, 53, 54, 55, 56, 57, 58, 59}
    float potentiometerControl = {21, 22, 23, 24, 25, 26, 27, 28};
    float leftTrackButtons = {103, 102};
    float rightSideButtons = {104, 105, 112, 113, 114, 115, 116, 117};
    float pitchModulation = {0, 1};
    //The 16 middle buttons are mapped to channel 10 using midi notes with velocity
    //bottom to Top, left to right
    float pitchedButtons = {40, 41, 42, 43, 48, 49, 50 51,
        36, 37, 38, 39, 44, 45, 46, 47};
};
