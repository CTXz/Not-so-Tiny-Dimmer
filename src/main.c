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

#include "strip.h"
#include "config.h"

////////////////////////
// Preprocessors
////////////////////////

#if F_CPU != 16000000L
#error "CPU clock (F_CPU) must be set to 16Mhz!"
#endif

#define MAX_BRIGHTNESS 255
#define TMR_COUNTS_PER_MS 63 // F_CPU - 16Mhz | Prescaler - None
#define BTN_STATE !(PINB & (1 << BTN))

#ifndef PATCH_0
#define PATCH_0 break
#endif
#ifndef PATCH_1
#define PATCH_1 break
#endif
#ifndef PATCH_2
#define PATCH_2 break
#endif
#ifndef PATCH_3
#define PATCH_3 break
#endif
#ifndef PATCH_4
#define PATCH_4 break
#endif
#ifndef PATCH_5
#define PATCH_5 break
#endif
#ifndef PATCH_6
#define PATCH_6 break
#endif
#ifndef PATCH_7
#define PATCH_7 break
#endif
#ifndef PATCH_8
#define PATCH_8 break
#endif
#ifndef PATCH_9
#define PATCH_9 break
#endif

////////////////////////
// Globals
////////////////////////

// Interrupt controlled
volatile static uint16_t timer_counter = 0; // Counts number of times TIMER0 has overflown
volatile bool btn_pressed = false;

// Patches

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

/* pot()
 * -----
 * Returns:
 *      The currently set potentiometer value
 * Description:
 *      Reads the current potentiometer value. Depending on the flags
 *      configured in config, this function may be as simple as
 *      simply returning the the current analog value of the pot
 *      input, or as complex as to calculate the average pot
 *      value.
 */

uint8_t inline pot()
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
void reset_timer()
{
        TCNT0 = 0;
        timer_counter = 0;
}

/* ms_passed
 * --------------
 * Description:
 *      Returns the number of miliseconds that have passed 
 *      since the timer has been reset
 */
unsigned long ms_passed()
{
        return timer_counter / TMR_COUNTS_PER_MS;
}

// LED Strip

// Patches

/* update_strip
 * ----------
 * Description:
 *      Updates the strip for the provided patch.
 *      For animations, this function must be called
 *      repeatedly.
 */
void update_strip(uint8_t patch)
{
        switch (patch) {
                case 0 : {
                        PATCH_0;
                        break;
                }
                case 1 : {
                        PATCH_1;
                        break;
                }
                case 2 : {
                        PATCH_2;
                        break;
                }
                case 3 : {
                        PATCH_3;
                        break;
                }
                case 4 : {
                        PATCH_4;
                        break;
                }
                case 5 : {
                        PATCH_5;
                        break;
                }
                case 6 : {
                        PATCH_6;
                        break;
                }
                case 7 : {
                        PATCH_7;
                        break;
                }
                case 8 : {
                        PATCH_8;
                        break;
                }
                case 9 : {
                        PATCH_9;
                        break;
                }
                default: {
                        break;
                }
        }
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
        TCCR0B |= (1 << CS00);  // 1024 prescaling
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

        sei();

        // Patches
        selected_patch = 0;
        update_strip(selected_patch);
        
        // Main loop

        _delay_ms(10);                         // Allow supply voltage to calm down 

        bool prev_btn_state = BTN_STATE;

        while(true)
        {
#if defined(BTN_MIN_RELEASED_READS) && BTN_MIN_RELEASED_READS > 1
                        bool btn_state = btn_min_reads(false, prev_btn_state ? BTN_MIN_RELEASED_READS : 0);
#else
                        bool btn_state = BTN_STATE;
#endif
                if (!prev_btn_state && btn_state) { // Button press
#if defined(BTN_DEBOUNCE_TIME) && BTN_DEBOUNCE_TIME > 0
                        _delay_ms(BTN_DEBOUNCE_TIME);
#endif
                } else if (prev_btn_state && !btn_state) { // Button Released
                        selected_patch = (selected_patch + 1) % NUM_PATCHES;
                        update_strip(selected_patch);
                }

                prev_btn_state = btn_state;
                update_strip(selected_patch);
        }
}