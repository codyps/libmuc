// PWM subsystem control (Timer 1)

#ifndef _PWM_H_
#define _PWM_H_


void pwm_init(void);

// Timer 1
#define T1_PLL 64000000
#define PWM_MAX_CLK 100000
#define T1_TOP  0x3FF
//#define T1_PRESCALE (ceil(log((float)T1_PLL/T1_TOP/PWM_MAX_CLK) / log(2)+1))
#define T1_PRESCALE 1
#define T1_CLK T1_PLL/pow(2,T1_PRESCALE-1)
#define T1_PWM_HZ   (T1_CLK/T1_TOP)

#if   (T1_PLL==64000000)
#define LSM_MODE 0
#elif (T1_PLL==32000000)
#define LSM_MODE 1
#else
#error T1_PLL value unrecognized
#endif


#endif
