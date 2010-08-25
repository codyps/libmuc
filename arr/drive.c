#include <stdint.h>
#include "motor.h"
#include "drive.h"

/* CONF */
#define DR_MTR_L 0
#define DR_MTR_R 1
/* ---- */

#define MOTOR_API_SPEED_MAX INT16_MAX

#define ABS(x) (((x)>0)?(x):(-x))
#define MAX(x,y) (((x)>(y))?(x):(y))
void drive_set(int16_t vel, int16_t dir)
{
	int32_t l = vel - dir;
	int32_t r = vel + dir;

	int32_t max = MAX(ABS(l),ABS(r));
	if (max > MOTOR_API_SPEED_MAX) {
		l = (l * MOTOR_API_SPEED_MAX) / max;
		r = (r * MOTOR_API_SPEED_MAX) / max;
	}

	motor_set(DR_MTR_L, l);
	motor_set(DR_MTR_R, r);
}
