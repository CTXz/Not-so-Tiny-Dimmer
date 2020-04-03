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

typedef struct RGB {
        uint8_t r, g ,b;
} RGB;

void ws2812_set_all(RGB rgb, uint16_t pixels, uint8_t maskhi);
