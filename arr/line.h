#ifndef LINE_H_
#define LINE_H_
#include <stdint.h>

struct sensor {
	uint16_t line;
	uint16_t floor;
	uint16_t past_value;
};

struct line {
	struct sensor *sensors;
	uint8_t sensor_ct;
};

#define LINE_INIT(_sensors) \
	{ _sensors, (sizeof(_sensors) / sizeof(*(_sensors))) }

int16_t line_update(struct line *line, uint16_t delta_time,
		uint16_t *vals);

#endif
