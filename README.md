# AVRBITS #

Various subprojects for AVR's sharing a small library of common code.

* mctrl - motor control. arduino. HDLC-like framing over USART. Interrupt
  driven. buffers packets in statically allocated space.

* arr - Code for the arduino (atmega328p), attempting to get usart message
  parsing and servo control functioning.

* lline - old linefollower code for an atiny861 

* heavy - a atmega644p with attached magnetic card reader and (eventually)
  lcd display.

* common - some code that is useful in general.  async adc macro generated
  queuing motor control servo control i2c noblock - currently borked for
  unknown reasons, bus is unable to leave the transmitting state for the
  stop state spi noblock - needs to be reworked spi blocking - limited and
  unused presently, useful @ high spi clocks.
