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
   * Description: Defines timing related functions.
   * 
   */


#include <avr/io.h>
#include <avr/interrupt.h>

#ifdef ARDUINO_BUILD
#include <Arduino.h>
#endif

#include "time.h"

#ifdef ARDUINO_BUILD

unsigned long start = millis();

#else

// Interrupt controlled
volatile static unsigned long timer_counter = 0; // Counts number of times TIMER0 has overflown

/* ISR(TIMER0_OVF_vect)
 * --------------------
 * Description:
 *      Increases the timer counter every time
 *      timer0 overflows.
 */
ISR(TIMER0_OVF_vect)
{
        timer_counter++;
}

#endif

/* reset_timer
 * -----------
 * Description:
 *      Resets the the timer.
 */
void reset_timer()
{
#ifdef ARDUINO_BUILD
        start = millis();
#else
        TCNT0 = 0;
        timer_counter = 0;
#endif
}

/* ms_passed
 * --------------
 * Description:
 *      Returns the number of miliseconds that have passed 
 *      since the timer has been reset
 */
unsigned long ms_passed()
{
#ifdef ARDUINO_BUILD
        return millis() - start;
#else
        return timer_counter / TMR_COUNTS_PER_MS;
#endif
}
