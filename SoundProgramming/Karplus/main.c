//
//  main.c
//  Karplus
//
//  Created by Kazuya Shida on 2017/11/17.
//  Copyright © 2017 mani3. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "wavlib.h"

int main(int argc, const char * argv[])
{
    if (argc != 6) {
        printf("Usage: %s <n_data> <amp> <f> <fs> <filename>\n", argv[0]);
        printf("  n_data  : the number of data\n");
        printf("  amp     : amplitude\n");
        printf("  f       : signal frequency in Hz\n");
        printf("  fs      : sampling frequency in Hz\n");
        printf("  filename: file name of sound\n");
        printf(" e.g. %s 40000 10000 261.626 44100 karplus.wav\n", argv[0]);
        return 1;
    }
    
    int bits = 16;
    wav_head bufhead;
    double taper_ms = 0.15;
    int n_data_id = atoi(argv[1]);
    int n_data = abs(n_data_id);
    int fmt_id = n_data / n_data_id;
    
    double *buffer = calloc(n_data, sizeof(double));
    bufhead.data = calloc(n_data, sizeof(short));
    char *fn = calloc(100, sizeof(char));
    
    double amp = atof(argv[2]);
    double f = atof(argv[3]);
    double fs = atof(argv[4]);
    sscanf(argv[5], "%s", fn);
    
    int wavt_len = (int) (fs / f);
    int taper_len = taper_ms * fs;
    
    srand((unsigned)time(NULL));
    
    printf("irandmax=%d, amp=%f, wavt_len=%d, taper_len=%d\n", RAND_MAX, amp, wavt_len, taper_len);
    
    // geratation of white noise
    for (int i = 0; i <= wavt_len; i++) {
        buffer[i] = -amp + rand() * (2 * amp) / (RAND_MAX + 1);
        bufhead.data[i] = (short) buffer[i];
    }
    
    for (int i = wavt_len + 1; i < n_data; i++) {
        buffer[i] = 0.5 * (buffer[i - wavt_len] + buffer[i - wavt_len - 1]);
        bufhead.data[i] = (short) buffer[i];
    }
    
    for (int i = n_data - taper_len; i < n_data; i++) {
        bufhead.data[i] = (short) buffer[i] *
            ((double)(i - n_data + taper_len) * (-1 / (double)taper_len) + 1.0);
    }
    
    write_wav(&bufhead, 1, fs, bits, n_data, fn, fmt_id);
    
    free(buffer);
    free(bufhead.data);
    free(fn);
    return 0;
}
