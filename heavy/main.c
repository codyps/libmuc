/*
 * Axon (atmega640)
 	general initialization and main loop
 */

#include "defines.h"
#include "common.h"

#include <stdio.h>
#include <stdlib.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "util.h"

#include "../common/spi_io.h"
#include "queue.h"

/*
 Card Reader:
  Vcc -> Power rail.
  Data -> PA0 (PCINT0)
  Clock-> PA1 (PCINT1)
  Card Motion -> PA2 (..)
  Card Detect -> PA3
  Card End Stop -> PA4
  Gnd -> gnd rail.
*/

#define MAGR_DATA 0
#define MAGR_CLK 1
#define MAGR_MOTION 2
#define MAGR_CDETECT 3
#define MAGR_ENDSTOP 4

#define MAGR_BITFIELD (\
  (1<<MAGR_DATA) | (1<<MAGR_CLK) | (1<<MAGR_MOTION) |\
  (1<<MAGR_CDETECT) | (1<<MAGR_ENDSTOP)\
  )

void init(void) {
	power_all_disable();

  clock_prescale_set(clock_div_1);

  PORTA &= (uint8_t) ~( MAGR_BITFIELD );
  DDRA  &= (uint8_t) ~( MAGR_BITFIELD );

  PCICR |= ( 1<<PCIE0 );
  PCMSK0|= ( MAGR_BITFIELD );

  spi_io_init();
  sei();  
  
  stdout = spi_io;

	printf_P(PSTR("\n\n[main init done]\n\n"));
}

ISR(SIG_PIN_CHANGE0) {
  static uint8_t pin =  0xFF; // all pins default high
  uint8_t new_pin = PINA;

  uint8_t pin_diff = pin ^ new_pin;

  static bool data;

  if ( pin_diff & 1<<MAGR_DATA ) {
    // data = !MAGR_DATA    
    data = (bool) ~(new_pin & 1<<MAGR_DATA);
  }
  if ( pin_diff & 1<<MAGR_CLK ) {
    // read done on falling edge.
    if ( new_pin & 1<<MAGR_CLK ) {
      //set (line 1, logic 0) 

    }
    else {
      //clear (line 0, logic 1)
      // read data
    }
  }
  if ( pin_diff & 1<<MAGR_MOTION ) {
    if ( new_pin & 1<<MAGR_MOTION) {
      //set (line 1, logic 0) 
      // state = mag card no longer in motion
    }
    else {
      //clear (line 0, logic 1)
      // state = mag card in motion
    }
  }
  if ( pin_diff & 1<<MAGR_CDETECT ) {
    if ( new_pin & 1<<MAGR_CDETECT) {
      //set (line 1, logic 0) 
      // state = card all the way out
    }
    else {
      //clear (line 0, logic 1)
      // state = card begginning in
    }  
  }
  if ( pin_diff & 1<<MAGR_ENDSTOP ) {
    if ( new_pin & 1<<MAGR_ENDSTOP) {
      //set (line 1, logic 0) 
      // state = card begining to move out
    }
    else {
      //clear (line 0, logic 1)
      // state = card all the way in
    }  
  }
}

/*
ISR(BADISR_vect){
}
*/

int main(void) {
	init();
	for(;;) {
        
    
		_delay_ms(400);
	}
	return 0;
}

