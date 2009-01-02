// Timer subsystem control

#ifndef _TIMER_H_
#define _TIMER_H_


void timers_init(void);


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


// Timer 0
#define TIMER0_CLK 1000

#define T0_PSK_64  ((0<<CS02)|(1<<CS01)|(1<<CS00))
#define T0_PSK_256 ((1<<CS02)|(0<<CS01)|(0<<CS00))

#if (TIMER0_CLK==1000)
	#define OCR0A_CTC_VAL 125
	#define T0_PRESCALE_MSK T0_PSK_64;
#else
	#error TIMER0_CLK unrecognized
#endif


#endif
