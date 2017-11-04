//
//  wavlib.h
//  SoundProgramming
//
//  Created by Kazuya Shida on 2017/10/30.
//  Copyright © 2017 mani3. All rights reserved.
//

#ifndef wavlib_h
#define wavlib_h

static int bits = 16;

typedef struct {
    char RIFF[4];
    int32_t riff_size;
    char riff_kind[4];
    char fmt[4];
    int32_t fmt_chnk;
    short fmt_id;
    short Nch;
    int32_t fs;
    int32_t dfs;
    short bl_size;
    short bit;
    short fmt_ext_size;
    char *cbuf;
    char data_chnk[4];
    int32_t Nbyte;
    short *data;
} wav_head;

extern int read_wav(wav_head *bufhead, char *fn);
extern int write_wav(wav_head *bufhead, short Nch, int32_t fs, int bit, int n_data, char *fn, int id);

#endif /* wavlib_h */
