#define I2C_PORT PORTA 
#define I2C_PIN  PINA
#define I2C_DDR  DDRA

#define I2C_SDA  0
#define I2C_SCL  2

#define I2C_PULLUPS 1

#define I2C_CLK 100000
#define I2C_T_LOW  4.7
#define I2C_T_HIGH 4

#define I2C_ACK	 	0
#define I2C_NACK 	1
#define I2C_BUS_LOSS 	2 // Loosing arbitration on MM.

static void i2c_line_high_bb(uint8_t pin) {
	#if I2C_PULLUPS	
	I2C_PORT&=(uint8_t)~(1<<pin);
	#endif
	I2C_DDR&=(uint8_t)~(1<<pin);
}

static void i2c_line_low_bb(uint8_t pin) {
	#if I2C_PULLUPS	
	I2C_PORT|=(1<<pin);
	#endif
	I2C_DDR|=(1<<pin);
}

void i2c_start_bb(void) {
	// expectations: SCL high, bus is free.
	i2c_line_high_bb(I2C_SDA);	
	_delay_us(4);
	i2c_line_low_bb(I2C_SCL);	
}

void i2c_restart_bb(void) {
	// expectations: SCL low, delay done.
	i2c_line_high_bb(I2C_SCL);	
	i2c_line_high_bb(I2C_SDA);	
	_delay_us(4);
	i2c_line_low_bb(I2C_SCL);	
}

void i2c_stop_bb(void) {
	// expectations: SCL *, delay done.
	i2c_line_low_bb(I2C_SDA);
	i2c_line_high_bb(I2C_SCL);
	_delay_us(4);
	i2c_line_high_bb(I2C_SDA);
}			

void i2c_reset_bb(void) {
	I2C_DDR&=(uint8_t)~((1<<I2C_SDA)|(1<<I2C_SCL));  //Set as 'inputs' (bus high/default)
	#if I2C_PULLUPS
	I2C_PORT|=(1<<I2C_SDA)|(1<<I2C_SCL); //Enable Pullups
	#else	
	I2C_PORT&=(uint8_t)~((1<<I2C_SDA)|(1<<I2C_SCL)); //Disable Pullups
	#endif
}

int8_t i2c_write_bb(uint8_t byte) { 
	// expectations	: SCL is low and START has been issued.
	// effects	: SCL low (for time required) , SDA high.
	for(int8_t i=7;i>=0;i++) {	
		
		_delay_us(I2C_T_LOW);
		//set bit
		if (byte>>i&1) {//1
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
		loop_until_bit_is_set(I2C_PIN,I2C_SCL);

		_delay_us(I2C_T_HIGH);//wait 4us			
	
		//lower clock	
		i2c_line_low_bb(I2C_SCL);
	}
	
	i2c_line_high_bb(I2C_SDA); // Release bus.
	_delay_us(I2C_T_LOW);	i2c_line_high_bb(I2C_SCL);
	i2c_line_high_bb(I2C_SCL);
	//look for ack
	uint8_t ack = (I2C_PIN&(1<<I2C_SDA)>>I2C_SDA);
	_delay_us(I2C_T_HIGH);
	i2c_line_low_bb(I2C_SCL);
	_delay_us(I2C_T_LOW);
	return ack;
}

uint8_t i2c_read_bb(int8_t ack) { 
	// expectations	: SCL is low and START has been issued.
	// effects	: SCL low (for time reqired), SDA high.
	uint8_t byte=0;
	for(int8_t i=7;i>=0;i++) {	
		_delay_us(I2C_T_LOW);
		//read bit
		byte |= (((I2C_PIN&(1<<I2C_SCL))>>I2C_SCL)<<i)

		//raise clock
		i2c_line_high_bb(I2C_SCL);

		//check raise
		loop_until_bit_is_set(I2C_PIN,I2C_SCL);

		_delay_us(I2C_T_HIGH);

		//lower clock	
		i2c_line_low_bb(I2C_SCL);
	}
	
	_delay_us(I2C_T_LOW);
	if (ack==I2C_ACK)
		i2c_line_low_bb(I2C_SDA); //send ack
	else 	
		i2c_line_high_bb(I2C_SDA); //send nack
	i2c_line_high_bb(I2C_SCL);
	_delay_us(I2C_T_HIGH);
	i2c_line_low_bb(I2C_SCL);
	i2c_line_high_bb(I2C_SDA); // Release bus.
	_delay_us(I2C_T_LOW);	
	return byte;
}
