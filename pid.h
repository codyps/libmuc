#ifndef PID_H_
#define PID_H_ 1

#define PID_SCALE 1024L

struct pid_const_vals {
	int32_t p;
	int32_t i;
	int32_t d;
	int16_t ilimit;
};

struct pid {
	struct pid_const_vals k;

	int16_t integral;

	int16_t previous_error;

	int16_t target;
};

#define pid_set_goal(pid, sp) ((pid).target = (sp))


#define PID_INITIALIZER(ikp, iki, ikd, imaxi)     \
	{ .k = { .p = (ikp), .d = (ikd), .i = (iki), .ilimit = (imaxi)} }

int16_t pid_update(struct pid *pid, int16_t curr_point);

#endif
