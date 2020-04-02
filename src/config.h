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
   * Description: Configuration file for the tiny dimmer firmware
   * 
   */

#pragma once

#define WS2812_DIN PB0
#define WS2812_PIXELS 36
#define WS2812_COLOR_ORDER BGR

#define BRIGHTNESS_POT_ADMUX_MSK (1 << MUX1) // PB4 (Refer to table 17-4 in the ATtiny25/45/85 datasheet)

#define BTN PB2 // INT0 (Must be INT0! See https://bit.ly/3dxVLa5)
#define BTN_DEBOUNCE_TIME 100 // ms
#define BTN_MIN_RELEASED_READS 300000 // Reduce the read of false releases when holding down noisy push buttons

#define FADE_STEP_SIZE_1 1
#define FADE_STEP_SIZE_2 2
#define FADE_STEP_SIZE_3 10
#define FADE_STEP_SIZE_4 100
#define FADE_BTN_HOLD_1 1 // s
#define FADE_BTN_HOLD_2 3 // s
#define FADE_BTN_HOLD_3 5 // s
#define FADE_BTN_HOLD_4 7 // s
#define FADE_DELAY 0 // ms Delay added to each step during fading.
                     // Note that this is added on top of the delay already caused
                     // by the code

#define ADC_AVG_SAMPLES 255 // Max 255
// #define POT_LOWER_BOUND