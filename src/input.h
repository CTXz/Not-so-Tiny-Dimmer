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
 * 
 */

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define BTN_STATE !(PINB & (1 << BTN))

uint8_t adc_avg(uint8_t num_samples);
uint8_t pot();