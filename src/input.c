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