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

int read_wav_header(wav_format *fmtdesc, wav_file_desc *filedesc)
{
    uint8_t chunk_id[4];
    uint32_t chunk_size;
    
    uint16_t f_sub = 0, d_sub = 0;
    uint64_t file_pos;
    wav_format *format = fmtdesc;
    wav_file_desc *fdesc = filedesc;
    
    rewind(fdesc->fp);
    
    fread(chunk_id, 1, 4, fdesc->fp);
    fread(&chunk_size, sizeof(uint8_t), sizeof(uint32_t), fdesc->fp);
    if (*(uint32_t *)chunk_id != *(uint32_t *)RIFF_ID) {
        fprintf(stderr, "This file is not RIFF type\n");
        return 1;
    }
    fdesc->file_size = chunk_size + 8; // bytes
    
    fread(chunk_id, 1, 4, fdesc->fp);
    if (*(uint32_t *)chunk_id != *(uint32_t *)WAVE_ID) {
        fprintf(stderr, "This file is not WAVE type\n");
        return 1;
    }
    
    do {
        fread(chunk_id, 1, 4, fdesc->fp);
        fread(&chunk_size, sizeof(uint8_t), sizeof(uint32_t), fdesc->fp);
        printf("%d, %d, %d\n", *(uint32_t *)chunk_id, *(uint32_t *)FMT_ID, chunk_size);
        
        if (*(uint32_t *)chunk_id == *(uint32_t *)FMT_ID) {
            if (chunk_size != FORMAT_CHUNK_PCM_SIZE) {
                fprintf(stderr, "Chunk size: %d, it is not PCM\n", chunk_size);
                return 1;
            }
            fread(&format->fmt, sizeof(uint8_t), sizeof(uint16_t), fdesc->fp);
            fread(&format->num_of_channels, sizeof(uint8_t), sizeof(uint16_t), fdesc->fp);
            fread(&format->samples_per_sec, sizeof(uint8_t), sizeof(uint32_t), fdesc->fp);
            fread(&format->bytes_per_sec, sizeof(uint8_t), sizeof(uint32_t), fdesc->fp);
            fread(&format->block_align, sizeof(uint8_t), sizeof(uint16_t), fdesc->fp);
            fread(&format->bits_per_sample, sizeof(uint8_t), sizeof(uint16_t), fdesc->fp);
            f_sub = 1;
        } else if (*(uint32_t *)chunk_id == *(uint32_t *)DATA_ID) {
            file_pos = ftell(fdesc->fp);
            fdesc->data_start = file_pos;
            fdesc->data_end = file_pos + chunk_size;
            d_sub = 1;
        }
    } while (!f_sub || !d_sub);
    return 0;
}

uint64_t read_sound(FILE *fp, void *sample_block, uint64_t n_samples, wav_format *fmt)
{
    uint64_t num_read_samples = 0;
    uint16_t sample16;
    uint32_t sample24;
    float *fdata_block = (float *)sample_block;
    uint64_t num_bytes;
    
    switch (fmt->bits_per_sample) {
        case PCM16:
            for (int i = 0; i < n_samples; i++) {
                num_bytes = fread(&sample16, sizeof(uint8_t), sizeof(uint16_t), fp);
                *fdata_block++ = (float) ((double)sample16 / UINT16_MAX);
                num_read_samples++;
            }
            break;
        case PCM24:
            for (int i = 0; i < n_samples; i++) {
                num_bytes = fread(&sample24, sizeof(uint8_t), 3, fp);
                sample24 <<= 8;
                *fdata_block++ = (float) ((double)sample24 / UINT32_MAX);
                num_read_samples++;
            }
            break;
    }
    return num_read_samples;
}
