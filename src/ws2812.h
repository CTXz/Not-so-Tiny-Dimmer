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

typedef uint8_t RGB_t[3];

typedef struct RGBA {
        uint8_t r, g ,b, a;
} RGBA;

void ws2812_set_all(RGBA rgba, uint16_t pixels, uint8_t maskhi);
