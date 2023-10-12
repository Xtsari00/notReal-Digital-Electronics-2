/***********************************************************************
 * 
 * Blink two LEDs in Arduino-, register-, and library-style. Use 
 * function from delay library to slow down blinking.
 * 
 * ATmega328P (Arduino Uno), 16 MHz, PlatformIO
 *
 * Copyright (c) 2019 Tomas Fryza
 * Dept. of Radio Electronics, Brno University of Technology, Czechia
 * This work is licensed under the terms of the MIT license.
 * 
 **********************************************************************/


/* Defines -----------------------------------------------------------*/
#define LED_GREEN PB5   // PB5 is AVR pin where green on-board LED
                        // is connected
#define LED_RED PB0  
#define PUSH_BUTTON PD2  
#define SHORT_DELAY 250 // Delay in milliseconds
#ifndef F_CPU
# define F_CPU 16000000 // CPU frequency in Hz required for delay funcs
#endif


/* Includes ----------------------------------------------------------*/
#include <avr/io.h>     // AVR device-specific IO definitions
#include <util/delay.h> // Functions for busy-wait delay loops
#include <gpio.h>
//#include <gpio.h>       // GPIO library for AVR-GCC


// -----
// This part is needed to use Arduino functions but also physical pin
// names. We are using Arduino-style just to simplify the first lab.
//#include "Arduino.h"
//#define PB5 13          // In Arduino world, PB5 is called "13"
//#define PB0 8
// -----


/* Function definitions ----------------------------------------------*/
/**********************************************************************
 * Function: Main function where the program execution begins
 * Purpose:  Toggle LEDs and use delay library.
 * Returns:  none
 **********************************************************************/
int main(void)
{
    uint8_t led_value = 0;  // Local variable to keep LED status

    // Set pins where LEDs are connected as output
    // Ver 1: Arduino style
    //pinMode(LED_GREEN, OUTPUT);
    //pinMode(LED_RED, OUTPUT);

    // Ver 2: Low-level (register) style
    //DDRB = DDRB | (1<<LED_GREEN);

    //DDRB = DDRB | (1<<LED_RED);

    // Ver 3: Library function style
    
    GPIO_mode_output(&DDRB, LED_GREEN);
    GPIO_mode_output(&DDRB, LED_RED);
    GPIO_mode_input_pullup(&DDRD, PUSH_BUTTON);
   


    // Infinite loop
    while (1)
    {
        // Pause several milliseconds
        _delay_ms(SHORT_DELAY);
        int res = GPIO_read(&PORTD, PUSH_BUTTON);

        // Change LED value
        if (res == 0 ) {
            led_value = 1;
            // Set pin(s) to HIGH
           //PORTB = PORTB | (1<<LED_GREEN);
           //PORTB = PORTB & ~(1<<LED_RED);
            GPIO_write_high(&PORTB, LED_GREEN);
            GPIO_write_high(&PORTB, LED_RED);

            
        }
        else {
            led_value = 0;
            // Clear pin(s) to LOW
           //PORTB = PORTB & ~(1<<LED_GREEN);
           //PORTB = PORTB | (1<<LED_RED);
            GPIO_write_low(&PORTB, LED_GREEN);
            GPIO_write_low(&PORTB, LED_RED);

        }
    }

    // Will never reach this
    return 0;
}