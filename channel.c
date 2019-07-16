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

double complex complex_from_polar(double radius, double theta) {
  return CMPLX(radius * cos(theta), radius * sin(theta));
}

void setup_channel(double channel_frequency, double central_frequency, channel_t* new_channel) {
  int sample_rate = SAMPLE_RATE;
  // int sample_size = SAMPLE_SIZE;
  int channel_rate = CHANNEL_RATE;
  int FLEN = (channel_rate / 1200) + 1;

  new_channel->channel_frequency = channel_frequency;
  new_channel->decimator_factor = sample_rate / channel_rate;
  new_channel->channel_rate = channel_rate;

  double frequency_diff = channel_frequency - central_frequency;
  double shift_factor = (frequency_diff * 1e6) / sample_rate;
  int oscilator_size = 1024;

  for (int i = 0; i < oscilator_size; i++) { new_channel->local_oscilator[i] = complex_from_polar(1, -shift_factor * 2 * M_PI * i); }

  for (int i = 0; i < FLEN; i++) {
		new_channel->demodulation_handler.h[i] = new_channel->demodulation_handler.h[i+FLEN] = cosf(2.0*M_PI*600.0/channel_rate*(i-FLEN/2));
		new_channel->demodulation_handler.inb[i] = 0;
	}

  new_channel->demodulation_handler.frame_stage = WSYN1;
  new_channel->demodulation_handler.nbits = 0;
  new_channel->demodulation_handler.size = 0;
}

double define_central_frequency(double *frequencies, int num_frequencies) {
  double delta = -0.250;
  double central_frequency = 0.0;

  for (int i =0; i < num_frequencies; i++) { central_frequency += frequencies[i]; }
  if (num_frequencies > 1) { delta = 0.0; }

  return (central_frequency /= num_frequencies) + delta; 
}
