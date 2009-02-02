/* 
  AVR-Butterfly Example
  - LCD with Driver from Atmel-Source
  - Joystick with GPIO (not Pin-Change Interrupt)
  - additional LCD-output-tests

  - Based on the Atmel Application-Source
  - Based on the avr-gcc/avr-libc Port of the Atmel Application
  
  Martin Thomas, Kaiserslautern, Germany
  <eversmith@heizung-thomas.de>
  
  Compiler: avr-gcc 3.4.3 / avr-libc 1.2.3
  Version : 1.2
  
  History:
  Version 1.0  - Initial (mt)
  Version 1.1  - Fixed side-effects with settings from Bootloader (mt)
  Version 1.2  - included additional demonstrations
*/
  

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "mydefs.h"
#include "LCD_functions.h"
#include "LCD_driver.h"

void DisableBootloaderSettings(void);
void Initialization(void);
void Delay(unsigned int millisec);
void OSCCAL_calibration(void);
void Button_Init(void);
uint8_t Button_Get(void);
void wait_key(uint8_t key_id);
void wait_enter(void);

//Button definitions
#define KEY_NULL    0
#define KEY_ENTER   1
#define KEY_NEXT    2
#define KEY_PREV    3
#define KEY_PLUS    4
#define KEY_MINUS   5

int main(void)
{
	const char* statetext = PSTR("BF IO Demo");
	uint8_t input;
	uint8_t flag;
	char astring[20];
	uint8_t cnt, i, l;
	float r;
	
	/* disable Watchdog early */
	/* Reset WDT */
	wdt_reset();
	/* Write logical one to WDCE and WDE */
	WDTCR |= (1<<WDCE) | (1<<WDE);
	/* Turn off WDT */
	WDTCR = 0x00;
	
	
	DisableBootloaderSettings();
	
	Initialization();
	
	sei(); 
	
	flag = 1;

	while (1) { /* endless loop */
	
		while (flag) {
			if (statetext) {
				LCD_puts_f(statetext, 1);
				LCD_Colon(0);
				/* set statetext to NULL so the text will not
				   be transfered to the LCD_driver over and over again */
				statetext = NULL;
			}
	
			input = Button_Get();           // Read buttons
			
			switch (input) {
				case KEY_ENTER:
					// statetext = PSTR("ENTER");
					flag = 0;
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
		} 

		/* show a "RAM-String" */
		strcpy( astring, "copy from " );
		strcat( astring, "RAM" );
		LCD_puts( astring, 1 );
		wait_enter();
		
		/* show a "Flash/Progmem-String" */
		strcpy_P( astring, PSTR("copy from Flash") );
		LCD_puts( astring, 1 );
		wait_enter();
		
		/* Blink */
		strcpy( astring, "Blinking Scroll" );
		for ( i=0; i<strlen(astring); i++ ) {
			astring[i]+=0x80;
		}
		LCD_puts( astring, 1 );
		wait_enter();
		
#if 0
		/* TODO: flashing does not work non-scroll - bug in Atmel-driver */
		strcpy( astring, "Blink" );
		for ( i=0; i<strlen(astring); i++ ) {
			astring[i]+=0x80;
		}
		LCD_puts( astring, 1 );
		wait_enter();
#endif
		
		
		/* show integer values left aligned*/
		for ( cnt=10; cnt>0; cnt-- ) {
			itoa( (int)cnt, astring, 10 );
			LCD_puts( astring, 1 );
			Delay(1000);
		}
		LCD_puts_f(PSTR("BUMM"), 1);
		wait_enter();
		
		/* show integer values right aligned */
		for ( cnt=0; cnt<160; cnt+=10 ) {
			itoa( (int)cnt, astring, 10 );
			l = strlen( astring );
			if ( l < 7 ) {
				LCD_Clear();
				for ( i=0; i<l; i++ ) {
					LCD_putc( i+6-l, astring[i] );
				}
				LCD_UpdateRequired( 1, 0 );
			}
			else {
				LCD_puts( astring, 1 );
			}
			Delay(1000);
		}
		LCD_puts_f(PSTR("JUHU"), 1);
		wait_enter();
		
		/* write to columns */
		LCD_Clear();
		for ( cnt=0; cnt<6; cnt++) {
			LCD_putc( cnt, cnt+'1' );
			/* UpdateRequired has to be set manualy since
			   putc does not flag the required update like puts */
			LCD_UpdateRequired( 1, 0 );
			Delay(1000);
		}
		for ( cnt=0; cnt<6; cnt++) {
			LCD_putc( cnt, ' ' );
			LCD_UpdateRequired( 1, 0 );
			Delay(1000);
		}
		// wait_enter();
		
		
		/* And last not least the memory-wasting sprintf with a float. 
		   Remind that the printf_flt-library must be linked to use
		   printf with floating-point-numbers (see makefile). */
		r = 0;
		for ( cnt=0; cnt<10; cnt++) {
			// TODO: modify decimal char in LCD_driver.c
			sprintf(astring,"X %4.2f", (double)r);
			LCD_puts( astring, 1 );
			r += 1.1;
			Delay(1000);
		}
		wait_enter();
		
		statetext = PSTR("Restart");
		
		flag = 1;

	}
	
	return 0; /* never reached */
}

#define PINB_MASK ((1<<PINB4)|(1<<PINB6)|(1<<PINB7))
#define PINE_MASK ((1<<PINE2)|(1<<PINE3))
#define BUTTON_O    4   // PUSH
#define BUTTON_A    6   // UP
#define BUTTON_B    7   // DOWN

#define BUTTON_C    2   // LEFT
#define BUTTON_D    3   // RIGHT

/* read joystick-position (no debouncing) */
uint8_t Button_Get(void)
{
	uint8_t key;
	
	if ( !(PINB & (1<<BUTTON_A)) )
		key = KEY_PLUS;
	else if ( !(PINB & (1<<BUTTON_B)) )
		key = KEY_MINUS;
	else if ( !(PINB & (1<<BUTTON_O)) )
		key = KEY_ENTER;
	else if ( !(PINE & (1<<BUTTON_C)) )
		key = KEY_PREV;
	else if ( !(PINE & (1<<BUTTON_D)) )
		key = KEY_NEXT;
	else
		key = KEY_NULL;
	
	return key;
}

/* wait for a key - with debouncing */
void wait_key(uint8_t key_id)
{
	const uint8_t debounce_resetval = 10;
	uint8_t debounce_cnt;
	
	debounce_cnt = debounce_resetval;
	
	while ( debounce_cnt ) {
		if ( Button_Get() == key_id ) {
			debounce_cnt--;
		}
		else {
			debounce_cnt = debounce_resetval;
		}
		Delay(10);
	}
}

/* wait for the enter-key to be pressed and released */
void wait_enter(void)
{
	wait_key(KEY_NULL);
	wait_key(KEY_ENTER);
	wait_key(KEY_NULL);
}

/*****************************************************************************
*
*   Function name : DisableBootloaderSettings
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Disables Settings made by the standard
*                   Bootloader (Interrupts etc.)
*
*****************************************************************************/
void DisableBootloaderSettings(void)
{
	cli();
	ACSR &= ~(1<<ACD);   // re-enable Analog-Comparator
	DIDR0 = 0x00;         // enable all ADC Inputs
	DIDR1 = 0x00;         // DIDR1 and DIDR0 mismatched in org. Bootloader
	                      // set both to default
	PORTB = 0x00;         // disable Port B & D int. Pull-Ups
	PORTE = 0x00;
	PCMSK1 = 0x00;        // disable Pin-Change at Pin B4 & B6
	EIFR  = (1<<PCIF1) | (1<<PCIF0);   // clear Pin-Change Interrupt Flags
	EIMSK = 0x00;         // disable Pin-Change Interrupt
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
	
	LCD_Init();                 // initialize the LCD
	
	Button_Init();
}

/*****************************************************************************
*
*   Function name : Button_Init
*
*   Returns :       None
*
*   Parameters :    None
*
*   Purpose :       Initializes the five button pin
*
*****************************************************************************/
void Button_Init(void)
{
	// Init port pins
	DDRB &= ~(1<<PB7);
	DDRB &= ~(1<<PB6);
	DDRB &= ~(1<<PB4);
	PORTB |= PINB_MASK;
	DDRE = 0x00;
	PORTE |= PINE_MASK;
	
	// enable AIN0, AIN1 input buffers (PE2/PE3)
	DIDR1 &= ~( (1<<AIN0D) | (1<<AIN1D) ); 
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
