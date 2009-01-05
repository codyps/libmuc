/*

File: i2c_master.h

Author: Reymond Dominic A. Tarlengco

Description: Header file for I2C Master Driver Library

Completed: October 2, 2008

Notes: - read notes on i2c_master.c

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

// I2C Bus Specification v2.1 FAST mode timing limits
#ifdef I2C_FAST_MODE
#define I2C_TLOW 1.3
#define I2C_THIGH 0.6

// I2C Bus Specification v2.1 STANDARD mode timing limits
#else
#define I2C_TLOW 4.7
#define I2C_THIGH 4.0
#endif

// Error Message Definitions
#define I2C_NO_DATA 0
#define I2C_DATA_OUTSIDE_RAM 1
#define I2C_UNEXPECTED_START 2
#define I2C_UNEXPECTED_STOP 3
#define I2C_DATA_COLLISION 4
#define I2C_NACK_DATA 5
#define I2C_NACK_ADDRESS 6
#define I2C_MISSING_START 7
#define I2C_MISSING_STOP 8

// Microcontroller Dependent Definitions
#define DDR_USI DDRA
#define PORT_USI PORTA
#define PIN_USI PINA
#define PORT_USI_SDA 0
#define PORT_USI_SCL 2
#define PIN_USI_SDA PINA6
#define PIN_USI_SCL PINA4

// i2c_master.c Function Prototypes
void i2c_init_usi ();
uint8_t I2C_Transceiver_Start (uint8_t *, uint8_t);
uint8_t I2C_Get_State_Info ();
uint8_t I2C_Master_Stop ();
