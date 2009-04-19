/*
  attiny 861
 */

#include "defines.h"

#include <stdio.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include <util/delay.h>
#include <util/atomic.h>

#include "timer.h"
//#include "usi-i2c.h"
//#include "i2c_master_bb.h"
#include "i2cmaster.h" // asm
#include "i2c_dev_HMC6343.h"
#include "softuart.h"

void clock_init(void) {
	
	#if   (F_CPU == 1000000)
		clock_prescale_set(clock_div_8);	
	#elif (F_CPU == 2000000)
		clock_prescale_set(clock_div_4);	
	#elif (F_CPU == 4000000)
		clock_prescale_set(clock_div_2);	
	#elif (F_CPU == 8000000)
		clock_prescale_set(clock_div_1);
	#elif (F_CPU == 16000000)
		#warning F_CPU=16Mhz untested	
	#else
		#error "F_CPU Unrecognized"
	#endif
}

static int softuart_stream_putchar( char c, FILE *stream )
{
	if ( c == '\n' ) {
		softuart_putchar( '\r' );
	}
	softuart_putchar( c );

	return 0;
}

FILE suart_out = FDEV_SETUP_STREAM( softuart_stream_putchar, NULL, _FDEV_SETUP_WRITE );

void init(void) {
	//power_all_disable();
	clock_init();
	timers_init();
	softuart_init();
	i2c_init(); //asm lib
	sei();
	DEBUG_LED_DDR|=(1<<DEBUG_LED_POS)|(1<<ERROR_LED_POS);
	DEBUG_LED_FLIP;
	fprintf_P(stderr,PSTR("\nmain: init:\t[done]\n\n"));
	softuart_puts_P("LED BLINKS :P\n");
}

static void  print_bin(uint8_t inp,FILE* stream) {
	for(int8_t j=7; j>=0; --j) {
	   	putc(((inp&(1<<j))>>j)+'0',stream);
	}
}

ISR(BADISR_vect){
	ERROR_LED_FLIP;
}

int main(void){ 	
	init();
	
	for(;;) {
		if (update_heartbeat_led) {
			
			typedef enum {DN, UP} dir_t;
			#define LED_TOP_A 0x3ff
			#define LED_STEP_A 1
			static dir_t led_dir_A=UP;
			uint16_t LED_A;
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				LED_A = OCR1A;
				LED_A += (TC1H<<8);
			}
			if (LED_A>(LED_TOP_A-LED_STEP_A))
				led_dir_A=DN;
			else if (LED_A<LED_STEP_A)
				led_dir_A=UP;
	
			if (led_dir_A==UP)
				LED_A+=LED_STEP_A;
			else LED_A-=LED_STEP_A;
	
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
				TC1H = LED_A>>8;
				OCR1A = LED_A&0xff;
			}
		}
		if (update_head_i2c) {	
			ERROR_LED_FLIP;		
			//Head[2], Pitch[2], Roll[2]
			struct {
				union {
					uint16_t head;
					struct {
					uint8_t head_lsb;
					uint8_t head_msb;
					};
				};
				union {
					int16_t pitch;
					struct {
					uint8_t pitch_lsb;
					uint8_t pitch_msb;
					};
				};
				union {
					int16_t roll;
					struct {
					uint8_t roll_lsb;
					uint8_t roll_msb;
					};
				};
			} heading_data;
			/*i2c_vcommand(HMC6343_ADDR,1,6,HMC6343_POST_HEAD,			\
					&(heading_data.head_msb),&(heading_data.head_lsb), 	\
					&(heading_data.pitch_msb),&(heading_data.pitch_lsb),	\
					&(heading_data.roll_msb),&(heading_data.roll_lsb));
			*/
			i2c_start_wait(HMC6343_ADDR_W);
    			i2c_write(HMC6343_POST_HEAD);                          
			i2c_rep_start(HMC6343_ADDR_R);
			heading_data.head_msb=i2c_read(1);
			heading_data.head_lsb=i2c_read(1);
			heading_data.pitch_msb=i2c_read(1);			
			heading_data.pitch_lsb=i2c_read(1);
			heading_data.roll_msb=i2c_read(1);
			heading_data.roll_lsb=i2c_read(0);
			i2c_stop();

			fprintf_P(&suart_out, PSTR("bearing:  head:%d  pitch:%d  roll:%d \n"),\
				 heading_data.head,heading_data.pitch,heading_data.roll);
			ERROR_LED_FLIP;
		}
	}
	return 0;
}
