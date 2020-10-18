  /*
   * Copyright (C) 2020  Patrick Pedersen

   * This program is free software: you can redistribute it and/or modify
   * it under the terms of the GNU General Public License as published by
   * the Free Software Foundation, either version 3 of the License, or
   * (at your option) any later version.

   * This program is distributed in the hope that it will be useful,
   * but WITHOUT ANY WARRANTY; without even the implied warranty of
   * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   * GNU General Public License for more details.

   * You should have received a copy of the GNU General Public License
   * along with this program.  If not, see <https://www.gnu.org/licenses/>.
   * 
   * Author: Patrick Pedersen <ctx.xda@gmail.com>
   * Description: Exposes timing related functions.
   * 
   */

#pragma once

#ifdef ARDUINO_BUILD
#include <Arduino.h>
#define DELAY_MS(ms) delay(ms)
#else
#include <util/delay.h>
#define DELAY_MS(ms) _delay_ms(ms)
#endif

#define TMR_COUNTS_PER_MS 63 // F_CPU - 16Mhz | Prescaler - None

void reset_timer();
unsigned long ms_passed();