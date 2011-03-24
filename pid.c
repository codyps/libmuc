/* previous_error = 0
 * integral = 0
 * start:
 *      error = setpoint - actual_position
 *      integral = integral + (error*dt)
 *      derivative = (error - previous_error)/dt
 *      output = (Kp*error) + (Ki*integral) + (Kd*derivative)
 *      previous_error = error
 *      wait(dt)
 *      goto start
 */

#include <stdint.h>
#include "pid.h"

int16_t pid_update(struct pid *pid, int16_t cur_pos)
{
	int16_t error = pid->target - cur_pos;

	pid->integral += error;
	if (pid->integral > pid->k.ilimit) {
		pid->integral = pid->k.ilimit;
	} else if (pid->integral < -pid->k.ilimit) {
		pid->integral = -pid->k.ilimit;
	}

	int16_t derivative = error - pid->previous_error;
	pid->previous_error = error;

	int32_t out = (
		  (pid->k.p * error)
		+ (pid->k.i * pid->integral)
		+ (pid->k.d * derivative)
		) / PID_SCALE;

	if (out > INT16_MAX)
		return INT16_MAX;
	else if (out < -INT16_MAX)
		return -INT16_MAX;
	return (int16_t)out;
}
