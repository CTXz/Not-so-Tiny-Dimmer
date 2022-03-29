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
   * Description: Configuration file for the Not-so-Tiny dimmer firmware.
   * 
   */

#include <stdlib.h>

#include "conifg_enums.h"

#include "ws2812.h"
#include "strip.h"
#include "patch_macros.h"

//////////////////////////////
// Non-Addressable RGB Strip
//////////////////////////////

// #define STRIP_TYPE NON_ADDR

// #define NON_ADDR_STRIP_R PB0 // Red LED pin (must support PWM)
// #define NON_ADDR_STRIP_G PB1 // Green LED pin (must support PWM)
// #define NON_ADDR_STRIP_B PB4 // Blue LED pin (must support PWM)

//////////////////////////////
// WS2812
//////////////////////////////

#define STRIP_TYPE WS2812

#define WS2812_DIN PB0                          // WS2812 DIN pin
#define WS2812_DIN_PORT PORTB                   // WS2812 DIN pin bank
#define WS2812_COLOR_ORDER GRB                  // Order in which color should be parsed to the strip (Most WS2812 strips use BGR)
#define WS2812_RESET_TIME  50                   // Time required for the WS2812 to reset
                                                // If runtime between strip writes exceeds the 
                                                // necessary reset time, this may be set to 0

//////////////////////////////
// Potentiometer
//////////////////////////////

#define BRIGHTNESS_POT_ADMUX_MSK (1 << MUX1)                  // PB4 (Refer to table 17-4 in the ATtiny25/45/85 datasheet)

// #define INVERT_POT                                         // Uncomment if pot is inverted

// #define ADC_AVG_SAMPLES XX                                 // Max 255 - Number of samples used to determine the average potentiometer value.
                                                              // Increase this if the LED strip is noisy, especially at lower settings.
                                                              // Higher values will reserve more runtime
                                                              // Set to <= 1 or comment out to disable

// #define POT_LOWER_BOUND 0                                     // Max 255 - Any potentiometer value lower or equal to the lower bound will be registered as 0
                                                              // Set to 0 or comment out to disable

// #define POT_UPPER_BOUND XX                                 // Max 255 - Any potentiometer value lower or equal to the lower bound will be registered as 255 

//////////////////////////////
// Push Button
//////////////////////////////

#define BTN PB1                                                // Push button pin

#define BTN_DEBOUNCE_TIME 10                                   // ms - Time to wait for button to debounce. Increasing this will reduce false trigger due to
                                                               // bouncing, but add a slight delay to color toggling.
                                                               // Set to 0 or comment out to disable

//////////////////////////////
// GATE
//////////////////////////////

#define GATE PB3

////////////////////////
// Patches
////////////////////////

// For a list of available patches, please refer to the
// patch_macros.h header
#define STRIP_SIZE 70
#define HALF STRIP_SIZE/2

#define NUM_PATCHES 18 // Max 10 (To increase, add cases to update_strip() in main.c)

#define PATCH_0 PATCH_DIAL_RGB(255)

// Red
#define PATCH_1 PATCH_SET_ALL(255, 0, 0)

// White
#define PATCH_2 PATCH_SET_ALL(255, 255, 255)

// Orange
#define PATCH_3 PATCH_SET_ALL(255, 25, 0)

// Jungle green
#define PATCH_4 PATCH_SET_ALL(50, 255, 50)

// Lime green
#define PATCH_5 PATCH_SET_ALL(206, 255, 20)

// Rosy pink
#define PATCH_6 PATCH_SET_ALL(255, 75, 75)

// Purple
#define PATCH_7 PATCH_SET_ALL(151, 0, 255)

// First strip half purple, second beige
#define PATCH_8 PATCH_SPLIT (255, 74,  33, 151, 0, 255, HALF)

// First strip second beige, half purple
#define PATCH_9 PATCH_SPLIT (151, 0, 255, 255, 74,  33, HALF)

// // Arizona
#define PATCH_10 PATCH_SPLIT (255, 75, 75, 126, 200, 69, HALF)

#define PATCH_11 PATCH_SPLIT (126, 200, 69, 255, 75, 75, HALF)

// // Peach blossom
#define PATCH_12 PATCH_SPLIT (200, 59, 44, 100, 150, 20, HALF)

#define PATCH_13 PATCH_SPLIT (100, 150, 20, 200, 59, 44, HALF)

// Gold Cyan
#define PATCH_14 PATCH_SPLIT (19, 150, 56, 255, 100, 0, HALF)

#define PATCH_15 PATCH_SPLIT (255, 100, 0, 19, 150, 56, HALF)

#define PATCH_16 PATCH_ANIMATION_ROTATE_RAINBOW_POT_CTRL(5)

#define PATCH_17 PATCH_ANIMATION_RAINBOW_POT_CTRL