//
//  wav_format.h
//  Tone
//
//  Created by Kazuya Shida on 2017/11/08.
//  Copyright © 2017 mani3. All rights reserved.
//

#ifndef wav_format_h
#define wav_format_h

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define PCM16 (16)

typedef struct wav_format_pcm {
    uint8_t RIFF[4];          // 'RIFF'
    uint32_t riff_size;       // File size - 8 bytes
    uint8_t TYPE[4];          // 'WAVE'
    uint8_t fmt_id[4];        // 'fmt '
    uint32_t fmt_size;        // LPCM: 16 bytes
    uint16_t fmt;             // Audio format 1=PCM, 6=mulaw, 7=alaw
    uint16_t num_of_channels; // Number of channels 1=Mono 2=Sterio
    uint32_t samples_per_sec; // Sampling Frequency (Hz)
    uint32_t bytes_per_sec;   // block_align * fs (bytes/sec)
    uint16_t block_align;     // Frame size (2=16-bit mono, 4=16-bit stereo)
    uint16_t bits_per_sample; // (16, 24, 32)
    uint8_t DATA[4];          // 'data'
    uint32_t data_size;       // bytes
} wav_format;

static uint8_t RIFF_ID[4] = {'R', 'I', 'F', 'F'};
static uint8_t WAVE_ID[4] = {'W', 'A', 'V', 'E'};
static uint8_t FMT_ID[4] = {'f', 'm', 't', ' '};
static uint8_t DATA_ID[4] = {'d', 'a', 't', 'a'};

extern void set_wav_header(wav_format *header, uint32_t fs, uint16_t channels, uint16_t n_bits, uint32_t data_bytes);

#endif /* wav_format_h */
