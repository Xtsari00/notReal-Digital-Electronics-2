/* Includes ----------------------------------------------------------*/
#include <avr/io.h>         // AVR device-specific IO definitions
#include <avr/interrupt.h>  // Interrupts standard C library for AVR-GCC
#include "timer.h"          // Timer library for AVR-GCC
#include <twi.h>            // I2C/TWI library for AVR-GCC
#include <oled.h>
#include <stdlib.h>         // C library. Needed for number conversions
#include <stdio.h>
#include <string.h>

#include <uart.h>

// Slave and internal addresses of temperature/humidity sensor DHT12
#define SENSOR_ADR 0x5c
#define SENSOR_HUM_MEM 0
#define SENSOR_TEMP_MEM 2
#define SENSOR_CHECKSUM 4


/* Global variables --------------------------------------------------*/
// Declaration of "dht12" variable with structure "DHT_values_structure"
struct DHT_values_structure {
    uint8_t hum_int;
    uint8_t hum_dec;
    uint8_t temp_int;
    uint8_t temp_dec;
    uint8_t checksum;
} dht12;

// Readable individual parameters of "gps_string"
struct gps
{
    char GPRMC[5];
    char utc[8];
    char data_status;
    char latitude[15];
    char latitude_hor[1];   
    char longitude[15];
    char longitude_hor[1];
    char sat_speed[10];
    char sat_dir[10];
    char date[8];
} gps_data;

char gps_string[120] = {0};     // GPRMC string buffer

// I/0 parameters and indexes for logical chain
uint8_t char_now = 0; // index of uart_getc()
uint8_t gps_line = 0; // I/0 for processing always one GPRMC line
uint8_t reading = 0; // I/0 for reading true gps data

uint8_t loadscreen = 0; // I/0 of loading screen (allows you to write out stabilized data)
uint8_t loadtime = 0; // I/0 for longer loading screen
uint8_t firstgps = 0; // I/0 to confirm the first line of "gps_string"
uint8_t timer_end = 0; // Timer for loading screen stages
uint8_t station = 0; // Index of actual loading screen stage
uint8_t process_write = 0; // I/0 for write individual parameters of "gps_string"

void serial_monitor();
void reset_string();

// Flag for printing new data from sensor
volatile uint8_t new_sensor_data = 0;


/* Function definitions ----------------------------------------------*/
int main(void)
{
    char string[2];
    short temp_out;

    uart_init(UART_BAUD_SELECT(9600, F_CPU));


    // Init OLED
    oled_init(OLED_DISP_ON);
    oled_clrscr();
    

    oled_charMode(DOUBLESIZE);
    oled_goto_xpix_y(23, 0);
    oled_puts("PROJECT");
    oled_goto_xpix_y(40, 2);
    oled_puts("DE-2");

    // Copy buffer to display RAM
    oled_display();

    // Timer1 (Writing data on serial monitor)
    TIM1_OVF_1SEC
    TIM1_OVF_ENABLE
    // Timer0 (Reading uart data from gps module)
    TIM0_OVF_4MS
    TIM0_OVF_ENABLE
    // Timer2 (Loading screen timer)
    TIM2_OVF_16MS
    TIM2_OVF_ENABLE

    sei();

    // Infinite loop
    while (1) {
        if (new_sensor_data == 1 && loadscreen == 1) {
            // Clear previous value
            oled_charMode(NORMALSIZE);
            oled_gotoxy(0, 0);
            oled_puts("Temperature [C]: ");
            oled_gotoxy(0, 1);
            oled_puts("Humidity    [\%]: ");

            oled_gotoxy(16, 0);
            oled_puts("    ");
            // Display DHT12 temp data
            oled_gotoxy(16, 0);
            itoa(dht12.temp_int, string, 10);
            itoa(dht12.temp_int, temp_out, 10);
            oled_puts(string);
            // uart_puts(string);
            oled_puts(".");
            /// uart_puts(".");
            itoa(dht12.temp_dec, string, 10);
            oled_puts(string);

            // Clear previous value
            oled_gotoxy(16, 1);
            oled_puts("    ");

            // Display DHT12 temp data
            oled_gotoxy(16, 1);
            itoa(dht12.hum_int, string, 10);
            oled_puts(string);
            oled_puts(".");
            itoa(dht12.hum_dec, string, 10);
            oled_puts(string);

            // Display parameters show from GPS module
            oled_gotoxy(0, 2);
            oled_puts("Latitude: ");

            oled_gotoxy(0, 3);
            oled_puts("Longitude: ");

            oled_gotoxy(0, 4);
            oled_puts("Place: ");

            oled_gotoxy(0, 5);
            oled_puts("Time: ");

            oled_gotoxy(0, 6);
            oled_puts("Date: ");

            if (firstgps == 1) {

                oled_gotoxy(12, 2);
                oled_putc(gps_data.latitude_hor[0]);
                oled_gotoxy(13, 2);
                oled_puts("          ");
                oled_gotoxy(14, 2);
                oled_putc(gps_data.latitude[0]);
                oled_putc(gps_data.latitude[1]);
                oled_puts(",");
                oled_putc(gps_data.latitude[2]);
                oled_putc(gps_data.latitude[3]);

                oled_gotoxy(12, 4);
                oled_puts("          ");
                if (temp_out > 20) { // If temperature is upper, then 20°C, then you are probably in a room. If lower, so you are in outside in cold :)  (Winter time usually)
                    oled_gotoxy(12, 4);
                    oled_puts("In room");
                }
                else {
                    oled_gotoxy(12, 4);
                    oled_puts("Outside");
                }

                oled_gotoxy(12, 3);
                oled_putc(gps_data.longitude_hor[0]);
                oled_gotoxy(13, 3);
                oled_puts("          ");
                oled_gotoxy(14, 3);
                oled_putc(gps_data.longitude[1]);
                oled_putc(gps_data.longitude[2]);
                oled_puts(",");
                oled_putc(gps_data.longitude[3]);
                oled_putc(gps_data.longitude[4]);

                oled_gotoxy(12, 6);
                oled_puts("         ");
                oled_gotoxy(12, 6);
                oled_putc(gps_data.date[0]);
                oled_putc(gps_data.date[1]);
                oled_puts(".");
                oled_putc(gps_data.date[2]);
                oled_putc(gps_data.date[3]);
                oled_puts(".");
                oled_putc(gps_data.date[4]);
                oled_putc(gps_data.date[5]);
            }

            oled_display();


            // Do not print it again and wait for the new data
            new_sensor_data = 0;
        }

        // If line is not ready, just loop
        if (gps_line != 1) {
            continue;
        }

        // Write the data from "gps_string" to "gps_data" parameters
        if (gps_line == 1 && process_write == 0) {
            process_write = 1; 
            uint8_t gps_par = 0; // Type of actual data
            uint8_t par_char = 0; // Index of actual parameter char
            
            for (size_t i = 0; i < 120; i++) // Loop for reading "gps_string"
            {
                char string_char = gps_string[i];

                if (string_char == ',') { // switch to next parameter
                    gps_par++;
                    par_char = 0;
                    continue;
                }

                if (string_char == '\0') { // End of "gps_string"
                    break;
                }

                if (gps_par == 0) {gps_data.GPRMC[par_char] = string_char;} // Type of data string
                else if (gps_par == 1) {gps_data.utc[par_char] = string_char;} // Actual UTC time
                else if (gps_par == 2) {gps_data.data_status = string_char;} // Status of satellite
                else if (gps_par == 3) {gps_data.latitude[par_char] = string_char;} // Lattitude
                else if (gps_par == 4) {gps_data.latitude_hor[par_char] = string_char;} // Lattitude direction
                else if (gps_par == 5) {gps_data.longitude[par_char] = string_char;} // Longitude
                else if (gps_par == 6) {gps_data.longitude_hor[par_char] = string_char;} // Longitude direction
                else if (gps_par == 7) {gps_data.sat_speed[par_char] = string_char;} // Speed of satellite
                else if (gps_par == 8) {gps_data.sat_dir[par_char] = string_char;} // Satellite direction
                else if (gps_par == 9) {gps_data.date[par_char] = string_char;} // Actual date

                par_char++;
            }
            if (gps_par >= 9) { // If "gps_string" is longer then it must be, stop parsing
                serial_monitor();
                firstgps = 1;
                gps_line = 0;
            }
            process_write = 0;
            reset_string();
        }

    }

    // Will never reach this
    return 0;
}


/* Interrupt service routines ----------------------------------------*/
/**********************************************************************
 * Function: Timer/Counter1 overflow interrupt
 * Purpose:  Read temperature and humidity from DHT12, SLA = 0x5c.
 **********************************************************************/
ISR(TIMER1_OVF_vect)
{
    static uint8_t n_ovfs = 0;

    n_ovfs++;
    // Read the data every 5 secs
    if (n_ovfs >= 5) {
        loadtime = 1;
        n_ovfs = 0;

        // Test ACK from Temp/Humid sensor
        twi_start();
        if (twi_write((SENSOR_ADR<<1) | TWI_WRITE) == 0) {
            // Set internal memory location
            twi_write(SENSOR_HUM_MEM);
            twi_stop();
            // Read data from internal memory
            twi_start();
            twi_write((SENSOR_ADR<<1) | TWI_READ);
            dht12.hum_int = twi_read(TWI_ACK);
            dht12.hum_dec = twi_read(TWI_ACK);
            dht12.temp_int = twi_read(TWI_ACK);
            dht12.temp_dec = twi_read(TWI_NACK);

            new_sensor_data = 1;
        }
        twi_stop();
    }
    if (loadscreen == 1 && firstgps == 1) { // Time updating every second
        oled_gotoxy(12, 5);
        oled_puts("      ");
        oled_gotoxy(12, 5);
        oled_putc(gps_data.utc[0]);
        oled_putc(gps_data.utc[1]);
        oled_puts(":");
        oled_putc(gps_data.utc[2]);
        oled_putc(gps_data.utc[3]);
        oled_puts(":");
        oled_putc(gps_data.utc[4]);
        oled_putc(gps_data.utc[5]);

        oled_display();
    }
}
/*
Reading dates from the GPS module with NMEA string detection with GPRMC values
*/
ISR(TIMER0_OVF_vect)
{
    if (gps_line == 0) {
        uint8_t veo = uart_getc(); // get char from uart

        if (veo == '$') { // First symbol of every data line
            reading = 1;
            char_now = 0;
            return;
        }

        if (veo == '\n') { // End of data line
            gps_line = 1;
            reading = 0;
            return;
        }

        if (reading == 1) { // Data reading
            gps_string[char_now] = veo;
            char_now++;

            // Checking the required line of gps data
            if (char_now == 1 && veo != 'G') {reset_string();}
            if (char_now == 2 && veo != 'P') {reset_string();}
            if (char_now == 3 && veo != 'R') {reset_string();}
            if (char_now == 4 && veo != 'M') {reset_string();}
            if (char_now == 5 && veo != 'C') {reset_string();}
        }
    }
}


void reset_string() { // Clear "gps_string"
    for (size_t i = 0; i < 120; i++)
    {
        gps_string[i] = '\0';
    }
    gps_line = 0;
}

ISR(TIMER2_OVF_vect)
{
    // Loading screen "PROJECT DE-2"
    if (loadscreen == 0 && loadtime == 1) {
        timer_end++;
    }
    // Loading screen of project name and participants
    if (timer_end >= 120 && loadscreen == 0)
    {
        if (station == 0) {
        oled_clrscr();
        oled_charMode(DOUBLESIZE);
        oled_goto_xpix_y(45, 0);
        oled_puts("G");

        oled_charMode(NORMALSIZE);
        oled_goto_xpix_y(47, 3);
        oled_puts("Migulia");
        

        timer_end = 0;
        station++;

        oled_display();
        }

        else if (station == 1) {
        oled_charMode(DOUBLESIZE);
        oled_goto_xpix_y(60, 0);
        oled_puts("P");

        oled_charMode(NORMALSIZE);
        oled_goto_xpix_y(30, 4);
        oled_puts("Tsariuchenko");

        timer_end = 0;
        station++;

        oled_display();
        }

        else if (station == 2) {
        oled_charMode(DOUBLESIZE);
        oled_goto_xpix_y(74, 0);
        oled_puts("S");

        oled_charMode(NORMALSIZE);
        oled_goto_xpix_y(45, 5);
        oled_puts("Shapoval");

        timer_end = 0;
        station++;

        oled_display();
        }
        // One more empty stage 
        else if (station == 3) {
            timer_end = 0;
            station++;
        }
        else { // End of loading screen
            oled_clrscr();
            loadscreen = 1;
            oled_display();
        }

    }


}

void serial_monitor() // Print data to serial monitor, which can be analyzed
    {
    if (gps_data.data_status == 'V') {
        uart_puts("GPS module signal ignore... ");
        uart_puts("\n");
    }
    else
    {     
        uart_puts(gps_string);
        uart_puts("\n");
    } 

    char string[2];  // String for converting numbers by itoa()

    uart_puts("TEMP: ");
    itoa(dht12.temp_int, string, 10);
    uart_puts(string);
    uart_puts(".");
    itoa(dht12.temp_dec, string, 10);
    uart_puts(string);
    uart_puts("\n");

    uart_puts("HUM: ");
    itoa(dht12.hum_int, string, 10);
    uart_puts(string);
    uart_puts(".");
    itoa(dht12.hum_dec, string, 10);
    uart_puts(string);
    uart_puts("\n"); 
    }