//
//  SoundData.cpp
//  ALPlayer
//
//  Created by Kazuya Shida on 2017/11/13.
//  Copyright © 2017 mani3. All rights reserved.
//

#include "SoundData.hpp"

SoundData::~SoundData() {
    printf("dispose sound\n");
    if (sourceId) {
        alSourceStop(sourceId);
        alDeleteSources(1, &sourceId);
        sourceId = 0;
    }
    if (bufferId) {
        alDeleteBuffers(1, &bufferId);
        bufferId = 0;
    }
}

void SoundData::setGain(float volume, bool isLoop) {
    alGenSources(1, &sourceId);
    
    alSourcei(sourceId, AL_BUFFER, 0);
    alSourcei(sourceId, AL_BUFFER, bufferId);
    
    // gain
    alSourcef(sourceId, AL_GAIN, volume);
    
    if (isLoop) {
        alSourcei(sourceId, AL_LOOPING, AL_TRUE);
    } else {
        alSourcei(sourceId, AL_LOOPING, AL_FALSE);
    }
    // printf("set gain status: %x\n", alGetError());
}
