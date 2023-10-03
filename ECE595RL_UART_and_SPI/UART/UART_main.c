/**
 * @file UART_main.c
 * @brief Main source code for the UART program.
 *
 * This file contains the main entry point for the UART program.
 *
 * Two UART drivers will be used:
 *  - EUSCI_A0_UART: Used to interact with the serial terminal by sending and receiving data via UART using USB
 *  - EUSCI_A2_UART: Used to transmit data using the P3.3 pin via UART based on the status of the user buttons.
 *                   Receives UART data using the P3.2 pin
 *
 * @author Aaron Nanas
 *
 */

#include <stdint.h>
#include "msp.h"
#include "../inc/Clock.h"
#include "../inc/GPIO.h"
#include "../inc/EUSCI_A0_UART.h"
#include "../inc/EUSCI_A2_UART.h"

// Comment or uncomment the lines to choose the UART program
//#define USE_EUSCI_A0_UART 1
//#define USE_EUSCI_A2_UART 1
#define UART_EXTERNAL_LOOPBACK 1

/**
 * @brief The Transmit_UART_Data function transmits data over UART based on the status of the user buttons.
 *
 * This function transmits different data values over UART based on the status of Button 1 and Button 2.
 * The data transmitted corresponds to the button status according to the following mapping:
 *
 *  button_status      Transmitted Data
 *  -------------      ----------------
 *      0x00               0x00
 *      0x10               0xAA
 *      0x02               0x46
 *      0x12               0xF0
 *
 * @return None
 */
uint8_t Transmit_UART_Data()
{
    uint8_t button_status = Get_Buttons_Status();
    uint8_t tx_data = 0x00;

    switch(button_status)
    {
        // Button 1 and Button 2 are pressed
        case 0x00:
        {
            tx_data = 0x00;
            EUSCI_A2_UART_OutChar(tx_data);
            break;
        }

        // Button 1 is pressed
        // Button 2 is not pressed
        case 0x10:
        {
            tx_data = 0xAA;
            EUSCI_A2_UART_OutChar(tx_data);
            break;
        }

        // Button 1 is not pressed
        // Button 2 is pressed
        case 0x02:
        {
            tx_data = 0x46;
            EUSCI_A2_UART_OutChar(tx_data);
            break;
        }

        // Button 1 and Button 2 are not pressed
        case 0x12:
        {
            tx_data = 0xF0;
            EUSCI_A2_UART_OutChar(tx_data);
            break;
        }
    }
    return tx_data;
}

#ifdef USE_EUSCI_A0_UART
int main(void)
{
    uint32_t user_value;

    char group_buffer[32];
    char name_buffer[128];

    // Initialize the 48 MHz Clock
    Clock_Init48MHz();

    // Initialize the built-in red LED
    LED1_Init();

    // Initialize the buttons
    Buttons_Init();

    // Initialize EUSCI_A0_UART
    EUSCI_A0_UART_Init_Printf();

    // Turn off the red LED at the start
    LED1_Output(RED_LED_OFF);

    printf("\nStart of EUSCI_A0_UART Test\n");
    printf("---------------------------");

    printf("\nOutChar Test - Character 1: ");
    EUSCI_A0_UART_OutChar('A');

    printf("\nOutChar Test - Character 2: ");
    EUSCI_A0_UART_OutChar('B');

    printf("\nOutChar Test - Character 3: ");
    EUSCI_A0_UART_OutChar('C');

    printf("\nOutString Test - String 1: ");
    EUSCI_A0_UART_OutString("ABCDEFGHIJKLMNOPQRSTUVWXYZ");

    printf("\nOutString Test - String 2: ");
    EUSCI_A0_UART_OutString("abcdefghijklmnopqrstuvwxyz");

    printf("\nOutString Test - String 3: ");
    EUSCI_A0_UART_OutString("1234567890");

    printf("\n\nInUDec Test\nEnter an unsigned decimal value: ");
    user_value = EUSCI_A0_UART_InUDec();

    printf("\nOutUDec Value: ");
    EUSCI_A0_UART_OutUDec(user_value);

    printf("\nOutSDec Value: ");
    EUSCI_A0_UART_OutSDec(-user_value);

    printf("\nOutUFix Value: ");
    EUSCI_A0_UART_OutUFix(user_value);

    printf("\n\nInUHex Test\nEnter an unsigned hexadecimal value: ");
    user_value = UART0_InUHex();

    printf("\nOutUHex Value: ");
    EUSCI_A0_UART_OutUHex(user_value);

    printf("\n\nInString Test\nEnter your lab group number: ");
    EUSCI_A0_UART_InString(group_buffer, 32);

    printf("\n\nEnter your names: ");
    EUSCI_A0_UART_InString(name_buffer, 128);

    printf("\n\nYour group number: ");
    EUSCI_A0_UART_OutString(group_buffer);

    printf("\nStudents: ");
    EUSCI_A0_UART_OutString(name_buffer);

    printf("\n---------------------------\n");
    printf("End of EUSCI_A0_UART Test\n");

    // Toggle the red LED at the end of the EUSCI_A0_UART test
    while(1)
    {
        LED1_Output(RED_LED_ON);
        Clock_Delay1ms(1000);
        LED1_Output(RED_LED_OFF);
        Clock_Delay1ms(1000);
    }
}
#endif

#ifdef USE_EUSCI_A2_UART
int main(void)
{
    // Initialize local variable to store UART TX data
    uint8_t UART_TX_Data = 0x00;

    // Initialize the 48 MHz Clock
    Clock_Init48MHz();

    // Initialize the built-in red LED
    LED1_Init();

    // Initialize the buttons
    Buttons_Init();

    // Initialize EUSCI_A2_UART
    EUSCI_A2_UART_Init();

    // Turn on the red LED
    LED1_Output(RED_LED_ON);

    while(1)
    {
        UART_TX_Data = Transmit_UART_Data();
        Clock_Delay1ms(100);
    }
}
#endif

#ifdef UART_EXTERNAL_LOOPBACK
#define BUFFER_LENGTH 255

uint8_t TX_Buffer[BUFFER_LENGTH];
uint8_t RX_Buffer[BUFFER_LENGTH];


/**
 * @brief The UART_Ramp_Data function sends the numbers 0 to 255 and reads the UART bus.
 *
 * This function is used to test the UART module in a loop-back test fashion. This is done by using the EUSCI_A2_UART_OutChar() and EUSCI_A2_UART_InChar()
 * functions to transmit the values from 0 to 255 and receive data from the bus simultaneously. Another function is used to verify that both data are the same.
 *
 * @param None
 *
 * @return None
 */
void UART_Ramp_Data()
{

    for (int i = 0; i <= BUFFER_LENGTH; i++){
        TX_Buffer[i] = i;
        EUSCI_A2_UART_OutChar(i);
        RX_Buffer[i] = EUSCI_A2_UART_InChar();
    }

}

/**
 * @brief The Validate_UART_Loopback function verifies if the data sent and data received is the same.
 *
 * This function is used to verify whether loop-back test was successful or not by comparing the data sent in TX_Buffer and data received in RX_Buffer.
 * It prints the content of both buffers and outputs a warning if they don't match.
 *
 * @param None
 *
 * @return None
 */
void Validate_UART_Loopback()
{
    for (int i = 0; i <= BUFFER_LENGTH; i++){
        printf("TX Data: 0x%02X | RX Data: 0x%02X\n", TX_Buffer[i], RX_Buffer[i]);
        Clock_Delay1us(100);
        if (TX_Buffer[i] != RX_Buffer[i]){
            printf("MISMATCH! TX Data: 0x%02X | RX Data: 0x%02X\n", TX_Buffer[i], RX_Buffer[i]);
        }
    }

}

int main(void)
{
    // Initialize local variable to store UART TX data
    uint8_t UART_TX_Data = 0x00;

    // Initialize local variable to store UART RX data
    uint8_t UART_RX_Data = 0x00;

    // Use a flag to run a specific code block once
    uint8_t run_once = 0x01;

    // Initialize the 48 MHz Clock
    Clock_Init48MHz();

    // Initialize the built-in red LED and the RGB LED
    LED1_Init();
    LED2_Init();

    // Initialize the buttons
    Buttons_Init();

    // Initialize EUSCI_A2_UART
    EUSCI_A2_UART_Init();

    // Initialize EUSCI_A0_UART
    EUSCI_A0_UART_Init_Printf();

    // Turn on the red LED
    LED1_Output(RED_LED_ON);

    while(1)
    {
//        UART_TX_Data = Transmit_UART_Data();
//        UART_RX_Data = EUSCI_A2_UART_InChar();
//        printf("TX Data: 0x%02X | RX Data: 0x%02X\n", UART_TX_Data, UART_RX_Data);
//        Clock_Delay1ms(100);

        if (run_once == 0x01)
        {
            run_once = 0x00;
            UART_Ramp_Data();
            Validate_UART_Loopback();
        }
    }
}
#endif
