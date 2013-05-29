#ifndef PCD8544_H_
#define PCD8544_H_

/* PCD8544 is used as the LCD controller for the Nokia 5110 and Nokia 3310 */

/* H = 0 or 1 */
#define NOP 0x00
#define FUNC_SET(PD, V, H) (0x10 | PD << 2 | V << 1 | H)
/* "write data" */

/* H = 0 */
#define DISP_CTRL(D, E) (0x08 | D << 2 | E )
/* 0 <= Y <= 5 */
#define SET_Y_ADDR(Y) (0x40 | Y)
/* 0 <= X <= 83 */
#define SET_X_ADDR(X) (0x80 | X)

/* H = 1 */
#define TEMP_CTRL(TC) (0x04 | TC) /* 2 bits */
#define BIAS_SYS(BS)  (0x08 | BS) /* 3 bits */
#define SET_VOP(VOP)  (0x40 | VOP) /* 7 bits */

/*
 * PD ? chip is active : chip is in powerdown mode
 * V  ? horizontal addressing : vertical addressing
 * H  ? use basic instruction set : use extended instruction set
 *
 *
 * D & E
 * 00 - blank
 * 10 - normal
 * 01 - all display segments on
 * 11 - inverse video mode
 *
 * TC1 & TC0
 * 00 - V_LCD tmep
 */

#include <muc/muc.h>
#include <stdint.h>

#define SCE   A,0
#define DC    A,1
#define SDIN  A,2
#define SCLK  A,3
#define RESET A,4

#define LCD_X     84
#define LCD_Y     48

/* Max Clock = 4Mhz.
 */

/* @dc - 0 = command, 1 = data, no other value permitted */
static inline void lcd_write(uint8_t dc, uint8_t data)
{
	SBIT(DC) = dc;
	SBIT(SCE) = 0;
	shift_out_msb_rising_edge(SBIT(SDIN), SBIT(SCLK), data);
	SBIT(SCE) = 1;
}

static inline void lcd_write_multi(uint8_t dc, uint8_t *data, uint8_t data_len)
{
	SBIT(DC) = dc;
	SBIT(SCE) = 0;
	uint8_t i;
	for (i = 0; i < data_len; i++)
		shift_out_msb_rising_edge(SBIT(SDIN), SBIT(SCLK), data[i]);
	SBIT(SCE) = 1;
}

static inline lcd_clear(void)
{
	SBIT(DC) = 0;
	SBIT(SCE) = 0;
	for (i = 0; i < LCD_X * LCD_Y / 8; i++)
		shift_out_msb_rising_edge(SBIT(SDIN), SBIT(SCLK), 0x00);
	SBIT(SCE) = 1;
}

#define _A(x) x,ARRAY_SIZE(x)
#define A(x) _A((uint8_t [])x)
static void inline lcd_goto_xy(uint8_t x, uint8_t y)
{
	lcd_write_multi(0, A({
				0x80 | x, /* column */
				0x40 | y, /* row */
			}));
}

/* Does structuring the writes() in groups like lcd_write_multi() make
 * sense?  could we queue a sequence of commands and then emit them?  The
 * arduino library appears to buffer display changes (it has obj.display())
 */

#if 0
static void inline lcd_draw_horiz_line(uint8_t x1, uint8_t y1, uint8_t x2)
{
}
#endif

static inline void lcd_if_init(void)
{
	pin_init_output(DC);
	pin_init_output(SCE);
	pin_init_output(SDIN);
	pin_init_output(SCLK);
	pin_init_output(RESET);
}

static inline void lcd_init(void)
{
	lcd_if_init();
	SBIT(RESET) = 0;
	SBIT(RESET) = 1;
	lcd_write_multi(0, A({
			0x21,	//switch to extended commands
			0x80 | 0x60,	//set value of Vop (controls contrast)
			0x04,	//set temperature coefficient
			0x14,	//set bias mode to 1:48.
			0x20,	//switch back to regular commands
			0x0C,	//enable normal display (dark on light), horizontal addressing
			}));
}

#endif
