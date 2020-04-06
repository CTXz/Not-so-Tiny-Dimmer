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
   * Description: Hardware abstraction routines for the LED strip.
   * 
   */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include <avr/io.h>

#include "ws2812.h"
#include "strip.h"
#include "time.h"

/* init_pxbuf
 * ----------
 * Parameters:
 *      pxbuf - Pointer to a pixel buffer
 * Description:
 *      Initializes an empty pixel buffer.
 */
void inline init_pxbuf(pixel_buffer_ptr pxbuf) {
        pxbuf = malloc(sizeof(pixel_buffer) * WS2812_PIXELS);
}

/* rgb_apply_brightness
 * --------------------
 * Parameters:
 *      rgb - A pointer to an RGB object
 *      brightness - Brightness to be applied to the RGB object
 * Description:
 *      Applies a brightness (0 - 0%, 255 - 100%) to the provided
 *      RGB object.
 */
void inline rgb_apply_brightness(RGB_ptr_t rgb, uint8_t brightness)
{
        if (brightness < 255) {
                rgb[R] = round(((double)brightness/255) * (double)rgb[R]);
                rgb[G] = round(((double)brightness/255) * (double)rgb[G]);
                rgb[B] = round(((double)brightness/255) * (double)rgb[B]);
        }
}

/* strip_apply_brightness
 * ----------------------
 * Parameters:
 *      strp - A pointer to an strip object
 *      brightness - Brightness to be applied to the RGB object
 * Description:
 *      Applies a brightness (0 - 0%, 255 - 100%) to the provided
 *      strip object.
 */
void inline strip_apply_brightness(strip *strp, uint8_t brightness)
{
        if (brightness < 255) {
                for (uint16_t i = 0; i < strp->n_substrips; i++) 
                        rgb_apply_brightness(strp->substrips[i].rgb, brightness);
        }
}

/* strip_cpy
 * ---------
 * Parameters:
 *      dst - Strip object to store copy
 *      src - Strip object to be copied
 * Description:
 *      Creates a deep copy of a strip object.
 */
void inline strip_cpy(strip *dst, strip *src)
{
        dst->substrips = malloc(sizeof(substrip) * src->n_substrips);
        dst->n_substrips = src->n_substrips;
        memcpy(dst->substrips, src->substrips, sizeof(substrip) * dst->n_substrips);
}

/* strip_free
 * ----------
 * Parameters:
 *      strp - Pointer to the strip object to be freed
 * Description:
 *      Frees a strip object.
 */
void inline strip_free(strip *strp)
{
        free(strp->substrips);
}

/* strip_set_all
 * -------------
 * Parameters:
 *      rgb - RGB object to be applied accross the LED strip
 * Description:
 *      Applies a RGB value accross the entire LED strip.
 */
void inline strip_set_all(RGB_ptr_t rgb)
{
        ws2812_prep_tx();
        for (uint16_t i = 0; i < WS2812_PIXELS; i++) {
                ws2812_tx_byte(rgb[WS2812_WIRING_RGB_0]);
                ws2812_tx_byte(rgb[WS2812_WIRING_RGB_1]);
                ws2812_tx_byte(rgb[WS2812_WIRING_RGB_2]);
        }
        ws2812_end_tx();
}

/* strip_set
 * ---------
 * Parameters:
 *      strp - Strip object to be applied accross the LED strip
 * Description:
 *      Applies a strip object accross the LED strip.
 */
void inline strip_set(strip strp)
{
        ws2812_prep_tx();
        for (uint16_t i = 0; i < strp.n_substrips; i++) {
                for (uint16_t j = 0; j < strp.substrips[i].length; j++) {
                        ws2812_tx_byte(strp.substrips[i].rgb[WS2812_WIRING_RGB_0]);
                        ws2812_tx_byte(strp.substrips[i].rgb[WS2812_WIRING_RGB_1]);
                        ws2812_tx_byte(strp.substrips[i].rgb[WS2812_WIRING_RGB_2]);
                }
        }
        ws2812_end_tx();
}

/* strip_set_pxbuf
 * ---------------
 * Parameters:
 *      pxbuf - Pixel buffer to be applied accross the LED strip
 * Description:
 *      Applies a pixel buffer accross the LED strip.
 */
void inline strip_set_pxbuf(pixel_buffer_ptr pxbuf)
{
        ws2812_prep_tx();
        for (uint8_t i = 0; i < WS2812_PIXELS; i++) {
                ws2812_tx_byte(pxbuf[i][WS2812_WIRING_RGB_0]);
                ws2812_tx_byte(pxbuf[i][WS2812_WIRING_RGB_1]);
                ws2812_tx_byte(pxbuf[i][WS2812_WIRING_RGB_2]);
        }
        ws2812_end_tx();
}

/* strip_set_pxbuf
 * ---------------
 * Parameters:
 *      rgb - Array of rgb objects to be distributed
 *      size - Size of the rgb array
 * Description:
 *      Evenly distributes an array of rgb values accross the LED strip.
 */
void inline strip_distribute_rgb(RGB_t rgb[], uint16_t size)
{
        strip strp;
        strp.n_substrips = size;
        strp.substrips = malloc(sizeof(substrip) * size);

        for (uint16_t i = 0; i < size; i++) {
                strp.substrips[i].length = WS2812_PIXELS/size;

                if (i == size - 1)
                        strp.substrips[i].length += WS2812_PIXELS % size;

                memcpy(&strp.substrips[i].rgb, &rgb[i], sizeof(RGB_t));
        }

        strip_set(strp);
        strip_free(&strp);
}

/* strip_breath
 * ------------
 * Parameters:
 *      rgb - RGB value to be "breathed"
 *      step_size - Color steps during breath
 * Returns:
 *      True - Breath completed
 *      False - Amidst breath
 * Description:
 *      "Breathes" the provided RGB value across the entire strip.
 */
bool inline strip_breath(RGB_ptr_t rgb, uint8_t step_size)
{
        static bool inc = true;
        static uint8_t brightness = 0;

        if (step_size == 0)
                step_size = 1;

        RGB_t rgb_cpy;
        memcpy(&rgb_cpy, rgb, sizeof(RGB_t));
        rgb_apply_brightness(rgb_cpy, brightness);
        strip_set_all(rgb_cpy);
        
        if (brightness == 0 && ms_passed() < 2000)
                return false;

        uint8_t prev_brightness = brightness;

        if (inc) {
                brightness += step_size;
                
                if (prev_brightness > brightness)
                        brightness = 255;
                
                inc = (brightness < 255);
        } else {
                brightness -= step_size;

                if (prev_brightness < brightness)
                        brightness = 0;
        }
        
        if (brightness == 0) {
                reset_timer();
                inc = true;
                return true;
        }

        return false;
}

/* strip_breath_array
 * ------------------
 * Parameters:
 *      rgb - Arrat of RGB values to be "breathed"
 *      size - Size of the RGB array
 *      step_size - Color steps during breath
 * Description:
 *      "Breathes" the provided RGB values across the entire strip.
 */
void inline strip_breath_array(RGB_t rgb[], uint8_t size, uint8_t step_size)
{
        static uint8_t i = 0;

        if(strip_breath(rgb[i], step_size))
                i = (i + 1) % size;
}

/* apply_rgb_fade
 * --------------
 * Parameters:
 *      rgb - RGB object to apply fade on
 *      step_size - Color steps after each call
 *      r2g - Callback to tell the routine which color transition is taking place
 * Returns:
 *      Whether the red to green color transition has completed. Parse the return to
 *      the r2g arg!
 * Description:
 *      Apples rgb fading to the provided rgb object.
 */
bool inline apply_rgb_fade(RGB_ptr_t rgb, uint8_t step_size, bool r2g)
{
        uint8_t tmp;

        if (step_size == 0)
                step_size = 1;

        if (r2g) {
                tmp = rgb[R];
                tmp -= step_size;

                if (tmp > rgb[R]) {
                        rgb[R] = 0;
                        rgb[G] = 255;
                } else {
                        rgb[R] = tmp;
                        rgb[G] += step_size;
                }

                return (rgb[R] > 0);
        } else if (rgb[G] > 0) {
                tmp = rgb[G];
                tmp -= step_size;

                if (tmp > rgb[G]) {
                        rgb[G] = 0;
                        rgb[B] = 255;
                } else {
                        rgb[G] = tmp;
                        rgb[B] += step_size;
                }

                return false;
        } else {
                tmp = rgb[B];
                tmp -= step_size;

                if (tmp > rgb[B]) {
                        rgb[B] = 0;
                        rgb[R] = 255;
                } else {
                        rgb[B] = tmp;
                        rgb[R] += step_size;
                }

                return (rgb[R] == 255);
        }
}
/* strip_fade_rgb
 * --------------
 * Parameters:
 *      step_size - Color steps between each call.
 *                  A greater value results in faster fading.
 *      brightness - Brightness value (0 - 0%, 255 - 100%) of the fade
 * Description:
 *      Gradiently fades the LED strip torugh red, green and blue.
 */
void inline strip_fade_rgb(uint8_t step_size, uint8_t brightness)
{
        static RGB_t rgb = {255, 0, 0};
        static bool r2g = true;
        
        r2g = apply_rgb_fade(rgb, step_size, r2g);
        
        if (brightness < 255) {
                RGB_t rgb_cpy;
                memcpy(&rgb_cpy, &rgb, sizeof(RGB_t));
                rgb_apply_brightness(rgb_cpy, brightness);
                strip_set_all(rgb_cpy);
        } else {
                strip_set_all(rgb);
        }
}

/* strip_breath_random
 * -------------------
 * Parameters:
 *      step_size - Brightness steps during breath.
 * Description:
 *      "Breathes" random RGB values across the entire strip.
 *      Due to the rather poor randomness of rand(), the outcomes tend
 *      to be similar.
 */
void inline strip_breath_random(uint8_t step_size)
{
        static RGB_t rgb;

        if (rgb[R] == 0 && rgb[G] == 0 && rgb[B] == 0) {
                rgb[R] = 255;
                rgb[G] = 255;
                rgb[B] = 255;
        }
        
        if (strip_breath(rgb, step_size)) {
                rgb[R] = (rand() % 256);
                rgb[G] = (rand() % 256);
                rgb[B] = (rand() % 256);
        }
}

/* strip_breath_rgb
 * ----------------
 * Parameters:
 *      breath_step_size - Brightness steps during breath.
 *      rgb_step_size - Color steps.
 * Description:
 *      Gradiently "Breathes" trough red, green and blue.
 */
void inline strip_breath_rgb(uint8_t breath_step_size, uint8_t rgb_step_size)
{
        static RGB_t rgb = {255, 0, 0};
        static bool r2g = true;

        if (strip_breath(rgb, breath_step_size))
                r2g = apply_rgb_fade(rgb, rgb_step_size, r2g);
}