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
#define MAX_BRIGHTNESS 255
#define TMR_COUNTS_PER_SEC 61 // F_CPU - 16Mhz | Prescaler - 1024 | Timer Overflow - 255
                              // Counts per sec = F_CPU/(Prescaler * Timeroverflow) ~= 61

////////////////////////
// Globals
////////////////////////

// Interrupt controlled
volatile uint16_t timer_counter = 0; // Counts number of times TIMER0 has overflown
volatile bool btn_pressed = false;

// Patches
RGB patches[] = {
        {255, 255, 255}, // White
        {255, 74,  33},  // Beige
        {255, 52,  255}, // Purple
        {232, 255, 44},  // Lime
        {106, 255, 255}  // Light Blue
};

uint8_t selected_patch = 0;

#define NUM_PACHES (sizeof(patches) / sizeof(RGB))

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

#if defined(ADC_AVG_SAMPLES) || ADC_AVG_SAMPLES > 1
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
#endif

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
        ws2812_set_all(apply_brightness(rgb, brightness), WS2812_PIXELS, WS2812_DIN_MSK);
}

/* fade
 * ----
 * Description:
 *      Fades the ws2812 strip. This function must be called repeatedly  
 */
void fade(uint8_t step_size) {

        static uint8_t prev_step_size = 0;
        static RGB rgb;
        static bool r2g = true;

        uint8_t tmp;

        if (prev_step_size != step_size) {
                rgb.r = 255;
                rgb.g = 0;
                rgb.b = 0;
        }

        if (r2g) {
                tmp = rgb.r;
                tmp -= step_size;

                if (tmp >= rgb.r) {
                        rgb.r = 0;
                        rgb.g = 255;
                } else {
                        rgb.r = tmp;
                        rgb.g += step_size;
                }

                r2g = (rgb.r > 0);
        } else if (rgb.g > 0) {
                tmp = rgb.g;
                tmp -= step_size;

                if (tmp >= rgb.g) {
                        rgb.g = 0;
                        rgb.b = 255;
                } else {
                        rgb.g = tmp;
                        rgb.b += step_size;
                }
        } else {
                tmp = rgb.b;
                tmp -= step_size;

                if (tmp >= rgb.b) {
                        rgb.b = 0;
                        rgb.r = 255;
                } else {
                        rgb.b = tmp;
                        rgb.r += step_size;
                }

                r2g = (rgb.r == 255);
        }
        
        set_ws2812(rgb, brightness());
        prev_step_size = step_size;
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
        set_ws2812(patches[selected_patch], brightness());
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

        // Main loop
        
        bool prev_btn_state = PINB & (1 << BTN);
        uint8_t fade_step_size;

        while(1)
        {
                static bool fade_mode = false;
                
                uint16_t s_passed = seconds_passed();
                bool btn_state = PINB & (1 << BTN);

                // Button press
                if (prev_btn_state && !btn_state) {
                        reset_timer();
                        _delay_ms(BTN_DEBOUNCE_TIME);
                } else if (!btn_state) {
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
                                
                } else if (!prev_btn_state && btn_state && s_passed < FADE_BTN_HOLD_1) {
                        fade_mode = false;
                        next_patch();
                }

                prev_btn_state = btn_state;

                if (fade_mode) {
                        fade(fade_step_size);
#if defined(FADE_DELAY) && FADE_DELAY > 0
                        _delay_ms(FADE_DELAY);
#endif
                } else {
                        set_ws2812(patches[selected_patch], brightness());
                }
        }
}