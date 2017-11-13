//
//  SoundManager.cpp
//  ALPlayer
//
//  Created by Kazuya Shida on 2017/11/13.
//  Copyright © 2017 mani3. All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>
#include "SoundManager.hpp"

extern "C" {
    #include "MyOpenALSupport.h"
}

SoundManager::SoundManager() {
    dict = map<string, SoundData>();
    device = alcOpenDevice(NULL);
    if (device != NULL) {
        context = alcCreateContext(device, 0);
        if (context != NULL) {
            alcMakeContextCurrent(context);
        }
    }
    if((alError = alGetError()) != AL_NO_ERROR) {
        printf("ERROR - : OpenAL reported error '%x'", alError);
    }
}

SoundManager::~SoundManager() {
    dict.clear();
    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

void SoundManager::load(string key, const char *path) {
    SoundData *sound = new SoundData();
    alGenBuffers(1, sound->buffer());
    printf("alGenBuffers: %x\n", alGetError());
    
    ALenum  format;
    ALsizei size;
    ALsizei freq;
    ALvoid *data;
    alError = AL_NO_ERROR;
    
    CFURLRef audioFileUrl = CFURLCreateFromFileSystemRepresentation(NULL, (const UInt8 *)path, strlen(path), false);
    
    if (audioFileUrl) {
        data = MyGetOpenALAudioData(audioFileUrl, &size, &format, &freq);
        CFRelease(audioFileUrl);
        
        if((alError = alGetError()) != AL_NO_ERROR) {
            printf("ERROR - SoundManager: Error loading sound: %s with error %x\n", path, alError);
        }
        alBufferData(sound->bufferId, format, data, size, freq);
        printf("buffer data: %s with status: %x\n", path, alGetError());
        if (data) free(data);
    } else {
        printf("ERROR - SoundManager: Could not find file '%s'", path);
    }
    sound->setGain(1.0f, false);
    dict[key] = *sound;
}

