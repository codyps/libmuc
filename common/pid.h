#ifndef PID_H_
#define PID_H_ 1

struct pid {
	int16_t kp;
	int16_t kd;
	int16_t ki;

	int16_t integral;
	int16_t previous_error;

	int16_t set_point; /* goal */
};

#define pid_set_goal(pid, sp) ((pid).set_point = (sp))
#define pid_set_kp(pid, n_kp) ((pid).kp = (n_kp))
#define pid_set_kd(pid, n_kd) ((pid).kd = (n_kd))
#define pit_set_ki(pid, n_ki) ((pid).ki = (n_ki))

#define PID_INITIALIZER(ikp, ikd, iki)           \
	{ .kp = (ikp), .kd = (ikd), .ki = (iki) }

int16_t pid_update(struct pid *pid, int16_t delta_time, int16_t curr_point);

#endif
