
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <inttypes.h>

#include "mydefs.h"
#include "LCD_functions.h"
#include "LCD_driver.h"
#include "button.h"

void Initialization(void);
void Delay(unsigned int millisec);
void OSCCAL_calibration(void);

int main(void)
{    
    PGM_P statetext = PSTR("BF JS Demo");
	uint8_t input, buttons;
	uint8_t last_buttons = KEY_NULL;
	
    Initialization();
	
    sei(); 
		
    for (;;) 
    {
        if (statetext) {
			LCD_puts_f(statetext, 1);
			LCD_Colon(0);
			statetext = NULL;
		}
    
		input = getkey();           // Read buttons
		
		switch (input) {
			case KEY_ENTER:
				statetext = PSTR("ENTER");
				break;
			case KEY_NEXT:
				statetext = PSTR("RIGHT");
				break;
			case KEY_PREV:
				statetext = PSTR("LEFT");
				break;
			case KEY_PLUS:
				statetext = PSTR("UP");
				break;
			case KEY_MINUS:
				statetext = PSTR("DOWN");
				break;
			default:
				break;
		}
    
		// Check if the joystick has been in the same position for some time, 
        // then activate auto press of the joystick
        buttons = (~PINB) & PINB_MASK;
        buttons |= (~PINE) & PINE_MASK;
        
        if( buttons != last_buttons ) 
        {
            last_buttons = buttons;
            gAutoPressJoystick = FALSE;
        }
        else if( buttons )
        {
            if( gAutoPressJoystick == TRUE)
            {
                PinChangeInterrupt();
                gAutoPressJoystick = AUTO;
            }
            else    
                gAutoPressJoystick = AUTO;
        }
    } 
	
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
    OSCCAL_calibration();       // calibrate the OSCCAL byte
    
    CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
    // set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
    CLKPR = (1<<CLKPS1) | (1<<CLKPS0);

    // Disable Analog Comparator (power save)
    ACSR = (1<<ACD);
	
	// Disable Digital input on PF0-2 (power save)
    DIDR0 = (7<<ADC0D);

	// Enable pullups
	PORTB = (15<<PB0);       
	PORTE = (15<<PE4);
   
    Button_Init();              // Initialize pin change interrupt on joystick
	
    LCD_Init();                 // initialize the LCD
}


/*****************************************************************************
*
*   Function name : Delay
*
*   Returns :       None
*
*   Parameters :    unsigned int millisec
*
*   Purpose :       Delay-loop
*
*****************************************************************************/
void Delay(unsigned int millisec)
{
    // mt, int i did not work in the simulator:  int i; 
	uint8_t i;
    
    while (millisec--) {
		for (i=0; i<125; i++) {
			asm volatile ("nop"::);
		}
	}
}

/*****************************************************************************
*
*   Function name : OSCCAL_calibration
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Calibrate the internal OSCCAL byte, using the external 
*                   32,768 kHz crystal as reference
*
*****************************************************************************/
void OSCCAL_calibration(void)
{
    unsigned char calibrate = FALSE;
    int temp;
    unsigned char tempL;

    CLKPR = (1<<CLKPCE);        // set Clock Prescaler Change Enable
    // set prescaler = 8, Inter RC 8Mhz / 8 = 1Mhz
    CLKPR = (1<<CLKPS1) | (1<<CLKPS0);
    
    TIMSK2 = 0;             //disable OCIE2A and TOIE2

    ASSR = (1<<AS2);        //select asynchronous operation of timer2 (32,768kHz)
    
    OCR2A = 200;            // set timer2 compare value 

    TIMSK0 = 0;             // delete any interrupt sources
        
    TCCR1B = (1<<CS10);     // start timer1 with no prescaling
    TCCR2A = (1<<CS20);     // start timer2 with no prescaling

    while((ASSR & 0x01) | (ASSR & 0x04));       //wait for TCN2UB and TCR2UB to be cleared

    Delay(1000);    // wait for external crystal to stabilise
    
    while(!calibrate)
    {
        cli(); // mt __disable_interrupt();  // disable global interrupt
        
        TIFR1 = 0xFF;   // delete TIFR1 flags
        TIFR2 = 0xFF;   // delete TIFR2 flags
        
        TCNT1H = 0;     // clear timer1 counter
        TCNT1L = 0;
        TCNT2 = 0;      // clear timer2 counter
           
        // shc/mt while ( !(TIFR2 && (1<<OCF2A)) );   // wait for timer2 compareflag    
        while ( !(TIFR2 & (1<<OCF2A)) );   // wait for timer2 compareflag

        TCCR1B = 0; // stop timer1

        sei(); // __enable_interrupt();  // enable global interrupt
    
        // shc/mt if ( (TIFR1 && (1<<TOV1)) )
        if ( (TIFR1 & (1<<TOV1)) )
        {
            temp = 0xFFFF;      // if timer1 overflows, set the temp to 0xFFFF
        }
        else
        {   // read out the timer1 counter value
            tempL = TCNT1L;
            temp = TCNT1H;
            temp = (temp << 8);
            temp += tempL;
        }
    
        if (temp > 6250)
        {
            OSCCAL--;   // the internRC oscillator runs to fast, decrease the OSCCAL
        }
        else if (temp < 6120)
        {
            OSCCAL++;   // the internRC oscillator runs to slow, increase the OSCCAL
        }
        else
            calibrate = TRUE;   // the interRC is correct
    
        TCCR1B = (1<<CS10); // start timer1
    }
}
