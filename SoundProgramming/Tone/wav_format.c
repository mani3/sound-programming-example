//
//  wav_format.c
//  Tone
//
//  Created by Kazuya Shida on 2017/11/08.
//  Copyright © 2017 mani3. All rights reserved.
//


#include "wav_format.h"

void set_wav_header(wav_format *header, uint32_t fs, uint16_t channels, uint16_t n_bits, uint32_t data_bytes)
{
    for (int i = 0; i < 4; i++) header->RIFF[i] = RIFF_ID[i];
    header->riff_size = data_bytes + sizeof(wav_format) - 8;
    for (int i = 0; i < 4; i++) header->TYPE[i] = WAVE_ID[i];
    for (int i = 0; i < 4; i++) header->fmt_id[i] = FMT_ID[i];
    header->fmt_size = 16;
    header->fmt = 1;
    header->num_of_channels = channels;
    header->samples_per_sec = fs;
    header->bytes_per_sec = (uint32_t) (fs * channels * n_bits / 8);
    header->block_align = (uint16_t) (channels * n_bits / 8);
    header->bits_per_sample = n_bits;
    for (int i = 0; i < 4; i++) header->DATA[i] = DATA_ID[i];
    header->data_size = data_bytes;
}
