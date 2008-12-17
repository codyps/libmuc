#include <stdio.h>
#include <avr/io.h> 
#include <avr/interrupt.h>
#include "Serial_Port.h"


void delay_ms(uint16_t x); // general purpose delay

unsigned char count = 0;

int main (void) 
{ 

	Serial_Port_Init();
	
	sei(); // Enable the Global Interrupt Enable flag so that interrupts can be processed 

	for (;;) // Loop forever 
	{ 
         // Do nothing - echoing is handled by the ISR instead of in the main loop
		 delay_ms(5);
		 count++;
		 printf("Count: %d\n", count);

	}    
} 


//General short delays
void delay_ms(uint16_t x)
{
  uint8_t y, z;
  for ( ; x > 0 ; x--){
    for ( y = 0 ; y < 80 ; y++){
      for ( z = 0 ; z < 40 ; z++){
        asm volatile ("nop");
      }
    }
  }
}
