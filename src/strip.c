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
   * Description: Hardware abstraction routines for the LED strip
   * 
   */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "ws2812.h"
#include "strip.h"

void inline rgb_apply_brightness(RGB_ptr_t rgb, uint8_t brightness)
{
        if (brightness < 255) {
                rgb[R] = round(((double)brightness/255) * (double)rgb[R]);
                rgb[G] = round(((double)brightness/255) * (double)rgb[G]);
                rgb[B] = round(((double)brightness/255) * (double)rgb[B]);
        }
}

void inline strip_apply_brightness(strip *strp, uint8_t brightness)
{
        if (brightness < 255) {
                for (uint16_t i = 0; i < strp->n_substrips; i++) 
                        rgb_apply_brightness(strp->substrips[i].rgb, brightness);
        }
}

void inline strip_cpy(strip *dst, strip *src)
{
        dst->substrips = malloc(sizeof(substrip) * src->n_substrips);
        dst->n_substrips = src->n_substrips;
        memcpy(dst->substrips, src->substrips, sizeof(substrip) * dst->n_substrips);
}

void inline strip_free(strip *strp)
{
        free(strp->substrips);
}

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

void strip_fade_rgb(RGB_ptr_t glob_rgb, uint8_t step_size, uint8_t brightness)
{
        static bool r2g = true;

        uint8_t tmp;

        if (step_size == 0) {
                step_size = 1;
        }

        if (r2g) {
                tmp = glob_rgb[R];
                tmp -= step_size;

                if (tmp > glob_rgb[R]) {
                        glob_rgb[R] = 0;
                        glob_rgb[G] = 255;
                } else {
                        glob_rgb[R] = tmp;
                        glob_rgb[G] += step_size;
                }

                r2g = (glob_rgb[R] > 0);
        } else if (glob_rgb[G] > 0) {
                tmp = glob_rgb[G];
                tmp -= step_size;

                if (tmp > glob_rgb[G]) {
                        glob_rgb[G] = 0;
                        glob_rgb[B] = 255;
                } else {
                        glob_rgb[G] = tmp;
                        glob_rgb[B] += step_size;
                }
        } else {
                tmp = glob_rgb[B];
                tmp -= step_size;

                if (tmp > glob_rgb[B]) {
                        glob_rgb[B] = 0;
                        glob_rgb[R] = 255;
                } else {
                        glob_rgb[B] = tmp;
                        glob_rgb[R] += step_size;
                }

                r2g = (glob_rgb[R] == 255);
        }
        
        if (brightness < 255) {
                RGB_t rgb_cpy;
                memcpy(&rgb_cpy, glob_rgb, sizeof(RGB_t));
                rgb_apply_brightness(rgb_cpy, brightness);
                strip_set_all(rgb_cpy);
        } else {
                strip_set_all(glob_rgb);
        }

}