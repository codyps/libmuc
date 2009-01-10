/*
	I2C software bitbang.
*/

/*
#define i2c_line_high_bb(pin)		\
	#if I2C_PULLUPS			\
	I2C_PORT&=(uint8_t)~(1<<pin);	\
	#endif				\
	I2C_DDR&=(uint8_t)~(1<<pin);
	

#define i2c_line_low_bb(pin)		\
	#if I2C_PULLUPS			\
	I2C_PORT|=(1<<pin);		\
	#endif				\
	I2C_DDR|=(1<<pin);		
*/

#include "defines.h"

#include <stdarg.h>
#include <stdint.h>

#include <avr/io.h>
#include <util/delay.h>

#include "i2c_master_bb.h"

#define i2c_wait_us(time) _delay_us(time)
#define i2c_wait_pin_high(pin) loop_until_bit_is_set(I2C_PIN,pin)
#define i2c_wait_pin_low(pin) loop_until_bit_is_clear(I2C_PIN,pin)


static inline void i2c_line_high_bb(uint8_t pin) {
	#if I2C_PULLUPS	
	I2C_PORT&=(uint8_t)~(1<<pin);
	#endif
	I2C_DDR&=(uint8_t)~(1<<pin);
}

static inline void i2c_line_low_bb(uint8_t pin) {
	#if I2C_PULLUPS	
	I2C_PORT|=(1<<pin);
	#endif
	I2C_DDR|=(1<<pin);
}

void i2c_init_bb(void) {
	I2C_PORT&=(uint8_t)~((1<<I2C_SDA)|(1<<I2C_SCL));	
	I2C_DDR&=(uint8_t)~((1<<I2C_SDA)|(1<<I2C_SCL));  //Set as 'inputs' (bus high/default)
	#if I2C_PULLUPS
	I2C_PORT|=(1<<I2C_SDA)|(1<<I2C_SCL); //Enable Pullups
	#else
	I2C_PORT&=(uint8_t)~((1<<I2C_SDA)|(1<<I2C_SCL)); //Disable Pullups
	#endif
}

void i2c_start_bb(void) {
	// expectations: SCL & SDA high, bus is free.	
	i2c_wait_us(I2C_T_SU_STA);
	i2c_line_low_bb(I2C_SDA);
	i2c_wait_pin_low(I2C_SDA);
	i2c_wait_us(I2C_T_HD_STA);
	i2c_line_low_bb(I2C_SCL);
}

void i2c_restart_bb(void) {
	// expectations	: i2c_init_bb called 
	// effects	: SCL low,delay needed; SDA low. 
	i2c_line_high_bb(I2C_SDA);
	i2c_line_high_bb(I2C_SCL);
	i2c_wait_pin_high(I2C_SCL);	
	i2c_wait_us(I2C_T_SU_STA);
	i2c_line_low_bb(I2C_SDA);
	i2c_wait_pin_low(I2C_SDA);
	i2c_wait_us(I2C_T_HD_STA);
	i2c_line_low_bb(I2C_SCL);	
}

void i2c_stop_bb(void) {
	// expectations	: SCL low, delay needed
	// effects	: SDA & SCL high.
	
	//i2c_wait_us(I2C_T_LOW);
	i2c_line_low_bb(I2C_SDA);
	i2c_line_high_bb(I2C_SCL);
	i2c_wait_pin_low(I2C_SDA);
	i2c_wait_pin_high(I2C_SCL);
	i2c_wait_us(I2C_T_SU_STO);
	i2c_line_high_bb(I2C_SDA);
}			



int8_t i2c_write_bb(uint8_t byte) { 
	// expectations	: SCL is low,d+ and START has been issued.
	// effects	: SCL low,d+; SDA high.
	for(int8_t i=7;i>=0;--i) {	
		
		i2c_wait_us(I2C_T_LOW);
		//set bit
		if ((byte>>i)&1) {//1
			i2c_line_high_bb(I2C_SDA);
			//arbitration: check bit
			if (I2C_PIN&(1<<I2C_SDA))
				return I2C_BUS_LOSS;
		}
		else {		//0
			i2c_line_low_bb(I2C_SDA);			
		}

		//raise clock
		i2c_line_high_bb(I2C_SCL);

		//synchronization: check raise
		i2c_wait_pin_high(I2C_SCL);

		i2c_wait_us(I2C_T_HIGH);
	
		//lower clock	
		i2c_line_low_bb(I2C_SCL);
	}
	// SDA:?,n; SCL:low,d	

	i2c_line_high_bb(I2C_SDA); // Release bus.
	i2c_wait_us(I2C_T_LOW);
	
	i2c_line_high_bb(I2C_SCL);
	//look for ack
	uint8_t ack = (I2C_PIN&(1<<I2C_SDA)>>I2C_SDA);
	i2c_wait_us(I2C_T_HIGH);
	
	i2c_line_low_bb(I2C_SCL);
	return ack;
}

uint8_t i2c_read_bb(int8_t ack) { 
	// expectations	: SCL is low,d+; START has been issued.
	// effects	: SCL low,d+; SDA high.
	uint8_t byte=0;
	for(int8_t i=7;i>=0;--i) {	
		i2c_wait_us(I2C_T_LOW);
		//read bit
		byte |= (((I2C_PIN&(1<<I2C_SCL))>>I2C_SCL)<<i);

		//raise clock
		i2c_line_high_bb(I2C_SCL);

		//check raise
		i2c_wait_pin_high(I2C_SCL);		

		i2c_wait_us(I2C_T_HIGH);

		//lower clock	
		i2c_line_low_bb(I2C_SCL);
	}
	
	i2c_wait_us(I2C_T_LOW);
	if (ack==I2C_ACK)
		i2c_line_low_bb(I2C_SDA); //send ack
	else 	
		i2c_line_high_bb(I2C_SDA); //send nack
	i2c_line_high_bb(I2C_SCL);
	i2c_wait_pin_high(I2C_SCL);		
	i2c_wait_us(I2C_T_HIGH);
	i2c_line_low_bb(I2C_SCL);
	i2c_line_high_bb(I2C_SDA); // Release bus.	
	return byte;
}


int8_t i2c_trans_bb(uint8_t addr, uint8_t len, uint8_t * data) {
	// expectations	: bus is in claimed state, start/restart called.
	//			addr in 8 bit form, len non-zero
	// effects	: bus is in claimed state.
	int8_t ack;
	ack = i2c_write_bb(addr); // address.
	if (ack != I2C_ACK) return I2C_DEVICE_NACK+ack;
	if (!(len)) return I2C_ZERO_LEN;
	if(((addr)&(uint8_t)(1))==I2C_MODE_WRITE) {	
		do {	
			ack=i2c_write_bb(*(data++));
			if (ack != I2C_ACK) return I2C_TRANS_END+ack;
		
		} while (--len);
	}
	else {//mode==I2C_MODE_READ
		while (--len) {	
			*(data++) = i2c_read_bb(I2C_ACK);
		} 
		*(data) = i2c_read_bb(I2C_NACK);
	}
	return I2C_TRANS_COMP;
}

int8_t i2c_vtrans_bb(uint8_t addr, uint8_t len, ...) {
	// expectations	: bus is in claimed state, start/restart called.
	//			addr in 8 bit form, len non-zero
	// effects	: bus is in claimed state.
	va_list list;
	va_start(list,len);

	int8_t ret = i2c_vatrans_bb(addr,len,list);
	va_end(list);
	return ret;
}

int8_t i2c_vatrans_bb(uint8_t addr, uint8_t len, va_list  list) {
	// expectations	: bus is in claimed state, start/restart called.
	//			addr in 8 bit form, len non-zero
	// effects	: bus is in claimed state.
	int8_t ack;
	ack = i2c_write(addr); // address.
	if (ack != I2C_ACK) return I2C_DEVICE_NACK+ack;
	if (!(len))	return I2C_ZERO_LEN;
	if(((addr)&(uint8_t)(1))==I2C_MODE_WRITE) {			
		do {
			ack=i2c_write_bb(va_next(list,uint8_t));
			if (ack != I2C_ACK)	return I2C_TRANS_END+ack;
		} while (--len);
	}
	else {//mode==I2C_MODE_READ
		while (--len) {	
			*(va_next(list,uint8_t *)) = i2c_read_bb(I2C_ACK);
		}
		*(va_next(list,uint8_t *)) = i2c_read_bb(I2C_NACK);
	}
	return I2C_TRANS_COMP;
}

int8_t i2c_command_bb(uint8_t addr, uint8_t arg_len, uint8_t * args ,uint8_t ret_len, uint8_t * ret) {
	int8_t ack;
	int8_t stat;	
	i2c_restart_bb();
	ack = i2c_trans_bb(addr<<1,arg_len,args);
	if (ack != I2C_TRANS_COMP) {
		if (ack & I2C_TRANS_END)
			stat = I2C_TRANS_END;
		else if (ack & I2C_DEVICE_NACK)
			return I2C_COMM_ARG+ack;
	}
	ack = i2c_trans_bb(addr<<1+1,ret_len,ret);
	if (ack != I2C_TRANS_COMP) {
		if (ack & I2C_DEVICE_NACK)
			return I2C_COMM_ARG+ack;
	}
	return stat<<4+ack;
}

int8_t i2c_vcommand_bb(uint8_t addr, uint8_t cmd, uint8_t arg_len, uint8_t ret_len, ...) {
	va_list args;
	va_start(args,arg_len+ret_len);

	//FIXME: Not implimented.
	
	va_end(args);
	return 0;	
}
