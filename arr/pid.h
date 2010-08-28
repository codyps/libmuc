#ifndef PID_H_
#define PID_H_


struct pid_s {
	uint16_t kp;
	uint16_t kd;
	uint16_t ki;

	int16_t last;
	int16_t accum;

	int16_t set_point; /* goal */
} pid_t;


#define PID_SETPOINT(pid, sp) (pid).set_point = (sp)
int16_t pid_update(struct pid_s *pid, int16_t delta_time, int16_t curr_point);

#endif
