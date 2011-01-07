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
	if (pid->integral > pid->ilimit) {
		pid->integral = pid->ilimit;
	} else if (pid->integral < -pid->ilimit) {
		pid->integral = -pid->ilimit;
	}

	int16_t derivative = error - pid->previous_error;
	pid->previous_error = error;

	int32_t out = (
		  (pid->kp * error)
		+ (pid->ki * pid->integral)
		+ (pid->kd * derivative)
		) / PID_SCALE;

	if (out > INT16_MAX)
		return INT16_MAX;
	else if (out < INT16_MIN)
		return INT16_MIN;
	return (int16_t)out;
}
