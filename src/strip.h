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
   * Description: Exposes hardware abstraction routines for the LED strip.
   * 
   */

#pragma once

#include <stdbool.h>
#include "config.h"

#define R 0
#define G 1
#define B 2

#define RGB 0
#define GRB 1
#define BRG 2
#define BGR 3

#if STRIP_TYPE == WS2812
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
#else
        #if NON_ADDR_STRIP_R == PB0
                #define NON_ADDR_STRIP_R_OCR OCR0A
        #elif NON_ADDR_STRIP_R == PB1
                #define NON_ADDR_STRIP_R_OCR OCR0B
        #elif NON_ADDR_STRIP_R == PB4
                #define NON_ADDR_STRIP_R_OCR OCR1B
        #else
                #error "Red LED pin does not support PWM! Please use PB0, PB1 or PB4!"
        #endif

        #if NON_ADDR_STRIP_G == PB0
                #define NON_ADDR_STRIP_G_OCR OCR0A
        #elif NON_ADDR_STRIP_G == PB1
                #define NON_ADDR_STRIP_G_OCR OCR0B
        #elif NON_ADDR_STRIP_G == PB4
                #define NON_ADDR_STRIP_G_OCR OCR1B
        #else
                #error "Green LED pin does not support PWM! Please use PB0, PB1 or PB4!"
        #endif

        #if NON_ADDR_STRIP_B == PB0
                #define NON_ADDR_STRIP_B_OCR OCR0A
        #elif NON_ADDR_STRIP_B == PB1
                #define NON_ADDR_STRIP_B_OCR OCR0B
        #elif NON_ADDR_STRIP_B == PB4
                #define NON_ADDR_STRIP_B_OCR OCR1B
        #else
                #error "Blue LED pin does not support PWM! Please use PB0, PB1 or PB4!"
        #endif
#endif

typedef uint8_t RGB_t[3];
typedef uint8_t* RGB_ptr_t;

typedef RGB_t* RGBbuf;

typedef struct substrp {
        uint16_t length;
        RGB_t rgb;
} substrp;

typedef struct substrpbuf {
        uint16_t n_substrps;
        substrp *substrps;
} substrpbuf;

typedef struct pxl {
        uint16_t pos;
        RGB_t rgb;
} pxl;

typedef struct pxbuf {
        uint16_t size;
        pxl* buf;
} pxbuf;

void rgb_apply_brightness(RGB_t rgb, uint8_t brightness);
void substripbuf_apply_brightness(substrpbuf *strp, uint8_t brightness);

void substrpbuf_cpy(substrpbuf *dst, substrpbuf *src);
void substrpbuf_free(substrpbuf *strp);

void pxbuf_init(pxbuf *buf);
void pxbuf_insert(pxbuf *buf, uint16_t pos, RGB_t rgb);
bool pxbuf_exists(pxbuf *buf, uint16_t pos);
void pxbuf_remove(pxbuf *buf, uint16_t index);
bool pxbuf_remove_at(pxbuf *buf, uint16_t pos);

void strip_apply_all(RGB_ptr_t rgb);

#if STRIP_TYPE == WS2812
void strip_apply_substrpbuf(substrpbuf strp);
void strip_apply_RGBbuf(RGBbuf RGBbuf);
void strip_apply_pxbuf(pxbuf *buf);
void strip_distribute_rgb(RGB_t rgb[], uint16_t size);
#endif

bool strip_breathe(RGB_ptr_t rgb, uint8_t step_size);
void strip_breathe_array(RGB_t rgb[], uint8_t size, uint8_t step_size);
void strip_breathe_random(uint8_t step_size);
void strip_breathe_rainbow(uint8_t breath_step_size, uint8_t rgb_step_size);
void strip_rainbow(uint8_t step_size, uint16_t delay, uint8_t brightness);

#if STRIP_TYPE == WS2812
void strip_rotate_rainbow(uint8_t step_size, uint16_t delay_ms);
void strip_rain(RGB_t rgb, uint16_t max_drops, uint16_t min_t_appart, uint16_t max_t_appart, uint16_t delay);
bool strip_override(RGB_t rgb, uint16_t delay);
void strip_override_array(RGB_t rgb[], uint8_t size, uint16_t delay);
void strip_override_rainbow(uint16_t delay, uint8_t step_size);
#endif