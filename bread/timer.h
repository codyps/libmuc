
typedef enum {DN, UP} dir_t;

volatile dir_t led_dir_A;
volatile dir_t led_dir_B;
#define LED_A OCR1A
#define LED_B OCR1B
#define LED_STEP_A 0x10
#define LED_STEP_B 0x10
#define LED_TOP_A 0xFFFF
#define LED_TOP_B 0xFFFF


void timers_init(void);
void timer1_init(void);
void timer2_init(void);
