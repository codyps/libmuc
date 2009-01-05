
#ifndef _TWI_I2C_H_
#define _TWI_I2C_H_

#define SCL_freq 100000 //100kHz


#define SCL_freq_calc (F_CPU/(16+2(TWI_br)*pow(4,TWI_ps)))


void twi_i2c_init(void);



#endif
