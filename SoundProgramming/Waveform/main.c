//
//  main.c
//  Waveform
//
//  Created by Kazuya Shida on 2017/11/14.
//  Copyright © 2017 mani3. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "wavlib.h"

typedef enum function {
    sine,
    white_noise,
    triangular,
    sawtooth,
    rectangle,
    pulse,
    rm,
    am,
    fm_,
} function;

int main(int argc, const char * argv[]) {
    
    double cons, cons1, cons2, xdash;
    
    short bits = 16;
    double pi = M_PI;
    wav_head bufhead;
    
    if (argc != 9) {
        printf("Usage: %s <func#> <n_data> <amp> <f> <fs> <fm> <I> <filename>\n", argv[0]);
        printf("  func#: 0: sine, 1: white noise, 2: triangular, 3: sawtooth, 4: rectangle, 5: pulse, 6: rm, 7: am, 8: fm\n");
        printf("  n_data  : the number of data\n");
        printf("  amp     : amplitude\n");
        printf("  f       : signal frequency in Hz\n");
        printf("  fs      : sampling frequency in Hz\n");
        printf("  fm      : signal frequency in Hz\n");
        printf("  I       : sampling frequency in Hz\n");
        printf("  filename: file name of sound\n");
        printf(" e.g. ./%s 0 40000 10000 2000 44100 0 0 sine.wav\n", argv[0]);
        return 1;
    }
    
    int n_data_id = atoi(argv[2]);
    int n_data = abs(n_data_id);
    int fmt_id = n_data/n_data_id;
    
    double *buffer = calloc(n_data, sizeof(double));
    bufhead.data = calloc(n_data, sizeof(short));
    char *fn = calloc(100, sizeof(char));
    
    int func_n = atoi(argv[1]);
    function func = (function) func_n;
    double amp = atof(argv[3]);
    double f = atof(argv[4]);
    double fs = atof(argv[5]);
    double fm = atof(argv[6]);
    double I = atof(argv[7]);
    double fc = f;
    sscanf(argv[8], "%s", fn);
    cons = fs / f;
    
    switch (func) {
        case sine:
            for (int i = 0; i < n_data; i++) {
                buffer[i] = amp * sin(2. * pi * f * (double)i / fs);
                bufhead.data[i] = (short)buffer[i];
            }
            break;
        case white_noise:
            srand((unsigned)time(NULL));
            for (int i = 0; i < n_data; i++) {
                buffer[i] = -amp + rand() * (2. * amp) / (RAND_MAX + 1.) ;
                bufhead.data[i] = (short)buffer[i];
            }
            break;
        case triangular:
            cons1 = cons / 2;
            cons2 = 4 * amp * f / fs;
            for (int i = 0; i < n_data; i++) {
                xdash = fmod((double)i, cons);
                buffer[i] = xdash < cons1 ? cons2 * xdash - amp : -cons2 * xdash + 3. * amp;
                bufhead.data[i] = (short)buffer[i];
            }
            break;
        case sawtooth:
            cons2 = 2 * amp * f / fs;
            for (int i = 0; i < n_data; i++) {
                xdash = fmod((double)i, cons);
                buffer[i] = cons2 * xdash - amp;
                bufhead.data[i] = (short)buffer[i];
            }
            break;
        case pulse:
            cons1 = 1.;
            for (int i = 0; i < n_data; i++) {
                xdash = fmod((double)i, cons);
                buffer[i] = xdash < cons1 ? amp : 0.;
                bufhead.data[i] = (short)buffer[i];
            }
            break;
        case rm:
            for (int i = 0; i < n_data; i++) {
                buffer[i] = amp * sin(2 * pi * fm * (double)i / fs) * sin(2. * pi * fc * (double)i / fs);
                bufhead.data[i] = (short)buffer[i];
            }
            break;
        case am:
            for (int i = 0; i < n_data; i++) {
                buffer[i] = amp * (0.5 * 0.5 * sin(2 * pi * fm * (double)i / fs)) * sin(2. * pi * fc * (double)i / fs);
                bufhead.data[i] = (short)buffer[i];
            }
            break;
        case fm_:
            for (int i = 0; i < n_data; i++) {
                buffer[i] = amp * sin(2. * pi * fc * (double)i / fs + I * sin(2 * pi * fm * (double)i / fs));
                bufhead.data[i] = (short)buffer[i];
            }
            break;
        default:
            break;
    }
    write_wav(&bufhead, 1, fs, bits, n_data, fn, fmt_id);
    
    free(buffer);
    free(bufhead.data);
    free(fn);
    return 0;
}
