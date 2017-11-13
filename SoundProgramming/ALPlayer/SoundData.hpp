//
//  SoundData.hpp
//  ALPlayer
//
//  Created by Kazuya Shida on 2017/11/13.
//  Copyright © 2017 mani3. All rights reserved.
//

#ifndef SoundData_hpp
#define SoundData_hpp

#include <iostream>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>

class SoundData {
    ALuint sourceId;
public:
    ALuint bufferId;
    SoundData() {};
    ~SoundData();
    ALuint *buffer() { return &bufferId; };
    void play() { alSourcePlay(sourceId); };
    void playback() {
        alSourcePlay(sourceId);
        
        // main thread blocking until source finishes playback
        ALint state;
        alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
        while (state == AL_PLAYING) {
            alGetSourcei(sourceId, AL_SOURCE_STATE, &state);
        }
    };
    void stop() { alSourceStop(sourceId); };
    void setGain(float volume, bool isLoop);
};

#endif /* SoundData_hpp */
