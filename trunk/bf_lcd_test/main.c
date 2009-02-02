
//  mt - This code has only been used for debugging 
//  It may not be up-to-date. Please refert to the
//  Current LCD-driver code can be found in the
//  application's gcc-port.

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <inttypes.h>

#include "main.h"
#include "lcd_functions.h"
#include "lcd_driver.h"

#define pLCDREG_test (*(char *)(0xEC))

// extern unsigned int LCD_character_table[] PROGMEM;

/*****************************************************************************
*
*   Function name : main
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Contains the main loop of the program
*
*****************************************************************************/
int main(void)
{    
	// mt static char __flash *statetext;
	PGM_P statetext;
	
	// Initial state variables
	statetext = PSTR("AVR BUTTERFLY GCC");

	// Program initalization
    Initialization();
    sei(); // mt __enable_interrupt();
	
	for (;;)            // Main loop
    {
		if (statetext)
        {
			LCD_puts_f(statetext, 1);
			LCD_Colon(0);
			statetext = NULL;   
		}
    } //End Main loop
	
	return 0;
}

/*****************************************************************************
*
*   Function name : Initialization
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Initializate the different modules
*
*****************************************************************************/
void Initialization(void)
{
	/*
	CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable

    // set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
    CLKPR = (1<<CLKPS1) | (1<<CLKPS0);

    // Disable Analog Comparator (power save)
    ACSR = (1<<ACD);

    // Disable Digital input on PF0-2 (power save)
    DIDR1 = (7<<ADC0D);

    // mt PORTB = (15<<PORTB0);       // Enable pullup on 
	PORTB = (15<<PB0);       // Enable pullup on 
    // mt PORTE = (15<<PORTE4);
	PORTE = (15<<PE4);

    sbi(DDRB, 5);               // set OC1A as output
    sbi(PORTB, 5);              // set OC1A high
	*/
                
    LCD_Init();                 // initialize the LCD
}





