/*
 *   Copyright (c) 2015 Thierry Leconte
 *   Copyright (c) 2019 Manoel Souza
 *
 *   Inspired by ACARSDEC by Thierry Leconte
 *   https://github.com/TLeconte/acarsdec
 *   
 *   This code is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2
 *   published by the Free Software Foundation.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "main.h"

void demodulate_msk(double* real_signal, int real_size, channel_t* channel) {
  demodmsk_t* demodulation = &channel->demodulation_handler;

  int idx = demodulation->idx;
  int FLEN = (channel->channel_rate / 1200) + 1;
  double PLLC = 3.8e-3;
  double p = demodulation->MskPhi;

  for (int i = 0; i < real_size; i++ ) {
    double complex v = CMPLX(0,0);
    int j = 0, o = 0;

    // VCO
    double s = 1800.0 / channel->channel_rate * 2.0 * M_PI + demodulation->mskDf;
    p += s;
    if (p >= 2 * M_PI) { p -= 2 * M_PI; }

    // Mixer
    double in = real_signal[i];
    demodulation->inb[idx] = in * cexp(CMPLX(0, -p));
    idx = (idx + 1) % FLEN;

    // Bit clock
    demodulation->mskClk += s;
    if (demodulation->mskClk >= 3 * M_PI/2.0) {
  		double dphi, lvl, vo;

  		demodulation->mskClk -= 3 * M_PI/2.0;

      // Matched Filter
      o = FLEN - idx;
      for (j = 0; j < FLEN; j++, o++) {
        v += demodulation->h[o] * demodulation->inb[j];
      }

      // Normalize
      lvl = cabs(v);
      v /= lvl + 1e-6;
		  demodulation->msklvl = 0.99 * demodulation->msklvl + 0.01*lvl/5.2;

      // printf("demodulation (out): %p\n", demodulation);

      switch (demodulation->mskS&3) {
      case 0: vo = creal(v); demodulate_acars( vo, demodulation); if (vo >= 0) { dphi =  cimag(v); } else { dphi = -cimag(v); } break;
      case 1: vo = cimag(v); demodulate_acars( vo, demodulation); if (vo >= 0) { dphi = -creal(v); } else { dphi =  creal(v); } break;
      case 2: vo = creal(v); demodulate_acars(-vo, demodulation); if (vo >= 0) { dphi =  cimag(v); } else { dphi = -cimag(v); } break;
      case 3: vo = cimag(v); demodulate_acars(-vo, demodulation); if (vo >= 0) { dphi = -creal(v); } else { dphi =  creal(v); } break;
      }

		  demodulation->mskS++;

		  /* PLL filter */
		  demodulation->mskDf=PLLC*dphi;
    }
  }

  demodulation->idx = idx;
  demodulation->MskPhi = p;
}
