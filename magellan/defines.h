#ifndef _DEFINES_H_
#define _DEFINES_H_ 

#include "debug.h"

#define F_CPU 8000000

#include <stdint.h>
typedef struct {
	union {
		uint16_t head;
		struct {
		uint8_t head_msb;
		uint8_t head_lsb;
		};
	};
	union {
		int16_t pitch;
		struct {
		uint8_t pitch_msb;
		uint8_t pitch_lsb;
		};
	};
	union {
		int16_t roll;
		struct {
		uint8_t roll_msb;
		uint8_t roll_lsb;
		};
	};
} heading_data_t;

heading_data_t head;

#endif
