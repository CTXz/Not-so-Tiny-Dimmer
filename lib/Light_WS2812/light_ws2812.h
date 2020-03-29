/*
 * light weight WS2812 lib include
 *
 * Version 2.3  - Nev 29th 2015
 * Author: Tim (cpldcpu@gmail.com), Patrick Pederse (ctx.xda@gmail.com)
 *
 * A stripped down and slightly altered version of cpldcpu's light_ws2812.h header
 *
 * License: GNU GPL v2+ (see License.txt)
 +
 */ 

#pragma once

#include "stdint.h"

#define ws2812_resettime  300 
#define ws2812_PORTREG PORTB
#define ws2812_DDRREG DDRB

typedef struct RGB {
        uint8_t r, g ,b;
} RGB;

void ws2812_set_all(RGB rgb, uint16_t pixels, uint8_t maskhi);
