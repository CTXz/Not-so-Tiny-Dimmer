  /*
   * Copyright (C) 2020  Patrick Pedersen, The TU-DO Makespace

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
   * Description: Configuration file for the tiny dimmer firmware
   * 
   */

#pragma once

#define WS2812_DIN PB0
#define WS2812_PIXELS 37
#define WS2812_COLOR_ORDER BGR

#define BRIGHTNESS_POT_ADMUX_MSK (1 << MUX1) // PB4 (Refer to table 17-4 in the ATtiny25/45/85 datasheet)

#define BTN PB2 // INT0 (Must be INT0! See https://bit.ly/3dxVLa5)
#define BTN_DEBOUNCE_TIME 50 // ms

#define FADE_DELAY 10 // ms

#define ADC_AVG_SAMPLES 50 // Max 255
// #define POT_LOWER_BOUND