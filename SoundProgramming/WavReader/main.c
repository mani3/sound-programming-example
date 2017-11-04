//
//  main.c
//  WavReader
//
//  Created by Kazuya Shida on 2017/10/30.
//  Copyright © 2017 mani3. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "wavlib.h"

#define FILE_NAME_SIZE 256

int main(int argc, const char * argv[]) {
    char *fnI, *fnO;
    wav_head bufhead;
    
    if (argc != 3) {
        printf("usage: %s <input> <output>\n", argv[0]);
        printf("    input: Input sound file name\n");
        printf("    output: Output sound file name\n");
        return 0;
    }
    
    fnI = calloc(FILE_NAME_SIZE, sizeof(char));
    fnO = calloc(FILE_NAME_SIZE, sizeof(char));
    sscanf(argv[1], "%s", fnI);
    sscanf(argv[2], "%s", fnO);

    read_wav(&bufhead, fnI);
    int n_data = bufhead.Nbyte / bufhead.bl_size;
    printf("riff_size=%d\n", bufhead.riff_size);
    printf("Nch=%d\n", bufhead.Nch);
    printf("fs=%d\n", bufhead.fs);
    printf("bit=%d\n", bufhead.bit);
    printf("n_data=%d\n", n_data);
    
    write_wav(&bufhead, bufhead.Nch, bufhead.fs, bufhead.bit, n_data, fnO, 1);
    return 0;
}
