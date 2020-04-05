/*
 * Author: Tim (cpldcpu@gmail.com), Patrick Pederse (ctx.xda@gmail.com)
 *
 * A stripped down and slightly altered version of cpldcpu's light_ws2812.h header
 *
 * License: GNU GPL v2+ (see License.txt)
 +
 */ 

#pragma once

#include "stdint.h"

#define R 0
#define G 1
#define B 2

#define RGB 0
#define GRB 1
#define BRG 2
#define BGR 3

#define WS2812_DIN_MSK (1 << WS2812_DIN)
#define WS2812_DIN_MSK_INV 

typedef uint8_t RGB_t[3];
typedef uint8_t* RGB_ptr_t;

typedef struct substrip {
        uint16_t length;
        RGB_t rgb;
} substrip;

typedef struct strip {
        uint16_t n_substrips;
        substrip *substrips;
} strip;

// void init

void ws2812_prep_tx();
void ws2812_end_tx();
void ws2812_set_all(RGB_t rgb, uint8_t brightness, uint16_t pixels);
void ws2812_set_strip(strip strp, uint8_t brightness);
