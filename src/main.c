  /*
   * Copyright (C) 2020  Patrick Pedersen, The TU-DO Makespace

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
   * Description: Main firmware routines for the Tiny Dimmer
   * 
   */


#if !defined(__AVR_ATtiny25__) && !defined(__AVR_ATtiny45__) && !defined (__AVR_ATtiny85__)
#error "Only ATtiny25/45/85 boards are supported!"
#endif

#include <math.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "light_ws2812.h"

#include "config.h"

////////////////////////
// Preprocessors
////////////////////////

#if F_CPU != 16000000L
#error "CPU clock (F_CPU) must be set to 16Mhz!"
#endif

#define WS2812_DIN_MSK (1 << WS2812_DIN)
#define BRIGHTNESS_POT ADCH
#define MAX_BRIGHTNESS 255
#define TMR_COUNTS_PER_SEC 61 // F_CPU - 16Mhz | Prescaler - 1024 | Timer Overflow - 255
                              // Counts per sec = F_CPU/(Prescaler * Timeroverflow) ~= 61

#define R_OFFSET 1
#define G_OFFSET 0
#define B_OFFSET 2

////////////////////////
// Structs
////////////////////////

typedef struct RGB {
        uint8_t r, g ,b;
} RGB;

////////////////////////
// Globals
////////////////////////

// Interrupt controlled
volatile uint16_t timer_counter = 0; // Counts number of times TIMER0 has overflown
volatile bool btn_pressed = false;

// Patches
RGB patches[] = {
        {255, 255, 255}, // White
        {255, 74, 33},    // Beige
        {255, 52, 255},   // Purple
        {232, 255, 44},   // Lime
        {106, 255, 255}  // Light Blue
};

uint8_t selected_patch = 0;

#define NUM_PACHES (sizeof(patches) / sizeof(RGB))

// WS2812 Pixel Buffer
uint8_t pxbuf[WS2812_PIXELS * 3];

////////////////////////
// Functions
////////////////////////

// Timing

/* reset_timer
 * -----------
 * Description:
 *      Resets the the timer to 0
 */
void inline reset_timer()
{
        TCNT0 = 0;
        timer_counter = 0;
}

/* seconds_passed
 * --------------
 * Description:
 *      Returns the number of seconds that have passed 
 *      since the timer has been reset
 */
uint16_t inline seconds_passed()
{
        return timer_counter / TMR_COUNTS_PER_SEC;
}

// LED Strip

/* apply_brightness
 * ----------------
 * Arguments:
 *      val - Value to which the brightness should be applied to
 *      brightness - A value between 0 and 255, where 255 is 100% brightness
 * Returns:
 *      Val after applying the brightness value to it
 * Description:
 *      Applies a brightness from 0 to 255 to a given 8-bit value 
 */
RGB inline apply_brightness(RGB rgb, uint8_t brightness)
{
        RGB ret;
        ret.r = round(((double)brightness/MAX_BRIGHTNESS) * (double)rgb.r);
        ret.g = round(((double)brightness/MAX_BRIGHTNESS) * (double)rgb.g);
        ret.b = round(((double)brightness/MAX_BRIGHTNESS) * (double)rgb.b);
        return ret;
}

/* fill_pxbuf
 * ----------
 * Arguments:
 *      pxbuf - Pointer to a pixel buffer 
 *      size - Size of the pixel buffer
 *      r - Red value
 *      g - Green value
 *      b - Blue value
 *      brigthess - Brightness value
 * Description:
 *      Fills all pixels in a pixel buffer with the provided RGB values at a given brightness between 0 and 255.
 */
void inline fill_pxbuf(uint8_t *pxbuf, uint16_t size, RGB rgb, uint8_t brightness)
{
        rgb = apply_brightness(rgb, brightness);

        for (uint16_t i = 0; i < size; i += 3) {
                pxbuf[i + R_OFFSET] = rgb.r;
                pxbuf[i + G_OFFSET] = rgb.g;
                pxbuf[i + B_OFFSET] = rgb.b;
        }
}

/* set_ws2812
 * ----------
 * Arguments:
 *      r - Red value
 *      g - Green value
 *      b - Blue value
 *      brightness - Brightness value
 * Description:
 *      Set the color of the entire ws2812 strip to the provided RGB values at a given brightness between 0 and 255.
 */
void set_ws2812(RGB rgb, uint8_t brightness)
{
        fill_pxbuf(pxbuf, sizeof(pxbuf), rgb, brightness);
        ws2812_sendarray_mask(pxbuf, sizeof(pxbuf), WS2812_DIN_MSK);
        _delay_us(ws2812_resettime);
}

/* fade
 * ----
 * Description:
 *      Fades the ws2812 strip. This function must be called repeatedly  
 */
void fade() {
        static RGB rgb = {255, 0, 0};
        static bool r2g = true;

        if (r2g) {
                rgb.r--;
                rgb.g++;
                r2g = (rgb.r > 0);
        } else if (rgb.g > 0) {
                rgb.g--;
                rgb.b++;
        } else {
                rgb.b--;
                rgb.r++;
                r2g = (rgb.r == 255);
        }

        set_ws2812(rgb, BRIGHTNESS_POT);
}

// Patches

/* next_patch
 * ----------
 * Description:
 *      Applies the next patch in the patches[] array.
 *      Once the end of the array has been reached, the
 *      first element is loaded again.
 */
void inline next_patch()
{
        selected_patch = (selected_patch + 1) % NUM_PACHES;
        set_ws2812(patches[selected_patch], BRIGHTNESS_POT);
}

////////////////////////
// Interrups
////////////////////////

/* TIMER0_OVF_vect
 * ---------------
 * Description:
 *      Increases the timer counter every time the
 *      timer overflows.
 */
ISR(TIMER0_OVF_vect)
{
        timer_counter++;
}

/* INT0_vect
 * ---------
 * Description:
 *      Interrupts upon button press (not release). Once interrupted,
 *      the timer is reset, a debouncing delay takes place and the global btn_pressed
 *      variable is set to true to signal the main routine that the button has been pressed.
 */
ISR(INT0_vect)
{
        reset_timer();
        cli();
        _delay_ms(BTN_DEBOUNCE_TIME);
        sei();
        btn_pressed = true;
}


////////////////////////
// Main routine
////////////////////////

/* main
 * ----
 * Description:
 *      The dimmer firmware main routine.
 */
int main()
{       
        // Initialization

        // Timer
        TCCR0B |= (1 << CS02) | (1 << CS00);  // 1024 prescaling
        TIMSK |= (1 << TOIE0);                // Enable Timer interrupts
        TIFR |= (1 << TOV0);                  // Interrupt on timer overflow

        // Pins
        DDRB |= WS2812_DIN_MSK;               // Set WS2812 Data input pin to output
        DDRB &= ~(1 << BTN);                  // Set button pin to input
        PORTB |= (1 << BTN);                  // Enable internal pull-up on Button pin

        // ADC
        ADMUX = (1 << ADLAR);                 // Reduce ADC input to 8-bit value (0-255)
        ADMUX |= BRIGHTNESS_POT_ADMUX_MSK;    // Set MUX to Brightness potentiometer pin

        ADCSRA = 
                (1 << ADEN)  |                // Enable ADC
                (1 << ADSC)  |                // Start first conversion 
                (1 << ADATE) |                // Enable Auto-Trigger
                (1 << ADPS2) |                // set prescaler to 128, bit 2 
                (1 << ADPS1) |                // set prescaler to 128, bit 1 
                (1 << ADPS0);                 // set prescaler to 128, bit 0

        // Interrupts
        GIMSK |= (1 << INT0);                 // Enable INT0
        MCUCR |= (1 << ISC01) | (0 << ISC00); // Interrupt on button press (falling due to pull-up)
        sei();                                // Enable interrups

        // Main loop
        while(1)
        {
                static bool fade_mode = false;

                if (btn_pressed) {
                        btn_pressed = !(PINB & (1 << BTN));
                        uint16_t secs_passed = seconds_passed();
                        if (btn_pressed && secs_passed >= 3) {          // Button held for 3 seconds, enable fade mode!
                                fade();
                                fade_mode = true;
                        } else if (!btn_pressed && secs_passed < 3) {   // Button released, load next patch!
                                fade_mode = false;
                                next_patch();
                        }
                }

                if (fade_mode) {
                        fade();
                        _delay_ms(FADE_DELAY);
                } else {
                        set_ws2812(patches[selected_patch], BRIGHTNESS_POT);
                }
        }
}