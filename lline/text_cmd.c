
#include "common.h"

#include <stdlib.h>

#include "motor.h"
#include "spi_io.h"
#include "adc.h"

void print_adc_get_i(uint8_t channel_i) {
  spi_puts("ADC ");
  spi_puth(channel_i);
  spi_putchar(' ');
  spi_putchar(':');
  spi_puth2(adc_get_i(channel_i));
  spi_putchar('\n');
}

typedef struct text_cmd_s {
  const char text;
  const uint8_t param_ct;
  union {
//  void (* const func0 )(void);
    void (* const func1 )(uint8_t,...);
//  void (* const func2 )(...);
  } func;
} text_cmd_t;

#define CMD(c,param,_func) \
  { .text = (c), .param_ct = (param), .func = {.func1=(_func)} }

text_cmd_t text_cmd_list[] = {
  CMD('m',2,motor_set),
  CMD('a',1,print_adc_get_i)
};
#define TEXT_CMD_LIST_SZ CT(text_cmd_list)


typedef enum input_state_e {INP_WAIT, INP_ATT, INP_CMD_FOUND, INP_CMP_PARAM} input_state_t;

void process_rx(void) {
  input_state_t state = INP_WAIT;
  uint8_t cmd_num;
  uint8_t param_ct = 0;

  if (spi_io_rx_nl>0) {
			spi_io_rx_nl--;
      int c = spi_getchar();
      do {
        switch(state) {
          default:
              state = INP_WAIT;          
          case INP_WAIT:
            if (c == '!')
              state = INP_ATT;
            break;

          case INP_ATT:
            for(uint8_t i=0;i<TEXT_CMD_LIST_SZ;i++) {
              if(text_cmd_list[i].text==c) {
                state = INP_CMD_FOUND;
                cmd_num = i;
                goto TEXT_SCAN_SUCC;
              }
            }
            //invalid command.
            state = INP_WAIT;

            TEXT_SCAN_SUCC:
            break;
          
          case INP_CMD_FOUND:
            // we just found a command (on the last character).
            if (text_cmd_list[cmd_num].param_ct==param_ct) {
              
            }
            break;
        }
        c = spi_getchar();
      } while (c!=EOF);
      // parse new input
	}
}
