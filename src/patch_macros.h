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

//////////////////////////////////
// Static
//////////////////////////////////

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
#define PATCH_SET_ALL(R, G, B) patch_set_all(R, G, B)

#define PATCH_SPLIT(R1, G1, B1, R2, G2, B2, SPLIT) patch_split(R1, G1, B1, R2, G2, B2, SPLIT);

#define PATCH_SPLIT_FIXED(R1, G1, B1, R2, G2, B2, SPLIT) patch_split_mixed(R1, G1, B1, R2, G2, B2, SPLIT);

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

/* PATCH_DIAL_RGB
 * --------------
 * Parameters:
 *      BRIGHTNESS - Brightness of the color spectrum
 * Description:
 *      Dials a color within RGB spectrum with the potentiometer
 */
#define PATCH_DIAL_RGB(BRIGHTNESS) strip_scroll_rgb(pot() * 3, BRIGHTNESS)


/* --------------------------------
 * CV Controllable
 * -------------------------------- */

#define PATCH_SET_ALL_GATED(R_HI, G_HI, B_HI, R_LO, G_LO, B_LO, TRIGGER) \
        RGB_t rgb; \
        if (cv() >= TRIGGER) { \
                rgb[R] = R_HI; \
                rgb[G] = G_HI; \
                rgb[B] = B_HI; \
                rgb_apply_brightness(rgb, pot()); \
        } else { \
                rgb[R] = R_LO; \
                rgb[G] = G_LO; \
                rgb[B] = B_LO; \
        } \
        strip_apply_all(rgb);

#define PATCH_SET_ALL_TOGGLE_ON_RISE(R1, G1, B1, R2, G2, B2, TRIGGER) \
        static bool prev_trigger = false; \
        static bool toggle = false; \
        RGB_t rgb; \
        bool trigger = (cv() >= TRIGGER); \
        if (!prev_trigger && trigger) \
                toggle = !toggle; \
        if (toggle) { \
                rgb[R] = R1; \
                rgb[G] = G1; \
                rgb[B] = B1; \
                rgb_apply_brightness(rgb, pot()); \
        } else { \
                rgb[R] = R2; \
                rgb[G] = G2; \
                rgb[B] = B2; \
        } \
        strip_apply_all(rgb); \
        prev_trigger = trigger;

//////////////////////////////////
// Animations
//////////////////////////////////

/* --------------------------------
 * Constant
 * -------------------------------- */

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

/* PATCH_ANIMATION_ROTATE_RAINBOW
 * ------------------------------
 *  * Parameters:
 *      STEP_SIZE - Color steps (0 - 255) between each pixel.
 *      DELAY - Delay between each call in ms
 * Description:
 *      Rotates the rgb spectrum across the strip.
 */
#define PATCH_ANIMATION_ROTATE_RAINBOW_POT_CTRL(STEP_SIZE) strip_rotate_rainbow(STEP_SIZE, 31 - (pot() >> 3) + 5);

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

/* PATCH_ANIMATION_RAIN
 * --------------------
 * Parameters:
 *      _R - Red color value
 *      _G - Green color value
 *      _B - Blue color value
 *      MAX_DROPS - Maximum amount of visible "droplets" at a time
 *      MIN_T_APPART - Minimum time in ms between drops
 *      MAX_T_APPART - Maximum time in ms between drops
 *      DELAY - Delay of droplet fading
 * Description:
 *      Creates a rain effect across the strip.
 *      Note that this effect makes use of an RGB buffer and will linearly increase 
 *      memory consumption with strip size. 
 *      Only supported on addressable strips.
 */
#define PATCH_ANIMATION_RAIN(_R, _G, _B, MAX_DROPS, MIN_T_APPART, MAX_T_APPART, DELAY) \
        RGB_t rgb; \
        rgb[R] = _R; \
        rgb[G] = _G; \
        rgb[B] = _B; \
        strip_rain(rgb, MAX_DROPS, MIN_T_APPART, MAX_T_APPART, DELAY);

#define PATCH_ANIMATION_OVERRIDE_ARR(RGB_ARR, DELAY) \
        RGB_t rgb[] = { \
                RGB_ARR \
        }; \
        strip_override_array(rgb, sizeof(rgb)/sizeof(RGB_t), DELAY);

#define PATCH_ANIMATION_OVERRIDE_RAND(DELAY) \
        static RGB_t rgb = {255, 255, 255}; \
        if (strip_override(rgb, DELAY)) { \
                rgb[R] = rand() % 256; \
                rgb[G] = rand() % 256; \
                rgb[B] = rand() % 256; \
        }

#define PATCH_ANIMATION_OVERRIDE_RAINBOW(DELAY, STEP_SIZE) strip_override_rainbow(DELAY, STEP_SIZE);

#define PATCH_ANIMATION_FADE(R, G, B, DELAY_MS, STEP_SIZE) \
        RGB_t rgb = {R, G, B}; \
        strip_fade(rgb, DELAY_MS, STEP_SIZE, false);

/* PATCH_ANIMATION_BREATHE
 * --------------------------------
 * Parameters:
 *      R - Red value (0 - 255)
 *      G - Green value (0 - 255)
 *      B - Blue value (0 - 255)
 *      DELAY_MS - Delay between each change in brightness
 *      STEP_SIZE - Brightness steps

 * Description:
 *      "Breathes" the provided RGB value across the entire strip.
 */
#define PATCH_ANIMATION_BREATHE(R, G, B, STEP_SIZE) \
        RGB_t rgb = {R, G, B}; \
        strip_breathe(rgb, DELAY_MS, STEP_SIZE)

/* PATCH_ANIMATION_BREATHE_RAND
 * -------------------------------------
 * Parameters:
 *      STEP_SIZE - Brightness steps
 *      DELAY_MS - Delay between each change in brightness
 *
 * Description:
 *      "Breathes" random RGB values across the entire strip.
 *      Due to the rather poor randomness of rand(), the outcomes tend
 *      to be similar.
 *      Supported on non-addressable strips.
 */
#define PATCH_ANIMATION_BREATHE_RAND(DELAY_MS, STEP_SIZE) strip_breathe_random(DELAY_MS, STEP_SIZE)

/* PATCH_ANIMATION_BREATHE_RAINBOW
 * ----------------------------------------
 * Parameters:
 *      BREATH_STEP_SIZE - Brightness steps
 *      RGB_STEP_SIZE    - Color steps after every "breath".
 *                         A greater step size means the color difference 
 *                         between each breath becomes more noticeable. 
 *      DELAY_MS         - Delay between each change in brightness
 * 
 * Description:
 *      Gradiently "Breathes" trough the rgb spectrum.
 */
#define PATCH_ANIMATION_BREATHE_RAINBOW(DELAY_MS, BREATH_STEP_SIZE, RGB_STEP_SIZE) strip_breathe_rainbow(DELAY_MS, BREATH_STEP_SIZE, RGB_STEP_SIZE)

/* PATCH_ANIMATION_BREATHE_ARR_POT_CTRL
 * ------------------------------------
 * Parameters:
 *      RGB_ARR   - An RGB_ARRAY() enclosed array of literal RGB arrays.
 *                  Ex. RGB_ARRAY({255, 255, 255}, {0, 1, 2}, ...)
 *      DELAY_MS  - Delay between each change in brightness
 *      STEP_SIZE - Brightness steps
 * 
 * Description:
 *      Gradiently "Breathes" trough the RGB array.
 *      Supported on non-addressable strips.
 */
#define PATCH_ANIMATION_BREATHE_ARR(RGB_ARR, DELAY_MS, STEP_SIZE) \
        RGB_t rgb[] = { \
                RGB_ARR \
        }; \
        strip_breathe_array(rgb, sizeof(rgb)/sizeof(RGB_t), DELAY_MS, STEP_SIZE);

/* --------------------------------
 * Potentiometer Controllable
 * -------------------------------- */

/* PATCH_ANIMATION_RAINBOW_POT_CTRL
 * ---------------------------------
 * Description:
 *      Gradiently fades all LEDs simultaneously trough the RGB spectrum.
 *      The step size, and thus speed, can be altered by the potentiometer.
 *      Supported on non-addressable strips.
 */
#define PATCH_ANIMATION_RAINBOW_POT_CTRL strip_rainbow(pot() >> 6, (255 - pot()) >> 3, 255)

#define PATCH_ANIMATION_SWAP_POT_CTRL(RFH, GFH, BFH, RSH, GSH, BSH, SPLIT) \
        static bool swap = false; \
        static bool prev_gate = GATE_STATE; \
        uint8_t read = pot(); \
        if (!read) { \
                if (!prev_gate && GATE_STATE) { \
                        if (swap) { \
                                PATCH_SPLIT_FIXED(RFH, GFH, BFH, RSH, GSH, BSH, SPLIT) \
                        } else { \
                                PATCH_SPLIT_FIXED(RSH, GSH, BSH, RFH, GFH, BFH, SPLIT) \
                        } \
                        swap = !swap; \
                } \
        } \
        else if (ms_passed() >= (uint16_t)(1020 - (pot() << 2) + 100)) { \
                if (swap) { \
                        PATCH_SPLIT_FIXED(RFH, GFH, BFH, RSH, GSH, BSH, SPLIT) \
                } else { \
                        PATCH_SPLIT_FIXED(RSH, GSH, BSH, RFH, GFH, BFH, SPLIT) \
                } \
                swap = !swap; \
                reset_timer(); \
        } \
        prev_gate = GATE_STATE;

/* PATCH_ANIMATION_ROTATE_RAINBOW
 * ------------------------------
 *  * Parameters:
 *      STEP_SIZE - Color steps (0 - 255) between each pixel.
 *      DELAY - Delay between each call in ms
 * Description:
 *      Rotates the rgb spectrum across the strip. The speed can be adjusted by the potentiometer.
 */
#define PATCH_ANIMATION_ROTATE_RAINBOW_POT_CTRL(STEP_SIZE) strip_rotate_rainbow(STEP_SIZE, 31 - (pot() >> 3) + 5);

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
        uint8_t delay = (31 - (pot_read >> 3)); \
        if (delay > 10) \
                delay = 10; \
        strip_rain(rgb, (pot_read * strip_size) / 255, 255 - pot_read + 5, 510 - (pot_read << 1) + 5, delay);

#define PATCH_ANIMATION_OVERRIDE_ARR_POT_CTRL(RGB_ARR) \
        RGB_t rgb[] = { \
                RGB_ARR \
        }; \
        strip_override_array(rgb, sizeof(rgb)/sizeof(RGB_t), 255 - pot() + 5);

#define PATCH_ANIMATION_OVERRIDE_RAND_POT_CTRL \
        static RGB_t rgb = {255, 255, 255}; \
        if (strip_override(rgb, 255 - pot())) { \
                rgb[R] = rand() % 256; \
                rgb[G] = rand() % 256; \
                rgb[B] = rand() % 256; \
        }

#define PATCH_ANIMATION_OVERRIDE_RAINBOW_POT_CTRL(STEP_SIZE) strip_override_rainbow(255 - pot(), STEP_SIZE);

/* --------------------------------
 * CV Controllable
 * -------------------------------- */

#define PATCH_ANIMATION_SWAP_ON_RISE(RFH, GFH, BFH, RSH, GSH, BSH, TRIGGER) \
        static bool prev_trigger = false; \
        static bool swap = false; \
        bool trigger = (cv() >= TRIGGER); \
        if (!prev_trigger && trigger) \
                swap = !swap; \
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
        prev_trigger = trigger;

#define PATCH_ANIMATION_MOVE_DIV_ON_RISE(_R, _G, _B, DIV_SIZE, TRIGGER) \
        static int remaining = strip_size - DIV_SIZE; \
        static bool prev_trigger = false; \
        static substrpbuf buf = {3, NULL}; \
        if (!buf.substrps) { \
                buf.substrps = (substrp *) malloc(sizeof(substrp) * 3); \
                buf.substrps[0].length = 0; \
                buf.substrps[0].rgb[R] = 0; \
                buf.substrps[0].rgb[G] = 0; \
                buf.substrps[0].rgb[B] = 0; \
                buf.substrps[1].length = DIV_SIZE; \
                buf.substrps[1].rgb[R] = _R; \
                buf.substrps[1].rgb[G] = _G; \
                buf.substrps[1].rgb[B] = _B; \
                buf.substrps[2].length = (uint16_t) remaining; \
                buf.substrps[2].rgb[R] = 0; \
                buf.substrps[2].rgb[G] = 0; \
                buf.substrps[2].rgb[B] = 0; \
        } \
        if (remaining <= -DIV_SIZE) { \
                Serial.println(remaining); \
                remaining = strip_size - DIV_SIZE; \
                buf.substrps[0].length = 0; \
                buf.substrps[2].length = (uint16_t) remaining; \
        } \
        strip_apply_substrpbuf(buf); \
        bool trigger = (cv() >= TRIGGER); \
        if (!prev_trigger && trigger) { \
                buf.substrps[0].length += DIV_SIZE; \
                remaining -= DIV_SIZE; \
                buf.substrps[2].length = (remaining > 0) ? (uint16_t) remaining : 0; \
        } \
        prev_trigger = trigger;

#define PATCH_ANIMATION_FADE_ON_RISE(_R, _G, _B, DELAY_MS, TRIGGER) \
        static bool prev_trigger = false; \
        static bool fade = true; \
        static RGB_t rgb = {_R, _G, _B}; \
        bool trigger = (cv() >= TRIGGER); \
        uint8_t steps = pot(); \
        if (!steps) \
                steps = 1; \
        if (!prev_trigger && trigger) { \
                fade = strip_fade(rgb, 1, steps, true); \
        } else if (!fade) { \
                fade = strip_fade(rgb, 1, steps, false); \
        } \
        prev_trigger = trigger;

