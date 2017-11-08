//
//  main.c
//  Tone
//
//  Created by Kazuya Shida on 2017/11/08.
//  Copyright © 2017 mani3. All rights reserved.
//

#include <stdio.h>
#include <math.h>
#include "wav_format.h"

int main(int argc, const char * argv[]) {
    
    const char *filename = "tone.wav";
    FILE *fp;
    short sample;
    uint32_t num_samples;
    uint32_t fs = 44100;
    uint32_t num_bits = PCM16;
    uint32_t data_bytes = 0;
    float duration = 5.0;
    float frequency = 1000.0;
    double fsample;
    double pi = 4. * atan(1.);
    double start = 1.0;
    double end = 1.0e-4;
    
    double current;
    double factor;
    double angle_increment;
    wav_format *header;
    
    num_samples = (uint32_t) (duration * fs);
    data_bytes = num_samples * sizeof(uint16_t);
    angle_increment = 2. * pi * frequency / fs;
    factor = pow(end/start, 1.0/num_samples);
    
    header = (wav_format *)malloc(sizeof(wav_format));
    set_wav_header(header, fs, 1, num_bits, data_bytes);
    fp = fopen(filename, "wb");
    fwrite(header, 1, sizeof(wav_format), fp);
    
    current = start;
    for (int i = 0; i < num_samples; i++) {
        fsample = current * sin(angle_increment * i);
        current *= factor;
        sample = (short) (SHRT_MAX * fsample);
        fwrite(&sample, sizeof(short), 1, fp);
    }
    
    free(header);
    fclose(fp);
    return 0;
}
