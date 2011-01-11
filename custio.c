
#include <stdio.h>
#include <stdint.h>
#include "custio.h"
#include "hex.h"

void puth(uint8_t hex, FILE *f) {
	putc( bin_to_hex4( (hex>>4 ) ), f);
	putc( bin_to_hex4( (hex>>0 ) ), f);
}

void puth2(uint16_t hex, FILE *f) {
	putc(bin_to_hex4( (uint8_t)(hex>>12) ), f);
	putc(bin_to_hex4( (uint8_t)(hex>> 8) ), f);
	putc(bin_to_hex4( (uint8_t)(hex>> 4) ), f);
	putc(bin_to_hex4( (uint8_t)(hex>> 0) ), f);
}

