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
   * Description: Exposes macros to easily create and select patches.
   * 
   */

#pragma once

#include "config.h"
#include "strip.h"
#include "time.h"

#define RGB_ARRAY(...) __VA_ARGS__ 

// Static

/* PATCH_SET_ALL
 * -------------
 * Parameters:
 *      R - Red value (0 - 255)
 *      G - Green value (0 - 255)
 *      B - Blue value (0 - 255)
 * Description:
 *      Sets the entire LED strip to one color.
 *      Supported on non-addressable strips.
 */
#define PATCH_SET_ALL(R, G, B) \
        RGB_t rgb = {R, G, B}; \
        rgb_apply_brightness(rgb, pot()); \
        strip_apply_all(rgb);

/* PATCH_DISTRIBUTE
 * ----------------
 * Parameters:
 *      RGB_ARR - An RGB_ARRAY() enclosed array of literal RGB arrays.
 *                Ex. RGB_ARRAY({255, 255, 255}, {0, 1, 2}, ...)
 * Description:
 *      Distributes the provided array of RGB values evenly across the entire LED strip.
 */
#define PATCH_DISTRIBUTE(RGB_ARR) \
        RGB_t rgb[] = { \
                RGB_ARR \
        }; \
        uint8_t brightness = pot(); \
        for (uint16_t i = 0; i < sizeof(rgb)/sizeof(RGB_t); i++) \
                rgb_apply_brightness(rgb[i], brightness); \
        strip_distribute_rgb(rgb, sizeof(rgb)/sizeof(RGB_t));

// Animations

/* PATCH_ANIMATION_BREATHE_POT_CTRL
 * --------------------------------
 * Parameters:
 *      R - Red value (0 - 255)
 *      G - Green value (0 - 255)
 *      B - Blue value (0 - 255)
 * Description:
 *      "Breathes" the provided RGB value across the entire strip.
 *      The duration of the "breath" can be altered by the potentiometer.
 *      Supported on non-addressable strips.
 */
#define PATCH_ANIMATION_BREATHE_POT_CTRL(R, G, B) strip_breathe((RGB_t){R, G, B}, pot())

/* PATCH_ANIMATION_BREATHE_RAND_POT_CTRL
 * -------------------------------------
 * Description:
 *      "Breathes" random RGB values across the entire strip.
 *      The duration of the "breath" can be altered by the potentiometer.
 *      Due to the rather poor randomness of rand(), the outcomes tend
 *      to be similar.
 *      Supported on non-addressable strips.
 */
#define PATCH_ANIMATION_BREATHE_RAND_POT_CTRL strip_breathe_random(pot())

/* PATCH_ANIMATION_BREATHE_RAINBOW_POT_CTRL
 * ----------------------------------------
 * Parameters:
 *      RGB_STEP_SIZE - Color steps after every "breath".
 *                      A greater step size means the color difference 
 *                      between each breath becomes more noticeable. 
 * Description:
 *      Gradiently "Breathes" trough the rgb spectrum.
 *      The duration of the "breath" can be altered by the potentiometer.
 */
#define PATCH_ANIMATION_BREATHE_RAINBOW_POT_CTRL(RGB_STEP_SIZE) strip_breathe_rainbow(pot(), RGB_STEP_SIZE)

/* PATCH_ANIMATION_BREATHE_ARR_POT_CTRL
 * ------------------------------------
 * Parameters:
 *      RGB_ARR - An RGB_ARRAY() enclosed array of literal RGB arrays.
 *                Ex. RGB_ARRAY({255, 255, 255}, {0, 1, 2}, ...)
 * Description:
 *      Gradiently "Breathes" trough the RGB array.
 *      The duration of the "breath" can be altered by the potentiometer.
 *      Supported on non-addressable strips.
 */
#define PATCH_ANIMATION_BREATHE_ARR_POT_CTRL(RGB_ARR) \
        RGB_t rgb[] = { \
                RGB_ARR \
        }; \
        strip_breathe_array(rgb, sizeof(rgb)/sizeof(RGB_t), pot());

/* PATCH_ANIMATION_RAINBOW
 * ------------------------
 * Parameters:
 *      STEP_SIZE - Color steps (0 - 255) between each call.
 *                  A greater value results in faster fading.
 * Description:
 *      Gradiently fades all LEDs simultaneously trough the RGB spectrum.
 *      Supported on non-addressable strips.
 */
#define PATCH_ANIMATION_RAINBOW(STEP_SIZE, DELAY, BRIGHTNESS) strip_rainbow(STEP_SIZE, DELAY, BRIGHTNESS)

/* PATCH_ANIMATION_RAINBOW_POT_CTRL
 * ---------------------------------
 * Description:
 *      Gradiently fades all LEDs simultaneously trough the RGB spectrum.
 *      The step size, and thus speed, can be altered by the potentiometer.
 *      Supported on non-addressable strips.
 */
#define PATCH_ANIMATION_RAINBOW_POT_CTRL strip_rainbow(pot() >> 6, (255 - pot()) >> 3, 255)

/* PATCH_ANIMATION_SWAP
 * --------------------
 * Parameters:
 *      RFH - Red value (0 - 255) of first strip half
 *      GFH - Green value (0 - 255) of first strip half
 *      BFH - Blue value (0 - 255) of first strip half
 *      RFH - Red value (0 - 255) of second strip half
 *      GFH - Green value (0 - 255) of second strip half
 *      BFH - Blue value (0 - 255) of second strip half
 *      SWAP_TIME - Time (ms) after which the halves get swapped
 * Description:
 *      Splits the strip in two halves and continiously swaps their colors.
 *      Only supported on addressable strips.
 */
#define PATCH_ANIMATION_SWAP(RFH, GFH, BFH, RSH, GSH, BSH, SWAP_TIME) \
        static bool swap = false; \
        if (ms_passed() >= SWAP_TIME) { \
                if (swap) { \
                        PATCH_DISTRIBUTE(RGB_ARRAY({RFH, GFH, BFH}, {RSH, GSH, BSH})); \
                } else { \
                        PATCH_DISTRIBUTE(RGB_ARRAY({RSH, GSH, BSH}, {RFH, GFH, BFH})); \
                } \
                swap = !swap; \
                reset_timer(); \
        }

/* PATCH_ANIMATION_SWAP_POT_CTRL
 * -----------------------------
 * Parameters:
 *      RFH - Red value (0 - 255) of first strip half
 *      GFH - Green value (0 - 255) of first strip half
 *      BFH - Blue value (0 - 255) of first strip half
 *      RFH - Red value (0 - 255) of second strip half
 *      GFH - Green value (0 - 255) of second strip half
 *      BFH - Blue value (0 - 255) of second strip half
 * Description:
 *      Splits the strip in two halves and continiously swaps their colors.
 *      The swap time can be altered by the potentiometer.
 *      Only supported on addressable strips.
 */
#define PATCH_ANIMATION_SWAP_POT_CTRL(RFH, GFH, BFH, RSH, GSH, BSH) \
        static bool swap = false; \
        if (ms_passed() >= (1020 - (pot() << 2) + 100)) { \
                if (swap) { \
                        RGB_t rgb[] = { \
                                {RFH, GFH, BFH}, {RSH, GSH, BSH} \
                        }; \
                        strip_distribute_rgb(rgb, sizeof(rgb)/sizeof(RGB_t)); \
                } else { \
                        RGB_t rgb[] = { \
                                {RSH, GSH, BSH}, {RFH, GFH, BFH} \
                        }; \
                        strip_distribute_rgb(rgb, sizeof(rgb)/sizeof(RGB_t)); \
                } \
                swap = !swap; \
                reset_timer(); \
        }

/* PATCH_ANIMATION_ROTATE_RAINBOW
 * ------------------------------
 *  * Parameters:
 *      STEP_SIZE - Color steps (0 - 255) between each pixel.
 *      DELAY - Delay between each call in ms
 * Description:
 *      Rotates the rgb spectrum across the strip.
 */
#define PATCH_ANIMATION_ROTATE_RAINBOW(STEP_SIZE, DELAY) strip_rotate_rainbow(STEP_SIZE, DELAY);

/* PATCH_ANIMATION_ROTATE_RAINBOW
 * ------------------------------
 *  * Parameters:
 *      STEP_SIZE - Color steps (0 - 255) between each pixel.
 *      DELAY - Delay between each call in ms
 * Description:
 *      Rotates the rgb spectrum across the strip. The speed can be adjusted by the potentiometer.
 */
#define PATCH_ANIMATION_ROTATE_RAINBOW_POT_CTRL(STEP_SIZE) strip_rotate_rainbow(STEP_SIZE, 31 - (pot() >> 3) + 5);

/* PATCH_ANIMATION_RAIN
 * --------------------
 * Parameters:
 *      _R - Red color value
 *      _G - Green color value
 *      _B - Blue color value
 *      MAX_DROPS - Maximum amount of visible "droplets" at a time
 *      MIN_T_APPART - Minimum time in ms between drops
 *      MAX_T_APPART - Maximum time in ms between drops
 *      STEP_SIZE - Step size of droplet fading
 * Description:
 *      Creates a rain effect across the strip.
 *      Note that this effect makes use of an RGB buffer and will linearly increase 
 *      memory consumption with strip size. 
 *      Only supported on addressable strips.
 */
#define PATCH_ANIMATION_RAIN(_R, _G, _B, MAX_DROPS, MIN_T_APPART, MAX_T_APPART, STEP_SIZE) \
        RGB_t rgb; \
        rgb[R] = _R; \
        rgb[G] = _G; \
        rgb[B] = _B; \
        strip_rain(rgb, MAX_DROPS, MIN_T_APPART, MAX_T_APPART, STEP_SIZE);

/* PATCH_ANIMATION_RAIN_POT_CTRL
 * -----------------------------
 * Parameters:
 *      _R - Red color value
 *      _G - Green color value
 *      _B - Blue color value
 * Description:
 *      Creates a rain effect across the strip.
 *      The "intensity" of the rain can be adjusted with the potentiometer.
 *      Note that this effect makes use of an RGB buffer and will linearly increase 
 *      memory consumption with strip size.
 *      Only supported on addressable strips.
 */
#define PATCH_ANIMATION_RAIN_POT_CTRL(_R, _G, _B) \
        uint8_t pot_read = pot(); \
        RGB_t rgb; \
        rgb[R] = _R; \
        rgb[G] = _G; \
        rgb[B] = _B; \
        strip_rain(rgb, (pot_read * WS2812_PIXELS) / 255, 255 - pot_read, 510 - (pot_read << 1), pot_read >> 5);

#define PATCH_ANIMATION_OVERRIDE_ARR(RGB_ARR, DELAY) \
        RGB_t rgb[] = { \
                RGB_ARR \
        }; \
        strip_override_array(rgb, sizeof(rgb)/sizeof(RGB_t), DELAY);

#define PATCH_ANIMATION_OVERRIDE_ARR_POT_CTRL(RGB_ARR) \
        RGB_t rgb[] = { \
                RGB_ARR \
        }; \
        strip_override_array(rgb, sizeof(rgb)/sizeof(RGB_t), 255 - pot() + 5);
        
#define PATCH_ANIMATION_OVERRIDE_RAND(DELAY) \
        static RGB_t rgb = {255, 255, 255}; \
        if (strip_override(rgb, DELAY)) { \
                rgb[R] = rand() % 256; \
                rgb[G] = rand() % 256; \
                rgb[B] = rand() % 256; \
        }

#define PATCH_ANIMATION_OVERRIDE_RAND_POT_CTRL \
        static RGB_t rgb = {255, 255, 255}; \
        if (strip_override(rgb, 255 - pot())) { \
                rgb[R] = rand() % 256; \
                rgb[G] = rand() % 256; \
                rgb[B] = rand() % 256; \
        }

#define PATCH_ANIMATION_OVERRIDE_RAINBOW(DELAY, STEP_SIZE) strip_override_rainbow(DELAY, STEP_SIZE);

#define PATCH_ANIMATION_OVERRIDE_RAINBOW_POT_CTRL(STEP_SIZE) strip_override_rainbow(255 - pot(), STEP_SIZE);

