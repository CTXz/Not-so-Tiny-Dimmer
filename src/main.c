#include <math.h>
#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "light_ws2812.h"

#define WS2812_DIN PB0
#define WS2812_DIN_MSK (1 << WS2812_DIN)
#define WS2812_PIXELS 8

#define BRIGHTNESS_POT ADCH
#define BRIGHTNESS_POT_ADMUX_MSK (1 << MUX1) // PB4 

#define MAX_BRIGHTNESS 255

#define BTN PB2 // INT0 (See https://bit.ly/3dxVLa5)
#define BTN_DEBOUNCE_TIME 50 // ms

#define FADE_DELAY 10 // ms

#define TMR_COUNTS_PER_SEC 61 // F_CPU - 16Mhz | Prescaler - 1024 | Timer Overflow - 255
                                        // Counts per sec = F_CPU/(Prescaler * Timeroverflow) ~= 61

volatile uint16_t timer_counter = 0; 
volatile bool btn_pressed = false;

uint8_t patches[][3] = {
        {255, 255, 255}, // White
        {255, 74, 33},    // Beige
        {255, 52, 255},   // Purple
        {232, 255, 44},   // Lime
        {106, 255, 255}  // Light Blue
};

uint8_t selected_patch = 0;

#define NUM_PACHES (sizeof(patches) / 3)

uint8_t pxbuf[WS2812_PIXELS * 3];

uint8_t inline apply_brightness(uint8_t val, uint8_t brightness)
{
        return round(((double)brightness/MAX_BRIGHTNESS) * (double)val);
}

void inline fill_pxbuf(uint8_t *pxbuf, uint16_t size, uint8_t r, uint8_t g, uint8_t b, uint8_t brightness)
{
        uint8_t _r = apply_brightness(r, brightness);
        uint8_t _g = apply_brightness(g, brightness);
        uint8_t _b = apply_brightness(b, brightness);

        for (uint16_t i = 0; i < size; i += 3) {
                pxbuf[i + R_OFFSET] = _r;
                pxbuf[i + G_OFFSET] = _g;
                pxbuf[i + B_OFFSET] = _b;
        }
}

void set_ws2812(uint8_t r, uint8_t g, uint8_t b, uint16_t brightness)
{
        fill_pxbuf(pxbuf, sizeof(pxbuf), r, g, b, brightness);
        ws2812_sendarray_mask(pxbuf, sizeof(pxbuf), WS2812_DIN_MSK);
        _delay_us(ws2812_resettime);
}

void fade() {
        static uint8_t r = 255;
        static uint8_t g = 0;
        static uint8_t b = 0;

        static bool r2g = true;

        if (r2g) {
                r--;
                g++;
                r2g = (r > 0);
        } else if (g > 0) {
                g--;
                b++;
        } else {
                b--;
                r++;
                r2g = (r == 255);
        }

        set_ws2812(r, g, b, BRIGHTNESS_POT);
}

void inline reset_timer()
{
        TCNT0 = 0;
        timer_counter = 0;
}

uint16_t inline seconds_passed()
{
        return timer_counter / TMR_COUNTS_PER_SEC;
}

void inline next_patch()
{
        selected_patch = (selected_patch + 1) % NUM_PACHES;
        set_ws2812(patches[selected_patch][0], patches[selected_patch][1], patches[selected_patch][2], BRIGHTNESS_POT);
}

ISR(INT0_vect)
{
        reset_timer();
        cli();
        _delay_ms(BTN_DEBOUNCE_TIME);
        sei();
        btn_pressed = true;
}

ISR(TIMER0_OVF_vect)
{
        timer_counter++;
}

int main()
{       
        // Timer
        TCCR0B |= (1 << CS02) | (1 << CS00); // 1024 prescaling
        TIMSK |= (1 << TOIE0); // Enable Timer interrupts
        TIFR |= (1 << TOV0); // Interrupt on timer overflow

        // Pins
        DDRB |= WS2812_DIN_MSK;
        DDRB &= ~(1 << BTN);
        PORTB |= (1 << BTN);

        // ADC
        ADMUX = (1 << ADLAR);
        ADMUX |= BRIGHTNESS_POT_ADMUX_MSK;

        ADCSRA = 
                (1 << ADEN)  |     // Enable ADC
                (1 << ADSC)  |     // Start first conversion 
                (1 << ADATE) |     // Enable Auto-Trigger
                (1 << ADPS2) |     // set prescaler to 128, bit 2 
                (1 << ADPS1) |     // set prescaler to 128, bit 1 
                (1 << ADPS0);      // set prescaler to 128, bit 0

        // Interrupts
        GIMSK |= (1 << INT0);                 // Enable INT0
        MCUCR |= (1 << ISC01) | (0 << ISC00); // Interrupt on button press (falling due to pull-up)
        sei();

        reset_timer();
        while(1)
        {
                static bool fade_mode = false;

                // Released
                if (btn_pressed) {
                        btn_pressed = !(PINB & (1 << BTN));
                        uint16_t secs_passed = seconds_passed();
                        if (btn_pressed && secs_passed >= 3) {
                                fade();
                                fade_mode = true;
                        } else if (!btn_pressed && secs_passed < 3) {
                                fade_mode = false;
                                next_patch();
                        }
                }

                if (fade_mode) {
                        fade();
                        _delay_ms(FADE_DELAY);
                } else {
                        set_ws2812(patches[selected_patch][0], patches[selected_patch][1], patches[selected_patch][2], BRIGHTNESS_POT);
                }
        }
}