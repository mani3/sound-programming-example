//
//  SoundManager.hpp
//  ALPlayer
//
//  Created by Kazuya Shida on 2017/11/13.
//  Copyright © 2017 mani3. All rights reserved.
//

#ifndef SoundManager_hpp
#define SoundManager_hpp

#include <iostream>
#include <map>
#include <string>
#include <memory>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#include "SoundData.hpp"

using namespace std;

class SoundManager {
    ALenum alError;
    ALCcontext *context;
    ALCdevice *device;
    map<string, shared_ptr<SoundData>> dict;
public:
    SoundManager();
    ~SoundManager();
    void load(string key, const char *path);
    void play(string key) { dict[key]->play(); };
    void playback(string key) { dict[key]->playback(); };
    void stop(string key) { dict[key]->stop(); };
};

#endif /* SoundManager_hpp */
