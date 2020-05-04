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
   * Description: Main firmware routines for the Not-so-Tiny dimmer.
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
#include "time.h"

////////////////////////
// Preprocessors
////////////////////////

#if F_CPU != 16000000L
#error "CPU clock (F_CPU) must be set to 16Mhz!"
#endif

#define MAX_BRIGHTNESS 255
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
        uint8_t _ADCSRA = ADCSRA;
        ADCSRA &= ~(1 << ADATE); // Temporarily disable auto triggering

        uint16_t ret = 0;
        for (uint8_t i = 0; i < num_samples; i++) {
                ADCSRA |= (1 << ADSC); // Trigger ADC
                loop_until_bit_is_clear(ADCSRA, ADSC);
                ret += ADCH;
        }

        ADCSRA = _ADCSRA; // Restore auto triggering

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

#ifdef INVERT_POT
        ret = ~ret;
#endif

#if defined(POT_LOWER_BOUND) && POT_LOWER_BOUND > 0
        if (ret <= POT_LOWER_BOUND)
                return 0;
#endif

#if defined(POT_UPPER_BOUND) && POT_UPPER_BOUND < 255
        if (ret >= POT_UPPER_BOUND)
                return 255;
#endif

        return ret;
}

// LED Strip

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

        // Timer 0

        TCCR0B |= (1 << CS00);                // No prescaling
        TIMSK |= (1 << TOIE0);                // Enable Timer interrupts
        TIFR |= (1 << TOV0);                  // Interrupt on timer overflow

#if STRIP_TYPE == NON_ADDR
        DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB4);

        // Enable Fast PWM
        TCCR0A |= (1 << WGM01) | (1 << WGM00);
        TCCR0B |= (0 << WGM02);

        TCCR0A |= (1 << COM0A1) | (0 << COM0A0)  // Non-Inverting PWM on OCR0A/PB0
               |  (1 << COM0B1) | (0 << COM0B1); // Non-Inverting PWM on OCR0B/PB1


        OCR0A = 0;
        OCR0B = 0;

        // Timer 1

	PLLCSR |= (1 << PLLE); // Enable PLL (64 MHz)
	PLLCSR |= (1 << PCKE); // Use PLL as timer clock source

        TCCR1 |= (1 << CS10); // 1024 Prescaler

        GTCCR |= (1 << PWM1B)
              |  (1 << COM1B1) | (0 << COM1B0); // Non-Inverting PWM on OCR1B/PB4

        TCCR1 |= (1 << COM1A0);

        OCR1B = 0;
        OCR1C = 255;
#endif

        // Pins
#if STRIP_TYPE == WS2812
        DDRB |= WS2812_DIN_MSK;               // Set WS2812 Data input pin to output
#endif

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

        while(true) {
                bool btn_state = BTN_STATE;

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