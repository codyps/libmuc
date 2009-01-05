// Timer subsystem control

#ifndef _TIMER_H_
#define _TIMER_H_

typedef enum {DN, UP} dir_t;

volatile dir_t led_dir_A;
volatile dir_t led_dir_B;


#define LED_A OCR1A
#define LED_B OCR1B

#define LED_STEP_A 0x10
#define LED_STEP_B 0x10

#define LED_TOP_A 0xFFFF
#define LED_TOP_B 0xFFFF


//#define LED_A OCR0A
//#define LED_B OCR0B

//#define LED_STEP_A 0x1
//#define LED_STEP_B 0x1

//#define LED_TOP_A 0xFF
//#define LED_TOP_B 0xFF


#define T2HZ 100


void timers_init(void);
static void timer0_init(void);
static void timer1_init(void);
static void timer2_init(void);

#endif
