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


int main() {
  // double frequencies[] = {131.550, 131.725};
  // double frequencies[] = {131.550};
  double frequencies[] = {129.125};
  control_rtl(0, frequencies, sizeof(frequencies) / sizeof(frequencies[0]));

  return EXIT_SUCCESS;
}






