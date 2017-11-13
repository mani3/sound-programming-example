//
//  main.cpp
//  ALPlayer
//
//  Created by Kazuya Shida on 2017/11/13.
//  Copyright © 2017 mani3. All rights reserved.
//

#include <iostream>
#include <unistd.h>
#include "SoundManager.hpp"

int main(int argc, const char * argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <filename>\n";
        exit(1);
    }
    
    SoundManager manager = SoundManager();
    manager.load("sound", argv[1]);
    manager.playback("sound");
    return 0;
}
