#include "defines.h"
#include <avr/io.h>
#include "usi-i2c.h"

struct  {
	uint8_t bit_state;
} usi_i2c;



void i2c_clear() {
	//TODO: clear any pending reads/writes
}

void i2c_start(i2c_packet_t [] packets) {
	//TODO: begin outputing and inputing data
}

int8_t i2c_wait() {
	//if Ongoing communication {
		//while Ongoing communication {}
		return I2C_DONE
}

int8_t i2c_work(i2c_packet_t packet) {
	
}

void init_usi_i2c(void) {
	power_usi_enable();

	USICR = 0; // Disable all.

	// Use PA[2..0]
	USIPP|=(1<<USIPOS);

	DDR_USI &=(uint8_t) ~((1<<PIN_USI_SDA)|(1<<PIN_USI_SCL));
	PORT_USI&=(uint8_t) ~((1<<PIN_USI_SDA)|(1<<PIN_USI_SCL));
	
	USISR=0xF0; // Clear all flags, set timer to 0

/*
		     USI Data Register Clock
USICS1 USICS0 USICLK Source                  4-bit Counter Clock Source
   0      0      0   No Clock                No Clock
                     Software clock strobe   Software clock strobe
   0      0      1   (USICLK)                (USICLK)
                     Timer/Counter0 Compare  Timer/Counter0 Compare
   0      1      X   Match                   Match
   1      0      0   External, positive edge External, both edges
   1      1      0   External, negative edge External, both edges
   1      0      1   External, positive edge Software clock strobe (USITC)
   1      1      1   External, negative edge Software clock strobe (USITC)
*/
	USICR = (0<<USISIE)|(1<<USIOIE)|\ // Interupts: Start Condition, 
		(1<<USIWM1)|(0<<USIWM0)|\ // Mode: 00=disable,01=3w,10=2w,11=2w(SCL held low)
		(0<<USICS1)|(0<<USICS0)|\ // Clock: 
		(0<<USICLK)|(0<<USITC)    // Clock Strobe; Toggle Clock

}

