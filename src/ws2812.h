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

#define ws2812_resettime  0 // Time required for the WS2812 to reset
                            // If runtime between strip writes exceeds the 
                            // necessary reset time, this may be set to 0

#define ws2812_PORTREG PORTB
#define ws2812_DDRREG DDRB

typedef struct RGB {
        uint8_t r, g ,b;
} RGB;

void ws2812_set_all(RGB rgb, uint16_t pixels, uint8_t maskhi);
