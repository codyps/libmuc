#include <stdint.h>
#include "hex.h"

uint8_t bin_to_hex8(uint8_t num) {
	return bin_to_hex4(num>>4) | bin_to_hex4(num);
}

uint8_t bin_to_hex4(uint8_t num) {
	num = 0x0F & num;
	num = (uint8_t) (num + '0');
	if (num > '9')
		return (uint8_t) (num + ('A' - '9' + 1));
	else
		return num;
}
