
#include <stdio.h>

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "../common.h"

#include "ds/glist.h"

#include "spi_io_conf.h"

#include "spi_io.h"

/*          name, fnattr, dattr, data_t, index_t*/
LIST_DEFINE(x, static, volatile, uint8_t, uint8_t);

// NEVER ACCESS THESE DIRECTLY (they are used as 'restrict').
static uint8_t tx_b[SPI_IO_TX_Q_LEN],	rx_b[SPI_IO_RX_Q_LEN]; 

static volatile list_t(x) tx = LIST_INITIALIZER(tx_b);
static volatile list_t(x) rx = LIST_INITIALIZER(rx_b);

volatile uint8_t spi_io_rx_nl;

#ifdef SPI_IO_STANDARD
static int spi_putc(char c, FILE * stream);
static int spi_getc(FILE * stream);
static FILE _spi_io = FDEV_SETUP_STREAM(spi_putc, spi_getc ,_FDEV_SETUP_RW);
FILE * spi_io;
#endif

// Internal Implimentation Specific Functions
static void spi_io_init_hw(void);
static void spi_isr_on (void);
static void spi_isr_off(void);

void spi_io_init(void) {
	spi_io_init_hw();

	#ifdef SPI_IO_STANDARD
	spi_io = &_spi_io;
	#endif

}

#define ISR_GIVE_OK() (!list_full(x)(&rx))
#define ISR_TAKE_OK() (!list_empty(x)(&tx))
#define ISR_GIVE(z) list_pushf(x)(&rx,z)
#define ISR_TAKE()  list_popb(x)(&tx)

// Pick which spi implimentation to use.
#if defined(SPI_IO_USE_SPI)
#include "spi_io_spi.c"
#elif defined(SPI_IO_USE_USI)
#include "spi_io_usi.c"
#elif defined(SPI_IO_USE_USART)
#error "SPI_IO over USART not implimented"
#endif

/*
 Common Code for a spi implimentation.
 
 Requires that the following be defined:
  spi_isr_{on,off}()

  tx, rx (as first func of the following) :
   list_empty(
   list_push(
   list_take(
   list_full(
 */

#ifdef SPI_IO_STANDARD
int spi_putc(char c, FILE * stream) {
	int r;	
	spi_isr_off();
	#ifdef SPI_IO_STD_WAIT
	while(list_full(x)(&tx)) {
		spi_isr_on();
		asm(
			"nop \n\t"
			"nop"
		);
	spi_isr_off();
	}
	#else
	if (list_full(x)(&tx))
		r = EOF;
	else 
	#endif /* SPI_IO_STD_WAIT */
		r = list_push(x)(&tx,c);
	spi_isr_on();
	return r;

}

int spi_getc(FILE * stream) {
	int r;
	spi_isr_off();
	#ifdef SPI_IO_STD_WAIT
	while(list_empty(x)(&rx)) {
		spi_isr_on();
		asm(
			"nop \n\t"
			"nop"
		);
		spi_isr_off();
	}
	#else
	if (list_empty(x)(&rx))
		r = EOF;
	else
	#endif /* SPI_IO_STD_WAIT */
		r = (int) list_get(x)(&rx);
	spi_isr_on();
	return r; 
}

#endif /* SPI_IO_STANDARD */

#if defined(SPI_IO_FAST_ERROR_ZERO)
  #undef EOF
  #define EOF 0
#endif

#if ( defined(SPI_IO_FAST_WAIT) || defined(SPI_IO_FAST_ERROR_ZERO) )
uint8_t spi_getchar(void) {
#else
int spi_getchar(void) {
#endif
	int r;
	spi_isr_off();
	#ifdef SPI_IO_FAST_WAIT
	while(list_empty(x)(&rx)) {
		spi_isr_on();
		asm(
			"nop \n\t"
			"nop"
		);
		spi_isr_off();
	}
	#else
	if (list_empty(x)(&rx))
		r = EOF;
	else
	#endif
		r = (int) list_get(x)(&rx);
	spi_isr_on();
	return r; 
}

static inline void spi_putchar_unsafe(uint8_t ch) {
	// expects spi interupt disabled on entry, leaves it disabled on exit.
	while(list_full(x)(&tx)) {
			spi_isr_on();
			asm(
				"nop\n\t"
				"nop"
			);			
			spi_isr_off();
	}
	list_push(x)(&tx,ch);
}

void spi_puts(const char * string) {
	spi_isr_off();
	while(*string) {
		spi_putchar_unsafe(*string);
		string++;
	}
	spi_isr_on();
}

void spi_o_puts(const char * string) {
	spi_isr_off();
	while(*string) {
		if(list_full(x)(&tx)) {
			spi_isr_on();
			asm(
				"nop\n\t"
				"nop\n\t"
				"nop"
			);			
			spi_isr_off();
		}
		list_pushfo(x)(&tx,*string);
		string++;
	}
	spi_isr_on();
}

void spi_putchar(uint8_t ch) {
	spi_isr_off();
	spi_putchar_unsafe(ch);
	spi_isr_on();
}

static inline uint8_t hex2ascii(uint8_t hex) {
	hex = 0x0F & hex;
	hex = (uint8_t) (hex + '0');
	if (hex > '9')
		return (uint8_t) (hex + 7); // 7 characters between nums and caps.
	else
		return hex;
}

void spi_puth(uint8_t hex) {
	spi_isr_off();

	spi_putchar_unsafe( hex2ascii( (hex>>4 ) ) );
	spi_putchar_unsafe( hex2ascii( (hex>>0 ) ) );

	spi_isr_on();
}

void spi_puth2(uint16_t hex) {
	spi_isr_off();

	spi_putchar_unsafe( hex2ascii( (uint8_t)(hex>>12) ) );
	spi_putchar_unsafe( hex2ascii( (uint8_t)(hex>>8 ) ) );
	spi_putchar_unsafe( hex2ascii( (uint8_t)(hex>>4 ) ) );
	spi_putchar_unsafe( hex2ascii( (uint8_t)(hex>>0 ) ) );

	spi_isr_on();
}
