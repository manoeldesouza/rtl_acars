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

void demodulate_acars(double v, demodmsk_t* demodulation) {

  // printf("demodulation  (in): %p\n", demodulation);

  demodulation->outbits >>= 1;
	if (v > 0) {
    demodulation->outbits |= 0x80;
  } 

	demodulation->nbits--;

  // printf("demodulation->nbits = %d\n", demodulation->nbits);

	if (demodulation->nbits <= 0) {

    char in_byte = demodulation->outbits & 0x7f;
    // printf("%c", in_byte);

    switch (demodulation->frame_stage) {
    case WSYN1:
      if (in_byte == SYN) { 
        demodulation->frame_stage = WSYN2;
        demodulation->nbits = 8;
        // printf("WSYN1\n");
      } else {
        demodulation->nbits = 0;
      }
      break;
    case WSYN2:
      if (in_byte == SYN) { 
        demodulation->frame_stage = WSOH;
        demodulation->nbits = 8;
        // printf("WSYN2\n");
      } else {
        demodulation->frame_stage = WSYN1;
      }
      break;
    case WSOH:
      if (in_byte == SOH) {
        demodulation->frame_stage = BLK;
        demodulation->nbits = 8;
        // printf("WSOH\n");
      } else {
        demodulation->frame_stage = WSYN1;
        demodulation->nbits = 0;
      }
      break;
    case BLK:
      // in_byte &= 0x7f;
      printf("%c", in_byte);
      demodulation->size++;
      demodulation->nbits = 8;
      if (in_byte == ETX || in_byte == ETB || demodulation->size >= MAXBLOCKSIZE) {
        demodulation->frame_stage = CRC1;
      } 
      break;
    case CRC1:
      demodulation->frame_stage = CRC2;
      demodulation->nbits = 8;
      // printf("\nCRC1\n");
      break;
    case CRC2:
      demodulation->frame_stage = END;
      demodulation->nbits = 8;
      // printf("CRC2\n");
      break;
    case END:
      demodulation->frame_stage = WSYN1;
      demodulation->nbits = 0;
      demodulation->size = 0;
      // printf("END\n");
      break;
    }

  }
}

