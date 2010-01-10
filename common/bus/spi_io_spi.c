/*
SPI_IO over the SPI hardware.
*/

static inline void spi_isr_on()	{SPCR |= (1<<SPIE);}
static inline void spi_isr_off(){SPCR &= (uint8_t)~(1<<SPIE);}

static void spi_io_init_hw(void) {
	power_spi_enable();

	DDR(SPI_PORT) |= (1<<SPI_MISO);
	DDR(SPI_PORT) &= (uint8_t)~((1<<SPI_MOSI)|(1<<SPI_CLK));
	PORT(SPI_PORT)&= (uint8_t)~( (1<<SPI_MOSI) | (1<<SPI_CLK) | (1<<SPI_MISO) );

	//XXX: finish spi.
	SPCR = (1<<SPIE) | // 1 -> enable spi interupt.
		(1<<SPE) |  // 1 -> enable spi.
		(0<<DORD)|  // 0 -> msb first. 1 -> lsb first
		(0<<MSTR)|  // 1 -> master. 0 -> slave.
		(0<<CPOL)|  // 0 -> leading edge = rising, trailing edge = falling
		(0<<CPHA)|  // 0 -> Leading edge = sample, trailing edge = setup. 1 -> reverse.
		(0<<SPR1)|(0<<SPR0); // Master Clock Rate (ignored in slave mode)

}

ISR( SIG_SPI ) {
	// Interupt flag auto cleared by vector execution.

	//transmit (tx)
	if (!q_empty(&tx)) {
		SPDR = q_pop(&tx);
	}
	else SPDR = 0;

	//recieve (rx)
	uint8_t in = SPDR;
	if (in != 0 && !q_full(&rx)) {
		q_push(&rx,in);
  	if (in == '\n')
		spi_io_rx_nl++;
	}

	/*// Alternate if we don't care about losing old data.
	if (USIBR != 0) {
		q_push_o(&rx,USIBR);
	}
	*/
}


