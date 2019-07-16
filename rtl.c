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
#include <rtl-sdr.h>


int process_device(device_t device_setup, channel_t channels[]);
void process_signal(unsigned char byte_buffer[], channel_t* channel, int n_read);


void control_rtl(int device_index, double* frequencies, int num_frequencies) {
  double central_frequency = define_central_frequency(frequencies, num_frequencies);

  device_t device_setup;
  channel_t channels[num_frequencies];

  device_setup.device_index = device_index;
  device_setup.num_frequencies = num_frequencies;
  device_setup.sample_rate = SAMPLE_RATE;
  device_setup.sample_size = SAMPLE_SIZE;
  device_setup.central_frequency = central_frequency;

  for (int i = 0; i < num_frequencies; i++) { setup_channel(frequencies[i], central_frequency, &channels[i]); }

  while(TRUE) {
    process_device(device_setup, channels);

    printf("Restarting RTL-SDR device %d in 5 secs...\n", device_index);
    sleep (5);
    }
}

int process_device(device_t device_setup, channel_t channels[]) {
  rtlsdr_dev_t* device_obj;
  int central_freq = (int)(device_setup.central_frequency * 1e6);
  int device_index = device_setup.device_index;
  int sample_rate = device_setup.sample_rate;
  int complex_buffer_size = 1024 * device_setup.sample_size;
  int buffer_size = 2 * complex_buffer_size;
  unsigned char byte_buffer[buffer_size];
  int n_read = 0;

  if (rtlsdr_open(&device_obj, device_index)           != EXIT_SUCCESS) { printf("Error rtlsdr_open\n");                return EXIT_FAILURE; }
  if (rtlsdr_set_sample_rate(device_obj, sample_rate)  != EXIT_SUCCESS) { printf("Error rtlsdr_set_sample_rate\n");     return EXIT_FAILURE; }
  if (rtlsdr_set_center_freq(device_obj, central_freq) != EXIT_SUCCESS) { printf("Error rtlsdr_set_center_freq\n");     return EXIT_FAILURE; }
  if (rtlsdr_set_tuner_gain_mode(device_obj, 0)        != EXIT_SUCCESS) { printf("Error rtlsdr_set_tuner_gain_mode\n"); return EXIT_FAILURE; }
  if (rtlsdr_reset_buffer(device_obj)                  != EXIT_SUCCESS) { printf("Error rtlsdr_reset_buffer\n");        return EXIT_FAILURE; }
  
  while (TRUE) {
    if (rtlsdr_read_sync(
        device_obj, byte_buffer, buffer_size, &n_read) != EXIT_SUCCESS) { printf("Error rtlsdr_read_sync\n");           return EXIT_FAILURE; }
    
    for (int i =0; i < device_setup.num_frequencies; i++) { process_signal(byte_buffer, &channels[i], n_read); }
  }

  return EXIT_SUCCESS;
}

void process_signal(unsigned char byte_buffer[], channel_t* channel, int n_read) {
  double complex D = CMPLX(0, 0);
  double real_signal[n_read/(2*channel->decimator_factor)];

  int real_size = 0;
  for (int i = 0, ii = 0; i < n_read; i+=2, ii++) {
    double r = (double)(signed char)byte_buffer[i]   / (255/2);
    double g = (double)(signed char)byte_buffer[i+1] / (255/2);

    D += (CMPLX(r, g) * channel->local_oscilator[ii]);

    if (ii % channel->decimator_factor == 0) {
      real_signal[real_size++] = cabs(D) / channel->decimator_factor;
      D = CMPLX(0, 0);
      ii=0;
    }
  }

  demodulate_msk(real_signal, real_size, channel);
}
