/*
 * light weight WS2812 lib include
 *
 * Version 2.3  - Nev 29th 2015
 * Author: Tim (cpldcpu@gmail.com), Patrick Pederse (ctx.xda@gmail.com)
 *
 * A stripped down version of cpldcpu's light_ws2812.h header
 *
 * License: GNU GPL v2+ (see License.txt)
 +
 */ 

#pragma once

#include "stdint.h"

#define ws2812_resettime  300 
#define ws2812_PORTREG PORTB
#define ws2812_DDRREG DDRB
#define R_OFFSET 1
#define G_OFFSET 0
#define B_OFFSET 2

void ws2812_sendarray_mask(uint8_t *data, uint16_t datlen, uint8_t maskhi);
