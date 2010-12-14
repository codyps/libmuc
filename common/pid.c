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

int16_t pid_update(struct pid *pid, int16_t delta_time, int16_t cur_pos)
{
	int16_t error = pid->setpoint - cur_pos;
	int32_t int_add = ((int32_t)error * delta_time);
	pid->integral = pid->integral + int_add;

	int16_t derivative = (error - previous_error) / delta_time;
	pid->previous_error = error;

	return (pid->kp * error) + (pid->ki * integral) + (pid->kd * derivative);
}
