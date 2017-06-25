//
//  SynthClassBase.h
//  SynthesizerRewrite
//
//  Created by Zachary Snow on 25/06/2017.
//
//

#pragma once


class SynthClassBase
{
public:
    SynthClassBase()
    {
        bInUse = false;
    }
    inline void setInUse(bool b)
    {
        bInUse = b;
    }
    inline bool getInUse()
    {
        return bInUse;
    }
    virtual void reset()
    {
        bInUse = false;
    }
    virtual float generate()
    {
        return 0.0;
    }
protected:
    bool bInUse;
};
