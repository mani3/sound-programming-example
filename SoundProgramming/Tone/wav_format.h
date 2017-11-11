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
#define PCM24 (24)

#define FORMAT_CHUNK_PCM_SIZE (16)
#define FRAMES_PER_BLOCK (64)

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

typedef struct wav_file_descriptor {
    FILE *fp;            // file
    uint64_t file_size;  // File size
    uint64_t data_start; // Start position
    uint64_t data_end;   // End position
} wav_file_desc;

extern void set_wav_header(wav_format *header, uint32_t fs, uint16_t channels, uint16_t n_bits, uint32_t data_bytes);
extern int read_wav_header(wav_format *fmtdesc, wav_file_desc *filedesc);
extern uint64_t read_sound(FILE *fp, void *sample_block, uint64_t n_samples, wav_format *fmt);

#endif /* wav_format_h */
