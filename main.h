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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <complex.h>
#include <math.h>


#define SAMPLE_RATE  2000000
#define SAMPLE_SIZE  32
#define CHANNEL_RATE 12500
#define TRUE         1
#define FALSE        0
#define M_PI         3.14159265358979323846
#define MAXBLOCKSIZE 230

#define	NUL 0x00
#define	SOH 0x01
#define	STX 0x02
#define	ETX 0x03
#define	EOT 0x04
#define	ENQ 0x05
#define	ACK 0x06
#define	EL  0x07
#define	BS  0x08
#define	HT  0x09
#define	LF  0x0a
#define	VT  0x0b
#define	FF  0x0c
#define	CR  0x0d
#define	SO  0x0e
#define	SI  0x0f
#define	DLE 0x10
#define	DC1 0x11
#define	DC2 0x12
#define	DC3 0x13
#define	DC4 0x14
#define	NAK 0x15
#define	SYN 0x16
#define	ETB 0x17
#define	CAN 0x18
#define	EM  0x19
#define	SUB 0x1a
#define	ESC 0x1b
#define	FS  0x1c
#define	GS  0x1d
#define	RS  0x1e
#define	US  0x1f
#define	DEL 0x7f


typedef struct _demodmsk_t {
  double         mskDf;
  double         MskPhi;
  double         mskClk;
  double         msklvl;
  int            mskS;
  int            idx;
  double         h[1024];
  double complex inb[1024];
  unsigned char  outbits;
	int	           nbits;
  int            frame_stage;
  int            size;
} demodmsk_t;

typedef struct _channel_t {
  double         channel_frequency;
  double         real_signal[1024];
  double         complex local_oscilator[1024];
  int            channel_rate;
  int            decimator_factor;
  char           acars_block[1024];
  demodmsk_t     demodulation_handler;
} channel_t;

typedef struct _device_t {
  int            device_index;
  int            num_frequencies;
  double         central_frequency;
  int            sample_size;
  int            sample_rate;
} device_t;

enum FrameStage {
	WSYN1,
	WSYN2,
	WSOH,
	BLK,
	CRC1,
	CRC2,
	END
};


void setup_channel(double channel_frequency, double central_frequency, channel_t* new_channel);

double complex complex_from_polar(double radius, double theta);

double define_central_frequency(double *frequencies, int num_frequencies);

void control_rtl(int device_index, double* frequencies, int num_frequencies);

void demodulate_msk(double* real_signal, int real_size, channel_t* channel);

void demodulate_acars(double v, demodmsk_t* demodulation);