// included by spi_slave_{spi,usi}.c

uint8_t 
spi_slave_put(uint8_t *b, uint8_t n)
{
  uint8_t i;

  i = 0;

  cli();

  while ((i < n) && (tx.len + i < NUM_BUFFER_BYTES)) {
     uint8_t pos = tx.cur+tx.len+i;
     while (pos >= NUM_BUFFER_BYTES) {
          pos -= NUM_BUFFER_BYTES;
     }
     tx.buf[pos] = b[i];
     i++;
  }
  
  tx.len += i;

  sei();

  // Return number of bytes added to tx buffer.
  return i;
}

uint8_t 
spi_slave_get(uint8_t *b, uint8_t n)
{
  uint8_t i;

  i = 0;

  cli();

  while (rx.len && i < n)
  {
    uint8_t cur;
    b[i] = rx.buf[rx.cur];
    //rx.cur = (rx.cur + 1) % NUM_BUFFER_BYTES;
    cur = (rx.cur + 1);
    while (cur >= NUM_BUFFER_BYTES) {
          cur -= NUM_BUFFER_BYTES;
    }
    rx.cur = cur;
    --rx.len;
    ++i;
  }

  sei();

  return i;
}
