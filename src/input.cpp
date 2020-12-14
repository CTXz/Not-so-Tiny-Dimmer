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
 * 
 */

#include "config.h"
#include "input.h"

// Analog To Digital Converter

/* adc_clear_mux_bits
 * ------------------
 * Description:
 *      Clears the ADMUX registers MUX (ls 4) bits
 *      so that the a new ADC channel can be selected
 */
void adc_clear_mux_bits()
{
        ADMUX &= ~(1 << MUX0| 1 << MUX1 | 1 << MUX2 | 1 << MUX3);
}

/* adc_avg
 * -------
 * Parameters:
 *      adc - ADMUX mask for AVR native builds, ADC pin for Arduino builds 
 *      num_samples - Number of ADC samples to be averaged (max 255)
 * Returns:
 *      Average 8-bit ADC reading
 * Description:
 *      Returns the average ADC reading from n samples
 */

uint8_t adc_avg(uint8_t adc, uint8_t samples)
{
        uint16_t ret = 0;

#ifdef ARDUINO_BUILD
        for (uint8_t i = 0; i < samples; i++)
                ret += analogRead(adc) >> 2;
#else
        adc_clear_mux_bits();
        ADMUX |= adc;

        for (uint8_t i = 0; i < samples; i++) {
                ADCSRA |= (1 << ADSC); // Trigger ADC
                loop_until_bit_is_clear(ADCSRA, ADSC);
                ret += ADCH;
        }

#endif

        return round((double)ret/samples);
}

// Potentiometer

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

uint8_t pot()
{
#ifndef BRIGHTNESS_POT_MISSING
        uint8_t ret;

#if defined(ADC_AVG_SAMPLES) && ADC_AVG_SAMPLES > 1
        ret = adc_avg(BRIGHTNESS_POT_ADMUX_MSK, ADC_AVG_SAMPLES);
#else

#ifdef ARDUINO_BUILD
        ret = analogRead(BRIGHTNESS_POT) >> 2;
#else
        adc_clear_mux_bits();
        ADMUX |= BRIGHTNESS_POT_ADMUX_MSK;
        ADCSRA |= (1 << ADSC); // Trigger ADC
        loop_until_bit_is_clear(ADCSRA, ADSC);
        ret = ADCH;
#endif

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
#else
        return 255;
#endif
}

/* pot_avg()
 * -----
 * Returns:
 *      The average potentiometer value with n samples
 * Description:
 *      Same as pot() except that the sample size for averaging isn't
 *      configuration defined but provided as a parameter.
 *      This is practical if average potentiometer readings
 *      are certainly required and are not simply an option.
 */
uint8_t pot_avg(uint8_t samples) {
#ifndef BRIGHTNESS_POT_MISSING
#ifdef ARDUINO_BUILD
        uint8_t ret = adc_avg(BRIGHTNESS_POT, samples);
#else
        uint8_t ret = adc_avg(BRIGHTNESS_POT_ADMUX_MSK, samples);
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
#else
        return 255;
#endif
}

#if defined(CV_INPUT_ADMUX_MSK) || defined(CV_INPUT)
uint8_t cv()
{
#ifdef ARDUINO
        return analogRead(CV_INPUT) >> 2;
#else
        adc_clear_mux_bits();
        ADMUX |= CV_INPUT_ADMUX_MSK;
        ADCSRA |= (1 << ADSC); // Trigger ADC
        loop_until_bit_is_clear(ADCSRA, ADSC);
        return ADCH;
#endif
}
#endif