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
   * Description: Main firmware routines for the Tiny Dimmer
   * 
   */


#if !defined(__AVR_ATtiny25__) && !defined(__AVR_ATtiny45__) && !defined (__AVR_ATtiny85__)
#error "Only ATtiny25/45/85 boards are supported!"
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "ws2812.h"

#include "config.h"

////////////////////////
// Preprocessors
////////////////////////

#if F_CPU != 16000000L
#error "CPU clock (F_CPU) must be set to 16Mhz!"
#endif

#define WS2812_DIN_MSK (1 << WS2812_DIN)
#define MAX_BRIGHTNESS 255
#define TMR_COUNTS_PER_SEC 61 // F_CPU - 16Mhz | Prescaler - 1024 | Timer Overflow - 255
                              // Counts per sec = F_CPU/(Prescaler * Timeroverflow) ~= 61
#define BTN_STATE !(PINB & (1 << BTN))

////////////////////////
// Globals
////////////////////////

// Interrupt controlled
volatile uint16_t timer_counter = 0; // Counts number of times TIMER0 has overflown
volatile bool btn_pressed = false;

// Patches

#if defined(DISABLE_FULL_PATCHES) && defined(DISABLE_STRIP_PATCHES)
#error "All patches have been disabled! Please enable at least one type of patch bank"
#endif

#ifndef DISABLE_FULL_PATCHES
RGB_t full_patches[5] = {
        {255, 255, 255},
        {10, 255, 202},
        {255, 20, 127},
        {151, 0,  255},
        {255, 74,  33}
};
#define NUM_FULL_PATCHES sizeof(full_patches) / sizeof(RGB_t)
#else
#define NUM_FULL_PATCHES 0
#endif

#ifndef DISABLE_STRIP_PATCHES
strip strip_patches[7];
#define NUM_STRIP_PATCHES sizeof(strip_patches) / sizeof(strip)
#else
#define NUM_STRIP_PATCHES 0
#endif

#define NUM_PACHES (NUM_FULL_PATCHES + NUM_STRIP_PATCHES)

uint8_t selected_patch;

////////////////////////
// Functions
////////////////////////

// Pots

/* adc_avg
 * -------
 * Parameters:
 *      num_samples - Number of ADC samples to be averaged (max 255)
 * Returns:
 *      Average 8-bit ADC reading
 * Description:
 *      Returns the average ADC reading from n samples
 */

uint8_t inline adc_avg(uint8_t num_samples)
{
        ADCSRA &= ~(1 << ADATE); // Temporarily disable auto triggering

        uint16_t ret = 0;
        for (uint8_t i = 0; i < num_samples; i++) {
                ADCSRA |= (1 << ADSC); // Trigger ADC
                loop_until_bit_is_clear(ADCSRA, ADSC);
                ret += ADCH;
        }

        ADCSRA |= (1 << ADATE); // Restore auto triggering

        return round((double)ret/num_samples);
}

/* brightness
 * ----------
 * Returns:
 *      The currently set brightness value (0 = 0%, 255 = 100%)
 * Description:
 *      Reads the current brightness value. Depending on the flags
 *      configured in config, this function may be as simple as
 *      simply returning the the current analog value of the pot
 *      input, or as complex as to calculate the average pot
 *      value.
 */

uint8_t inline brightness()
{
        uint8_t ret;

#if defined(ADC_AVG_SAMPLES) && ADC_AVG_SAMPLES > 1
        ret = adc_avg(ADC_AVG_SAMPLES);
#else
        ret = ADCH;
#endif

#if defined(POT_LOWER_BOUND) && POT_LOWER_BOUND > 0
        if (ret < POT_LOWER_BOUND)
                return 0;
#endif

        return ret;
}

// Buttons

/* btn
 * ---
 * Returns:
 *      True - Button is being held down
 *      False - Button is released
 * Description:
 *      Returns the current state of the push button.
 *      If the BTN_MIN_RELEASED_READS has been defined
 *      in the configuration header, the function
 *      will only return false if the button state
 *      has been read as released for the specified
 *      ammount of times. This helps reduce the read of
 *      false released when the button is being held.
 */

bool inline btn_min_reads(bool pressed, uint32_t min_reads)
{
        if (min_reads == 0)
                return BTN_STATE;
        
        for (uint32_t i = 0; i < min_reads; i++)
                if (BTN_STATE)
                        return true;

        return false;
}

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

/* fade
 * ----
 * Description:
 *      Fades the ws2812 strip. This function must be called repeatedly  
 */
void fade(uint8_t step_size) {

        static uint8_t prev_step_size = 0;
        static RGB_t rgb;
        static bool r2g = true;

        uint8_t tmp;

        if (prev_step_size != step_size) {
                rgb[R] = 255;
                rgb[G] = 0;
                rgb[B] = 0;
        }

        if (r2g) {
                tmp = rgb[R];
                tmp -= step_size;

                if (tmp >= rgb[R]) {
                        rgb[R] = 0;
                        rgb[G] = 255;
                } else {
                        rgb[R] = tmp;
                        rgb[G] += step_size;
                }

                r2g = (rgb[R] > 0);
        } else if (rgb[G] > 0) {
                tmp = rgb[G];
                tmp -= step_size;

                if (tmp >= rgb[G]) {
                        rgb[G] = 0;
                        rgb[B] = 255;
                } else {
                        rgb[G] = tmp;
                        rgb[B] += step_size;
                }
        } else {
                tmp = rgb[B];
                tmp -= step_size;

                if (tmp >= rgb[B]) {
                        rgb[B] = 0;
                        rgb[R] = 255;
                } else {
                        rgb[B] = tmp;
                        rgb[R] += step_size;
                }

                r2g = (rgb[R] == 255);
        }
        
        ws2812_set_all(rgb, brightness(), WS2812_PIXELS, WS2812_DIN_MSK);
        prev_step_size = step_size;
}

// Patches

strip strip_patch_full(uint16_t pixels, RGB_ptr_t rgb)
{
        strip ret;
        
        ret.n_substrips = 1;
        ret.substrips = malloc(sizeof(substrip));

        ret.substrips[0].length = pixels;
        memcpy(&ret.substrips[0].rgb, rgb, sizeof(RGB_t));

        return ret;
}

strip strip_patch_half(uint16_t pixels, RGB_ptr_t fhalf, RGB_ptr_t shalf)
{
        strip ret;

        ret.n_substrips = 2;
        ret.substrips = malloc(sizeof(substrip) * 2);

        ret.substrips[0].length = pixels/2;
        memcpy(&ret.substrips[0].rgb, fhalf, sizeof(RGB_t));

        ret.substrips[1].length = pixels - (pixels/2);
        memcpy(&ret.substrips[1].rgb, shalf, sizeof(RGB_t));

        return ret;
}

#ifndef DISABLE_STRIP_PATCHES
void init_strip_patches()
{
        strip_patches[0] = strip_patch_full(WS2812_PIXELS, (RGB_t){255, 255, 255});
        strip_patches[1] = strip_patch_half(WS2812_PIXELS, (RGB_t){255, 255, 255}, (RGB_t){0, 0, 0});
        strip_patches[2] = strip_patch_half(WS2812_PIXELS, (RGB_t){0, 0, 0}, (RGB_t){255, 255, 255});
        strip_patches[3] = strip_patch_half(WS2812_PIXELS, (RGB_t){10, 255, 202}, (RGB_t){255, 20, 127});
        strip_patches[4] = strip_patch_half(WS2812_PIXELS, (RGB_t){255, 20, 127}, (RGB_t){10, 255, 202});
        strip_patches[5] = strip_patch_half(WS2812_PIXELS, (RGB_t){151, 0, 255}, (RGB_t){255, 74,  33});
        strip_patches[6] = strip_patch_half(WS2812_PIXELS, (RGB_t){255, 74,  33}, (RGB_t){151, 0, 255});
}
#endif

/* next_patch
 * ----------
 * Description:
 *      Applies the next patch in the patches[] array.
 *      Once the end of the array has been reached, the
 *      first element is loaded again.
 */
void update_strip(uint8_t patch)
{
        bool set = false;

#ifndef DISABLE_FULL_PATCHES
        if ((set = (patch < NUM_FULL_PATCHES)))
                ws2812_set_all(full_patches[patch], brightness(), WS2812_PIXELS, WS2812_DIN_MSK);
#endif
#ifndef DISABLE_STRIP_PATCHES
        if ((set = (!set && (patch - NUM_FULL_PATCHES)  < NUM_STRIP_PATCHES)))
                ws2812_set_strip(strip_patches[patch - NUM_FULL_PATCHES], brightness(), WS2812_DIN_MSK);
#endif
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
        sei();                                // Enable interrupts

        // Patches
#ifndef DISABLE_STRIP_PATCHES
        init_strip_patches();
#endif

        selected_patch = 0;
        update_strip(selected_patch);
        
        // Main loop

        _delay_ms(10);                         // Allow supply voltage to calm down 

        uint8_t fade_step_size;
        bool prev_btn_state = BTN_STATE;

        while(1)
        {
                static bool fade_mode = false;
                
                uint16_t s_passed = seconds_passed();

#if defined(BTN_MIN_RELEASED_READS) && BTN_MIN_RELEASED_READS > 1
                        bool btn_state = btn_min_reads(false, prev_btn_state ? BTN_MIN_RELEASED_READS : 0);
#else
                        bool btn_state = BTN_STATE;
#endif

                if (!prev_btn_state && btn_state) { // Button press
                        reset_timer();
#if defined(BTN_DEBOUNCE_TIME) && BTN_DEBOUNCE_TIME > 0
                        _delay_ms(BTN_DEBOUNCE_TIME);
#endif
                } else if (btn_state) {             // Button Held
                        fade_mode = true;
                        if (s_passed >= FADE_BTN_HOLD_4)
                                fade_step_size = FADE_STEP_SIZE_4;
                        else if (s_passed >= FADE_BTN_HOLD_3)
                                fade_step_size = FADE_STEP_SIZE_3;
                        else if (s_passed >= FADE_BTN_HOLD_2)
                                fade_step_size = FADE_STEP_SIZE_2;
                        else if (s_passed >= FADE_BTN_HOLD_1)
                                fade_step_size = FADE_STEP_SIZE_1;
                        else
                                fade_mode = false;

                } else if (prev_btn_state && !btn_state && s_passed < FADE_BTN_HOLD_1) { // Button Released
                        fade_mode = false;
                        selected_patch = (selected_patch + 1) % NUM_PACHES;
                        update_strip(selected_patch);
                }

                prev_btn_state = btn_state;

                if (fade_mode) {
                        fade(fade_step_size);
#if defined(FADE_DELAY) && FADE_DELAY > 0
                        _delay_ms(FADE_DELAY);
#endif
                } else {
                        update_strip(selected_patch);
                }
        }
}