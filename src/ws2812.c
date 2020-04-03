/*
 * A stripped down and modified version of cpldcpu's light_ws2812 library
 * 
 * Author: Tim (cpldcpu@gmail.com), Patrick Pedersen (ctx.xda@gmail.com)
 *
 * Jan 18th, 2014  v2.0b Initial Version
 * Nov 29th, 2015  v2.3  Added SK6812RGBW support
 *
 * License: GNU GPL v2+ (see License.txt)
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "config.h"
#include "ws2812.h"
 
/*
  This routine writes an array of bytes with RGB values to the Dataout pin
  using the fast 800kHz clockless WS2811/2812 protocol.
*/

// Timing in ns
#define w_zeropulse   350
#define w_onepulse    900
#define w_totalperiod 1250

// Fixed cycles used by the inner loop
#define w_fixedlow    3
#define w_fixedhigh   6
#define w_fixedtotal  10   

// Insert NOPs to match the timing, if possible
#define w_zerocycles    (((F_CPU/1000)*w_zeropulse          )/1000000)
#define w_onecycles     (((F_CPU/1000)*w_onepulse    +500000)/1000000)
#define w_totalcycles   (((F_CPU/1000)*w_totalperiod +500000)/1000000)

// w1 - nops between rising edge and falling edge - low
#define w1 (w_zerocycles-w_fixedlow)
// w2   nops between fe low and fe high
#define w2 (w_onecycles-w_fixedhigh-w1)
// w3   nops to complete loop
#define w3 (w_totalcycles-w_fixedtotal-w1-w2)

#if w1>0
  #define w1_nops w1
#else
  #define w1_nops  0
#endif

// The only critical timing parameter is the minimum pulse length of the "0"
// Warn or throw error if this timing can not be met with current F_CPU settings.
#define w_lowtime ((w1_nops+w_fixedlow)*1000000)/(F_CPU/1000)
#if w_lowtime>550
   #error "Light_ws2812: Sorry, the clock speed is too low. Did you set F_CPU correctly?"
#elif w_lowtime>450
   #warning "Light_ws2812: The timing is critical and may only work on WS2812B, not on WS2812(S)."
   #warning "Please consider a higher clockspeed, if possible"
#endif   

#if w2>0
#define w2_nops w2
#else
#define w2_nops  0
#endif

#if w3>0
#define w3_nops w3
#else
#define w3_nops  0
#endif

#define w_nop1  "nop      \n\t"
#define w_nop2  "rjmp .+0 \n\t"
#define w_nop4  w_nop2 w_nop2
#define w_nop8  w_nop4 w_nop4
#define w_nop16 w_nop8 w_nop8

void RGBA_to_RGB(RGBA rgba, RGB_t rgb)
{
        rgb[R] = round(((double)rgba.a/255) * (double)rgba.r);
        rgb[G] = round(((double)rgba.a/255) * (double)rgba.g);
        rgb[B] = round(((double)rgba.a/255) * (double)rgba.b);
}

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

#pragma GCC push_options
#pragma GCC optimize ("O0")

void ws2812_transmit_byte(uint8_t data, uint8_t maskhi, uint8_t masklo)
{
        uint8_t ctr;

        asm volatile(
                "       ldi   %0,8  \n\t"
                "loop%=:            \n\t"
                "       st    X,%3 \n\t"    //  '1' [02] '0' [02] - re
                #if (w1_nops&1)
                w_nop1
                #endif
                #if (w1_nops&2)
                w_nop2
                #endif
                #if (w1_nops&4)
                w_nop4
                #endif
                #if (w1_nops&8)
                w_nop8
                #endif
                #if (w1_nops&16)
                w_nop16
                #endif
                "       sbrs  %1,7  \n\t"    //  '1' [04] '0' [03]
                "       st    X,%4 \n\t"     //  '1' [--] '0' [05] - fe-low
                "       lsl   %1    \n\t"    //  '1' [05] '0' [06]
                #if (w2_nops&1)
                w_nop1
                #endif
                #if (w2_nops&2)
                w_nop2
                #endif
                #if (w2_nops&4)
                w_nop4
                #endif
                #if (w2_nops&8)
                w_nop8
                #endif
                #if (w2_nops&16)
                w_nop16 
                #endif
                "       brcc skipone%= \n\t"    //  '1' [+1] '0' [+2] - 
                "       st   X,%4      \n\t"    //  '1' [+3] '0' [--] - fe-high
                "skipone%=:               "     //  '1' [+3] '0' [+2] - 

                #if (w3_nops&1)
                w_nop1
                #endif
                #if (w3_nops&2)
                w_nop2
                #endif
                #if (w3_nops&4)
                w_nop4
                #endif
                #if (w3_nops&8)
                w_nop8
                #endif
                #if (w3_nops&16)
                w_nop16
                #endif
                "       dec   %0    \n\t"    //  '1' [+4] '0' [+3]
                "       brne  loop%=\n\t"    //  '1' [+5] '0' [+4]
                :	"=&d" (ctr)
                :	"r" (data), "x" ((uint8_t *) &PORTB), "r" (maskhi), "r" (masklo)
        );
}

#pragma GCC pop_options

void inline ws2812_set_all(RGBA rgba, uint16_t pixels, uint8_t maskhi)
{
        uint8_t masklo;
        uint8_t sreg_prev;

        RGB_t rgb;
        RGBA_to_RGB(rgba, rgb);

        masklo = ~maskhi & PORTB;
        maskhi |= PORTB;

        sreg_prev=SREG;
        cli();  
  
        while (pixels--) {
                ws2812_transmit_byte(rgb[WS2812_WIRING_RGB_0], maskhi, masklo);
                ws2812_transmit_byte(rgb[WS2812_WIRING_RGB_1], maskhi, masklo);
                ws2812_transmit_byte(rgb[WS2812_WIRING_RGB_2], maskhi, masklo);
        }

        SREG=sreg_prev;

#if defined(WS2812_RESET_TIME) && WS2812_RESET_TIME > 0
        _delay_us(WS2812_RESET_TIME);
#endif
}