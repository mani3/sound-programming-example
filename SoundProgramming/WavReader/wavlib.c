//
//  wavread.c
//  WavReader
//
//  Created by Kazuya Shida on 2017/10/30.
//  Copyright © 2017 mani3. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "wavlib.h"

int read_wav(wav_head *bufhead, char *fn)
{
    FILE *fp;
    int ie;
    fp = fopen(fn, "rb");
    fread(bufhead->RIFF, 1, 4, fp);
    fread(&bufhead->riff_size, 4, 1, fp);
    
    fread(bufhead->riff_kind, 1, 4, fp);
    fread(bufhead->fmt, 1, 4, fp);
    fread(&bufhead->fmt_chnk, 4, 1, fp);
    fread(&bufhead->fmt_id, 2, 1, fp);
    
    fread(&bufhead->Nch, 2, 1, fp);
    fread(&bufhead->fs, 4, 1, fp);
    fread(&bufhead->dfs, 4, 1, fp);
    fread(&bufhead->bl_size, 2, 1, fp);
    fread(&bufhead->bit, 2, 1, fp);
    
    if (bufhead->fmt_chnk != 16) {
        fread(&bufhead->fmt_ext_size, 2, 1, fp);
        bufhead->cbuf = calloc(bufhead->fmt_ext_size, sizeof(char));
        fread(bufhead->cbuf, 1, bufhead->fmt_ext_size, fp);
    }
    
    fread(bufhead->data_chnk, 1, 4, fp);
    fread(&bufhead->Nbyte, 4, 1, fp);
    bufhead->data = calloc(bufhead->Nbyte, sizeof(short));
    
    ie = (int) fread(bufhead->data, 1, bufhead->Nbyte, fp);
    if (ie < bufhead->Nbyte) {
        ie = 1;
    }
    
    fclose(fp);
    return ie;
}

int write_wav(wav_head *bufhead, short Nch, int32_t fs, int bit, int n_data, char *fn, int id)
{
    // id: 1 wav format, -1 raw format
    static char chbuf0[4] = "RIFF";
    static char chbuf1[4] = "WAVE";
    static char chbuf2[4] = "fmt ";
    static char chbuf3[4] = "data";
    
    int ie, header_size;
    
    header_size = 44; // no extension for linear PCM case
    for (int i = 0; i < 4; i++) bufhead->RIFF[i] = chbuf0[i];
    bufhead->riff_size = (header_size - 8) + n_data * (bit / 8);
    for (int i = 0; i < 4; i++) bufhead->riff_kind[i] = chbuf1[i];
    for (int i = 0; i < 4; i++) bufhead->fmt[i] = chbuf2[i];
    bufhead->fmt_chnk = 16;
    bufhead->fmt_id = 1;
    bufhead->Nch = Nch;
    bufhead->fs = fs;
    
    bufhead->dfs = bufhead->fs * (bit / 8) * bufhead->Nch;
    bufhead->bl_size = (bit / 8) * bufhead->Nch;
    bufhead->dfs = bufhead->fs * bufhead->bl_size;
    bufhead->bit = (short) bit;
    
    for (int i = 0; i < 4; i++) bufhead->data_chnk[i] = chbuf3[i];
    bufhead->Nbyte = n_data * (bit / 8);
    
    FILE *fp = fopen(fn, "wb");
    if (id == 1) {
        fwrite(bufhead, (header_size - 8), 1, fp);
        fwrite(bufhead->data_chnk, 8, 1, fp);
    }
    ie = (int) fwrite(bufhead->data, sizeof(short), n_data, fp);
    fclose(fp);
    return ie;
}
