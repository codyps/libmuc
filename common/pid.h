#ifndef PID_H_
#define PID_H_ 1

#define PID_SCALE 1024L

struct pid {
	int32_t kp;
	int32_t kd;
	int32_t ki;
	int16_t ilimit;

	int16_t integral;
	int16_t previous_error;

	int16_t target;
};

#define pid_set_goal(pid, sp) ((pid).target = (sp))
#define pid_set_kp(pid, n_kp) ((pid).kp = (n_kp))
#define pid_set_kd(pid, n_kd) ((pid).kd = (n_kd))
#define pit_set_ki(pid, n_ki) ((pid).ki = (n_ki))

#define PID_INITIALIZER(ikp, ikd, iki)           \
	{ .kp = (ikp), .kd = (ikd), .ki = (iki) }

int16_t pid_update(struct pid *pid, int16_t curr_point);

#endif
