//
//  main.c
//  Sine
//
//  Created by Kazuya Shida on 2017/11/07.
//  Copyright © 2017 mani3. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "wavlib.h"

int main(int argc, char *argv[]) {
    int n_data, n_data_id, fmt_id, fs;
    double f, amp, *buffer;
    short *ibuf;
    char *fn;
    double pi = M_PI;
    short bits = 16;
    wav_head bufhead;
    
    if (argc != 6) {
        printf("Usage: sine n_data amp, f, fs, filename\n");
        printf("  n_data  : the number of data\n");
        printf("  amp      : amplitude\n");
        printf("  f        : signal frequency in Hz\n");
        printf("  fs       : sampling frequency in Hz\n");
        printf("  filename : file name of sound\n");
        printf(" note: if n_data is given in negative, output file is 'raw' format. Otherwise 'wav' format.\n");
        printf(" e.g. ./Sine 40000 10000 2000 44100 sine.wav");
        return 0;
    }

    n_data_id = atoi(argv[1]);
    n_data = abs(n_data_id);
    fmt_id = n_data/n_data_id;
    buffer = calloc(n_data, sizeof(double));
    ibuf = bufhead.data = calloc(n_data, sizeof(short));
    fn = calloc(100, sizeof(char));

    amp = atof(argv[2]);
    f = atof(argv[3]);
    fs = atoi(argv[4]);
    sscanf(argv[5], "%s", fn);

    for (int i = 0; i < n_data; i++) {
        buffer[i] = amp * sin(2. * pi * f * (double)i / (double)fs);
        ibuf[i] = (short)buffer[i];
        printf("%d: buffer=%f\n", i, buffer[i]);
    }
    write_wav(&bufhead, 1, fs, bits, n_data, fn, fmt_id);
    
    free(buffer);
    free(bufhead.data);
    free(fn);
    return 0;
}
