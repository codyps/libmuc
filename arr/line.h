#ifndef LINE_H_
#define LINE_H_
#include <stdint.h>

typedef struct sensor_s {
	uint16_t line;
	uint16_t floor;
	uint16_t past_value;
} sensor_t;

typedef struct line_s {
	sensor_t *sensors;
	uint8_t sensor_ct;
} line_t;

#define LINE_INIT(_sensors) \
	{ _sensors, (sizeof(_sensors) / sizeof(*(_sensors))) }

int16_t line_update(line_t *line, uint16_t delta_time,
		uint16_t *vals);

#endif
