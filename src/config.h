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

#include <stdlib.h>

#include "ws2812.h"
#include "strip.h"
#include "patch_macros.h"

// Patches
// #define DISABLE_FULL_PATCHES
// #define DISABLE_STRIP_PATCHES
#define NUM_PATCHES 9

#define PATCH_0 PATCH_SET_ALL(255, 255, 255)

#define PATCH_1 PATCH_DISTRIBUTE ( \
        RGB_ARRAY (                \
                {255, 255, 255},   \
                {0, 0, 0}          \
        )                          \
)

#define PATCH_2 PATCH_DISTRIBUTE ( \
        RGB_ARRAY (                \
                {0, 0, 0},         \
                {255, 255, 255}    \
        )                          \
)

#define PATCH_3 PATCH_DISTRIBUTE ( \
        RGB_ARRAY (                \
                {10, 255, 202},    \
                {255, 20, 127}     \
        )                          \
)

#define PATCH_4 PATCH_DISTRIBUTE ( \
        RGB_ARRAY (                \
                {255, 20, 127},    \
                {10, 255, 202}     \
        )                          \
)

#define PATCH_5 PATCH_DISTRIBUTE ( \
        RGB_ARRAY (                \
                {151, 0, 255},     \
                {255, 74,  33}     \
        )                          \
)

#define PATCH_6 PATCH_DISTRIBUTE ( \
        RGB_ARRAY (                \
                {255, 74,  33},    \
                {151, 0, 255}      \
        )                          \
)

#define PATCH_7 PATCH_ANIMATION_FADE_RGB_POT_CTRL

#define PATCH_8 PATCH_ANIMATION_SWAP_POT_CTRL(rand() % 255, rand() % 255, rand() % 255, 0, 0, 0);

// WS2812
#define WS2812_DIN PB0                          // WS2812 DIN pin
#define WS2812_PIXELS 36                        // !! UPDATE THIS !! Number of pixels on the WS2812 strip
#define WS2812_COLOR_ORDER GRB                  // Order in which color should be parsed to the strip (Most WS2812 strips use BGR)
#define WS2812_RESET_TIME  0                    // Time required for the WS2812 to reset
                                                // If runtime between strip writes exceeds the 
                                                // necessary reset time, this may be set to 0

// Pots
#define BRIGHTNESS_POT_ADMUX_MSK (1 << MUX1)    // PB4 (Refer to table 17-4 in the ATtiny25/45/85 datasheet)

#define ADC_AVG_SAMPLES 255                     // Max 255 - Number of samples used to determine the average potentiometer value.
                                                // Increase this if the LED strip is noisy, especially at lower settings.
                                                // Higher values will reserve more runtime
                                                // Set to <= 1 or comment out to disable

// #define POT_LOWER_BOUND 5                    // Max 255 - Any potentiometer value lower or equal to the lower bound will disable the strip
                                                // Set to 0 or comment out to disable

// Push Button

#define BTN PB2                                 // Push button pin

#define BTN_DEBOUNCE_TIME 100                   // ms - Time to wait for button to debounce. Increasing this will reduce false trigger due to
                                                // bouncing, but add a slight delay to color toggling.
                                                // Set to 0 or comment out to disable
                                                
#define BTN_MIN_RELEASED_READS 300000           // Reduce the read of false releases when holding down noisy push buttons. If your strip randomly
                                                // toggles while holding the button, set this value higher. Increasing this will add a delay to
                                                // button releases. Set to <= 1 or comment out to disable. 

// Fading

// Steps sizes for all four fade settings
// A greater step size means faster toggling
#define FADE_STEP_SIZE_1 1                      // First setting
#define FADE_STEP_SIZE_2 2                      
#define FADE_STEP_SIZE_3 10
#define FADE_STEP_SIZE_4 100                    // Last setting

// Button hold duration for all four fade settings 
#define FADE_BTN_HOLD_1 1 // s                  // First setting
#define FADE_BTN_HOLD_2 3 // s
#define FADE_BTN_HOLD_3 5 // s
#define FADE_BTN_HOLD_4 7 // s                  // Last setting

#define FADE_DELAY 0 // ms Delay added to each step during fading.
                     // Note that this is added on top of the delay already caused
                     // by the code