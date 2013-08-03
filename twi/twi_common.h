#ifndef TWI_COMMON_H_
#define TWI_COMMON_H_

#ifndef F_SCL
#define F_SCL KILO(100ULL) /* default to 100Khz */
#endif

/* Writing 1 to TWINT clears the interrupt flag */
/* "The TWI hardware will not start any operation as long as the TWINT bit in TWCR is set" */
#define TWCR_BASE  (1<<TWINT)
#define TWCR_NACK  TWCR_BASE
#define TWCR_ACK   TWCR_BASE
#define TWCR_START TWCR_BASE
#define TWCR_STOP  TWCR_BASE

static inline bool twi_addr_is_write(uint8_t addr)
{
	return !!(addr & 1);
}
#endif
