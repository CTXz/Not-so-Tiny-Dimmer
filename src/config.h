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
#define WS2812_PIXELS 35                        // !! UPDATE THIS !! Number of pixels on the WS2812 strip
#define WS2812_COLOR_ORDER GRB                  // Order in which color should be parsed to the strip (Most WS2812 strips use BGR)
#define WS2812_RESET_TIME  30                   // Time required for the WS2812 to reset
                                                // If runtime between strip writes exceeds the 
                                                // necessary reset time, this may be set to 0

//////////////////////////////
// Pots
//////////////////////////////

#define BRIGHTNESS_POT_ADMUX_MSK (1 << MUX1)               // PB4 (Refer to table 17-4 in the ATtiny25/45/85 datasheet)
// #define BRIGHTNESS_POT_ADMUX_MSK (1 << MUX1) | (1 << MUX0)    // PB3 (Refer to table 17-4 in the ATtiny25/45/85 datasheet)
// #define INVERT_POT                                         // Uncomment if pot is inverted
#define ADC_AVG_SAMPLES 255                                   // Max 255 - Number of samples used to determine the average potentiometer value.
                                                              // Increase this if the LED strip is noisy, especially at lower settings.
                                                              // Higher values will reserve more runtime
                                                              // Set to <= 1 or comment out to disable

// #define POT_LOWER_BOUND 0                                  // Max 255 - Any potentiometer value lower or equal to the lower bound will disable the strip
                                                              // Set to 0 or comment out to disable

//////////////////////////////
// Push Button
//////////////////////////////

#define BTN PB2                                                // Push button pin

#define BTN_DEBOUNCE_TIME 100                                  // ms - Time to wait for button to debounce. Increasing this will reduce false trigger due to
                                                               // bouncing, but add a slight delay to color toggling.
                                                               // Set to 0 or comment out to disable
                                                
#define BTN_MIN_RELEASED_READS 300000                          // Reduce the read of false releases when holding down noisy push buttons. If your strip randomly
                                                               // toggles while holding the button, set this value higher. Increasing this will add a delay to
                                                               // button releases. Set to <= 1 or comment out to disable. 

////////////////////////
// Patches
////////////////////////

// For a list of available patches, please refer to the
// patch_macros.h header

#define NUM_PATCHES 10 // Max 10 (To increase, add cases to update_strip() in main.c)

// Plain white
#define PATCH_0 PATCH_SET_ALL(255, 255, 255)

// First strip half white, second off 
#define PATCH_1 PATCH_DISTRIBUTE ( \
        RGB_ARRAY (                \
                {255, 255, 255},   \
                {0, 0, 0}          \
        )                          \
)

// First strip half off, second white 
#define PATCH_2 PATCH_DISTRIBUTE ( \
        RGB_ARRAY (                \
                {0, 0, 0},         \
                {255, 255, 255}    \
        )                          \
)

// First strip half pink, second cyan
#define PATCH_3 PATCH_DISTRIBUTE ( \
        RGB_ARRAY (                \
                {10, 255, 202},    \
                {255, 20, 127}     \
        )                          \
)

// First strip half purple, second beige
#define PATCH_4 PATCH_DISTRIBUTE ( \
        RGB_ARRAY (                \
                {255, 74,  33},    \
                {151, 0, 255}      \
        )                          \
)

// Cyan white rain effect with potentiometer intensity control
#define PATCH_5 PATCH_ANIMATION_RAIN_POT_CTRL(rand() % 256, 255, 255)

#define PATCH_6 PATCH_ANIMATiON_OVERRIDE_ARR_POT_CTRL ( \
        RGB_ARRAY (                                     \
                {10, 255, 202},                         \
                {255, 20, 127}                          \
        )                                               \
)
// Rotating rainbow animation
#define PATCH_7 PATCH_ANIMATION_ROTATE_RAINBOW(10)

#define PATCH_8 PATCH_ANIMATION_RAINBOW_POT_CTRL

// Halves swapping with potentiometer speed control
#define PATCH_9 PATCH_ANIMATION_SWAP_POT_CTRL(rand() % 256, rand() % 256, rand() % 256, 0, 0, 0)