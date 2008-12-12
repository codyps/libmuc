
typedef enum {DN, UP} dir_t;

dir_t led_dir_A;
dir_t led_dir_B;
#define LED_A OCR1A
#define LED_B OCR1B
#define LED_STEP_A 0x1 
#define LED_STEP_B 0x1
#define LED_TOP_A 0xFF
#define LED_TOP_B 0xFF


void timers_init(void);
void timer1_init(void);
void timer2_init(void);
