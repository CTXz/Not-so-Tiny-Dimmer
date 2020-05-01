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
#include <util/delay.h>

#include "config.h"
#include "ws2812.h"
#include "strip.h"
#include "time.h"

#if STRIP_TYPE == WS2812

/* zero_RGBbuf
 * -------
 * Parameters:
 *      buf - RGB buffer to be zeroed
 *      size - Size of RGB buffer
 * Description:
 *      Sets all rgb objects in an RGB buffer to black (0, 0, 0).
 */
void inline zero_RGBbuf(RGBbuf buf, uint16_t size)
{
        memset(buf, 0, size * sizeof(RGB_t));
}

/* init_RGBbuf
 * -------
 * Parameters:
 *      size - Size of RGB buffer
 * Returns:
 *      Pointer to a newly allocated RGB buffer
 * Description:
 *      Allocates a new RGB buffer.
 */
RGBbuf inline init_RGBbuf(uint16_t size)
{
        RGBbuf ret = malloc(size * sizeof(RGB_t));
        zero_RGBbuf(ret, size);
        return ret;
}

#endif

/* rgb_cpy
 * -------
 * Parameters:
 *      dst - Destination RGB object
 *      src - Source RGB object
 * Description:
 *      Copies values from the source RGB object to
 *      the destination RGB object.
 */
void inline rgb_cpy(RGB_ptr_t dst, RGB_t src)
{
        dst[R] = src[R];
        dst[G] = src[G];
        dst[B] = src[B];
}

/* rgb_apply_brightness
 * --------------------
 * Parameters:
 *      rgb - A pointer to an RGB object
 *      brightness - Brightness to be applied to the RGB object
 * Description:
 *      Applies a brightness (0 = 0%, 255 = 100%) to the provided
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

/* substripbuf_apply_brightness
 * ----------------------
 * Parameters:
 *      substrpbuf - Pointer to a substrip buffer
 *      brightness - Brightness to be applied to the substrip buffer
 * Description:
 *      Applies a brightness (0 = 0%, 255 = 100%) to the provided
 *      substrip buffer.
 */
void inline substripbuf_apply_brightness(substrpbuf *substrpbuf, uint8_t brightness)
{
        if (brightness < 255) {
                for (uint16_t i = 0; i < substrpbuf->n_substrps; i++) 
                        rgb_apply_brightness(substrpbuf->substrps[i].rgb, brightness);
        }
}

/* rgb_apply_fade
 * --------------
 * Parameters:
 *      rgb - RGB object to store fade values
 *      step_size - Color steps after each call
 * Description:
 *      Apples rgb fading to the provided rgb object.
 */
void inline rgb_apply_fade(RGB_ptr_t rgb, uint8_t step_size)
{
        bool r2g;

        // Invalid RGB, correct it!
        if (rgb[R] != 0 && rgb[G] != 0 && rgb[B] != 0) {
                rgb[R] = 255;
                rgb[G] = 0;
                rgb[B] = 0;
        }        

        r2g = (rgb[G] < 255 && rgb[B] == 0);

        uint8_t tmp;

        if (step_size == 0)
                step_size = 1;

        if (r2g) {
                tmp = rgb[R];
                tmp -= step_size;

                if (tmp > rgb[R]) {
                        rgb[R] = 0;
                        rgb[G] = tmp;
                        rgb[B] = 255 - tmp;
                } else {
                        rgb[R] = tmp;
                        rgb[G] += step_size;
                }
        } else if (rgb[G] > 0) {
                tmp = rgb[G];
                tmp -= step_size;

                if (tmp > rgb[G]) {
                        rgb[G] = 0;
                        rgb[B] = tmp;
                        rgb[R] = 255 - tmp;
                } else {
                        rgb[G] = tmp;
                        rgb[B] += step_size;
                }
        } else {
                tmp = rgb[B];
                tmp -= step_size;

                if (tmp > rgb[B]) {
                        rgb[B] = 0;
                        rgb[R] = tmp;
                        rgb[G] = 255 - tmp;
                } else {
                        rgb[B] = tmp;
                        rgb[R] += step_size;
                }

        }
}

#if STRIP_TYPE == WS2812

/* substripbuf_cpy
 * ---------------
 * Parameters:
 *      dst - Strip object to store copy
 *      src - Strip object to be copied
 * Description:
 *      Creates a deep copy of a substrip object.
 */
void inline substripbuf_cpy(substrpbuf *dst, substrpbuf *src)
{
        dst->substrps = malloc(sizeof(substrp) * src->n_substrps);
        dst->n_substrps = src->n_substrps;
        memcpy(dst->substrps, src->substrps, sizeof(substrp) * dst->n_substrps);
}

/* substrpbuf_free
 * ---------------
 * Parameters:
 *      substrpbuf - Pointer to the substrip buffer to be freed
 * Description:
 *      Frees a substrip buffer.
 */
void inline substrpbuf_free(substrpbuf *substrpbuf)
{
        free(substrpbuf->substrps);
}

/* pxbuf_init
 * ----------
 * Parameters:
 *      buf - Pointer to a pixel buffer
 * Description:
 *      Initializes a pixel buffer.
 */
void inline pxbuf_init(pxbuf *buf)
{
        buf->buf = NULL;
        buf->size = 0;
}

/* pxbuf_insert
 * -------------
 * Parameters:
 *      buf - Pointer to a pixel buffer
 *      pos - Position of the pixel to be inserted
 *      rgb - RGB value of the pixel
 * Description:
 *      Adds/Allocates a pixel in the pixel buffer.
 */
void inline pxbuf_insert(pxbuf *buf, uint16_t pos, RGB_t rgb)
{
        if (buf->size == 0) {
                buf->buf = malloc(sizeof(pxl));
                buf->buf[0].pos = pos;
                rgb_cpy(buf->buf[0].rgb, rgb);
                buf->size++;
                return;
        }

        for (uint16_t i = 0; i < buf->size; i++) {

                // Pixel already allocated
                if (buf->buf[i].pos == pos) {
                        rgb_cpy(buf->buf[i].rgb, rgb);
                        return;
                }

                // Not the last pixel in the pxbuf, insert and shift array!
                if (buf->buf[i].pos > pos) {
                        buf->size++;
                        buf->buf = realloc(buf->buf, sizeof(pxl) * buf->size);

                        for (uint16_t j = buf->size-1; j > i; j--) {
                                pxl* prev_px = &(buf->buf[j-1]);
                                buf->buf[j].pos = prev_px->pos;
                                rgb_cpy(buf->buf[j].rgb, prev_px->rgb);
                        }

                        buf->buf[i].pos = pos;
                        rgb_cpy(buf->buf[i].rgb, rgb);

                        return;
                }
        }

        // Last pixel in the pxbuf, simply append it!
        buf->size++;
        buf->buf = realloc(buf->buf, sizeof(pxl) * buf->size);
        buf->buf[buf->size-1].pos = pos;
        rgb_cpy(buf->buf[buf->size-1].rgb, rgb);
}

bool inline pxbuf_exists(pxbuf *buf, uint16_t pos)
{
        for (uint16_t i = 0; i < buf->size; i++) {
                if (buf->buf[i].pos == pos)
                        return true;
                else if (buf->buf[i].pos > pos)                
                        break;
        }

        return false;
}

/* pxbuf_remove
 * -------------
 * Parameters:
 *      buf - Pointer to a pixel buffer
 *      index - Index of pixel element to be deleted
 * Description:
 *      Deletes the pixel object stored at the provided index (NOT POSITION!).
 */
void inline pxbuf_remove(pxbuf *buf, uint16_t index)
{
        buf->size--;
        
        if (buf->size == 0) {
                free(buf->buf);
                buf->buf = NULL;
                return;
        }

        for (uint16_t i = index; i < buf->size; i++) {
                buf->buf[i] = buf->buf[i+1];               
        }

        buf->buf = realloc(buf->buf, sizeof(pxl) * buf->size);
}

/* pxbuf_remove_at
 * ---------------
 * Parameters:
 *      buf - Pointer to a pixel buffer
 *      pos - Position of pixel to be deleted from the pixel buffer
 * Returns:
 *      true - Pixel object deleted
 *      false - No pixel object assigned to the position
 * Description:
 *      Deletes the pixel object assigned to the position.
 */
bool inline pxbuf_remove_at(pxbuf *buf, uint16_t pos)
{
        for (uint16_t i = 0; i < buf->size; i++)
        {
                if (buf->buf[i].pos == pos) {
                        pxbuf_remove(buf, i);
                        return true;
                }

                // If no previous pixel has matched
                // and we've reached a position exceeding pos,
                // we can assume the position has no entry
                // in the pxbuf
                if (buf->buf[i].pos > pos) {
                        return false;
                }
        }

        return false;
}

#endif

/* strip_apply_all
 * ---------------
 * Parameters:
 *      rgb - RGB value to be applied across the LED strip
 * Description:
 *      Applies a RGB value across the entire LED strip.
 */
void inline strip_apply_all(RGB_ptr_t rgb)
{
#if STRIP_TYPE == WS2812
        ws2812_prep_tx();
        for (uint16_t i = 0; i < WS2812_PIXELS; i++) {
                ws2812_tx_byte(rgb[WS2812_WIRING_RGB_0]);
                ws2812_tx_byte(rgb[WS2812_WIRING_RGB_1]);
                ws2812_tx_byte(rgb[WS2812_WIRING_RGB_2]);
        }
        ws2812_end_tx();
#else
        NON_ADDR_STRIP_R_OCR = rgb[R];
        NON_ADDR_STRIP_G_OCR = rgb[G];
        NON_ADDR_STRIP_B_OCR = rgb[B];
#endif
}

#if STRIP_TYPE == WS2812

/* strip_apply_substrpbuf
 * ----------------------
 * Parameters:
 *      substrpbuf - Sub strip buffer to be applied across the LED strip
 * Description:
 *      Applies a strip object across the LED strip.
 */
void inline strip_apply_substrpbuf(substrpbuf substrpbuf)
{
        ws2812_prep_tx();
        for (uint16_t i = 0; i < substrpbuf.n_substrps; i++) {
                for (uint16_t j = 0; j < substrpbuf.substrps[i].length; j++) {
                        ws2812_tx_byte(substrpbuf.substrps[i].rgb[WS2812_WIRING_RGB_0]);
                        ws2812_tx_byte(substrpbuf.substrps[i].rgb[WS2812_WIRING_RGB_1]);
                        ws2812_tx_byte(substrpbuf.substrps[i].rgb[WS2812_WIRING_RGB_2]);
                }
        }
        ws2812_end_tx();
}

/* strip_apply_RGBbuf
 * ------------------
 * Parameters:
 *      RGBbuf - RGB buffer to be applied across the LED strip
 * Description:
 *      Applies a RGB buffer with the strip size across the LED strip.
 */
void inline strip_apply_RGBbuf(RGBbuf RGBbuf)
{
        ws2812_prep_tx();
        for (uint8_t i = 0; i < WS2812_PIXELS; i++) {
                ws2812_tx_byte(RGBbuf[i][WS2812_WIRING_RGB_0]);
                ws2812_tx_byte(RGBbuf[i][WS2812_WIRING_RGB_1]);
                ws2812_tx_byte(RGBbuf[i][WS2812_WIRING_RGB_2]);
        }
        ws2812_end_tx();
}

/* strip_distribute_rgb
 * --------------------
 * Parameters:
 *      rgb - Array of rgb values to be distributed
 *      size - Size of the rgb array
 * Description:
 *      Evenly distributes an array of rgb values across the LED strip.
 */
void inline strip_distribute_rgb(RGB_t rgb[], uint16_t size)
{
        substrpbuf substrpbuf;
        substrpbuf.n_substrps = size;
        substrpbuf.substrps = malloc(sizeof(substrp) * size);

        for (uint16_t i = 0; i < size; i++) {
                substrpbuf.substrps[i].length = WS2812_PIXELS/size;

                if (i == size - 1)
                        substrpbuf.substrps[i].length += WS2812_PIXELS % size;

                rgb_cpy(substrpbuf.substrps[i].rgb, rgb[i]);
        }

        strip_apply_substrpbuf(substrpbuf);
        substrpbuf_free(&substrpbuf);
}

#endif

/* strip_breathe
 * -------------
 * Parameters:
 *      rgb - RGB value to be "breathed"
 *      step_size - Color steps during breath
 * Returns:
 *      True - Breath completed
 *      False - Amidst breath
 * Description:
 *      "Breathes" the provided RGB value across the entire strip.
 */
bool inline strip_breathe(RGB_ptr_t rgb, uint8_t step_size)
{
        static bool inc = true;
        static uint8_t brightness = 0;

        RGB_t rgbcpy;

        if (step_size == 0)
                step_size = 1;
        
        rgb_cpy(rgbcpy, rgb);
        rgb_apply_brightness(rgbcpy, brightness);
        strip_apply_all(rgbcpy);

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

/* strip_breathe_array
 * -------------------
 * Parameters:
 *      rgb - Arrat of RGB values to be "breathed"
 *      size - Size of the RGB array
 *      step_size - Color steps during breath
 * Description:
 *      "Breathes" the provided RGB values across the entire strip.
 */
void inline strip_breathe_array(RGB_t rgb[], uint8_t size, uint8_t step_size)
{
        static uint8_t i = 0;

        if(strip_breathe(rgb[i], step_size))
                i = (i + 1) % size;
}

/* strip_rainbow
 * -------------
 * Parameters:
 *      step_size - Color steps between each call.
 *                  A greater value results in faster fading.
 *      brightness - Brightness value (0 = 0%, 255 = 100%) of the fade
 * Description:
 *      Gradiently fades all LEDs simultaneously trough the RGB spectrum.
 */
void inline strip_rainbow(uint8_t step_size, uint16_t delay, uint8_t brightness)
{
        static RGB_t rgb = {255, 0, 0};

        RGB_t rgbcpy;

        if (ms_passed() < delay)
                return;

        rgb_apply_fade(rgb, step_size);
        
        if (brightness < 255) {
                rgb_cpy(rgbcpy, rgb);
                rgb_apply_brightness(rgbcpy, brightness);
                strip_apply_all(rgbcpy);
        } else {
                strip_apply_all(rgb);
        }

        reset_timer();
}

/* strip_breathe_random
 * --------------------
 * Parameters:
 *      step_size - Brightness steps during breath.
 * Description:
 *      "Breathes" random RGB values across the entire strip.
 *      Due to the rather poor randomness of rand(), the outcomes tend
 *      to be similar.
 */
void inline strip_breathe_random(uint8_t step_size)
{
        static RGB_t rgb;

        if (rgb[R] == 0 && rgb[G] == 0 && rgb[B] == 0) {
                rgb[R] = 255;
                rgb[G] = 255;
                rgb[B] = 255;
        }
        
        if (strip_breathe(rgb, step_size)) {
                rgb[R] = (rand() % 256);
                rgb[G] = (rand() % 256);
                rgb[B] = (rand() % 256);
        }
}

/* strip_breathe_rainbow
 * ---------------------
 * Parameters:
 *      breath_step_size - Brightness steps during breath.
 *      rgb_step_size - Color steps.
 * Description:
 *      Gradiently "Breathes" trough the rgb spectrum
 */
void inline strip_breathe_rainbow(uint8_t breath_step_size, uint8_t rgb_step_size)
{
        static RGB_t rgb = {255, 0, 0};

        if (strip_breathe(rgb, breath_step_size))
                rgb_apply_fade(rgb, rgb_step_size);
}

#if STRIP_TYPE == WS2812

/* strip_rotate_rainbow
 * --------------------
 *  * Parameters:
 *      step_size - Color steps between each pixel
 * Description:
 *      Rotates the rgb spectrum across the strip.
 *      Unlike other RGB fade effects, this one doesn't 
 *      allow for changes in speed, as the addition of delays, 
 *      changes in step size, and even just additional code can
 *      easily lead to uncomfortable lag.
 */
void inline strip_rotate_rainbow(uint8_t step_size, uint16_t delay_ms)
{
        
        static RGB_t rgb = {255, 0 , 0};
        
        if (ms_passed() < delay_ms)
                return;

        rgb_apply_fade(rgb, step_size);

        RGB_t tmp;
        rgb_cpy(tmp, rgb);

        ws2812_prep_tx();
                for (uint16_t i = 0; i < WS2812_PIXELS; i++) {
                        ws2812_tx_byte(tmp[WS2812_WIRING_RGB_0]);
                        ws2812_tx_byte(tmp[WS2812_WIRING_RGB_1]);
                        ws2812_tx_byte(tmp[WS2812_WIRING_RGB_2]);
                        rgb_apply_fade(tmp, step_size);
                }
        ws2812_end_tx();

        reset_timer();
}

/* strip_apply_RGBbuf
 * ------------------
 * Parameters:
 *      pxbuf - Pixel buffer to be applied across the LED strip
 * Description:
 *      Applies a pixel buffer across the LED strip.
 */
void inline strip_apply_pxbuf(pxbuf *buf)
{
        uint16_t px_i;

        if (buf->size == 0) {
                strip_apply_all((RGB_t){0, 0, 0});
                return;
        }

        px_i = 0;
        
        ws2812_prep_tx();
        for (uint16_t i = 0; i < WS2812_PIXELS; i++) {
                if (px_i < buf->size && i == buf->buf[px_i].pos) {
                        ws2812_tx_byte(buf->buf[px_i].rgb[WS2812_WIRING_RGB_0]);
                        ws2812_tx_byte(buf->buf[px_i].rgb[WS2812_WIRING_RGB_1]);
                        ws2812_tx_byte(buf->buf[px_i].rgb[WS2812_WIRING_RGB_2]);
                        px_i++;
                } else {
                        ws2812_tx_byte(0);
                        ws2812_tx_byte(0);
                        ws2812_tx_byte(0); 
                }
        }
        ws2812_end_tx();
}

/* strip_rain
 * ----------
 * Parameters:
 *      rgb - RGB value of rain droplets
 *      max_drops - Maximum amount of visible "droplets" at a time
 *      min_t_appart - Minimum time in ms between drops
 *      max_t_appart - Maximum time in ms between drops
 *      dealy - Delay of droplet fading
 * Description:
 *      Creates a rain effect across the strip.
 *      Note that this effect makes use of an RGB buffer and will linearly increase 
 *      memory consumption with strip size. 
 */
void inline strip_rain(RGB_t rgb, uint16_t max_drops, uint16_t min_t_appart, uint16_t max_t_appart, uint16_t delay)
{
        static pxbuf pxbuf = {
                .buf = NULL, 
                .size = 0
        };

        static uint16_t wait_until = 0;

        uint16_t ms;
        uint16_t pos;
        bool t_passed;

        t_passed = ms_passed() >= wait_until;

        for (uint16_t i = 0; i < pxbuf.size; i++) {
                if (pxbuf.buf[i].rgb[R] == 0 && pxbuf.buf[i].rgb[G] == 0 && pxbuf.buf[i].rgb[B] == 0) {
                        pxbuf_remove(&pxbuf, i);
                } else if (t_passed) {
                        if (pxbuf.buf[i].rgb[R] != 0)
                                pxbuf.buf[i].rgb[R]--;
                        if (pxbuf.buf[i].rgb[G] != 0)
                                pxbuf.buf[i].rgb[G]--;
                        if (pxbuf.buf[i].rgb[B] != 0)
                                pxbuf.buf[i].rgb[B]--;                        
                }
        }
        
        if (t_passed)
                wait_until = ms_passed() + delay;

        t_passed = ms_passed() >= (rand() % (max_t_appart - min_t_appart + 1)) + min_t_appart;

        if (t_passed && pxbuf.size < max_drops) {
                pos = rand() % WS2812_PIXELS;

                if (!pxbuf_exists(&pxbuf, pos)) {
                        pxbuf_insert(&pxbuf, pos, rgb);
                        ms = ms_passed();
                        
                        if (ms < wait_until)
                                wait_until -= ms;
                        else
                                wait_until = 0; // Already passed 

                        reset_timer();   
                }
        }

        strip_apply_pxbuf(&pxbuf);
}

bool inline strip_override(RGB_t rgb, uint16_t delay)
{

        static uint16_t pos = 0;

        if (pos == WS2812_PIXELS) {
                pos = 0;
                return true;
        }

        if (ms_passed() < delay)
                return false;
        
        ws2812_prep_tx();        
        for (uint16_t i = 0; i <= pos; i++) {
                ws2812_tx_byte(rgb[WS2812_WIRING_RGB_0]);
                ws2812_tx_byte(rgb[WS2812_WIRING_RGB_1]);
                ws2812_tx_byte(rgb[WS2812_WIRING_RGB_2]);
        }
        ws2812_end_tx();

        pos++;

        reset_timer();
        return false;
}

void inline strip_override_array(RGB_t rgb[], uint8_t size, uint16_t delay)
{
        static uint8_t i = 0;

        if (strip_override(rgb[i], delay))
                i = (i + 1) % size;
}

void inline strip_override_rainbow(uint16_t delay, uint8_t step_size)
{
        static RGB_t rgb = {255, 0, 0};

        if (strip_override(rgb, delay))
                rgb_apply_fade(rgb, step_size);
}

#endif