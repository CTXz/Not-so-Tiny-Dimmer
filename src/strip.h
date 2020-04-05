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
   * Description: Exposes hardware abstraction routines for the LED strip
   * 
   */

#pragma once

#include "config.h"

#define R 0
#define G 1
#define B 2

#define RGB 0
#define GRB 1
#define BRG 2
#define BGR 3

#if WS2812_COLOR_ORDER == RGB
        #define WS2812_WIRING_RGB_0 0
        #define WS2812_WIRING_RGB_1 1
        #define WS2812_WIRING_RGB_2 2
#elif WS2812_COLOR_ORDER == GRB
        #define WS2812_WIRING_RGB_0 1
        #define WS2812_WIRING_RGB_1 0
        #define WS2812_WIRING_RGB_2 2
#elif WS2812_COLOR_ORDER == BRG
        #define WS2812_WIRING_RGB_0 2
        #define WS2812_WIRING_RGB_1 0
        #define WS2812_WIRING_RGB_2 1
#elif WS2812_COLOR_ORDER == BGR
        #define WS2812_WIRING_RGB_0 2
        #define WS2812_WIRING_RGB_1 1
        #define WS2812_WIRING_RGB_2 0
#else
        #error "No color order specified! Please set the WS2812_COLOR_ORDER directive in the config file!"
#endif

typedef uint8_t RGB_t[3];
typedef uint8_t* RGB_ptr_t;

typedef RGB_t pixel_buffer[WS2812_PIXELS];
typedef RGB_t* pixel_buffer_ptr;

typedef struct substrip {
        uint16_t length;
        RGB_t rgb;
} substrip;

typedef struct strip {
        uint16_t n_substrips;
        substrip *substrips;
} strip;


void init_pxbuf(pixel_buffer_ptr pxbuf);

void rgb_apply_brightness(RGB_t rgb, uint8_t brightness);
void strip_apply_brightness(strip *strp, uint8_t brightness);

void strip_cpy(strip *dst, strip *src);
void strip_free(strip *strp);

void strip_set_all(RGB_ptr_t rgb);
void strip_set(strip strp);
void strip_set_pxbuf(pixel_buffer_ptr pxbuf);
void strip_distribute_rgb(RGB_t rgb[], uint16_t size);
void strip_fade_rgb(uint8_t step_size, uint8_t brightness);