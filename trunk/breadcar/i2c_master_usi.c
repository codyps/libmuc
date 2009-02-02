/*

File: i2c_master.c

Author: Reymond Dominic A. Tarlengco

Description: I2C Master Driver Library

Completed: October 2, 2008

Notes: - based on Atmel Application Note AVR310
- no interrupts used, timing based solely on delay
- driver was made on the assumption that the controller
  will be a single master on the bus thus it cannot be used
  in a multi-master bus
- specifically created for ATtiny24, porting to other
  controllers will need modification of register names

Copyright (c) 2008, Reymond Dominic A. Tarlengco
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
    * Neither the name of the author nor the names of its contributors may be
  used to endorse or promote products derived from this software without
  specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

<dominic.tarlengco@gmail.com>

*/

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include "i2c_master_usi.h"

struct i2c_state
{
	uint8_t address_mode : 1;
	uint8_t write_mode : 1;
	uint8_t error_state : 6;
} i2c_state;

// Master Initialization
void i2c_init_usi ()
{
	// Pull SDA and SCL high
	PORT_USI |= (1<<PORT_USI_SDA) | (1<<PORT_USI_SCL);
	// Set SDA and SCL pins as output
	DDR_USI |= (1<<PORT_USI_SDA) | (1<<PORT_USI_SCL);
	USIDR = 0xFF;
	USICR = (0<<USISIE)|(0<<USIOIE)| // Disable USI Generated Interrupts
	(1<<USIWM1)|(0<<USIWM0)| // Set USI to 2-Wire Mode
	(1<<USICS1)|(0<<USICS0)|(1<<USICLK)| // Software Strobe
	(0<<USITC);
	USISR = (1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)| // Clear USI Flags
	(0x00<<USICNT0); // Reset USI Counter
}

uint8_t i2c_master_transfer_usi (uint8_t temp)
{
	USISR = temp; // Set USISR as requested by calling function
	temp =	(0<<USISIE)|(0<<USIOIE)| // Disable USI Generated Interrupts
	   	(1<<USIWM1)|(0<<USIWM0)| // Set USI to 2-Wire Mode
	   	(1<<USICS1)|(0<<USICS0)|(1<<USICLK)| // Software Strobe
	  	(1<<USITC); // Toggle Clock

	// Shift Data
	do
	{
		_delay_us (I2C_TLOW);
		USICR = temp; // SCL Positive Edge
		while (!(PIN_USI&(1<<PIN_USI_SCL))); // Wait for SCL to go high
		_delay_us (I2C_THIGH);
		USICR = temp; // SCL Negative Edge
	} while (!(USISR&(1<<USIOIF))); // Do until transfer is complete

	_delay_us (I2C_TLOW);
	temp = USIDR;
	USIDR = 0xFF;
	DDR_USI |= (1<<PORT_USI_SDA); // Set SDA as output

	return temp; // Return data from USIDR
}

uint8_t i2c_master_stop ()
{
	PORT_USI &= ~(1<<PORT_USI_SDA); // Release SDA internal pullup
	PORT_USI |= (1<<PORT_USI_SCL); // Pull SCL high
	while (!(PIN_USI & (1<<PIN_USI_SCL))); // Wait for SCL to go high
	_delay_us (I2C_THIGH);
	PORT_USI |= (1<<PORT_USI_SDA); // Pull SDA high
	_delay_us (I2C_TLOW);

	#ifdef FRAME_VERIFY
	if (!(USISR & (1<<USIPF))) // Check USI Stop Flag
	{
		i2c_state.error_state = I2C_MISSING_STOP;
		return 0;
	}
	#endif

	return 1;
}

uint8_t I2C_Get_State_Info ()
{
	return (i2c_state.error_state);
}

uint8_t I2C_Transceiver_Start (uint8_t *msg, uint8_t msg_size)
{
	uint8_t temp_USISR_8bit = (1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|
	  (0x00<<USICNT0);
	uint8_t temp_USISR_1bit = (1<<USISIF)|(1<<USIOIF)|(1<<USIPF)|(1<<USIDC)|
	  (0x0E<<USICNT0);

	i2c_state.address_mode = 1;

	if (!(*msg & 0x01)) // Check if message is read/write
		i2c_state.write_mode = 1;
	else
		i2c_state.write_mode = 0;

	PORT_USI |= (1<<PORT_USI_SCL);
	while (!(PIN_USI & (1<<PIN_USI_SCL))); // Wait for SCL to go high

	#ifdef I2C_FAST_MODE
	_delay_us (I2C_THIGH);

	#else
	_delay_us (I2C_TLOW);
	#endif

	// Start Condition
	PORT_USI &= ~(1<<PORT_USI_SDA); // Pull SDA low
	_delay_us (I2C_THIGH);
	PORT_USI &= ~(1<<PORT_USI_SCL); // Pull SCL low
	PORT_USI |= (1<<PORT_USI_SDA);

	do
	{
		if (i2c_state.address_mode || i2c_state.write_mode)
		{
			PORT_USI &= ~(1<<PORT_USI_SCL); // Pull SCL low
			USIDR = *(msg++); // Load data then increment buffer
			i2c_master_transfer_usi (temp_USISR_8bit);

			// Get Acknowledgement from slave
			DDR_USI &= ~(1<<PORT_USI_SDA); // Set SDA as input
			if (i2c_master_transfer_usi (temp_USISR_1bit) & 0x01)
			{
				if (i2c_state.address_mode)
				i2c_state.error_state = I2C_NACK_ADDRESS;
				else
				i2c_state.error_state = I2C_NACK_DATA;
				return 0;
			}

			i2c_state.address_mode = 0;
		}
		else
		{
			DDR_USI &= ~(1<<PORT_USI_SDA); // Set SDA as input
			*(msg++) = i2c_master_transfer_usi (temp_USISR_8bit);
			// Read data then increment buffer
			if (msg_size == 1) // Last byte transmitted
				USIDR = 0xFF; // Load NACK to end transmission
			else
				USIDR = 0x00; // Load ACK
			i2c_master_transfer_usi (temp_USISR_1bit); // Output Acknowledgement
		}
	} while (--msg_size); // Do until all data is read/written

	return 1;
}

