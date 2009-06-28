#include <stdint.h>

int cmp_16(const void * a, const void * b) {
	return *( (uint16_t *) a) - *( (uint16_t *) b);
}
