//
//  main.c
//  WavePlayer
//
//  Created by Kazuya Shida on 2017/11/09.
//  Copyright © 2017 mani3. All rights reserved.
//

#include <stdio.h>
#include "portaudio.h"

#include "wav_format.h"

int main(int argc, const char * argv[]) {
    
    PaStreamParameters outputParamaters;
    PaStream *stream;
    PaError error;
    
    const char *filename = NULL;
    uint64_t n_samples, num_play_samples = 0;
    uint32_t sample_per_block, bytes_pre_sample, end = 0;
    uint64_t cur_data_pos;
    float *sample_block;
    wav_format *fmt = (wav_format *)malloc(sizeof(wav_format));
    wav_file_desc *fd = (wav_file_desc *)malloc(sizeof(wav_file_desc));
    
    if (argc != 2) {
        fprintf(stderr, "Usage: ./WavePlay <filename>\n");
        return 1;
    }
    
    filename = argv[1];
    FILE *fp = fopen(filename, "rb");
    fd->fp = fp;
    if (read_wav_header(fmt, fd)) {
        return 1;
    }
    if (fmt->bits_per_sample > 24) {
        fprintf(stderr, "Unsupport a quantum bit: %d bits", fmt->bits_per_sample);
        return 1;
    }
    
    error = Pa_Initialize();
    if (error != paNoError) {
        goto error;
    }
    outputParamaters.device = Pa_GetDefaultOutputDevice();
    if (outputParamaters.device == paNoError) {
        fprintf(stderr, "Not found default output device\n");
    }
    outputParamaters.channelCount = fmt->num_of_channels;
    outputParamaters.sampleFormat = paFloat32;
    outputParamaters.suggestedLatency = Pa_GetDeviceInfo(outputParamaters.device)->defaultHighOutputLatency;
    outputParamaters.hostApiSpecificStreamInfo = NULL;
    
    error = Pa_OpenStream(&stream,
                        NULL,
                        &outputParamaters,
                        fmt->samples_per_sec,
                        FRAMES_PER_BLOCK,
                        paNoFlag,
                        NULL,
                        NULL);
    if (error != paNoError) { goto error; }
    error = Pa_StartStream(stream);
    if (error != paNoError) { goto error; }
    
    sample_per_block = FRAMES_PER_BLOCK * fmt->num_of_channels;
    sample_block = (float *)malloc(sizeof(float) * sample_per_block);
    bytes_pre_sample = fmt->bits_per_sample / 8;
    n_samples = sample_per_block;
    fseek(fp, fd->data_start, SEEK_SET);
    
    printf("Play start\n");
    printf("File name: %s\n", filename);
    printf("Number of channel: %d\n", fmt->num_of_channels);
    printf("fs: %d sample/sec\n", fmt->samples_per_sec);
    printf("Quantum bit: %d bit\n", fmt->bits_per_sample);
    
    for (;;) {
        num_play_samples += read_sound(fp, sample_block, n_samples, fmt);
        error = Pa_WriteStream(stream, sample_block, FRAMES_PER_BLOCK);
        if (error != paNoError) { goto error; }
        if (end == 1) { break; }
        cur_data_pos = ftell(fp);
        if ((fd->data_end - cur_data_pos) <= sample_per_block * bytes_pre_sample) {
            n_samples = (fd->data_end - cur_data_pos) / bytes_pre_sample;
            end = 1;
        }
        continue;
    }
    printf("  Total: %llu \n", num_play_samples);
    
    error = Pa_StopStream(stream);
    if (error != paNoError) { goto error; }
    error = Pa_CloseStream(stream);
    if (error != paNoError) { goto error; }
    Pa_Terminate();
    
    free(fmt);
    free(fd);
    free(sample_block);
    fclose(fp);
    return error;
    
error:
    Pa_Terminate();
    fprintf(stderr, "Error PortAudio\n");
    fprintf(stderr, "%s\n", Pa_GetErrorText(error));
    return error;
}
