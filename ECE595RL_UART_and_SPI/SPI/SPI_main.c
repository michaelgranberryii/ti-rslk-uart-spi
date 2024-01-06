/**
 * @file SPI_main.c
 * @brief Main source code for the SPI program.
 *
 * This file contains the main entry point for the SPI program.
 *
 * Two SPI drivers will be used:
 *  - EUSCI_A3_SPI: Serves as a baseline for the Nokia 5110 LCD driver.
 *    Used to transmit data using the following pins via SPI.
 *       - P9.4 (SCE, Chip Enable)
 *       - P9.5 (SCLK)
 *       - P9.7 (MOSI, Master Out Slave In)
 *  - Nokia5110_LCD: Used to interface with the Nokia 5110 LCD
 *
 * @author Michael Granberry, Abdullah Hendy, Aaron Nanas
 *
 */

#include <stdint.h>
#include "msp.h"
#include "../inc/Clock.h"
#include "../inc/GPIO.h"

// Comment or uncomment the lines to choose the SPI driver
//#define USE_SPI_TEST 1
#define USE_NOKIA_LCD 1

#ifdef USE_SPI_TEST
#include "../inc/EUSCI_A3_SPI.h"
#endif

#ifdef USE_NOKIA_LCD
#include "../inc/Nokia5110_LCD.h"
#endif

#ifdef USE_NOKIA_LCD


/**
 * @brief The Change_Counter_Speed returns a delay value based on the status of the user buttons.
 *
 * This function returns different delay values that can be used with different functions based on the status of Button 1 and Button 2.
 * The data transmitted corresponds to the button status according to the following mapping:
 *
 *  button_status         Delay Value
 *  -------------      ----------------
 *      0x00                1000
 *      0x10                200
 *      0x02                3000
 *      0x12                1000
 *
 * @return delay value
 */
uint16_t Change_Counter_Speed()
{
    uint8_t button_status = Get_Buttons_Status();
    uint16_t clock_delay = 0;
    switch(button_status)
    {
        // Button 1 and Button 2 are pressed
        case 0x00:
        {
            clock_delay = 1000;
            break;
        }

        // Button 1 is pressed
        // Button 2 is not pressed
        case 0x10:
        {
            clock_delay = 200;
            break;
        }

        // Button 1 is not pressed
        // Button 2 is pressed
        case 0x02:
        {
            clock_delay = 3000;
            break;
        }

        // Button 1 and Button 2 are not pressed
        case 0x12:
        {
            clock_delay = 1000;
            break;
        }
    }
    return clock_delay;
}
#endif

#ifdef USE_SPI_TEST
int main()
{
    // Initialize the 48 MHz Clock
    Clock_Init48MHz();

    // Initialize the built-in red LED
    LED1_Init();

    // Initialize the buttons
    Buttons_Init();

    // Initialize SPI_Init
    EUSCI_A3_SPI_Init();

    // Turn on the red LED
    LED1_Output(RED_LED_ON);

    while(1)
    {
        EUSCI_A3_SPI_Command_Write(0xAA);
        EUSCI_A3_SPI_Command_Write(0xF0);
        Clock_Delay1ms(100);
    }
}
#endif

#ifdef USE_NOKIA_LCD

int main()
{
    uint32_t counter = 0;

    // Initialize the 48 MHz Clock
    Clock_Init48MHz();

    // Initialize the built-in red LED
    LED1_Init();

    // Initialize the buttons
    Buttons_Init();

    // Initialize the Nokia 5110 LCD
    Nokia5110_Init();

    // Clear the Nokia 5110 buffer
    Nokia5110_ClearBuffer();

    // Clear the Nokia 5110 LCD
    Nokia5110_Clear();

    Nokia5110_SetCursor(0, 1);

    Nokia5110_OutString("Counter");

    Nokia5110_SetCursor(0, 3);

    Nokia5110_OutUDec(counter);

    // Change Contrast
    Nokia5110_Set_Contrast(250);

    // Turn on the red LED
    LED1_Output(RED_LED_ON);

    while(1)
    {

//        counter = counter + 1;
//        Nokia5110_SetCursor(0, 3);
//        Nokia5110_OutUDec(counter);
//        Clock_Delay1ms(1000);

        // Task 4
        uint16_t clock_delay = Change_Counter_Speed();
        counter = counter + 1;
        Nokia5110_SetCursor(0, 3);
        Nokia5110_OutUDec(counter);
        Nokia5110_SetCursor(0, 5);
        Nokia5110_OutString("Delay");
        Nokia5110_SetCursor(6,  5);
        Nokia5110_OutChar(0x3D);
        Nokia5110_SetCursor(7, 5);
        Nokia5110_OutUDec(clock_delay);
        Clock_Delay1ms(clock_delay);
    }
}
#endif
