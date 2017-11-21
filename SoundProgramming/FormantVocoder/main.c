//
//  main.c
//  FormantVocoder
//
//  Created by Kazuya Shida on 2017/11/21.
//  Copyright © 2017 mani3. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "wavlib.h"

#define NBUF1 2000

void resfil(double x[], double y[], double v[], double a, double w, double T, int n, int id, double E);

int main(int argc, const char * argv[]) {
    
    wav_head bufhead;
    
    const double two_pi = 2 * M_PI;
    const double expo = 2.7182818284590452354;
    const double fs = 22050; // sampleing frequency in Hz
    const double sec = 1.6; // speech wave duration (sec)
    int idg_wave = 1; // glottal wave identifier, id: 0/1 = Triangular/impulse
    
    const double dt = 1. / fs;
    const int nsample = (int)(fs * sec);
    const double morapsec = 10.0 / 3.0; // utterance speed
    
    int nres = 4;
    int fmt_id = 1;
    int bits = 16;
    
    bufhead.data = calloc(nsample, sizeof(short));
    double *x = calloc(NBUF1, sizeof(double));
    double **y = calloc(nres, sizeof(double *));
    double **y1 = calloc(nres, sizeof(double *));
    for (int j = 0; j <=nres; j++) {
        y[j] = calloc(NBUF1, sizeof(double));
        y1[j] = calloc(NBUF1, sizeof(double));
    }
    
    // F0 and Formant Frequency definition
    double F[5][10]; // F[0][*]: pitch
    F[0][0] = 110; F[1][0] = 250; F[2][0] = 2100; F[3][0] = 2900; F[4][0] = 3700;
    F[0][1] = 160; F[1][1] = 450; F[2][1] = 1900; F[3][1] = 2650; F[4][1] = 3800;
    F[0][2] = 110; F[1][2] = 700; F[2][2] = 1250; F[3][2] = 2500; F[4][2] = 3900;
    F[0][3] = 110; F[1][3] = 500; F[2][3] = 1050; F[3][3] = 2700; F[4][3] = 3700;
    F[0][4] = 110; F[1][4] = 330; F[2][4] = 1500; F[3][4] = 2400; F[4][4] = 3650;
    F[0][5] = 110; F[1][5] = 335; F[2][5] = 1550; F[3][5] = 2450; F[4][5] = 3800;
    
    double f0 = (F[0][0] + F[0][1]) / 2.0;
    double band[4] = { 50.0, 70.0, 110.0, 200.0 };
    double a[4];
    for (int j = 0; j < 4; j++) a[j] = M_PI * band[j];
    double gain = 0.5 * 0.5;
    
    int ipos = 0, nrepeat = 0, id = 0, index = 0;
    double rindex = 0;
    for (int i = 0; ; i++) {
        if (ipos > nsample) {
            break;
        }
        
        rindex = dt * morapsec * (double)ipos;
        index = rindex > 4 ? 4 : (int)rindex;
        
        // pitch frequency decision
        f0 = (F[0][0] + F[0][1]) / 2.0 + (F[0][0] - F[0][1]) / 2.0 * sin(two_pi * dt * morapsec / 5 * (double)ipos + two_pi / 4);
        nrepeat = fs / f0;
        
        if (idg_wave == 0) {
            /**
             * Triangular approximation to glottal wave
             */
            
            double tau1 = 0.25 * (double)nrepeat;
            double tau2 = 0.5 * (double)nrepeat;
            for (int j = 0; j < nrepeat; j++) {
                if (j <= tau1) {
                    x[j] = gain * (double)j / (double)tau1;
                } else if (j < tau1 + tau2) {
                    x[j] = -gain / tau2 * (double)(j - tau1) * gain;
                } else {
                    x[j] = 0.0;
                }
            }
        } else {
            /**
             * Impulse approximation to glottal wave
             */
            
            x[0] = gain;
            for (int j = 1; j < nrepeat; j++) {
                x[j] = 0.0;
            }
        }
        
        for (int k = 1; k <= 4; k++) {
            double fk = F[k][index] + (F[k][index+1] - F[k][index]) * (rindex - (double)index);
            double w = two_pi * fk;
            
            resfil(x, y[k-1], y1[k-1], a[k-1], w, dt, nrepeat, id, expo);
            if (k != 1) {
                for (int j = 0; j < nrepeat; j++) {
                    y[0][j] += y[k-1][j];
                }
            }
        }
        
        for (int j = 0; j < nrepeat; j++) {
            bufhead.data[ipos + j] = y[0][j];
        }
        ipos += nrepeat;
        id = 1;
    }
    
    write_wav(&bufhead, 1, fs, bits, nsample, "sig.wav", fmt_id);
    
    free(bufhead.data);
    free(x);
    free(y);
    free(y1);
    return 0;
}

void resfil(double x[], double y[], double v[], double a, double w, double T, int n, int id, double E) {
    double B = 2.0 * pow(E, -a * T) * cos(w * T);
    double C = pow(E, -2.0 * a * T);
    double D = ((a * a + w * w) / w) * pow(E, -a * T) * sin(w * T);
    
    if (id == 0) {
        v[0] = v[1] = 0.0;
    } else {
        v[0] = v[n-2];
        v[1] = v[n-1];
    }
    for (int i = 0; i < n; i++) {
        v[i+2] = B * v[i+1] - C * v[i] + x[i];
        y[i] = D * v[i+1];
    }
}
