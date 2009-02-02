
main.elf:     file format elf32-avr

Disassembly of section .text:

00000000 <__vectors>:
   0:	0c 94 2e 00 	jmp	0x5c	; 0x5c <__ctors_end>
   4:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
   8:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
   c:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  10:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  14:	0c 94 1e 02 	jmp	0x43c	; 0x43c <__vector_5>
  18:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  1c:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  20:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  24:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  28:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  2c:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  30:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  34:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  38:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  3c:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  40:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  44:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  48:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  4c:	0c 94 d0 00 	jmp	0x1a0	; 0x1a0 <__vector_19>
  50:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  54:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  58:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>

0000005c <__ctors_end>:
  5c:	11 24       	eor	r1, r1
  5e:	1f be       	out	0x3f, r1	; 63
  60:	cf ef       	ldi	r28, 0xFF	; 255
  62:	d4 e0       	ldi	r29, 0x04	; 4
  64:	de bf       	out	0x3e, r29	; 62
  66:	cd bf       	out	0x3d, r28	; 61

00000068 <__do_copy_data>:
  68:	11 e0       	ldi	r17, 0x01	; 1
  6a:	a0 e0       	ldi	r26, 0x00	; 0
  6c:	b1 e0       	ldi	r27, 0x01	; 1
  6e:	ec e0       	ldi	r30, 0x0C	; 12
  70:	fb e0       	ldi	r31, 0x0B	; 11
  72:	02 c0       	rjmp	.+4      	; 0x78 <.do_copy_data_start>

00000074 <.do_copy_data_loop>:
  74:	05 90       	lpm	r0, Z+
  76:	0d 92       	st	X+, r0

00000078 <.do_copy_data_start>:
  78:	a6 37       	cpi	r26, 0x76	; 118
  7a:	b1 07       	cpc	r27, r17
  7c:	d9 f7       	brne	.-10     	; 0x74 <.do_copy_data_loop>

0000007e <__do_clear_bss>:
  7e:	11 e0       	ldi	r17, 0x01	; 1
  80:	a6 e7       	ldi	r26, 0x76	; 118
  82:	b1 e0       	ldi	r27, 0x01	; 1
  84:	01 c0       	rjmp	.+2      	; 0x88 <.do_clear_bss_start>

00000086 <.do_clear_bss_loop>:
  86:	1d 92       	st	X+, r1

00000088 <.do_clear_bss_start>:
  88:	a2 39       	cpi	r26, 0x92	; 146
  8a:	b1 07       	cpc	r27, r17
  8c:	e1 f7       	brne	.-8      	; 0x86 <.do_clear_bss_loop>
  8e:	0e 94 7f 00 	call	0xfe	; 0xfe <main>
  92:	0c 94 84 05 	jmp	0xb08	; 0xb08 <_exit>

00000096 <__bad_interrupt>:
  96:	0c 94 00 00 	jmp	0	; 0x0 <__heap_end>

0000009a <clock_init>:
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void clock_init(void) {
  9a:	e1 e6       	ldi	r30, 0x61	; 97
  9c:	f0 e0       	ldi	r31, 0x00	; 0
  9e:	80 81       	ld	r24, Z
  a0:	80 83       	st	Z, r24
	//CLKPR|=(uint8_t)(1<<CLKPCE);
	CLKPR|=0x10000000;
	
	//CLKPR&=(uint8_t)~((1<<CLKPS3)|(1<<CLKPS2));
	//CLKPR&=~((1<<CLKPS3)|(1<<CLKPS2));
	CLKPR&=0x11110011;
  a2:	80 81       	ld	r24, Z
  a4:	81 71       	andi	r24, 0x11	; 17
  a6:	80 83       	st	Z, r24
		//CLKPR|=(uint8_t)(1<<CLKPS1)|(1<<CLKPS0);						//1 Mhz 0011
		CLKPR|=0x00000011;
	#elif F_CPU == 2000000
		CLKPR|=(uint8_t)(1<<CLKPS1); CLKPR&=(uint8_t)~(1<<CLKPS0);	//2 Mhz 0010
	#elif F_CPU == 4000000
		CLKPR|=(uint8_t)(1<<CLKPS0); CLKPR&=(uint8_t)~(1<<CLKPS1);	//4 Mhz 0001
  a8:	80 81       	ld	r24, Z
  aa:	81 60       	ori	r24, 0x01	; 1
  ac:	80 83       	st	Z, r24
  ae:	80 81       	ld	r24, Z
  b0:	8d 7f       	andi	r24, 0xFD	; 253
  b2:	80 83       	st	Z, r24
		1	0	0	0	256
	*/
	
	//Do something with OSCCAL ?
	
}
  b4:	08 95       	ret

000000b6 <init>:



void init(void) {
  b6:	e4 e6       	ldi	r30, 0x64	; 100
  b8:	f0 e0       	ldi	r31, 0x00	; 0
  ba:	80 81       	ld	r24, Z
  bc:	80 61       	ori	r24, 0x10	; 16
  be:	80 83       	st	Z, r24
	power_lcd_disable();
	power_spi_disable();
  c0:	80 81       	ld	r24, Z
  c2:	84 60       	ori	r24, 0x04	; 4
  c4:	80 83       	st	Z, r24
#include <avr/interrupt.h>
#include <util/delay.h>

void clock_init(void) {
	//CLKPR|=(uint8_t)(1<<CLKPCE);
	CLKPR|=0x10000000;
  c6:	e1 e6       	ldi	r30, 0x61	; 97
  c8:	f0 e0       	ldi	r31, 0x00	; 0
  ca:	80 81       	ld	r24, Z
  cc:	80 83       	st	Z, r24
	
	//CLKPR&=(uint8_t)~((1<<CLKPS3)|(1<<CLKPS2));
	//CLKPR&=~((1<<CLKPS3)|(1<<CLKPS2));
	CLKPR&=0x11110011;
  ce:	80 81       	ld	r24, Z
  d0:	81 71       	andi	r24, 0x11	; 17
  d2:	80 83       	st	Z, r24
		//CLKPR|=(uint8_t)(1<<CLKPS1)|(1<<CLKPS0);						//1 Mhz 0011
		CLKPR|=0x00000011;
	#elif F_CPU == 2000000
		CLKPR|=(uint8_t)(1<<CLKPS1); CLKPR&=(uint8_t)~(1<<CLKPS0);	//2 Mhz 0010
	#elif F_CPU == 4000000
		CLKPR|=(uint8_t)(1<<CLKPS0); CLKPR&=(uint8_t)~(1<<CLKPS1);	//4 Mhz 0001
  d4:	80 81       	ld	r24, Z
  d6:	81 60       	ori	r24, 0x01	; 1
  d8:	80 83       	st	Z, r24
  da:	80 81       	ld	r24, Z
  dc:	8d 7f       	andi	r24, 0xFD	; 253
  de:	80 83       	st	Z, r24

void init(void) {
	power_lcd_disable();
	power_spi_disable();
	clock_init();
	usart_init();
  e0:	0e 94 c5 01 	call	0x38a	; 0x38a <usart_init>
	timers_init();
  e4:	0e 94 78 02 	call	0x4f0	; 0x4f0 <timers_init>
	adc_init();
  e8:	0e 94 1f 01 	call	0x23e	; 0x23e <adc_init>
	//Motor Control Lines, set to outputs
	DDRD|=(uint8_t)(1<<1)|(1<<3)|(1<<5)|(1<<7);
  ec:	8a b1       	in	r24, 0x0a	; 10
  ee:	8a 6a       	ori	r24, 0xAA	; 170
  f0:	8a b9       	out	0x0a, r24	; 10

	sei(); //We use interupts, so enable them.
  f2:	78 94       	sei
	printf("init: done\n\n");
  f4:	80 e0       	ldi	r24, 0x00	; 0
  f6:	91 e0       	ldi	r25, 0x01	; 1
  f8:	0e 94 99 02 	call	0x532	; 0x532 <puts>
}
  fc:	08 95       	ret

000000fe <main>:

int main(void) {
  fe:	0f 93       	push	r16
 100:	1f 93       	push	r17
 102:	cf 93       	push	r28
 104:	df 93       	push	r29
	init();
 106:	0e 94 5b 00 	call	0xb6	; 0xb6 <init>
	PORTD|=(1<<M_AIN1)|(1<<M_BIN1);
 10a:	8b b1       	in	r24, 0x0b	; 11
 10c:	82 62       	ori	r24, 0x22	; 34
 10e:	8b b9       	out	0x0b, r24	; 11
	PORTD&=~((1<<M_AIN2)|(1<<M_BIN2));
 110:	8b b1       	in	r24, 0x0b	; 11
 112:	87 77       	andi	r24, 0x77	; 119
 114:	8b b9       	out	0x0b, r24	; 11
	MOTOR_LEFT=MOTOR_RIGHT=0xFFFF;
 116:	8f ef       	ldi	r24, 0xFF	; 255
 118:	9f ef       	ldi	r25, 0xFF	; 255
 11a:	90 93 8b 00 	sts	0x008B, r25
 11e:	80 93 8a 00 	sts	0x008A, r24
 122:	80 91 8a 00 	lds	r24, 0x008A
 126:	90 91 8b 00 	lds	r25, 0x008B
 12a:	90 93 89 00 	sts	0x0089, r25
 12e:	80 93 88 00 	sts	0x0088, r24
    milliseconds can be achieved.
 */
void
_delay_loop_2(uint16_t __count)
{
	__asm__ volatile (
 132:	04 e6       	ldi	r16, 0x64	; 100
 134:	10 e0       	ldi	r17, 0x00	; 0
			printf("AIN1 & BIN1\n");
		else		// 2nd
			printf("AIN2 & BIN2\n");
		_delay_ms(1000);
//		PORTD^=(1<<M_AIN1)|(1<<M_AIN2)|(1<<M_BIN1)|(1<<M_BIN2);
		printf("CLKPR %X\n",CLKPR);
 136:	c4 e2       	ldi	r28, 0x24	; 36
 138:	d1 e0       	ldi	r29, 0x01	; 1
	PORTD|=(1<<M_AIN1)|(1<<M_BIN1);
	PORTD&=~((1<<M_AIN2)|(1<<M_BIN2));
	MOTOR_LEFT=MOTOR_RIGHT=0xFFFF;
	static unsigned int ct;
	for (;;) {
		++ct;
 13a:	80 91 76 01 	lds	r24, 0x0176
 13e:	90 91 77 01 	lds	r25, 0x0177
 142:	01 96       	adiw	r24, 0x01	; 1
 144:	90 93 77 01 	sts	0x0177, r25
 148:	80 93 76 01 	sts	0x0176, r24
		//ct = 1 for first iter
		if (ct%2)	// 1st
 14c:	80 ff       	sbrs	r24, 0
 14e:	03 c0       	rjmp	.+6      	; 0x156 <main+0x58>
			printf("AIN1 & BIN1\n");
 150:	8c e0       	ldi	r24, 0x0C	; 12
 152:	91 e0       	ldi	r25, 0x01	; 1
 154:	02 c0       	rjmp	.+4      	; 0x15a <main+0x5c>
		else		// 2nd
			printf("AIN2 & BIN2\n");
 156:	88 e1       	ldi	r24, 0x18	; 24
 158:	91 e0       	ldi	r25, 0x01	; 1
 15a:	0e 94 99 02 	call	0x532	; 0x532 <puts>
 15e:	80 e1       	ldi	r24, 0x10	; 16
 160:	97 e2       	ldi	r25, 0x27	; 39
 162:	f8 01       	movw	r30, r16
 164:	31 97       	sbiw	r30, 0x01	; 1
 166:	f1 f7       	brne	.-4      	; 0x164 <main+0x66>
		__ticks = (uint16_t) (__ms * 10.0);
		while(__ticks)
		{
			// wait 1/10 ms
			_delay_loop_2(((F_CPU) / 4e3) / 10);
			__ticks --;
 168:	01 97       	sbiw	r24, 0x01	; 1
		__ticks = 1;
	else if (__tmp > 65535)
	{
		//	__ticks = requested delay in 1/10 ms
		__ticks = (uint16_t) (__ms * 10.0);
		while(__ticks)
 16a:	d9 f7       	brne	.-10     	; 0x162 <main+0x64>
		_delay_ms(1000);
//		PORTD^=(1<<M_AIN1)|(1<<M_AIN2)|(1<<M_BIN1)|(1<<M_BIN2);
		printf("CLKPR %X\n",CLKPR);
 16c:	80 91 61 00 	lds	r24, 0x0061
 170:	90 e0       	ldi	r25, 0x00	; 0
 172:	9f 93       	push	r25
 174:	8f 93       	push	r24
 176:	df 93       	push	r29
 178:	cf 93       	push	r28
 17a:	0e 94 7d 02 	call	0x4fa	; 0x4fa <printf>
 17e:	0f 90       	pop	r0
 180:	0f 90       	pop	r0
 182:	0f 90       	pop	r0
 184:	0f 90       	pop	r0
 186:	d9 cf       	rjmp	.-78     	; 0x13a <main+0x3c>

00000188 <adc_set_channel>:
	//adc_set_channel(++curr_ch);
	// Right now it will compute the first sensor twice, and store it's first find in the 4th sensor.
	// Could use one of the timers to do this, but probably unesicary
}

void adc_set_channel(uint8_t channel) {
 188:	ac e7       	ldi	r26, 0x7C	; 124
 18a:	b0 e0       	ldi	r27, 0x00	; 0
 18c:	9c 91       	ld	r25, X
 18e:	e3 e6       	ldi	r30, 0x63	; 99
 190:	f1 e0       	ldi	r31, 0x01	; 1
 192:	e8 0f       	add	r30, r24
 194:	f1 1d       	adc	r31, r1
 196:	90 7e       	andi	r25, 0xE0	; 224
 198:	80 81       	ld	r24, Z
 19a:	98 2b       	or	r25, r24
 19c:	9c 93       	st	X, r25
	//From Data Sheet: (Switching to Free Running mode (ADTS[2:0]=0) will)
	//(not cause a trigger event, even if the ADC Interrupt Flag is set.)
	ADMUX  = (uint8_t)((uint8_t)(ADMUX &0b11100000)|channels[channel]);
	//print_adc_values();
	//printf("V - chan %X\n",channel);
}
 19e:	08 95       	ret

000001a0 <__vector_19>:

//ADC Interupt handler
ISR(ADC_vect) {
 1a0:	1f 92       	push	r1
 1a2:	0f 92       	push	r0
 1a4:	0f b6       	in	r0, 0x3f	; 63
 1a6:	0f 92       	push	r0
 1a8:	11 24       	eor	r1, r1
 1aa:	2f 93       	push	r18
 1ac:	3f 93       	push	r19
 1ae:	4f 93       	push	r20
 1b0:	8f 93       	push	r24
 1b2:	9f 93       	push	r25
 1b4:	af 93       	push	r26
 1b6:	bf 93       	push	r27
 1b8:	ef 93       	push	r30
 1ba:	ff 93       	push	r31
	// New conversion has already started.
	uint16_t adc_value;
	adc_value  =  ADCL;  
 1bc:	40 91 78 00 	lds	r20, 0x0078
	adc_value += (ADCH<<8);
 1c0:	30 91 79 00 	lds	r19, 0x0079
	uint8_t real_channel;

	// the curr_ch now has the chan of the ongoing conversion, we need the last one
	if (!curr_ch)	real_channel = channel_amt-1; //curr_ch==0 
 1c4:	20 91 83 01 	lds	r18, 0x0183
 1c8:	22 23       	and	r18, r18
 1ca:	11 f4       	brne	.+4      	; 0x1d0 <__vector_19+0x30>
 1cc:	e3 e0       	ldi	r30, 0x03	; 3
 1ce:	02 c0       	rjmp	.+4      	; 0x1d4 <__vector_19+0x34>
	else		real_channel = curr_ch-1;
 1d0:	e2 2f       	mov	r30, r18
 1d2:	e1 50       	subi	r30, 0x01	; 1

	adc_val[real_channel] = adc_value;
 1d4:	f0 e0       	ldi	r31, 0x00	; 0
 1d6:	ee 0f       	add	r30, r30
 1d8:	ff 1f       	adc	r31, r31
 1da:	df 01       	movw	r26, r30
 1dc:	a5 58       	subi	r26, 0x85	; 133
 1de:	be 4f       	sbci	r27, 0xFE	; 254
 1e0:	93 2f       	mov	r25, r19
 1e2:	80 e0       	ldi	r24, 0x00	; 0
 1e4:	84 0f       	add	r24, r20
 1e6:	91 1d       	adc	r25, r1
 1e8:	8d 93       	st	X+, r24
 1ea:	9c 93       	st	X, r25
	++adc_amt[real_channel];
 1ec:	ec 57       	subi	r30, 0x7C	; 124
 1ee:	fe 4f       	sbci	r31, 0xFE	; 254
 1f0:	80 81       	ld	r24, Z
 1f2:	91 81       	ldd	r25, Z+1	; 0x01
 1f4:	01 96       	adiw	r24, 0x01	; 1
 1f6:	91 83       	std	Z+1, r25	; 0x01
 1f8:	80 83       	st	Z, r24

	// Change the channel for the conversion after the one currently processing.
	if (++curr_ch >= channel_amt)	curr_ch = 0;
 1fa:	82 2f       	mov	r24, r18
 1fc:	8f 5f       	subi	r24, 0xFF	; 255
 1fe:	80 93 83 01 	sts	0x0183, r24
 202:	84 30       	cpi	r24, 0x04	; 4
 204:	10 f0       	brcs	.+4      	; 0x20a <__vector_19+0x6a>
 206:	10 92 83 01 	sts	0x0183, r1

void adc_set_channel(uint8_t channel) {
	//Prescale reset on each "trigger event"s
	//From Data Sheet: (Switching to Free Running mode (ADTS[2:0]=0) will)
	//(not cause a trigger event, even if the ADC Interrupt Flag is set.)
	ADMUX  = (uint8_t)((uint8_t)(ADMUX &0b11100000)|channels[channel]);
 20a:	80 91 7c 00 	lds	r24, 0x007C
 20e:	e0 91 83 01 	lds	r30, 0x0183
 212:	f0 e0       	ldi	r31, 0x00	; 0
 214:	ed 59       	subi	r30, 0x9D	; 157
 216:	fe 4f       	sbci	r31, 0xFE	; 254
 218:	80 7e       	andi	r24, 0xE0	; 224
 21a:	90 81       	ld	r25, Z
 21c:	89 2b       	or	r24, r25
 21e:	80 93 7c 00 	sts	0x007C, r24
	
	//printf("adc_value: %d",adc_value);
	
	//TODO: modify for running average (Ave= (Ave*(ct-1)+New)/ct)
	//INFO: Vin[V]=(ADCH·256+ADCL)·Vref[V]/1024
}
 222:	ff 91       	pop	r31
 224:	ef 91       	pop	r30
 226:	bf 91       	pop	r27
 228:	af 91       	pop	r26
 22a:	9f 91       	pop	r25
 22c:	8f 91       	pop	r24
 22e:	4f 91       	pop	r20
 230:	3f 91       	pop	r19
 232:	2f 91       	pop	r18
 234:	0f 90       	pop	r0
 236:	0f be       	out	0x3f, r0	; 63
 238:	0f 90       	pop	r0
 23a:	1f 90       	pop	r1
 23c:	18 95       	reti

0000023e <adc_init>:
	for (uint8_t chan=0;chan<channel_amt;chan++) 
		printf(" [ %d : %d ] ", chan, adc_val[chan]);
	printf("\n");
}

void adc_init() {
 23e:	ef 92       	push	r14
 240:	ff 92       	push	r15
 242:	0f 93       	push	r16
 244:	1f 93       	push	r17
	printf("adc: init...\n");
 246:	8e e2       	ldi	r24, 0x2E	; 46
 248:	91 e0       	ldi	r25, 0x01	; 1
 24a:	0e 94 99 02 	call	0x532	; 0x532 <puts>

	power_adc_enable();
 24e:	e4 e6       	ldi	r30, 0x64	; 100
 250:	f0 e0       	ldi	r31, 0x00	; 0
 252:	80 81       	ld	r24, Z
 254:	8e 7f       	andi	r24, 0xFE	; 254
 256:	80 83       	st	Z, r24

	//Set Voltage to AVCC with external capacitor at AREF pin
	ADMUX|= (uint8_t)(1<<REFS0);
 258:	9c e7       	ldi	r25, 0x7C	; 124
 25a:	e9 2e       	mov	r14, r25
 25c:	f1 2c       	mov	r15, r1
 25e:	f7 01       	movw	r30, r14
 260:	80 81       	ld	r24, Z
 262:	80 64       	ori	r24, 0x40	; 64
 264:	80 83       	st	Z, r24
	ADMUX&=~(uint8_t)(1<<REFS1);
 266:	80 81       	ld	r24, Z
 268:	8f 77       	andi	r24, 0x7F	; 127
 26a:	80 83       	st	Z, r24
	//ADMUX&=~(1<<ADLAR); // Default disabled
	
	// Enable ADC, Inturupt, Trigger mode and set prescaler
	//ADCSRA=(((1<<ADEN)|(1<<ADIE)|(1<<ADATE))&0b11111000)|(ADC_PRESCALE);
	ADCSRA|= (uint8_t)(1<<ADEN)|(1<<ADIE)|(1<<ADATE);
 26c:	0a e7       	ldi	r16, 0x7A	; 122
 26e:	10 e0       	ldi	r17, 0x00	; 0
 270:	f8 01       	movw	r30, r16
 272:	80 81       	ld	r24, Z
 274:	88 6a       	ori	r24, 0xA8	; 168
 276:	80 83       	st	Z, r24
	ADCSRA = (uint8_t)(ADCSRA & 0b11111000)|((uint8_t)ADC_PRESCALE);
 278:	80 81       	ld	r24, Z
 27a:	88 7f       	andi	r24, 0xF8	; 248
 27c:	85 60       	ori	r24, 0x05	; 5
 27e:	80 83       	st	Z, r24
	
	// Enable Free Running Mode 
	ADCSRB|= (uint8_t)(1<<7); //reserved bit.
 280:	eb e7       	ldi	r30, 0x7B	; 123
 282:	f0 e0       	ldi	r31, 0x00	; 0
 284:	80 81       	ld	r24, Z
 286:	80 68       	ori	r24, 0x80	; 128
 288:	80 83       	st	Z, r24
	ADCSRB&= (uint8_t)~(0b111); //(ADTS2:0)=0
 28a:	80 81       	ld	r24, Z
 28c:	88 7f       	andi	r24, 0xF8	; 248
 28e:	80 83       	st	Z, r24
	
	// Disable Digital reads from analog pins
	DIDR0 |= (uint8_t)((1<<ADC4D)|(1<<ADC5D)|(1<<ADC6D)|(1<<ADC7D));
 290:	ee e7       	ldi	r30, 0x7E	; 126
 292:	f0 e0       	ldi	r31, 0x00	; 0
 294:	80 81       	ld	r24, Z
 296:	80 6f       	ori	r24, 0xF0	; 240
 298:	80 83       	st	Z, r24
	
	set_sleep_mode(SLEEP_MODE_ADC);
 29a:	83 b7       	in	r24, 0x33	; 51
 29c:	81 7f       	andi	r24, 0xF1	; 241
 29e:	82 60       	ori	r24, 0x02	; 2
 2a0:	83 bf       	out	0x33, r24	; 51
	printf("adc: setup convertions...\n");
 2a2:	8b e3       	ldi	r24, 0x3B	; 59
 2a4:	91 e0       	ldi	r25, 0x01	; 1
 2a6:	0e 94 99 02 	call	0x532	; 0x532 <puts>

void adc_set_channel(uint8_t channel) {
	//Prescale reset on each "trigger event"s
	//From Data Sheet: (Switching to Free Running mode (ADTS[2:0]=0) will)
	//(not cause a trigger event, even if the ADC Interrupt Flag is set.)
	ADMUX  = (uint8_t)((uint8_t)(ADMUX &0b11100000)|channels[channel]);
 2aa:	f7 01       	movw	r30, r14
 2ac:	90 81       	ld	r25, Z
 2ae:	80 91 83 01 	lds	r24, 0x0183
 2b2:	e3 e6       	ldi	r30, 0x63	; 99
 2b4:	f1 e0       	ldi	r31, 0x01	; 1
 2b6:	e8 0f       	add	r30, r24
 2b8:	f1 1d       	adc	r31, r1
 2ba:	90 7e       	andi	r25, 0xE0	; 224
 2bc:	80 81       	ld	r24, Z
 2be:	98 2b       	or	r25, r24
 2c0:	f7 01       	movw	r30, r14
 2c2:	90 83       	st	Z, r25
	
	set_sleep_mode(SLEEP_MODE_ADC);
	printf("adc: setup convertions...\n");
	adc_set_channel(curr_ch);
	//Start the convertions
	ADCSRA|= (uint8_t)(1<<ADSC);
 2c4:	f8 01       	movw	r30, r16
 2c6:	80 81       	ld	r24, Z
 2c8:	80 64       	ori	r24, 0x40	; 64
 2ca:	80 83       	st	Z, r24
	// Wait one adc clock cycle and change the channel
	// 125KHz
	//adc_set_channel(++curr_ch);
	// Right now it will compute the first sensor twice, and store it's first find in the 4th sensor.
	// Could use one of the timers to do this, but probably unesicary
}
 2cc:	1f 91       	pop	r17
 2ce:	0f 91       	pop	r16
 2d0:	ff 90       	pop	r15
 2d2:	ef 90       	pop	r14
 2d4:	08 95       	ret

000002d6 <print_adc_values>:
#include <avr/sleep.h>
#include <avr/power.h>
#include <stdio.h>
#include <math.h>

void print_adc_values() {
 2d6:	0f 93       	push	r16
 2d8:	1f 93       	push	r17
	//printf("ADC: ");
	for (uint8_t chan=0;chan<channel_amt;chan++) 
		printf(" [ %d : %d ] ", chan, adc_val[chan]);
 2da:	80 91 7b 01 	lds	r24, 0x017B
 2de:	90 91 7c 01 	lds	r25, 0x017C
 2e2:	9f 93       	push	r25
 2e4:	8f 93       	push	r24
 2e6:	1f 92       	push	r1
 2e8:	1f 92       	push	r1
 2ea:	05 e5       	ldi	r16, 0x55	; 85
 2ec:	11 e0       	ldi	r17, 0x01	; 1
 2ee:	1f 93       	push	r17
 2f0:	0f 93       	push	r16
 2f2:	0e 94 7d 02 	call	0x4fa	; 0x4fa <printf>
 2f6:	80 91 7d 01 	lds	r24, 0x017D
 2fa:	90 91 7e 01 	lds	r25, 0x017E
 2fe:	9f 93       	push	r25
 300:	8f 93       	push	r24
 302:	81 e0       	ldi	r24, 0x01	; 1
 304:	90 e0       	ldi	r25, 0x00	; 0
 306:	9f 93       	push	r25
 308:	8f 93       	push	r24
 30a:	1f 93       	push	r17
 30c:	0f 93       	push	r16
 30e:	0e 94 7d 02 	call	0x4fa	; 0x4fa <printf>
 312:	80 91 7f 01 	lds	r24, 0x017F
 316:	90 91 80 01 	lds	r25, 0x0180
 31a:	9f 93       	push	r25
 31c:	8f 93       	push	r24
 31e:	82 e0       	ldi	r24, 0x02	; 2
 320:	90 e0       	ldi	r25, 0x00	; 0
 322:	9f 93       	push	r25
 324:	8f 93       	push	r24
 326:	1f 93       	push	r17
 328:	0f 93       	push	r16
 32a:	0e 94 7d 02 	call	0x4fa	; 0x4fa <printf>
 32e:	80 91 81 01 	lds	r24, 0x0181
 332:	90 91 82 01 	lds	r25, 0x0182
 336:	9f 93       	push	r25
 338:	8f 93       	push	r24
 33a:	83 e0       	ldi	r24, 0x03	; 3
 33c:	90 e0       	ldi	r25, 0x00	; 0
 33e:	9f 93       	push	r25
 340:	8f 93       	push	r24
 342:	1f 93       	push	r17
 344:	0f 93       	push	r16
 346:	0e 94 7d 02 	call	0x4fa	; 0x4fa <printf>
	printf("\n");
 34a:	8a e0       	ldi	r24, 0x0A	; 10
 34c:	90 e0       	ldi	r25, 0x00	; 0
 34e:	0e 94 92 02 	call	0x524	; 0x524 <putchar>
 352:	8d b7       	in	r24, 0x3d	; 61
 354:	9e b7       	in	r25, 0x3e	; 62
 356:	48 96       	adiw	r24, 0x18	; 24
 358:	0f b6       	in	r0, 0x3f	; 63
 35a:	f8 94       	cli
 35c:	9e bf       	out	0x3e, r25	; 62
 35e:	0f be       	out	0x3f, r0	; 63
 360:	8d bf       	out	0x3d, r24	; 61
}
 362:	1f 91       	pop	r17
 364:	0f 91       	pop	r16
 366:	08 95       	ret

00000368 <usart_putchar>:
static int usart_getchar(char c, FILE *stream) {
	return 1;
}
*/

static int usart_putchar(char c, FILE *stream) {
 368:	1f 93       	push	r17
 36a:	18 2f       	mov	r17, r24

  if (c == '\n')
 36c:	8a 30       	cpi	r24, 0x0A	; 10
 36e:	19 f4       	brne	.+6      	; 0x376 <usart_putchar+0xe>
	usart_putchar('\r', stream);
 370:	8d e0       	ldi	r24, 0x0D	; 13
 372:	0e 94 b4 01 	call	0x368	; 0x368 <usart_putchar>
  loop_until_bit_is_set(UCSR0A, UDRE0);
 376:	80 91 c0 00 	lds	r24, 0x00C0
 37a:	85 ff       	sbrs	r24, 5
 37c:	fc cf       	rjmp	.-8      	; 0x376 <usart_putchar+0xe>
  UDR0 = c;
 37e:	10 93 c6 00 	sts	0x00C6, r17
  return 0;
}
 382:	80 e0       	ldi	r24, 0x00	; 0
 384:	90 e0       	ldi	r25, 0x00	; 0
 386:	1f 91       	pop	r17
 388:	08 95       	ret

0000038a <usart_init>:


void usart_init(void) {
 38a:	e4 e6       	ldi	r30, 0x64	; 100
 38c:	f0 e0       	ldi	r31, 0x00	; 0
 38e:	80 81       	ld	r24, Z
 390:	8d 7f       	andi	r24, 0xFD	; 253
 392:	80 83       	st	Z, r24
	power_usart0_enable();

	/* Set baud rate (12bit) */
	UBRR0 = UBRR_VALUE;
 394:	8c e0       	ldi	r24, 0x0C	; 12
 396:	90 e0       	ldi	r25, 0x00	; 0
 398:	90 93 c5 00 	sts	0x00C5, r25
 39c:	80 93 c4 00 	sts	0x00C4, r24
	#if USE_2X
	UCSR0A |= (1 << U2X0);
	#else
	UCSR0A &= ~(1 << U2X0);
 3a0:	e0 ec       	ldi	r30, 0xC0	; 192
 3a2:	f0 e0       	ldi	r31, 0x00	; 0
 3a4:	80 81       	ld	r24, Z
 3a6:	8d 7f       	andi	r24, 0xFD	; 253
 3a8:	80 83       	st	Z, r24
	#endif
	/* Double the uart clock */
	//UCSR0A |=(1<<U2X0);
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)	;
 3aa:	88 e1       	ldi	r24, 0x18	; 24
 3ac:	80 93 c1 00 	sts	0x00C1, r24
	/* Enable r/t interupts, hangles input when used with some buffering functions */
	//UCSR0B =|(1<<RXCIE0)|(1<<TXCIE0);
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (0<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01);
 3b0:	86 e0       	ldi	r24, 0x06	; 6
 3b2:	80 93 c2 00 	sts	0x00C2, r24
	
	stdout=&usart_stdout;
 3b6:	87 e6       	ldi	r24, 0x67	; 103
 3b8:	91 e0       	ldi	r25, 0x01	; 1
 3ba:	90 93 8f 01 	sts	0x018F, r25
 3be:	80 93 8e 01 	sts	0x018E, r24
 3c2:	08 95       	ret

000003c4 <timer2_init>:
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>


void timer2_init(void) {
 3c4:	80 91 70 00 	lds	r24, 0x0070
 3c8:	8c 7f       	andi	r24, 0xFC	; 252
 3ca:	80 93 70 00 	sts	0x0070, r24
	TIMSK2&=(uint8_t)~((1<<OCIE2A)|(1<<TOIE2));
	
	
	
	// Enable asyncronous clocking.
	ASSR&=(uint8_t)~(1<<EXCLK);
 3ce:	80 91 b6 00 	lds	r24, 0x00B6
 3d2:	8f 7e       	andi	r24, 0xEF	; 239
 3d4:	80 93 b6 00 	sts	0x00B6, r24
	ASSR|=(uint8_t)(1<<AS2);
 3d8:	80 91 b6 00 	lds	r24, 0x00B6
 3dc:	88 60       	ori	r24, 0x08	; 8
 3de:	80 93 b6 00 	sts	0x00B6, r24
	
	// Reset acculumator
	TCNT2 = 0;
 3e2:	10 92 b2 00 	sts	0x00B2, r1
	
	TCCR2A&=(uint8_t)~(1<<FOC2A);
 3e6:	80 91 b0 00 	lds	r24, 0x00B0
 3ea:	8f 77       	andi	r24, 0x7F	; 127
 3ec:	80 93 b0 00 	sts	0x00B0, r24
	*/
	// : CTC Mode.
	//TCCR2A|=(1<<WGM20);
	//TCCR2A&=~(1<<WGM21);
	// : Normal Mode.
	TCCR2A&=(uint8_t)~((1<<WGM20)|(1<<WGM21));
 3f0:	80 91 b0 00 	lds	r24, 0x00B0
 3f4:	87 7b       	andi	r24, 0xB7	; 183
 3f6:	80 93 b0 00 	sts	0x00B0, r24
		0		1		Toggle on compare match
		1		0		clear on compare match
		1		1		set on 		"		"
	*/
	// : No output
	TCCR2A&=(uint8_t)~((1<<COM2A1)|(1<<COM2A0));
 3fa:	80 91 b0 00 	lds	r24, 0x00B0
 3fe:	8f 7c       	andi	r24, 0xCF	; 207
 400:	80 93 b0 00 	sts	0x00B0, r24
	//TCCR2A|= (1<<CS20);
	
	// 32768/1024 = 32, 256 clicks, 1/8Hz
	//TCCR2A|= (1<<CS21)|(1<<CS20)|(1<<CS22); //1024 Presacle
	
	TCCR2A|= (uint8_t)(1<<CS20); //8 Presacle, 16Hz
 404:	80 91 b0 00 	lds	r24, 0x00B0
 408:	81 60       	ori	r24, 0x01	; 1
 40a:	80 93 b0 00 	sts	0x00B0, r24
	TCCR2A&=(uint8_t)~((uint8_t)((1<<CS21)|(1<<CS22)));
 40e:	80 91 b0 00 	lds	r24, 0x00B0
 412:	89 7f       	andi	r24, 0xF9	; 249
 414:	80 93 b0 00 	sts	0x00B0, r24
	
	// Wait for stuff to take affect.
	loop_until_bit_is_clear(ASSR,TCN2UB);
 418:	80 91 b6 00 	lds	r24, 0x00B6
 41c:	82 fd       	sbrc	r24, 2
 41e:	fc cf       	rjmp	.-8      	; 0x418 <timer2_init+0x54>
	loop_until_bit_is_clear(ASSR,OCR2UB);
 420:	80 91 b6 00 	lds	r24, 0x00B6
 424:	81 fd       	sbrc	r24, 1
 426:	fc cf       	rjmp	.-8      	; 0x420 <timer2_init+0x5c>
	loop_until_bit_is_clear(ASSR,TCR2UB);
 428:	80 91 b6 00 	lds	r24, 0x00B6
 42c:	80 fd       	sbrc	r24, 0
 42e:	fc cf       	rjmp	.-8      	; 0x428 <timer2_init+0x64>
	
	// Enable overflow interrupt, disable match.
	TIMSK2|= (uint8_t)(1<<TOIE2);
 430:	80 91 70 00 	lds	r24, 0x0070
 434:	81 60       	ori	r24, 0x01	; 1
 436:	80 93 70 00 	sts	0x0070, r24
	//TIMSK2&=~(1<<OCIE2A); // Disabled
}
 43a:	08 95       	ret

0000043c <__vector_5>:


// Timer2 Overflow
ISR(TIMER2_OVF_vect) {
 43c:	1f 92       	push	r1
 43e:	0f 92       	push	r0
 440:	0f b6       	in	r0, 0x3f	; 63
 442:	0f 92       	push	r0
 444:	11 24       	eor	r1, r1
 446:	8f 93       	push	r24
 448:	9f 93       	push	r25
	static uint8_t ct;//=0
	static uint16_t sec;//=0
	if (ct < 16) {
 44a:	80 91 7a 01 	lds	r24, 0x017A
 44e:	80 31       	cpi	r24, 0x10	; 16
 450:	20 f4       	brcc	.+8      	; 0x45a <__vector_5+0x1e>
		++ct; 
 452:	8f 5f       	subi	r24, 0xFF	; 255
 454:	80 93 7a 01 	sts	0x017A, r24
 458:	0b c0       	rjmp	.+22     	; 0x470 <__vector_5+0x34>
	}
	else {
		++sec;
 45a:	80 91 78 01 	lds	r24, 0x0178
 45e:	90 91 79 01 	lds	r25, 0x0179
 462:	01 96       	adiw	r24, 0x01	; 1
 464:	90 93 79 01 	sts	0x0179, r25
 468:	80 93 78 01 	sts	0x0178, r24
		ct=0;
 46c:	10 92 7a 01 	sts	0x017A, r1
	
	if (!(ct%4)) {
		// 4 Hz (16/4)
		
	}
}
 470:	9f 91       	pop	r25
 472:	8f 91       	pop	r24
 474:	0f 90       	pop	r0
 476:	0f be       	out	0x3f, r0	; 63
 478:	0f 90       	pop	r0
 47a:	1f 90       	pop	r1
 47c:	18 95       	reti

0000047e <timer1_init>:

void timer1_init(void) { // Runs the PWMs
 47e:	e0 e8       	ldi	r30, 0x80	; 128
 480:	f0 e0       	ldi	r31, 0x00	; 0
 482:	80 81       	ld	r24, Z
 484:	80 6c       	ori	r24, 0xC0	; 192
 486:	80 83       	st	Z, r24

	// Set OC1A on up, clear on down
	TCCR1A|= (uint8_t) (1<<COM1A1)|(1<<COM1A0);
	
	// Set OC1B on up, clear on down
	TCCR1A|= (uint8_t) (1<<COM1B1)|(1<<COM1B0);
 488:	80 81       	ld	r24, Z
 48a:	80 63       	ori	r24, 0x30	; 48
 48c:	80 83       	st	Z, r24
	
	// Waveform Generation Set to PWM (Phase and frequency correct, mode 8)
	TCCR1A&= (uint8_t) ~((1<<WGM11)|(1<<WGM10));
 48e:	80 81       	ld	r24, Z
 490:	8c 7f       	andi	r24, 0xFC	; 252
 492:	80 83       	st	Z, r24
	
	TCCR1B|= (uint8_t) (1<<5); //Reserved bit
 494:	e1 e8       	ldi	r30, 0x81	; 129
 496:	f0 e0       	ldi	r31, 0x00	; 0
 498:	80 81       	ld	r24, Z
 49a:	80 62       	ori	r24, 0x20	; 32
 49c:	80 83       	st	Z, r24
	
	TCCR1B|= (uint8_t) (1<<WGM13);
 49e:	80 81       	ld	r24, Z
 4a0:	80 61       	ori	r24, 0x10	; 16
 4a2:	80 83       	st	Z, r24
	TCCR1B&= (uint8_t)~(1<<WGM12);
 4a4:	80 81       	ld	r24, Z
 4a6:	87 7f       	andi	r24, 0xF7	; 247
 4a8:	80 83       	st	Z, r24
	
	// Disable Input noise canceler
	TCCR1B|= (uint8_t)(1<<ICNC1);
 4aa:	80 81       	ld	r24, Z
 4ac:	80 68       	ori	r24, 0x80	; 128
 4ae:	80 83       	st	Z, r24
	
	// Set TOP
	ICR1=0xFFFF;
 4b0:	2f ef       	ldi	r18, 0xFF	; 255
 4b2:	3f ef       	ldi	r19, 0xFF	; 255
 4b4:	30 93 87 00 	sts	0x0087, r19
 4b8:	20 93 86 00 	sts	0x0086, r18
	//prescale = F_CPU/(2*100000*0xFFFF) = 
	// At 2MHz the best we get is 15Hz (bad. very bad.)
	
	// Set prescale to 1
	// TCCR1B = (TCCR1B & 0b11111000)|(log(prescale)/log(2));
	TCCR1B&= (uint8_t)~((1<<CS12)|(1<<CS11));
 4bc:	80 81       	ld	r24, Z
 4be:	89 7f       	andi	r24, 0xF9	; 249
 4c0:	80 83       	st	Z, r24
	TCCR1B|= (uint8_t) (1<<CS10);
 4c2:	80 81       	ld	r24, Z
 4c4:	81 60       	ori	r24, 0x01	; 1
 4c6:	80 83       	st	Z, r24
	
	//OCR1A and OCR1B are the Compare / PWM registers
	OCR1A = OCR1B = 0xFFFF; // Max = 65535,0xFFFF
 4c8:	ea e8       	ldi	r30, 0x8A	; 138
 4ca:	f0 e0       	ldi	r31, 0x00	; 0
 4cc:	31 83       	std	Z+1, r19	; 0x01
 4ce:	20 83       	st	Z, r18
 4d0:	80 81       	ld	r24, Z
 4d2:	91 81       	ldd	r25, Z+1	; 0x01
 4d4:	90 93 89 00 	sts	0x0089, r25
 4d8:	80 93 88 00 	sts	0x0088, r24
	
	// disable the interupts (probably done by default).
	TIMSK1&= (uint8_t)~((1<<ICIE1)|(1<<OCIE1B)|(1<<OCIE1A)|(1<<TOIE1));
 4dc:	ef e6       	ldi	r30, 0x6F	; 111
 4de:	f0 e0       	ldi	r31, 0x00	; 0
 4e0:	80 81       	ld	r24, Z
 4e2:	88 7d       	andi	r24, 0xD8	; 216
 4e4:	80 83       	st	Z, r24
	
	DDRB|= (uint8_t)(1<<PB5)|(1<<PB6);
 4e6:	84 b1       	in	r24, 0x04	; 4
 4e8:	80 66       	ori	r24, 0x60	; 96
 4ea:	84 b9       	out	0x04, r24	; 4
}
 4ec:	08 95       	ret

000004ee <timer0_init>:

void timer0_init(void) {
 4ee:	08 95       	ret

000004f0 <timers_init>:
}

void timers_init(void) {
 4f0:	0e 94 3f 02 	call	0x47e	; 0x47e <timer1_init>
	timer0_init();
	timer1_init();
	timer2_init();
 4f4:	0e 94 e2 01 	call	0x3c4	; 0x3c4 <timer2_init>
}
 4f8:	08 95       	ret

000004fa <printf>:
 4fa:	a0 e0       	ldi	r26, 0x00	; 0
 4fc:	b0 e0       	ldi	r27, 0x00	; 0
 4fe:	e3 e8       	ldi	r30, 0x83	; 131
 500:	f2 e0       	ldi	r31, 0x02	; 2
 502:	0c 94 5d 05 	jmp	0xaba	; 0xaba <__prologue_saves__+0x20>
 506:	fe 01       	movw	r30, r28
 508:	35 96       	adiw	r30, 0x05	; 5
 50a:	61 91       	ld	r22, Z+
 50c:	71 91       	ld	r23, Z+
 50e:	af 01       	movw	r20, r30
 510:	80 91 8e 01 	lds	r24, 0x018E
 514:	90 91 8f 01 	lds	r25, 0x018F
 518:	0e 94 ce 02 	call	0x59c	; 0x59c <vfprintf>
 51c:	20 96       	adiw	r28, 0x00	; 0
 51e:	e2 e0       	ldi	r30, 0x02	; 2
 520:	0c 94 79 05 	jmp	0xaf2	; 0xaf2 <__epilogue_restores__+0x20>

00000524 <putchar>:
 524:	60 91 8e 01 	lds	r22, 0x018E
 528:	70 91 8f 01 	lds	r23, 0x018F
 52c:	0e 94 c3 04 	call	0x986	; 0x986 <fputc>
 530:	08 95       	ret

00000532 <puts>:
 532:	0f 93       	push	r16
 534:	1f 93       	push	r17
 536:	cf 93       	push	r28
 538:	df 93       	push	r29
 53a:	8c 01       	movw	r16, r24
 53c:	e0 91 8e 01 	lds	r30, 0x018E
 540:	f0 91 8f 01 	lds	r31, 0x018F
 544:	83 81       	ldd	r24, Z+3	; 0x03
 546:	81 ff       	sbrs	r24, 1
 548:	21 c0       	rjmp	.+66     	; 0x58c <puts+0x5a>
 54a:	c0 e0       	ldi	r28, 0x00	; 0
 54c:	d0 e0       	ldi	r29, 0x00	; 0
 54e:	0d c0       	rjmp	.+26     	; 0x56a <puts+0x38>
 550:	e0 91 8e 01 	lds	r30, 0x018E
 554:	f0 91 8f 01 	lds	r31, 0x018F
 558:	20 85       	ldd	r18, Z+8	; 0x08
 55a:	31 85       	ldd	r19, Z+9	; 0x09
 55c:	bf 01       	movw	r22, r30
 55e:	f9 01       	movw	r30, r18
 560:	09 95       	icall
 562:	89 2b       	or	r24, r25
 564:	11 f0       	breq	.+4      	; 0x56a <puts+0x38>
 566:	cf ef       	ldi	r28, 0xFF	; 255
 568:	df ef       	ldi	r29, 0xFF	; 255
 56a:	f8 01       	movw	r30, r16
 56c:	81 91       	ld	r24, Z+
 56e:	8f 01       	movw	r16, r30
 570:	88 23       	and	r24, r24
 572:	71 f7       	brne	.-36     	; 0x550 <puts+0x1e>
 574:	e0 91 8e 01 	lds	r30, 0x018E
 578:	f0 91 8f 01 	lds	r31, 0x018F
 57c:	20 85       	ldd	r18, Z+8	; 0x08
 57e:	31 85       	ldd	r19, Z+9	; 0x09
 580:	bf 01       	movw	r22, r30
 582:	8a e0       	ldi	r24, 0x0A	; 10
 584:	f9 01       	movw	r30, r18
 586:	09 95       	icall
 588:	89 2b       	or	r24, r25
 58a:	11 f0       	breq	.+4      	; 0x590 <puts+0x5e>
 58c:	cf ef       	ldi	r28, 0xFF	; 255
 58e:	df ef       	ldi	r29, 0xFF	; 255
 590:	ce 01       	movw	r24, r28
 592:	df 91       	pop	r29
 594:	cf 91       	pop	r28
 596:	1f 91       	pop	r17
 598:	0f 91       	pop	r16
 59a:	08 95       	ret

0000059c <vfprintf>:
 59c:	ab e0       	ldi	r26, 0x0B	; 11
 59e:	b0 e0       	ldi	r27, 0x00	; 0
 5a0:	e4 ed       	ldi	r30, 0xD4	; 212
 5a2:	f2 e0       	ldi	r31, 0x02	; 2
 5a4:	0c 94 4d 05 	jmp	0xa9a	; 0xa9a <__prologue_saves__>
 5a8:	3c 01       	movw	r6, r24
 5aa:	2b 01       	movw	r4, r22
 5ac:	5a 01       	movw	r10, r20
 5ae:	fc 01       	movw	r30, r24
 5b0:	17 82       	std	Z+7, r1	; 0x07
 5b2:	16 82       	std	Z+6, r1	; 0x06
 5b4:	83 81       	ldd	r24, Z+3	; 0x03
 5b6:	81 fd       	sbrc	r24, 1
 5b8:	03 c0       	rjmp	.+6      	; 0x5c0 <vfprintf+0x24>
 5ba:	6f ef       	ldi	r22, 0xFF	; 255
 5bc:	7f ef       	ldi	r23, 0xFF	; 255
 5be:	c8 c1       	rjmp	.+912    	; 0x950 <vfprintf+0x3b4>
 5c0:	9a e0       	ldi	r25, 0x0A	; 10
 5c2:	89 2e       	mov	r8, r25
 5c4:	1e 01       	movw	r2, r28
 5c6:	08 94       	sec
 5c8:	21 1c       	adc	r2, r1
 5ca:	31 1c       	adc	r3, r1
 5cc:	f3 01       	movw	r30, r6
 5ce:	23 81       	ldd	r18, Z+3	; 0x03
 5d0:	f2 01       	movw	r30, r4
 5d2:	23 fd       	sbrc	r18, 3
 5d4:	85 91       	lpm	r24, Z+
 5d6:	23 ff       	sbrs	r18, 3
 5d8:	81 91       	ld	r24, Z+
 5da:	2f 01       	movw	r4, r30
 5dc:	88 23       	and	r24, r24
 5de:	09 f4       	brne	.+2      	; 0x5e2 <vfprintf+0x46>
 5e0:	b4 c1       	rjmp	.+872    	; 0x94a <vfprintf+0x3ae>
 5e2:	85 32       	cpi	r24, 0x25	; 37
 5e4:	39 f4       	brne	.+14     	; 0x5f4 <vfprintf+0x58>
 5e6:	23 fd       	sbrc	r18, 3
 5e8:	85 91       	lpm	r24, Z+
 5ea:	23 ff       	sbrs	r18, 3
 5ec:	81 91       	ld	r24, Z+
 5ee:	2f 01       	movw	r4, r30
 5f0:	85 32       	cpi	r24, 0x25	; 37
 5f2:	29 f4       	brne	.+10     	; 0x5fe <vfprintf+0x62>
 5f4:	b3 01       	movw	r22, r6
 5f6:	90 e0       	ldi	r25, 0x00	; 0
 5f8:	0e 94 c3 04 	call	0x986	; 0x986 <fputc>
 5fc:	e7 cf       	rjmp	.-50     	; 0x5cc <vfprintf+0x30>
 5fe:	98 2f       	mov	r25, r24
 600:	dd 24       	eor	r13, r13
 602:	cc 24       	eor	r12, r12
 604:	99 24       	eor	r9, r9
 606:	ff e1       	ldi	r31, 0x1F	; 31
 608:	fd 15       	cp	r31, r13
 60a:	d0 f0       	brcs	.+52     	; 0x640 <vfprintf+0xa4>
 60c:	9b 32       	cpi	r25, 0x2B	; 43
 60e:	69 f0       	breq	.+26     	; 0x62a <vfprintf+0x8e>
 610:	9c 32       	cpi	r25, 0x2C	; 44
 612:	28 f4       	brcc	.+10     	; 0x61e <vfprintf+0x82>
 614:	90 32       	cpi	r25, 0x20	; 32
 616:	59 f0       	breq	.+22     	; 0x62e <vfprintf+0x92>
 618:	93 32       	cpi	r25, 0x23	; 35
 61a:	91 f4       	brne	.+36     	; 0x640 <vfprintf+0xa4>
 61c:	0e c0       	rjmp	.+28     	; 0x63a <vfprintf+0x9e>
 61e:	9d 32       	cpi	r25, 0x2D	; 45
 620:	49 f0       	breq	.+18     	; 0x634 <vfprintf+0x98>
 622:	90 33       	cpi	r25, 0x30	; 48
 624:	69 f4       	brne	.+26     	; 0x640 <vfprintf+0xa4>
 626:	41 e0       	ldi	r20, 0x01	; 1
 628:	24 c0       	rjmp	.+72     	; 0x672 <vfprintf+0xd6>
 62a:	52 e0       	ldi	r21, 0x02	; 2
 62c:	d5 2a       	or	r13, r21
 62e:	84 e0       	ldi	r24, 0x04	; 4
 630:	d8 2a       	or	r13, r24
 632:	28 c0       	rjmp	.+80     	; 0x684 <vfprintf+0xe8>
 634:	98 e0       	ldi	r25, 0x08	; 8
 636:	d9 2a       	or	r13, r25
 638:	25 c0       	rjmp	.+74     	; 0x684 <vfprintf+0xe8>
 63a:	e0 e1       	ldi	r30, 0x10	; 16
 63c:	de 2a       	or	r13, r30
 63e:	22 c0       	rjmp	.+68     	; 0x684 <vfprintf+0xe8>
 640:	d7 fc       	sbrc	r13, 7
 642:	29 c0       	rjmp	.+82     	; 0x696 <vfprintf+0xfa>
 644:	89 2f       	mov	r24, r25
 646:	80 53       	subi	r24, 0x30	; 48
 648:	8a 30       	cpi	r24, 0x0A	; 10
 64a:	70 f4       	brcc	.+28     	; 0x668 <vfprintf+0xcc>
 64c:	d6 fe       	sbrs	r13, 6
 64e:	05 c0       	rjmp	.+10     	; 0x65a <vfprintf+0xbe>
 650:	98 9c       	mul	r9, r8
 652:	90 2c       	mov	r9, r0
 654:	11 24       	eor	r1, r1
 656:	98 0e       	add	r9, r24
 658:	15 c0       	rjmp	.+42     	; 0x684 <vfprintf+0xe8>
 65a:	c8 9c       	mul	r12, r8
 65c:	c0 2c       	mov	r12, r0
 65e:	11 24       	eor	r1, r1
 660:	c8 0e       	add	r12, r24
 662:	f0 e2       	ldi	r31, 0x20	; 32
 664:	df 2a       	or	r13, r31
 666:	0e c0       	rjmp	.+28     	; 0x684 <vfprintf+0xe8>
 668:	9e 32       	cpi	r25, 0x2E	; 46
 66a:	29 f4       	brne	.+10     	; 0x676 <vfprintf+0xda>
 66c:	d6 fc       	sbrc	r13, 6
 66e:	6d c1       	rjmp	.+730    	; 0x94a <vfprintf+0x3ae>
 670:	40 e4       	ldi	r20, 0x40	; 64
 672:	d4 2a       	or	r13, r20
 674:	07 c0       	rjmp	.+14     	; 0x684 <vfprintf+0xe8>
 676:	9c 36       	cpi	r25, 0x6C	; 108
 678:	19 f4       	brne	.+6      	; 0x680 <vfprintf+0xe4>
 67a:	50 e8       	ldi	r21, 0x80	; 128
 67c:	d5 2a       	or	r13, r21
 67e:	02 c0       	rjmp	.+4      	; 0x684 <vfprintf+0xe8>
 680:	98 36       	cpi	r25, 0x68	; 104
 682:	49 f4       	brne	.+18     	; 0x696 <vfprintf+0xfa>
 684:	f2 01       	movw	r30, r4
 686:	23 fd       	sbrc	r18, 3
 688:	95 91       	lpm	r25, Z+
 68a:	23 ff       	sbrs	r18, 3
 68c:	91 91       	ld	r25, Z+
 68e:	2f 01       	movw	r4, r30
 690:	99 23       	and	r25, r25
 692:	09 f0       	breq	.+2      	; 0x696 <vfprintf+0xfa>
 694:	b8 cf       	rjmp	.-144    	; 0x606 <vfprintf+0x6a>
 696:	89 2f       	mov	r24, r25
 698:	85 54       	subi	r24, 0x45	; 69
 69a:	83 30       	cpi	r24, 0x03	; 3
 69c:	18 f0       	brcs	.+6      	; 0x6a4 <vfprintf+0x108>
 69e:	80 52       	subi	r24, 0x20	; 32
 6a0:	83 30       	cpi	r24, 0x03	; 3
 6a2:	38 f4       	brcc	.+14     	; 0x6b2 <vfprintf+0x116>
 6a4:	44 e0       	ldi	r20, 0x04	; 4
 6a6:	50 e0       	ldi	r21, 0x00	; 0
 6a8:	a4 0e       	add	r10, r20
 6aa:	b5 1e       	adc	r11, r21
 6ac:	5f e3       	ldi	r21, 0x3F	; 63
 6ae:	59 83       	std	Y+1, r21	; 0x01
 6b0:	0f c0       	rjmp	.+30     	; 0x6d0 <vfprintf+0x134>
 6b2:	93 36       	cpi	r25, 0x63	; 99
 6b4:	31 f0       	breq	.+12     	; 0x6c2 <vfprintf+0x126>
 6b6:	93 37       	cpi	r25, 0x73	; 115
 6b8:	79 f0       	breq	.+30     	; 0x6d8 <vfprintf+0x13c>
 6ba:	93 35       	cpi	r25, 0x53	; 83
 6bc:	09 f0       	breq	.+2      	; 0x6c0 <vfprintf+0x124>
 6be:	56 c0       	rjmp	.+172    	; 0x76c <vfprintf+0x1d0>
 6c0:	20 c0       	rjmp	.+64     	; 0x702 <vfprintf+0x166>
 6c2:	f5 01       	movw	r30, r10
 6c4:	80 81       	ld	r24, Z
 6c6:	89 83       	std	Y+1, r24	; 0x01
 6c8:	42 e0       	ldi	r20, 0x02	; 2
 6ca:	50 e0       	ldi	r21, 0x00	; 0
 6cc:	a4 0e       	add	r10, r20
 6ce:	b5 1e       	adc	r11, r21
 6d0:	71 01       	movw	r14, r2
 6d2:	01 e0       	ldi	r16, 0x01	; 1
 6d4:	10 e0       	ldi	r17, 0x00	; 0
 6d6:	12 c0       	rjmp	.+36     	; 0x6fc <vfprintf+0x160>
 6d8:	f5 01       	movw	r30, r10
 6da:	e0 80       	ld	r14, Z
 6dc:	f1 80       	ldd	r15, Z+1	; 0x01
 6de:	d6 fc       	sbrc	r13, 6
 6e0:	03 c0       	rjmp	.+6      	; 0x6e8 <vfprintf+0x14c>
 6e2:	6f ef       	ldi	r22, 0xFF	; 255
 6e4:	7f ef       	ldi	r23, 0xFF	; 255
 6e6:	02 c0       	rjmp	.+4      	; 0x6ec <vfprintf+0x150>
 6e8:	69 2d       	mov	r22, r9
 6ea:	70 e0       	ldi	r23, 0x00	; 0
 6ec:	42 e0       	ldi	r20, 0x02	; 2
 6ee:	50 e0       	ldi	r21, 0x00	; 0
 6f0:	a4 0e       	add	r10, r20
 6f2:	b5 1e       	adc	r11, r21
 6f4:	c7 01       	movw	r24, r14
 6f6:	0e 94 b8 04 	call	0x970	; 0x970 <strnlen>
 6fa:	8c 01       	movw	r16, r24
 6fc:	5f e7       	ldi	r21, 0x7F	; 127
 6fe:	d5 22       	and	r13, r21
 700:	14 c0       	rjmp	.+40     	; 0x72a <vfprintf+0x18e>
 702:	f5 01       	movw	r30, r10
 704:	e0 80       	ld	r14, Z
 706:	f1 80       	ldd	r15, Z+1	; 0x01
 708:	d6 fc       	sbrc	r13, 6
 70a:	03 c0       	rjmp	.+6      	; 0x712 <vfprintf+0x176>
 70c:	6f ef       	ldi	r22, 0xFF	; 255
 70e:	7f ef       	ldi	r23, 0xFF	; 255
 710:	02 c0       	rjmp	.+4      	; 0x716 <vfprintf+0x17a>
 712:	69 2d       	mov	r22, r9
 714:	70 e0       	ldi	r23, 0x00	; 0
 716:	42 e0       	ldi	r20, 0x02	; 2
 718:	50 e0       	ldi	r21, 0x00	; 0
 71a:	a4 0e       	add	r10, r20
 71c:	b5 1e       	adc	r11, r21
 71e:	c7 01       	movw	r24, r14
 720:	0e 94 ad 04 	call	0x95a	; 0x95a <strnlen_P>
 724:	8c 01       	movw	r16, r24
 726:	50 e8       	ldi	r21, 0x80	; 128
 728:	d5 2a       	or	r13, r21
 72a:	d3 fe       	sbrs	r13, 3
 72c:	07 c0       	rjmp	.+14     	; 0x73c <vfprintf+0x1a0>
 72e:	1a c0       	rjmp	.+52     	; 0x764 <vfprintf+0x1c8>
 730:	b3 01       	movw	r22, r6
 732:	80 e2       	ldi	r24, 0x20	; 32
 734:	90 e0       	ldi	r25, 0x00	; 0
 736:	0e 94 c3 04 	call	0x986	; 0x986 <fputc>
 73a:	ca 94       	dec	r12
 73c:	8c 2d       	mov	r24, r12
 73e:	90 e0       	ldi	r25, 0x00	; 0
 740:	08 17       	cp	r16, r24
 742:	19 07       	cpc	r17, r25
 744:	a8 f3       	brcs	.-22     	; 0x730 <vfprintf+0x194>
 746:	0e c0       	rjmp	.+28     	; 0x764 <vfprintf+0x1c8>
 748:	f7 01       	movw	r30, r14
 74a:	d7 fc       	sbrc	r13, 7
 74c:	85 91       	lpm	r24, Z+
 74e:	d7 fe       	sbrs	r13, 7
 750:	81 91       	ld	r24, Z+
 752:	7f 01       	movw	r14, r30
 754:	b3 01       	movw	r22, r6
 756:	90 e0       	ldi	r25, 0x00	; 0
 758:	0e 94 c3 04 	call	0x986	; 0x986 <fputc>
 75c:	c1 10       	cpse	r12, r1
 75e:	ca 94       	dec	r12
 760:	01 50       	subi	r16, 0x01	; 1
 762:	10 40       	sbci	r17, 0x00	; 0
 764:	01 15       	cp	r16, r1
 766:	11 05       	cpc	r17, r1
 768:	79 f7       	brne	.-34     	; 0x748 <vfprintf+0x1ac>
 76a:	ec c0       	rjmp	.+472    	; 0x944 <vfprintf+0x3a8>
 76c:	94 36       	cpi	r25, 0x64	; 100
 76e:	11 f0       	breq	.+4      	; 0x774 <vfprintf+0x1d8>
 770:	99 36       	cpi	r25, 0x69	; 105
 772:	71 f5       	brne	.+92     	; 0x7d0 <vfprintf+0x234>
 774:	d7 fe       	sbrs	r13, 7
 776:	08 c0       	rjmp	.+16     	; 0x788 <vfprintf+0x1ec>
 778:	f5 01       	movw	r30, r10
 77a:	e0 80       	ld	r14, Z
 77c:	f1 80       	ldd	r15, Z+1	; 0x01
 77e:	02 81       	ldd	r16, Z+2	; 0x02
 780:	13 81       	ldd	r17, Z+3	; 0x03
 782:	44 e0       	ldi	r20, 0x04	; 4
 784:	50 e0       	ldi	r21, 0x00	; 0
 786:	0a c0       	rjmp	.+20     	; 0x79c <vfprintf+0x200>
 788:	f5 01       	movw	r30, r10
 78a:	80 81       	ld	r24, Z
 78c:	91 81       	ldd	r25, Z+1	; 0x01
 78e:	7c 01       	movw	r14, r24
 790:	00 27       	eor	r16, r16
 792:	f7 fc       	sbrc	r15, 7
 794:	00 95       	com	r16
 796:	10 2f       	mov	r17, r16
 798:	42 e0       	ldi	r20, 0x02	; 2
 79a:	50 e0       	ldi	r21, 0x00	; 0
 79c:	a4 0e       	add	r10, r20
 79e:	b5 1e       	adc	r11, r21
 7a0:	5f e6       	ldi	r21, 0x6F	; 111
 7a2:	d5 22       	and	r13, r21
 7a4:	17 ff       	sbrs	r17, 7
 7a6:	0a c0       	rjmp	.+20     	; 0x7bc <vfprintf+0x220>
 7a8:	10 95       	com	r17
 7aa:	00 95       	com	r16
 7ac:	f0 94       	com	r15
 7ae:	e0 94       	com	r14
 7b0:	e1 1c       	adc	r14, r1
 7b2:	f1 1c       	adc	r15, r1
 7b4:	01 1d       	adc	r16, r1
 7b6:	11 1d       	adc	r17, r1
 7b8:	80 e8       	ldi	r24, 0x80	; 128
 7ba:	d8 2a       	or	r13, r24
 7bc:	2a e0       	ldi	r18, 0x0A	; 10
 7be:	30 e0       	ldi	r19, 0x00	; 0
 7c0:	a1 01       	movw	r20, r2
 7c2:	c8 01       	movw	r24, r16
 7c4:	b7 01       	movw	r22, r14
 7c6:	0e 94 ef 04 	call	0x9de	; 0x9de <__ultoa_invert>
 7ca:	f8 2e       	mov	r15, r24
 7cc:	f2 18       	sub	r15, r2
 7ce:	40 c0       	rjmp	.+128    	; 0x850 <vfprintf+0x2b4>
 7d0:	95 37       	cpi	r25, 0x75	; 117
 7d2:	29 f4       	brne	.+10     	; 0x7de <vfprintf+0x242>
 7d4:	1d 2d       	mov	r17, r13
 7d6:	1f 7e       	andi	r17, 0xEF	; 239
 7d8:	2a e0       	ldi	r18, 0x0A	; 10
 7da:	30 e0       	ldi	r19, 0x00	; 0
 7dc:	1d c0       	rjmp	.+58     	; 0x818 <vfprintf+0x27c>
 7de:	1d 2d       	mov	r17, r13
 7e0:	19 7f       	andi	r17, 0xF9	; 249
 7e2:	9f 36       	cpi	r25, 0x6F	; 111
 7e4:	61 f0       	breq	.+24     	; 0x7fe <vfprintf+0x262>
 7e6:	90 37       	cpi	r25, 0x70	; 112
 7e8:	20 f4       	brcc	.+8      	; 0x7f2 <vfprintf+0x256>
 7ea:	98 35       	cpi	r25, 0x58	; 88
 7ec:	09 f0       	breq	.+2      	; 0x7f0 <vfprintf+0x254>
 7ee:	ad c0       	rjmp	.+346    	; 0x94a <vfprintf+0x3ae>
 7f0:	0f c0       	rjmp	.+30     	; 0x810 <vfprintf+0x274>
 7f2:	90 37       	cpi	r25, 0x70	; 112
 7f4:	39 f0       	breq	.+14     	; 0x804 <vfprintf+0x268>
 7f6:	98 37       	cpi	r25, 0x78	; 120
 7f8:	09 f0       	breq	.+2      	; 0x7fc <vfprintf+0x260>
 7fa:	a7 c0       	rjmp	.+334    	; 0x94a <vfprintf+0x3ae>
 7fc:	04 c0       	rjmp	.+8      	; 0x806 <vfprintf+0x26a>
 7fe:	28 e0       	ldi	r18, 0x08	; 8
 800:	30 e0       	ldi	r19, 0x00	; 0
 802:	0a c0       	rjmp	.+20     	; 0x818 <vfprintf+0x27c>
 804:	10 61       	ori	r17, 0x10	; 16
 806:	14 fd       	sbrc	r17, 4
 808:	14 60       	ori	r17, 0x04	; 4
 80a:	20 e1       	ldi	r18, 0x10	; 16
 80c:	30 e0       	ldi	r19, 0x00	; 0
 80e:	04 c0       	rjmp	.+8      	; 0x818 <vfprintf+0x27c>
 810:	14 fd       	sbrc	r17, 4
 812:	16 60       	ori	r17, 0x06	; 6
 814:	20 e1       	ldi	r18, 0x10	; 16
 816:	32 e0       	ldi	r19, 0x02	; 2
 818:	17 ff       	sbrs	r17, 7
 81a:	08 c0       	rjmp	.+16     	; 0x82c <vfprintf+0x290>
 81c:	f5 01       	movw	r30, r10
 81e:	60 81       	ld	r22, Z
 820:	71 81       	ldd	r23, Z+1	; 0x01
 822:	82 81       	ldd	r24, Z+2	; 0x02
 824:	93 81       	ldd	r25, Z+3	; 0x03
 826:	44 e0       	ldi	r20, 0x04	; 4
 828:	50 e0       	ldi	r21, 0x00	; 0
 82a:	08 c0       	rjmp	.+16     	; 0x83c <vfprintf+0x2a0>
 82c:	f5 01       	movw	r30, r10
 82e:	80 81       	ld	r24, Z
 830:	91 81       	ldd	r25, Z+1	; 0x01
 832:	bc 01       	movw	r22, r24
 834:	80 e0       	ldi	r24, 0x00	; 0
 836:	90 e0       	ldi	r25, 0x00	; 0
 838:	42 e0       	ldi	r20, 0x02	; 2
 83a:	50 e0       	ldi	r21, 0x00	; 0
 83c:	a4 0e       	add	r10, r20
 83e:	b5 1e       	adc	r11, r21
 840:	a1 01       	movw	r20, r2
 842:	0e 94 ef 04 	call	0x9de	; 0x9de <__ultoa_invert>
 846:	f8 2e       	mov	r15, r24
 848:	f2 18       	sub	r15, r2
 84a:	8f e7       	ldi	r24, 0x7F	; 127
 84c:	d8 2e       	mov	r13, r24
 84e:	d1 22       	and	r13, r17
 850:	d6 fe       	sbrs	r13, 6
 852:	0b c0       	rjmp	.+22     	; 0x86a <vfprintf+0x2ce>
 854:	5e ef       	ldi	r21, 0xFE	; 254
 856:	d5 22       	and	r13, r21
 858:	f9 14       	cp	r15, r9
 85a:	38 f4       	brcc	.+14     	; 0x86a <vfprintf+0x2ce>
 85c:	d4 fe       	sbrs	r13, 4
 85e:	07 c0       	rjmp	.+14     	; 0x86e <vfprintf+0x2d2>
 860:	d2 fc       	sbrc	r13, 2
 862:	05 c0       	rjmp	.+10     	; 0x86e <vfprintf+0x2d2>
 864:	8f ee       	ldi	r24, 0xEF	; 239
 866:	d8 22       	and	r13, r24
 868:	02 c0       	rjmp	.+4      	; 0x86e <vfprintf+0x2d2>
 86a:	1f 2d       	mov	r17, r15
 86c:	01 c0       	rjmp	.+2      	; 0x870 <vfprintf+0x2d4>
 86e:	19 2d       	mov	r17, r9
 870:	d4 fe       	sbrs	r13, 4
 872:	0d c0       	rjmp	.+26     	; 0x88e <vfprintf+0x2f2>
 874:	fe 01       	movw	r30, r28
 876:	ef 0d       	add	r30, r15
 878:	f1 1d       	adc	r31, r1
 87a:	80 81       	ld	r24, Z
 87c:	80 33       	cpi	r24, 0x30	; 48
 87e:	19 f4       	brne	.+6      	; 0x886 <vfprintf+0x2ea>
 880:	99 ee       	ldi	r25, 0xE9	; 233
 882:	d9 22       	and	r13, r25
 884:	08 c0       	rjmp	.+16     	; 0x896 <vfprintf+0x2fa>
 886:	1f 5f       	subi	r17, 0xFF	; 255
 888:	d2 fe       	sbrs	r13, 2
 88a:	05 c0       	rjmp	.+10     	; 0x896 <vfprintf+0x2fa>
 88c:	03 c0       	rjmp	.+6      	; 0x894 <vfprintf+0x2f8>
 88e:	8d 2d       	mov	r24, r13
 890:	86 78       	andi	r24, 0x86	; 134
 892:	09 f0       	breq	.+2      	; 0x896 <vfprintf+0x2fa>
 894:	1f 5f       	subi	r17, 0xFF	; 255
 896:	0d 2d       	mov	r16, r13
 898:	d3 fc       	sbrc	r13, 3
 89a:	14 c0       	rjmp	.+40     	; 0x8c4 <vfprintf+0x328>
 89c:	d0 fe       	sbrs	r13, 0
 89e:	0f c0       	rjmp	.+30     	; 0x8be <vfprintf+0x322>
 8a0:	1c 15       	cp	r17, r12
 8a2:	10 f0       	brcs	.+4      	; 0x8a8 <vfprintf+0x30c>
 8a4:	9f 2c       	mov	r9, r15
 8a6:	0b c0       	rjmp	.+22     	; 0x8be <vfprintf+0x322>
 8a8:	9f 2c       	mov	r9, r15
 8aa:	9c 0c       	add	r9, r12
 8ac:	91 1a       	sub	r9, r17
 8ae:	1c 2d       	mov	r17, r12
 8b0:	06 c0       	rjmp	.+12     	; 0x8be <vfprintf+0x322>
 8b2:	b3 01       	movw	r22, r6
 8b4:	80 e2       	ldi	r24, 0x20	; 32
 8b6:	90 e0       	ldi	r25, 0x00	; 0
 8b8:	0e 94 c3 04 	call	0x986	; 0x986 <fputc>
 8bc:	1f 5f       	subi	r17, 0xFF	; 255
 8be:	1c 15       	cp	r17, r12
 8c0:	c0 f3       	brcs	.-16     	; 0x8b2 <vfprintf+0x316>
 8c2:	04 c0       	rjmp	.+8      	; 0x8cc <vfprintf+0x330>
 8c4:	1c 15       	cp	r17, r12
 8c6:	10 f4       	brcc	.+4      	; 0x8cc <vfprintf+0x330>
 8c8:	c1 1a       	sub	r12, r17
 8ca:	01 c0       	rjmp	.+2      	; 0x8ce <vfprintf+0x332>
 8cc:	cc 24       	eor	r12, r12
 8ce:	04 ff       	sbrs	r16, 4
 8d0:	10 c0       	rjmp	.+32     	; 0x8f2 <vfprintf+0x356>
 8d2:	b3 01       	movw	r22, r6
 8d4:	80 e3       	ldi	r24, 0x30	; 48
 8d6:	90 e0       	ldi	r25, 0x00	; 0
 8d8:	0e 94 c3 04 	call	0x986	; 0x986 <fputc>
 8dc:	02 ff       	sbrs	r16, 2
 8de:	1e c0       	rjmp	.+60     	; 0x91c <vfprintf+0x380>
 8e0:	01 fd       	sbrc	r16, 1
 8e2:	03 c0       	rjmp	.+6      	; 0x8ea <vfprintf+0x34e>
 8e4:	88 e7       	ldi	r24, 0x78	; 120
 8e6:	90 e0       	ldi	r25, 0x00	; 0
 8e8:	02 c0       	rjmp	.+4      	; 0x8ee <vfprintf+0x352>
 8ea:	88 e5       	ldi	r24, 0x58	; 88
 8ec:	90 e0       	ldi	r25, 0x00	; 0
 8ee:	b3 01       	movw	r22, r6
 8f0:	0c c0       	rjmp	.+24     	; 0x90a <vfprintf+0x36e>
 8f2:	80 2f       	mov	r24, r16
 8f4:	86 78       	andi	r24, 0x86	; 134
 8f6:	91 f0       	breq	.+36     	; 0x91c <vfprintf+0x380>
 8f8:	01 ff       	sbrs	r16, 1
 8fa:	02 c0       	rjmp	.+4      	; 0x900 <vfprintf+0x364>
 8fc:	8b e2       	ldi	r24, 0x2B	; 43
 8fe:	01 c0       	rjmp	.+2      	; 0x902 <vfprintf+0x366>
 900:	80 e2       	ldi	r24, 0x20	; 32
 902:	d7 fc       	sbrc	r13, 7
 904:	8d e2       	ldi	r24, 0x2D	; 45
 906:	b3 01       	movw	r22, r6
 908:	90 e0       	ldi	r25, 0x00	; 0
 90a:	0e 94 c3 04 	call	0x986	; 0x986 <fputc>
 90e:	06 c0       	rjmp	.+12     	; 0x91c <vfprintf+0x380>
 910:	b3 01       	movw	r22, r6
 912:	80 e3       	ldi	r24, 0x30	; 48
 914:	90 e0       	ldi	r25, 0x00	; 0
 916:	0e 94 c3 04 	call	0x986	; 0x986 <fputc>
 91a:	9a 94       	dec	r9
 91c:	f9 14       	cp	r15, r9
 91e:	c0 f3       	brcs	.-16     	; 0x910 <vfprintf+0x374>
 920:	fa 94       	dec	r15
 922:	f1 01       	movw	r30, r2
 924:	ef 0d       	add	r30, r15
 926:	f1 1d       	adc	r31, r1
 928:	b3 01       	movw	r22, r6
 92a:	80 81       	ld	r24, Z
 92c:	90 e0       	ldi	r25, 0x00	; 0
 92e:	0e 94 c3 04 	call	0x986	; 0x986 <fputc>
 932:	ff 20       	and	r15, r15
 934:	a9 f7       	brne	.-22     	; 0x920 <vfprintf+0x384>
 936:	06 c0       	rjmp	.+12     	; 0x944 <vfprintf+0x3a8>
 938:	b3 01       	movw	r22, r6
 93a:	80 e2       	ldi	r24, 0x20	; 32
 93c:	90 e0       	ldi	r25, 0x00	; 0
 93e:	0e 94 c3 04 	call	0x986	; 0x986 <fputc>
 942:	ca 94       	dec	r12
 944:	cc 20       	and	r12, r12
 946:	c1 f7       	brne	.-16     	; 0x938 <vfprintf+0x39c>
 948:	41 ce       	rjmp	.-894    	; 0x5cc <vfprintf+0x30>
 94a:	f3 01       	movw	r30, r6
 94c:	66 81       	ldd	r22, Z+6	; 0x06
 94e:	77 81       	ldd	r23, Z+7	; 0x07
 950:	cb 01       	movw	r24, r22
 952:	2b 96       	adiw	r28, 0x0b	; 11
 954:	e2 e1       	ldi	r30, 0x12	; 18
 956:	0c 94 69 05 	jmp	0xad2	; 0xad2 <__epilogue_restores__>

0000095a <strnlen_P>:
 95a:	fc 01       	movw	r30, r24
 95c:	05 90       	lpm	r0, Z+
 95e:	61 50       	subi	r22, 0x01	; 1
 960:	70 40       	sbci	r23, 0x00	; 0
 962:	01 10       	cpse	r0, r1
 964:	d8 f7       	brcc	.-10     	; 0x95c <strnlen_P+0x2>
 966:	80 95       	com	r24
 968:	90 95       	com	r25
 96a:	8e 0f       	add	r24, r30
 96c:	9f 1f       	adc	r25, r31
 96e:	08 95       	ret

00000970 <strnlen>:
 970:	fc 01       	movw	r30, r24
 972:	61 50       	subi	r22, 0x01	; 1
 974:	70 40       	sbci	r23, 0x00	; 0
 976:	01 90       	ld	r0, Z+
 978:	01 10       	cpse	r0, r1
 97a:	d8 f7       	brcc	.-10     	; 0x972 <strnlen+0x2>
 97c:	80 95       	com	r24
 97e:	90 95       	com	r25
 980:	8e 0f       	add	r24, r30
 982:	9f 1f       	adc	r25, r31
 984:	08 95       	ret

00000986 <fputc>:
 986:	0f 93       	push	r16
 988:	1f 93       	push	r17
 98a:	cf 93       	push	r28
 98c:	df 93       	push	r29
 98e:	8c 01       	movw	r16, r24
 990:	eb 01       	movw	r28, r22
 992:	8b 81       	ldd	r24, Y+3	; 0x03
 994:	81 ff       	sbrs	r24, 1
 996:	1b c0       	rjmp	.+54     	; 0x9ce <fputc+0x48>
 998:	82 ff       	sbrs	r24, 2
 99a:	0d c0       	rjmp	.+26     	; 0x9b6 <fputc+0x30>
 99c:	2e 81       	ldd	r18, Y+6	; 0x06
 99e:	3f 81       	ldd	r19, Y+7	; 0x07
 9a0:	8c 81       	ldd	r24, Y+4	; 0x04
 9a2:	9d 81       	ldd	r25, Y+5	; 0x05
 9a4:	28 17       	cp	r18, r24
 9a6:	39 07       	cpc	r19, r25
 9a8:	64 f4       	brge	.+24     	; 0x9c2 <fputc+0x3c>
 9aa:	e8 81       	ld	r30, Y
 9ac:	f9 81       	ldd	r31, Y+1	; 0x01
 9ae:	01 93       	st	Z+, r16
 9b0:	f9 83       	std	Y+1, r31	; 0x01
 9b2:	e8 83       	st	Y, r30
 9b4:	06 c0       	rjmp	.+12     	; 0x9c2 <fputc+0x3c>
 9b6:	e8 85       	ldd	r30, Y+8	; 0x08
 9b8:	f9 85       	ldd	r31, Y+9	; 0x09
 9ba:	80 2f       	mov	r24, r16
 9bc:	09 95       	icall
 9be:	89 2b       	or	r24, r25
 9c0:	31 f4       	brne	.+12     	; 0x9ce <fputc+0x48>
 9c2:	8e 81       	ldd	r24, Y+6	; 0x06
 9c4:	9f 81       	ldd	r25, Y+7	; 0x07
 9c6:	01 96       	adiw	r24, 0x01	; 1
 9c8:	9f 83       	std	Y+7, r25	; 0x07
 9ca:	8e 83       	std	Y+6, r24	; 0x06
 9cc:	02 c0       	rjmp	.+4      	; 0x9d2 <fputc+0x4c>
 9ce:	0f ef       	ldi	r16, 0xFF	; 255
 9d0:	1f ef       	ldi	r17, 0xFF	; 255
 9d2:	c8 01       	movw	r24, r16
 9d4:	df 91       	pop	r29
 9d6:	cf 91       	pop	r28
 9d8:	1f 91       	pop	r17
 9da:	0f 91       	pop	r16
 9dc:	08 95       	ret

000009de <__ultoa_invert>:
 9de:	fa 01       	movw	r30, r20
 9e0:	aa 27       	eor	r26, r26
 9e2:	28 30       	cpi	r18, 0x08	; 8
 9e4:	51 f1       	breq	.+84     	; 0xa3a <__ultoa_invert+0x5c>
 9e6:	20 31       	cpi	r18, 0x10	; 16
 9e8:	81 f1       	breq	.+96     	; 0xa4a <__ultoa_invert+0x6c>
 9ea:	e8 94       	clt
 9ec:	6f 93       	push	r22
 9ee:	6e 7f       	andi	r22, 0xFE	; 254
 9f0:	6e 5f       	subi	r22, 0xFE	; 254
 9f2:	7f 4f       	sbci	r23, 0xFF	; 255
 9f4:	8f 4f       	sbci	r24, 0xFF	; 255
 9f6:	9f 4f       	sbci	r25, 0xFF	; 255
 9f8:	af 4f       	sbci	r26, 0xFF	; 255
 9fa:	b1 e0       	ldi	r27, 0x01	; 1
 9fc:	3e d0       	rcall	.+124    	; 0xa7a <__ultoa_invert+0x9c>
 9fe:	b4 e0       	ldi	r27, 0x04	; 4
 a00:	3c d0       	rcall	.+120    	; 0xa7a <__ultoa_invert+0x9c>
 a02:	67 0f       	add	r22, r23
 a04:	78 1f       	adc	r23, r24
 a06:	89 1f       	adc	r24, r25
 a08:	9a 1f       	adc	r25, r26
 a0a:	a1 1d       	adc	r26, r1
 a0c:	68 0f       	add	r22, r24
 a0e:	79 1f       	adc	r23, r25
 a10:	8a 1f       	adc	r24, r26
 a12:	91 1d       	adc	r25, r1
 a14:	a1 1d       	adc	r26, r1
 a16:	6a 0f       	add	r22, r26
 a18:	71 1d       	adc	r23, r1
 a1a:	81 1d       	adc	r24, r1
 a1c:	91 1d       	adc	r25, r1
 a1e:	a1 1d       	adc	r26, r1
 a20:	20 d0       	rcall	.+64     	; 0xa62 <__ultoa_invert+0x84>
 a22:	09 f4       	brne	.+2      	; 0xa26 <__ultoa_invert+0x48>
 a24:	68 94       	set
 a26:	3f 91       	pop	r19
 a28:	2a e0       	ldi	r18, 0x0A	; 10
 a2a:	26 9f       	mul	r18, r22
 a2c:	11 24       	eor	r1, r1
 a2e:	30 19       	sub	r19, r0
 a30:	30 5d       	subi	r19, 0xD0	; 208
 a32:	31 93       	st	Z+, r19
 a34:	de f6       	brtc	.-74     	; 0x9ec <__ultoa_invert+0xe>
 a36:	cf 01       	movw	r24, r30
 a38:	08 95       	ret
 a3a:	46 2f       	mov	r20, r22
 a3c:	47 70       	andi	r20, 0x07	; 7
 a3e:	40 5d       	subi	r20, 0xD0	; 208
 a40:	41 93       	st	Z+, r20
 a42:	b3 e0       	ldi	r27, 0x03	; 3
 a44:	0f d0       	rcall	.+30     	; 0xa64 <__ultoa_invert+0x86>
 a46:	c9 f7       	brne	.-14     	; 0xa3a <__ultoa_invert+0x5c>
 a48:	f6 cf       	rjmp	.-20     	; 0xa36 <__ultoa_invert+0x58>
 a4a:	46 2f       	mov	r20, r22
 a4c:	4f 70       	andi	r20, 0x0F	; 15
 a4e:	40 5d       	subi	r20, 0xD0	; 208
 a50:	4a 33       	cpi	r20, 0x3A	; 58
 a52:	18 f0       	brcs	.+6      	; 0xa5a <__ultoa_invert+0x7c>
 a54:	49 5d       	subi	r20, 0xD9	; 217
 a56:	31 fd       	sbrc	r19, 1
 a58:	40 52       	subi	r20, 0x20	; 32
 a5a:	41 93       	st	Z+, r20
 a5c:	02 d0       	rcall	.+4      	; 0xa62 <__ultoa_invert+0x84>
 a5e:	a9 f7       	brne	.-22     	; 0xa4a <__ultoa_invert+0x6c>
 a60:	ea cf       	rjmp	.-44     	; 0xa36 <__ultoa_invert+0x58>
 a62:	b4 e0       	ldi	r27, 0x04	; 4
 a64:	a6 95       	lsr	r26
 a66:	97 95       	ror	r25
 a68:	87 95       	ror	r24
 a6a:	77 95       	ror	r23
 a6c:	67 95       	ror	r22
 a6e:	ba 95       	dec	r27
 a70:	c9 f7       	brne	.-14     	; 0xa64 <__ultoa_invert+0x86>
 a72:	00 97       	sbiw	r24, 0x00	; 0
 a74:	61 05       	cpc	r22, r1
 a76:	71 05       	cpc	r23, r1
 a78:	08 95       	ret
 a7a:	9b 01       	movw	r18, r22
 a7c:	ac 01       	movw	r20, r24
 a7e:	0a 2e       	mov	r0, r26
 a80:	06 94       	lsr	r0
 a82:	57 95       	ror	r21
 a84:	47 95       	ror	r20
 a86:	37 95       	ror	r19
 a88:	27 95       	ror	r18
 a8a:	ba 95       	dec	r27
 a8c:	c9 f7       	brne	.-14     	; 0xa80 <__ultoa_invert+0xa2>
 a8e:	62 0f       	add	r22, r18
 a90:	73 1f       	adc	r23, r19
 a92:	84 1f       	adc	r24, r20
 a94:	95 1f       	adc	r25, r21
 a96:	a0 1d       	adc	r26, r0
 a98:	08 95       	ret

00000a9a <__prologue_saves__>:
 a9a:	2f 92       	push	r2
 a9c:	3f 92       	push	r3
 a9e:	4f 92       	push	r4
 aa0:	5f 92       	push	r5
 aa2:	6f 92       	push	r6
 aa4:	7f 92       	push	r7
 aa6:	8f 92       	push	r8
 aa8:	9f 92       	push	r9
 aaa:	af 92       	push	r10
 aac:	bf 92       	push	r11
 aae:	cf 92       	push	r12
 ab0:	df 92       	push	r13
 ab2:	ef 92       	push	r14
 ab4:	ff 92       	push	r15
 ab6:	0f 93       	push	r16
 ab8:	1f 93       	push	r17
 aba:	cf 93       	push	r28
 abc:	df 93       	push	r29
 abe:	cd b7       	in	r28, 0x3d	; 61
 ac0:	de b7       	in	r29, 0x3e	; 62
 ac2:	ca 1b       	sub	r28, r26
 ac4:	db 0b       	sbc	r29, r27
 ac6:	0f b6       	in	r0, 0x3f	; 63
 ac8:	f8 94       	cli
 aca:	de bf       	out	0x3e, r29	; 62
 acc:	0f be       	out	0x3f, r0	; 63
 ace:	cd bf       	out	0x3d, r28	; 61
 ad0:	09 94       	ijmp

00000ad2 <__epilogue_restores__>:
 ad2:	2a 88       	ldd	r2, Y+18	; 0x12
 ad4:	39 88       	ldd	r3, Y+17	; 0x11
 ad6:	48 88       	ldd	r4, Y+16	; 0x10
 ad8:	5f 84       	ldd	r5, Y+15	; 0x0f
 ada:	6e 84       	ldd	r6, Y+14	; 0x0e
 adc:	7d 84       	ldd	r7, Y+13	; 0x0d
 ade:	8c 84       	ldd	r8, Y+12	; 0x0c
 ae0:	9b 84       	ldd	r9, Y+11	; 0x0b
 ae2:	aa 84       	ldd	r10, Y+10	; 0x0a
 ae4:	b9 84       	ldd	r11, Y+9	; 0x09
 ae6:	c8 84       	ldd	r12, Y+8	; 0x08
 ae8:	df 80       	ldd	r13, Y+7	; 0x07
 aea:	ee 80       	ldd	r14, Y+6	; 0x06
 aec:	fd 80       	ldd	r15, Y+5	; 0x05
 aee:	0c 81       	ldd	r16, Y+4	; 0x04
 af0:	1b 81       	ldd	r17, Y+3	; 0x03
 af2:	aa 81       	ldd	r26, Y+2	; 0x02
 af4:	b9 81       	ldd	r27, Y+1	; 0x01
 af6:	ce 0f       	add	r28, r30
 af8:	d1 1d       	adc	r29, r1
 afa:	0f b6       	in	r0, 0x3f	; 63
 afc:	f8 94       	cli
 afe:	de bf       	out	0x3e, r29	; 62
 b00:	0f be       	out	0x3f, r0	; 63
 b02:	cd bf       	out	0x3d, r28	; 61
 b04:	ed 01       	movw	r28, r26
 b06:	08 95       	ret

00000b08 <_exit>:
 b08:	f8 94       	cli

00000b0a <__stop_program>:
 b0a:	ff cf       	rjmp	.-2      	; 0xb0a <__stop_program>

main.elf:     file format elf32-avr

Disassembly of section .text:

00000000 <__vectors>:
   0:	0c 94 2e 00 	jmp	0x5c	; 0x5c <__ctors_end>
   4:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
   8:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
   c:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  10:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  14:	0c 94 1e 02 	jmp	0x43c	; 0x43c <__vector_5>
  18:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  1c:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  20:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  24:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  28:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  2c:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  30:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  34:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  38:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  3c:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  40:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  44:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  48:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  4c:	0c 94 d0 00 	jmp	0x1a0	; 0x1a0 <__vector_19>
  50:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  54:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>
  58:	0c 94 4b 00 	jmp	0x96	; 0x96 <__bad_interrupt>

0000005c <__ctors_end>:
  5c:	11 24       	eor	r1, r1
  5e:	1f be       	out	0x3f, r1	; 63
  60:	cf ef       	ldi	r28, 0xFF	; 255
  62:	d4 e0       	ldi	r29, 0x04	; 4
  64:	de bf       	out	0x3e, r29	; 62
  66:	cd bf       	out	0x3d, r28	; 61

00000068 <__do_copy_data>:
  68:	11 e0       	ldi	r17, 0x01	; 1
  6a:	a0 e0       	ldi	r26, 0x00	; 0
  6c:	b1 e0       	ldi	r27, 0x01	; 1
  6e:	ec e0       	ldi	r30, 0x0C	; 12
  70:	fb e0       	ldi	r31, 0x0B	; 11
  72:	02 c0       	rjmp	.+4      	; 0x78 <.do_copy_data_start>

00000074 <.do_copy_data_loop>:
  74:	05 90       	lpm	r0, Z+
  76:	0d 92       	st	X+, r0

00000078 <.do_copy_data_start>:
  78:	a6 37       	cpi	r26, 0x76	; 118
  7a:	b1 07       	cpc	r27, r17
  7c:	d9 f7       	brne	.-10     	; 0x74 <.do_copy_data_loop>

0000007e <__do_clear_bss>:
  7e:	11 e0       	ldi	r17, 0x01	; 1
  80:	a6 e7       	ldi	r26, 0x76	; 118
  82:	b1 e0       	ldi	r27, 0x01	; 1
  84:	01 c0       	rjmp	.+2      	; 0x88 <.do_clear_bss_start>

00000086 <.do_clear_bss_loop>:
  86:	1d 92       	st	X+, r1

00000088 <.do_clear_bss_start>:
  88:	a2 39       	cpi	r26, 0x92	; 146
  8a:	b1 07       	cpc	r27, r17
  8c:	e1 f7       	brne	.-8      	; 0x86 <.do_clear_bss_loop>
  8e:	0e 94 7f 00 	call	0xfe	; 0xfe <main>
  92:	0c 94 84 05 	jmp	0xb08	; 0xb08 <_exit>

00000096 <__bad_interrupt>:
  96:	0c 94 00 00 	jmp	0	; 0x0 <__heap_end>

0000009a <clock_init>:
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void clock_init(void) {
  9a:	e1 e6       	ldi	r30, 0x61	; 97
  9c:	f0 e0       	ldi	r31, 0x00	; 0
  9e:	80 81       	ld	r24, Z
  a0:	80 83       	st	Z, r24
	CLKPR|=(uint8_t)(1<<CLKPCE);
	//CLKPR|=0x10000000;
	
	//CLKPR&=(uint8_t)~((1<<CLKPS3)|(1<<CLKPS2));
	CLKPR&=~((1<<CLKPS3)|(1<<CLKPS2));
	//CLKPR&=0x11110011;
  a2:	80 81       	ld	r24, Z
  a4:	81 71       	andi	r24, 0x11	; 17
  a6:	80 83       	st	Z, r24
		//CLKPR|=(uint8_t)(1<<CLKPS1)|(1<<CLKPS0);						//1 Mhz 0011
		CLKPR|=0x00000011;
	#elif F_CPU == 2000000
		CLKPR|=(uint8_t)(1<<CLKPS1); CLKPR&=(uint8_t)~(1<<CLKPS0);	//2 Mhz 0010
	#elif F_CPU == 4000000
		CLKPR|=(uint8_t)(1<<CLKPS0); CLKPR&=(uint8_t)~(1<<CLKPS1);	//4 Mhz 0001
  a8:	80 81       	ld	r24, Z
  aa:	81 60       	ori	r24, 0x01	; 1
  ac:	80 83       	st	Z, r24
  ae:	80 81       	ld	r24, Z
  b0:	8d 7f       	andi	r24, 0xFD	; 253
  b2:	80 83       	st	Z, r24
		1	0	0	0	256
	*/
	
	//Do something with OSCCAL ?
	
}
  b4:	08 95       	ret

000000b6 <init>:



void init(void) {
  b6:	e4 e6       	ldi	r30, 0x64	; 100
  b8:	f0 e0       	ldi	r31, 0x00	; 0
  ba:	80 81       	ld	r24, Z
  bc:	80 61       	ori	r24, 0x10	; 16
  be:	80 83       	st	Z, r24
	power_lcd_disable();
	power_spi_disable();
  c0:	80 81       	ld	r24, Z
  c2:	84 60       	ori	r24, 0x04	; 4
  c4:	80 83       	st	Z, r24
#include <avr/interrupt.h>
#include <util/delay.h>

void clock_init(void) {
	CLKPR|=(uint8_t)(1<<CLKPCE);
	//CLKPR|=0x10000000;
  c6:	e1 e6       	ldi	r30, 0x61	; 97
  c8:	f0 e0       	ldi	r31, 0x00	; 0
  ca:	80 81       	ld	r24, Z
  cc:	80 83       	st	Z, r24
	
	//CLKPR&=(uint8_t)~((1<<CLKPS3)|(1<<CLKPS2));
	CLKPR&=~((1<<CLKPS3)|(1<<CLKPS2));
	//CLKPR&=0x11110011;
  ce:	80 81       	ld	r24, Z
  d0:	81 71       	andi	r24, 0x11	; 17
  d2:	80 83       	st	Z, r24
		//CLKPR|=(uint8_t)(1<<CLKPS1)|(1<<CLKPS0);						//1 Mhz 0011
		CLKPR|=0x00000011;
	#elif F_CPU == 2000000
		CLKPR|=(uint8_t)(1<<CLKPS1); CLKPR&=(uint8_t)~(1<<CLKPS0);	//2 Mhz 0010
	#elif F_CPU == 4000000
		CLKPR|=(uint8_t)(1<<CLKPS0); CLKPR&=(uint8_t)~(1<<CLKPS1);	//4 Mhz 0001
  d4:	80 81       	ld	r24, Z
  d6:	81 60       	ori	r24, 0x01	; 1
  d8:	80 83       	st	Z, r24
  da:	80 81       	ld	r24, Z
  dc:	8d 7f       	andi	r24, 0xFD	; 253
  de:	80 83       	st	Z, r24

void init(void) {
	power_lcd_disable();
	power_spi_disable();
	clock_init();
	usart_init();
  e0:	0e 94 c5 01 	call	0x38a	; 0x38a <usart_init>
	timers_init();
  e4:	0e 94 78 02 	call	0x4f0	; 0x4f0 <timers_init>
	adc_init();
  e8:	0e 94 1f 01 	call	0x23e	; 0x23e <adc_init>
	//Motor Control Lines, set to outputs
	DDRD|=(uint8_t)(1<<1)|(1<<3)|(1<<5)|(1<<7);
  ec:	8a b1       	in	r24, 0x0a	; 10
  ee:	8a 6a       	ori	r24, 0xAA	; 170
  f0:	8a b9       	out	0x0a, r24	; 10

	sei(); //We use interupts, so enable them.
  f2:	78 94       	sei
	printf("init: done\n\n");
  f4:	80 e0       	ldi	r24, 0x00	; 0
  f6:	91 e0       	ldi	r25, 0x01	; 1
  f8:	0e 94 99 02 	call	0x532	; 0x532 <puts>
}
  fc:	08 95       	ret

000000fe <main>:

int main(void) {
  fe:	0f 93       	push	r16
 100:	1f 93       	push	r17
 102:	cf 93       	push	r28
 104:	df 93       	push	r29
	init();
 106:	0e 94 5b 00 	call	0xb6	; 0xb6 <init>
	PORTD|=(1<<M_AIN1)|(1<<M_BIN1);
 10a:	8b b1       	in	r24, 0x0b	; 11
 10c:	82 62       	ori	r24, 0x22	; 34
 10e:	8b b9       	out	0x0b, r24	; 11
	PORTD&=~((1<<M_AIN2)|(1<<M_BIN2));
 110:	8b b1       	in	r24, 0x0b	; 11
 112:	87 77       	andi	r24, 0x77	; 119
 114:	8b b9       	out	0x0b, r24	; 11
	MOTOR_LEFT=MOTOR_RIGHT=0xFFFF;
 116:	8f ef       	ldi	r24, 0xFF	; 255
 118:	9f ef       	ldi	r25, 0xFF	; 255
 11a:	90 93 8b 00 	sts	0x008B, r25
 11e:	80 93 8a 00 	sts	0x008A, r24
 122:	80 91 8a 00 	lds	r24, 0x008A
 126:	90 91 8b 00 	lds	r25, 0x008B
 12a:	90 93 89 00 	sts	0x0089, r25
 12e:	80 93 88 00 	sts	0x0088, r24
    milliseconds can be achieved.
 */
void
_delay_loop_2(uint16_t __count)
{
	__asm__ volatile (
 132:	04 e6       	ldi	r16, 0x64	; 100
 134:	10 e0       	ldi	r17, 0x00	; 0
			printf("AIN1 & BIN1\n");
		else		// 2nd
			printf("AIN2 & BIN2\n");
		_delay_ms(1000);
//		PORTD^=(1<<M_AIN1)|(1<<M_AIN2)|(1<<M_BIN1)|(1<<M_BIN2);
		printf("CLKPR %X\n",CLKPR);
 136:	c4 e2       	ldi	r28, 0x24	; 36
 138:	d1 e0       	ldi	r29, 0x01	; 1
	PORTD|=(1<<M_AIN1)|(1<<M_BIN1);
	PORTD&=~((1<<M_AIN2)|(1<<M_BIN2));
	MOTOR_LEFT=MOTOR_RIGHT=0xFFFF;
	static unsigned int ct;
	for (;;) {
		++ct;
 13a:	80 91 76 01 	lds	r24, 0x0176
 13e:	90 91 77 01 	lds	r25, 0x0177
 142:	01 96       	adiw	r24, 0x01	; 1
 144:	90 93 77 01 	sts	0x0177, r25
 148:	80 93 76 01 	sts	0x0176, r24
		//ct = 1 for first iter
		if (ct%2)	// 1st
 14c:	80 ff       	sbrs	r24, 0
 14e:	03 c0       	rjmp	.+6      	; 0x156 <main+0x58>
			printf("AIN1 & BIN1\n");
 150:	8c e0       	ldi	r24, 0x0C	; 12
 152:	91 e0       	ldi	r25, 0x01	; 1
 154:	02 c0       	rjmp	.+4      	; 0x15a <main+0x5c>
		else		// 2nd
			printf("AIN2 & BIN2\n");
 156:	88 e1       	ldi	r24, 0x18	; 24
 158:	91 e0       	ldi	r25, 0x01	; 1
 15a:	0e 94 99 02 	call	0x532	; 0x532 <puts>
 15e:	80 e1       	ldi	r24, 0x10	; 16
 160:	97 e2       	ldi	r25, 0x27	; 39
 162:	f8 01       	movw	r30, r16
 164:	31 97       	sbiw	r30, 0x01	; 1
 166:	f1 f7       	brne	.-4      	; 0x164 <main+0x66>
		__ticks = (uint16_t) (__ms * 10.0);
		while(__ticks)
		{
			// wait 1/10 ms
			_delay_loop_2(((F_CPU) / 4e3) / 10);
			__ticks --;
 168:	01 97       	sbiw	r24, 0x01	; 1
		__ticks = 1;
	else if (__tmp > 65535)
	{
		//	__ticks = requested delay in 1/10 ms
		__ticks = (uint16_t) (__ms * 10.0);
		while(__ticks)
 16a:	d9 f7       	brne	.-10     	; 0x162 <main+0x64>
		_delay_ms(1000);
//		PORTD^=(1<<M_AIN1)|(1<<M_AIN2)|(1<<M_BIN1)|(1<<M_BIN2);
		printf("CLKPR %X\n",CLKPR);
 16c:	80 91 61 00 	lds	r24, 0x0061
 170:	90 e0       	ldi	r25, 0x00	; 0
 172:	9f 93       	push	r25
 174:	8f 93       	push	r24
 176:	df 93       	push	r29
 178:	cf 93       	push	r28
 17a:	0e 94 7d 02 	call	0x4fa	; 0x4fa <printf>
 17e:	0f 90       	pop	r0
 180:	0f 90       	pop	r0
 182:	0f 90       	pop	r0
 184:	0f 90       	pop	r0
 186:	d9 cf       	rjmp	.-78     	; 0x13a <main+0x3c>

00000188 <adc_set_channel>:
	//adc_set_channel(++curr_ch);
	// Right now it will compute the first sensor twice, and store it's first find in the 4th sensor.
	// Could use one of the timers to do this, but probably unesicary
}

void adc_set_channel(uint8_t channel) {
 188:	ac e7       	ldi	r26, 0x7C	; 124
 18a:	b0 e0       	ldi	r27, 0x00	; 0
 18c:	9c 91       	ld	r25, X
 18e:	e3 e6       	ldi	r30, 0x63	; 99
 190:	f1 e0       	ldi	r31, 0x01	; 1
 192:	e8 0f       	add	r30, r24
 194:	f1 1d       	adc	r31, r1
 196:	90 7e       	andi	r25, 0xE0	; 224
 198:	80 81       	ld	r24, Z
 19a:	98 2b       	or	r25, r24
 19c:	9c 93       	st	X, r25
	//From Data Sheet: (Switching to Free Running mode (ADTS[2:0]=0) will)
	//(not cause a trigger event, even if the ADC Interrupt Flag is set.)
	ADMUX  = (uint8_t)((uint8_t)(ADMUX &0b11100000)|channels[channel]);
	//print_adc_values();
	//printf("V - chan %X\n",channel);
}
 19e:	08 95       	ret

000001a0 <__vector_19>:

//ADC Interupt handler
ISR(ADC_vect) {
 1a0:	1f 92       	push	r1
 1a2:	0f 92       	push	r0
 1a4:	0f b6       	in	r0, 0x3f	; 63
 1a6:	0f 92       	push	r0
 1a8:	11 24       	eor	r1, r1
 1aa:	2f 93       	push	r18
 1ac:	3f 93       	push	r19
 1ae:	4f 93       	push	r20
 1b0:	8f 93       	push	r24
 1b2:	9f 93       	push	r25
 1b4:	af 93       	push	r26
 1b6:	bf 93       	push	r27
 1b8:	ef 93       	push	r30
 1ba:	ff 93       	push	r31
	// New conversion has already started.
	uint16_t adc_value;
	adc_value  =  ADCL;  
 1bc:	40 91 78 00 	lds	r20, 0x0078
	adc_value += (ADCH<<8);
 1c0:	30 91 79 00 	lds	r19, 0x0079
	uint8_t real_channel;

	// the curr_ch now has the chan of the ongoing conversion, we need the last one
	if (!curr_ch)	real_channel = channel_amt-1; //curr_ch==0 
 1c4:	20 91 83 01 	lds	r18, 0x0183
 1c8:	22 23       	and	r18, r18
 1ca:	11 f4       	brne	.+4      	; 0x1d0 <__vector_19+0x30>
 1cc:	e3 e0       	ldi	r30, 0x03	; 3
 1ce:	02 c0       	rjmp	.+4      	; 0x1d4 <__vector_19+0x34>
	else		real_channel = curr_ch-1;
 1d0:	e2 2f       	mov	r30, r18
 1d2:	e1 50       	subi	r30, 0x01	; 1

	adc_val[real_channel] = adc_value;
 1d4:	f0 e0       	ldi	r31, 0x00	; 0
 1d6:	ee 0f       	add	r30, r30
 1d8:	ff 1f       	adc	r31, r31
 1da:	df 01       	movw	r26, r30
 1dc:	a5 58       	subi	r26, 0x85	; 133
 1de:	be 4f       	sbci	r27, 0xFE	; 254
 1e0:	93 2f       	mov	r25, r19
 1e2:	80 e0       	ldi	r24, 0x00	; 0
 1e4:	84 0f       	add	r24, r20
 1e6:	91 1d       	adc	r25, r1
 1e8:	8d 93       	st	X+, r24
 1ea:	9c 93       	st	X, r25
	++adc_amt[real_channel];
 1ec:	ec 57       	subi	r30, 0x7C	; 124
 1ee:	fe 4f       	sbci	r31, 0xFE	; 254
 1f0:	80 81       	ld	r24, Z
 1f2:	91 81       	ldd	r25, Z+1	; 0x01
 1f4:	01 96       	adiw	r24, 0x01	; 1
 1f6:	91 83       	std	Z+1, r25	; 0x01
 1f8:	80 83       	st	Z, r24

	// Change the channel for the conversion after the one currently processing.
	if (++curr_ch >= channel_amt)	curr_ch = 0;
 1fa:	82 2f       	mov	r24, r18
 1fc:	8f 5f       	subi	r24, 0xFF	; 255
 1fe:	80 93 83 01 	sts	0x0183, r24
 202:	84 30       	cpi	r24, 0x04	; 4
 204:	10 f0       	brcs	.+4      	; 0x20a <__vector_19+0x6a>
 206:	10 92 83 01 	sts	0x0183, r1

void adc_set_channel(uint8_t channel) {
	//Prescale reset on each "trigger event"s
	//From Data Sheet: (Switching to Free Running mode (ADTS[2:0]=0) will)
	//(not cause a trigger event, even if the ADC Interrupt Flag is set.)
	ADMUX  = (uint8_t)((uint8_t)(ADMUX &0b11100000)|channels[channel]);
 20a:	80 91 7c 00 	lds	r24, 0x007C
 20e:	e0 91 83 01 	lds	r30, 0x0183
 212:	f0 e0       	ldi	r31, 0x00	; 0
 214:	ed 59       	subi	r30, 0x9D	; 157
 216:	fe 4f       	sbci	r31, 0xFE	; 254
 218:	80 7e       	andi	r24, 0xE0	; 224
 21a:	90 81       	ld	r25, Z
 21c:	89 2b       	or	r24, r25
 21e:	80 93 7c 00 	sts	0x007C, r24
	
	//printf("adc_value: %d",adc_value);
	
	//TODO: modify for running average (Ave= (Ave*(ct-1)+New)/ct)
	//INFO: Vin[V]=(ADCH·256+ADCL)·Vref[V]/1024
}
 222:	ff 91       	pop	r31
 224:	ef 91       	pop	r30
 226:	bf 91       	pop	r27
 228:	af 91       	pop	r26
 22a:	9f 91       	pop	r25
 22c:	8f 91       	pop	r24
 22e:	4f 91       	pop	r20
 230:	3f 91       	pop	r19
 232:	2f 91       	pop	r18
 234:	0f 90       	pop	r0
 236:	0f be       	out	0x3f, r0	; 63
 238:	0f 90       	pop	r0
 23a:	1f 90       	pop	r1
 23c:	18 95       	reti

0000023e <adc_init>:
	for (uint8_t chan=0;chan<channel_amt;chan++) 
		printf(" [ %d : %d ] ", chan, adc_val[chan]);
	printf("\n");
}

void adc_init() {
 23e:	ef 92       	push	r14
 240:	ff 92       	push	r15
 242:	0f 93       	push	r16
 244:	1f 93       	push	r17
	printf("adc: init...\n");
 246:	8e e2       	ldi	r24, 0x2E	; 46
 248:	91 e0       	ldi	r25, 0x01	; 1
 24a:	0e 94 99 02 	call	0x532	; 0x532 <puts>

	power_adc_enable();
 24e:	e4 e6       	ldi	r30, 0x64	; 100
 250:	f0 e0       	ldi	r31, 0x00	; 0
 252:	80 81       	ld	r24, Z
 254:	8e 7f       	andi	r24, 0xFE	; 254
 256:	80 83       	st	Z, r24

	//Set Voltage to AVCC with external capacitor at AREF pin
	ADMUX|= (uint8_t)(1<<REFS0);
 258:	9c e7       	ldi	r25, 0x7C	; 124
 25a:	e9 2e       	mov	r14, r25
 25c:	f1 2c       	mov	r15, r1
 25e:	f7 01       	movw	r30, r14
 260:	80 81       	ld	r24, Z
 262:	80 64       	ori	r24, 0x40	; 64
 264:	80 83       	st	Z, r24
	ADMUX&=~(uint8_t)(1<<REFS1);
 266:	80 81       	ld	r24, Z
 268:	8f 77       	andi	r24, 0x7F	; 127
 26a:	80 83       	st	Z, r24
	//ADMUX&=~(1<<ADLAR); // Default disabled
	
	// Enable ADC, Inturupt, Trigger mode and set prescaler
	//ADCSRA=(((1<<ADEN)|(1<<ADIE)|(1<<ADATE))&0b11111000)|(ADC_PRESCALE);
	ADCSRA|= (uint8_t)(1<<ADEN)|(1<<ADIE)|(1<<ADATE);
 26c:	0a e7       	ldi	r16, 0x7A	; 122
 26e:	10 e0       	ldi	r17, 0x00	; 0
 270:	f8 01       	movw	r30, r16
 272:	80 81       	ld	r24, Z
 274:	88 6a       	ori	r24, 0xA8	; 168
 276:	80 83       	st	Z, r24
	ADCSRA = (uint8_t)(ADCSRA & 0b11111000)|((uint8_t)ADC_PRESCALE);
 278:	80 81       	ld	r24, Z
 27a:	88 7f       	andi	r24, 0xF8	; 248
 27c:	85 60       	ori	r24, 0x05	; 5
 27e:	80 83       	st	Z, r24
	
	// Enable Free Running Mode 
	ADCSRB|= (uint8_t)(1<<7); //reserved bit.
 280:	eb e7       	ldi	r30, 0x7B	; 123
 282:	f0 e0       	ldi	r31, 0x00	; 0
 284:	80 81       	ld	r24, Z
 286:	80 68       	ori	r24, 0x80	; 128
 288:	80 83       	st	Z, r24
	ADCSRB&= (uint8_t)~(0b111); //(ADTS2:0)=0
 28a:	80 81       	ld	r24, Z
 28c:	88 7f       	andi	r24, 0xF8	; 248
 28e:	80 83       	st	Z, r24
	
	// Disable Digital reads from analog pins
	DIDR0 |= (uint8_t)((1<<ADC4D)|(1<<ADC5D)|(1<<ADC6D)|(1<<ADC7D));
 290:	ee e7       	ldi	r30, 0x7E	; 126
 292:	f0 e0       	ldi	r31, 0x00	; 0
 294:	80 81       	ld	r24, Z
 296:	80 6f       	ori	r24, 0xF0	; 240
 298:	80 83       	st	Z, r24
	
	set_sleep_mode(SLEEP_MODE_ADC);
 29a:	83 b7       	in	r24, 0x33	; 51
 29c:	81 7f       	andi	r24, 0xF1	; 241
 29e:	82 60       	ori	r24, 0x02	; 2
 2a0:	83 bf       	out	0x33, r24	; 51
	printf("adc: setup convertions...\n");
 2a2:	8b e3       	ldi	r24, 0x3B	; 59
 2a4:	91 e0       	ldi	r25, 0x01	; 1
 2a6:	0e 94 99 02 	call	0x532	; 0x532 <puts>

void adc_set_channel(uint8_t channel) {
	//Prescale reset on each "trigger event"s
	//From Data Sheet: (Switching to Free Running mode (ADTS[2:0]=0) will)
	//(not cause a trigger event, even if the ADC Interrupt Flag is set.)
	ADMUX  = (uint8_t)((uint8_t)(ADMUX &0b11100000)|channels[channel]);
 2aa:	f7 01       	movw	r30, r14
 2ac:	90 81       	ld	r25, Z
 2ae:	80 91 83 01 	lds	r24, 0x0183
 2b2:	e3 e6       	ldi	r30, 0x63	; 99
 2b4:	f1 e0       	ldi	r31, 0x01	; 1
 2b6:	e8 0f       	add	r30, r24
 2b8:	f1 1d       	adc	r31, r1
 2ba:	90 7e       	andi	r25, 0xE0	; 224
 2bc:	80 81       	ld	r24, Z
 2be:	98 2b       	or	r25, r24
 2c0:	f7 01       	movw	r30, r14
 2c2:	90 83       	st	Z, r25
	
	set_sleep_mode(SLEEP_MODE_ADC);
	printf("adc: setup convertions...\n");
	adc_set_channel(curr_ch);
	//Start the convertions
	ADCSRA|= (uint8_t)(1<<ADSC);
 2c4:	f8 01       	movw	r30, r16
 2c6:	80 81       	ld	r24, Z
 2c8:	80 64       	ori	r24, 0x40	; 64
 2ca:	80 83       	st	Z, r24
	// Wait one adc clock cycle and change the channel
	// 125KHz
	//adc_set_channel(++curr_ch);
	// Right now it will compute the first sensor twice, and store it's first find in the 4th sensor.
	// Could use one of the timers to do this, but probably unesicary
}
 2cc:	1f 91       	pop	r17
 2ce:	0f 91       	pop	r16
 2d0:	ff 90       	pop	r15
 2d2:	ef 90       	pop	r14
 2d4:	08 95       	ret

000002d6 <print_adc_values>:
#include <avr/sleep.h>
#include <avr/power.h>
#include <stdio.h>
#include <math.h>

void print_adc_values() {
 2d6:	0f 93       	push	r16
 2d8:	1f 93       	push	r17
	//printf("ADC: ");
	for (uint8_t chan=0;chan<channel_amt;chan++) 
		printf(" [ %d : %d ] ", chan, adc_val[chan]);
 2da:	80 91 7b 01 	lds	r24, 0x017B
 2de:	90 91 7c 01 	lds	r25, 0x017C
 2e2:	9f 93       	push	r25
 2e4:	8f 93       	push	r24
 2e6:	1f 92       	push	r1
 2e8:	1f 92       	push	r1
 2ea:	05 e5       	ldi	r16, 0x55	; 85
 2ec:	11 e0       	ldi	r17, 0x01	; 1
 2ee:	1f 93       	push	r17
 2f0:	0f 93       	push	r16
 2f2:	0e 94 7d 02 	call	0x4fa	; 0x4fa <printf>
 2f6:	80 91 7d 01 	lds	r24, 0x017D
 2fa:	90 91 7e 01 	lds	r25, 0x017E
 2fe:	9f 93       	push	r25
 300:	8f 93       	push	r24
 302:	81 e0       	ldi	r24, 0x01	; 1
 304:	90 e0       	ldi	r25, 0x00	; 0
 306:	9f 93       	push	r25
 308:	8f 93       	push	r24
 30a:	1f 93       	push	r17
 30c:	0f 93       	push	r16
 30e:	0e 94 7d 02 	call	0x4fa	; 0x4fa <printf>
 312:	80 91 7f 01 	lds	r24, 0x017F
 316:	90 91 80 01 	lds	r25, 0x0180
 31a:	9f 93       	push	r25
 31c:	8f 93       	push	r24
 31e:	82 e0       	ldi	r24, 0x02	; 2
 320:	90 e0       	ldi	r25, 0x00	; 0
 322:	9f 93       	push	r25
 324:	8f 93       	push	r24
 326:	1f 93       	push	r17
 328:	0f 93       	push	r16
 32a:	0e 94 7d 02 	call	0x4fa	; 0x4fa <printf>
 32e:	80 91 81 01 	lds	r24, 0x0181
 332:	90 91 82 01 	lds	r25, 0x0182
 336:	9f 93       	push	r25
 338:	8f 93       	push	r24
 33a:	83 e0       	ldi	r24, 0x03	; 3
 33c:	90 e0       	ldi	r25, 0x00	; 0
 33e:	9f 93       	push	r25
 340:	8f 93       	push	r24
 342:	1f 93       	push	r17
 344:	0f 93       	push	r16
 346:	0e 94 7d 02 	call	0x4fa	; 0x4fa <printf>
	printf("\n");
 34a:	8a e0       	ldi	r24, 0x0A	; 10
 34c:	90 e0       	ldi	r25, 0x00	; 0
 34e:	0e 94 92 02 	call	0x524	; 0x524 <putchar>
 352:	8d b7       	in	r24, 0x3d	; 61
 354:	9e b7       	in	r25, 0x3e	; 62
 356:	48 96       	adiw	r24, 0x18	; 24
 358:	0f b6       	in	r0, 0x3f	; 63
 35a:	f8 94       	cli
 35c:	9e bf       	out	0x3e, r25	; 62
 35e:	0f be       	out	0x3f, r0	; 63
 360:	8d bf       	out	0x3d, r24	; 61
}
 362:	1f 91       	pop	r17
 364:	0f 91       	pop	r16
 366:	08 95       	ret

00000368 <usart_putchar>:
static int usart_getchar(char c, FILE *stream) {
	return 1;
}
*/

static int usart_putchar(char c, FILE *stream) {
 368:	1f 93       	push	r17
 36a:	18 2f       	mov	r17, r24

  if (c == '\n')
 36c:	8a 30       	cpi	r24, 0x0A	; 10
 36e:	19 f4       	brne	.+6      	; 0x376 <usart_putchar+0xe>
	usart_putchar('\r', stream);
 370:	8d e0       	ldi	r24, 0x0D	; 13
 372:	0e 94 b4 01 	call	0x368	; 0x368 <usart_putchar>
  loop_until_bit_is_set(UCSR0A, UDRE0);
 376:	80 91 c0 00 	lds	r24, 0x00C0
 37a:	85 ff       	sbrs	r24, 5
 37c:	fc cf       	rjmp	.-8      	; 0x376 <usart_putchar+0xe>
  UDR0 = c;
 37e:	10 93 c6 00 	sts	0x00C6, r17
  return 0;
}
 382:	80 e0       	ldi	r24, 0x00	; 0
 384:	90 e0       	ldi	r25, 0x00	; 0
 386:	1f 91       	pop	r17
 388:	08 95       	ret

0000038a <usart_init>:


void usart_init(void) {
 38a:	e4 e6       	ldi	r30, 0x64	; 100
 38c:	f0 e0       	ldi	r31, 0x00	; 0
 38e:	80 81       	ld	r24, Z
 390:	8d 7f       	andi	r24, 0xFD	; 253
 392:	80 83       	st	Z, r24
	power_usart0_enable();

	/* Set baud rate (12bit) */
	UBRR0 = UBRR_VALUE;
 394:	8c e0       	ldi	r24, 0x0C	; 12
 396:	90 e0       	ldi	r25, 0x00	; 0
 398:	90 93 c5 00 	sts	0x00C5, r25
 39c:	80 93 c4 00 	sts	0x00C4, r24
	#if USE_2X
	UCSR0A |= (1 << U2X0);
	#else
	UCSR0A &= ~(1 << U2X0);
 3a0:	e0 ec       	ldi	r30, 0xC0	; 192
 3a2:	f0 e0       	ldi	r31, 0x00	; 0
 3a4:	80 81       	ld	r24, Z
 3a6:	8d 7f       	andi	r24, 0xFD	; 253
 3a8:	80 83       	st	Z, r24
	#endif
	/* Double the uart clock */
	//UCSR0A |=(1<<U2X0);
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)	;
 3aa:	88 e1       	ldi	r24, 0x18	; 24
 3ac:	80 93 c1 00 	sts	0x00C1, r24
	/* Enable r/t interupts, hangles input when used with some buffering functions */
	//UCSR0B =|(1<<RXCIE0)|(1<<TXCIE0);
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (0<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01);
 3b0:	86 e0       	ldi	r24, 0x06	; 6
 3b2:	80 93 c2 00 	sts	0x00C2, r24
	
	stdout=&usart_stdout;
 3b6:	87 e6       	ldi	r24, 0x67	; 103
 3b8:	91 e0       	ldi	r25, 0x01	; 1
 3ba:	90 93 8f 01 	sts	0x018F, r25
 3be:	80 93 8e 01 	sts	0x018E, r24
 3c2:	08 95       	ret

000003c4 <timer2_init>:
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>


void timer2_init(void) {
 3c4:	80 91 70 00 	lds	r24, 0x0070
 3c8:	8c 7f       	andi	r24, 0xFC	; 252
 3ca:	80 93 70 00 	sts	0x0070, r24
	TIMSK2&=(uint8_t)~((1<<OCIE2A)|(1<<TOIE2));
	
	
	
	// Enable asyncronous clocking.
	ASSR&=(uint8_t)~(1<<EXCLK);
 3ce:	80 91 b6 00 	lds	r24, 0x00B6
 3d2:	8f 7e       	andi	r24, 0xEF	; 239
 3d4:	80 93 b6 00 	sts	0x00B6, r24
	ASSR|=(uint8_t)(1<<AS2);
 3d8:	80 91 b6 00 	lds	r24, 0x00B6
 3dc:	88 60       	ori	r24, 0x08	; 8
 3de:	80 93 b6 00 	sts	0x00B6, r24
	
	// Reset acculumator
	TCNT2 = 0;
 3e2:	10 92 b2 00 	sts	0x00B2, r1
	
	TCCR2A&=(uint8_t)~(1<<FOC2A);
 3e6:	80 91 b0 00 	lds	r24, 0x00B0
 3ea:	8f 77       	andi	r24, 0x7F	; 127
 3ec:	80 93 b0 00 	sts	0x00B0, r24
	*/
	// : CTC Mode.
	//TCCR2A|=(1<<WGM20);
	//TCCR2A&=~(1<<WGM21);
	// : Normal Mode.
	TCCR2A&=(uint8_t)~((1<<WGM20)|(1<<WGM21));
 3f0:	80 91 b0 00 	lds	r24, 0x00B0
 3f4:	87 7b       	andi	r24, 0xB7	; 183
 3f6:	80 93 b0 00 	sts	0x00B0, r24
		0		1		Toggle on compare match
		1		0		clear on compare match
		1		1		set on 		"		"
	*/
	// : No output
	TCCR2A&=(uint8_t)~((1<<COM2A1)|(1<<COM2A0));
 3fa:	80 91 b0 00 	lds	r24, 0x00B0
 3fe:	8f 7c       	andi	r24, 0xCF	; 207
 400:	80 93 b0 00 	sts	0x00B0, r24
	//TCCR2A|= (1<<CS20);
	
	// 32768/1024 = 32, 256 clicks, 1/8Hz
	//TCCR2A|= (1<<CS21)|(1<<CS20)|(1<<CS22); //1024 Presacle
	
	TCCR2A|= (uint8_t)(1<<CS20); //8 Presacle, 16Hz
 404:	80 91 b0 00 	lds	r24, 0x00B0
 408:	81 60       	ori	r24, 0x01	; 1
 40a:	80 93 b0 00 	sts	0x00B0, r24
	TCCR2A&=(uint8_t)~((uint8_t)((1<<CS21)|(1<<CS22)));
 40e:	80 91 b0 00 	lds	r24, 0x00B0
 412:	89 7f       	andi	r24, 0xF9	; 249
 414:	80 93 b0 00 	sts	0x00B0, r24
	
	// Wait for stuff to take affect.
	loop_until_bit_is_clear(ASSR,TCN2UB);
 418:	80 91 b6 00 	lds	r24, 0x00B6
 41c:	82 fd       	sbrc	r24, 2
 41e:	fc cf       	rjmp	.-8      	; 0x418 <timer2_init+0x54>
	loop_until_bit_is_clear(ASSR,OCR2UB);
 420:	80 91 b6 00 	lds	r24, 0x00B6
 424:	81 fd       	sbrc	r24, 1
 426:	fc cf       	rjmp	.-8      	; 0x420 <timer2_init+0x5c>
	loop_until_bit_is_clear(ASSR,TCR2UB);
 428:	80 91 b6 00 	lds	r24, 0x00B6
 42c:	80 fd       	sbrc	r24, 0
 42e:	fc cf       	rjmp	.-8      	; 0x428 <timer2_init+0x64>
	
	// Enable overflow interrupt, disable match.
	TIMSK2|= (uint8_t)(1<<TOIE2);
 430:	80 91 70 00 	lds	r24, 0x0070
 434:	81 60       	ori	r24, 0x01	; 1
 436:	80 93 70 00 	sts	0x0070, r24
	//TIMSK2&=~(1<<OCIE2A); // Disabled
}
 43a:	08 95       	ret

0000043c <__vector_5>:


// Timer2 Overflow
ISR(TIMER2_OVF_vect) {
 43c:	1f 92       	push	r1
 43e:	0f 92       	push	r0
 440:	0f b6       	in	r0, 0x3f	; 63
 442:	0f 92       	push	r0
 444:	11 24       	eor	r1, r1
 446:	8f 93       	push	r24
 448:	9f 93       	push	r25
	static uint8_t ct;//=0
	static uint16_t sec;//=0
	if (ct < 16) {
 44a:	80 91 7a 01 	lds	r24, 0x017A
 44e:	80 31       	cpi	r24, 0x10	; 16
 450:	20 f4       	brcc	.+8      	; 0x45a <__vector_5+0x1e>
		++ct; 
 452:	8f 5f       	subi	r24, 0xFF	; 255
 454:	80 93 7a 01 	sts	0x017A, r24
 458:	0b c0       	rjmp	.+22     	; 0x470 <__vector_5+0x34>
	}
	else {
		++sec;
 45a:	80 91 78 01 	lds	r24, 0x0178
 45e:	90 91 79 01 	lds	r25, 0x0179
 462:	01 96       	adiw	r24, 0x01	; 1
 464:	90 93 79 01 	sts	0x0179, r25
 468:	80 93 78 01 	sts	0x0178, r24
		ct=0;
 46c:	10 92 7a 01 	sts	0x017A, r1
	
	if (!(ct%4)) {
		// 4 Hz (16/4)
		
	}
}
 470:	9f 91       	pop	r25
 472:	8f 91       	pop	r24
 474:	0f 90       	pop	r0
 476:	0f be       	out	0x3f, r0	; 63
 478:	0f 90       	pop	r0
 47a:	1f 90       	pop	r1
 47c:	18 95       	reti

0000047e <timer1_init>:

void timer1_init(void) { // Runs the PWMs
 47e:	e0 e8       	ldi	r30, 0x80	; 128
 480:	f0 e0       	ldi	r31, 0x00	; 0
 482:	80 81       	ld	r24, Z
 484:	80 6c       	ori	r24, 0xC0	; 192
 486:	80 83       	st	Z, r24

	// Set OC1A on up, clear on down
	TCCR1A|= (uint8_t) (1<<COM1A1)|(1<<COM1A0);
	
	// Set OC1B on up, clear on down
	TCCR1A|= (uint8_t) (1<<COM1B1)|(1<<COM1B0);
 488:	80 81       	ld	r24, Z
 48a:	80 63       	ori	r24, 0x30	; 48
 48c:	80 83       	st	Z, r24
	
	// Waveform Generation Set to PWM (Phase and frequency correct, mode 8)
	TCCR1A&= (uint8_t) ~((1<<WGM11)|(1<<WGM10));
 48e:	80 81       	ld	r24, Z
 490:	8c 7f       	andi	r24, 0xFC	; 252
 492:	80 83       	st	Z, r24
	
	TCCR1B|= (uint8_t) (1<<5); //Reserved bit
 494:	e1 e8       	ldi	r30, 0x81	; 129
 496:	f0 e0       	ldi	r31, 0x00	; 0
 498:	80 81       	ld	r24, Z
 49a:	80 62       	ori	r24, 0x20	; 32
 49c:	80 83       	st	Z, r24
	
	TCCR1B|= (uint8_t) (1<<WGM13);
 49e:	80 81       	ld	r24, Z
 4a0:	80 61       	ori	r24, 0x10	; 16
 4a2:	80 83       	st	Z, r24
	TCCR1B&= (uint8_t)~(1<<WGM12);
 4a4:	80 81       	ld	r24, Z
 4a6:	87 7f       	andi	r24, 0xF7	; 247
 4a8:	80 83       	st	Z, r24
	
	// Disable Input noise canceler
	TCCR1B|= (uint8_t)(1<<ICNC1);
 4aa:	80 81       	ld	r24, Z
 4ac:	80 68       	ori	r24, 0x80	; 128
 4ae:	80 83       	st	Z, r24
	
	// Set TOP
	ICR1=0xFFFF;
 4b0:	2f ef       	ldi	r18, 0xFF	; 255
 4b2:	3f ef       	ldi	r19, 0xFF	; 255
 4b4:	30 93 87 00 	sts	0x0087, r19
 4b8:	20 93 86 00 	sts	0x0086, r18
	//prescale = F_CPU/(2*100000*0xFFFF) = 
	// At 2MHz the best we get is 15Hz (bad. very bad.)
	
	// Set prescale to 1
	// TCCR1B = (TCCR1B & 0b11111000)|(log(prescale)/log(2));
	TCCR1B&= (uint8_t)~((1<<CS12)|(1<<CS11));
 4bc:	80 81       	ld	r24, Z
 4be:	89 7f       	andi	r24, 0xF9	; 249
 4c0:	80 83       	st	Z, r24
	TCCR1B|= (uint8_t) (1<<CS10);
 4c2:	80 81       	ld	r24, Z
 4c4:	81 60       	ori	r24, 0x01	; 1
 4c6:	80 83       	st	Z, r24
	
	//OCR1A and OCR1B are the Compare / PWM registers
	OCR1A = OCR1B = 0xFFFF; // Max = 65535,0xFFFF
 4c8:	ea e8       	ldi	r30, 0x8A	; 138
 4ca:	f0 e0       	ldi	r31, 0x00	; 0
 4cc:	31 83       	std	Z+1, r19	; 0x01
 4ce:	20 83       	st	Z, r18
 4d0:	80 81       	ld	r24, Z
 4d2:	91 81       	ldd	r25, Z+1	; 0x01
 4d4:	90 93 89 00 	sts	0x0089, r25
 4d8:	80 93 88 00 	sts	0x0088, r24
	
	// disable the interupts (probably done by default).
	TIMSK1&= (uint8_t)~((1<<ICIE1)|(1<<OCIE1B)|(1<<OCIE1A)|(1<<TOIE1));
 4dc:	ef e6       	ldi	r30, 0x6F	; 111
 4de:	f0 e0       	ldi	r31, 0x00	; 0
 4e0:	80 81       	ld	r24, Z
 4e2:	88 7d       	andi	r24, 0xD8	; 216
 4e4:	80 83       	st	Z, r24
	
	DDRB|= (uint8_t)(1<<PB5)|(1<<PB6);
 4e6:	84 b1       	in	r24, 0x04	; 4
 4e8:	80 66       	ori	r24, 0x60	; 96
 4ea:	84 b9       	out	0x04, r24	; 4
}
 4ec:	08 95       	ret

000004ee <timer0_init>:

void timer0_init(void) {
 4ee:	08 95       	ret

000004f0 <timers_init>:
}

void timers_init(void) {
 4f0:	0e 94 3f 02 	call	0x47e	; 0x47e <timer1_init>
	timer0_init();
	timer1_init();
	timer2_init();
 4f4:	0e 94 e2 01 	call	0x3c4	; 0x3c4 <timer2_init>
}
 4f8:	08 95       	ret

000004fa <printf>:
 4fa:	a0 e0       	ldi	r26, 0x00	; 0
 4fc:	b0 e0       	ldi	r27, 0x00	; 0
 4fe:	e3 e8       	ldi	r30, 0x83	; 131
 500:	f2 e0       	ldi	r31, 0x02	; 2
 502:	0c 94 5d 05 	jmp	0xaba	; 0xaba <__prologue_saves__+0x20>
 506:	fe 01       	movw	r30, r28
 508:	35 96       	adiw	r30, 0x05	; 5
 50a:	61 91       	ld	r22, Z+
 50c:	71 91       	ld	r23, Z+
 50e:	af 01       	movw	r20, r30
 510:	80 91 8e 01 	lds	r24, 0x018E
 514:	90 91 8f 01 	lds	r25, 0x018F
 518:	0e 94 ce 02 	call	0x59c	; 0x59c <vfprintf>
 51c:	20 96       	adiw	r28, 0x00	; 0
 51e:	e2 e0       	ldi	r30, 0x02	; 2
 520:	0c 94 79 05 	jmp	0xaf2	; 0xaf2 <__epilogue_restores__+0x20>

00000524 <putchar>:
 524:	60 91 8e 01 	lds	r22, 0x018E
 528:	70 91 8f 01 	lds	r23, 0x018F
 52c:	0e 94 c3 04 	call	0x986	; 0x986 <fputc>
 530:	08 95       	ret

00000532 <puts>:
 532:	0f 93       	push	r16
 534:	1f 93       	push	r17
 536:	cf 93       	push	r28
 538:	df 93       	push	r29
 53a:	8c 01       	movw	r16, r24
 53c:	e0 91 8e 01 	lds	r30, 0x018E
 540:	f0 91 8f 01 	lds	r31, 0x018F
 544:	83 81       	ldd	r24, Z+3	; 0x03
 546:	81 ff       	sbrs	r24, 1
 548:	21 c0       	rjmp	.+66     	; 0x58c <puts+0x5a>
 54a:	c0 e0       	ldi	r28, 0x00	; 0
 54c:	d0 e0       	ldi	r29, 0x00	; 0
 54e:	0d c0       	rjmp	.+26     	; 0x56a <puts+0x38>
 550:	e0 91 8e 01 	lds	r30, 0x018E
 554:	f0 91 8f 01 	lds	r31, 0x018F
 558:	20 85       	ldd	r18, Z+8	; 0x08
 55a:	31 85       	ldd	r19, Z+9	; 0x09
 55c:	bf 01       	movw	r22, r30
 55e:	f9 01       	movw	r30, r18
 560:	09 95       	icall
 562:	89 2b       	or	r24, r25
 564:	11 f0       	breq	.+4      	; 0x56a <puts+0x38>
 566:	cf ef       	ldi	r28, 0xFF	; 255
 568:	df ef       	ldi	r29, 0xFF	; 255
 56a:	f8 01       	movw	r30, r16
 56c:	81 91       	ld	r24, Z+
 56e:	8f 01       	movw	r16, r30
 570:	88 23       	and	r24, r24
 572:	71 f7       	brne	.-36     	; 0x550 <puts+0x1e>
 574:	e0 91 8e 01 	lds	r30, 0x018E
 578:	f0 91 8f 01 	lds	r31, 0x018F
 57c:	20 85       	ldd	r18, Z+8	; 0x08
 57e:	31 85       	ldd	r19, Z+9	; 0x09
 580:	bf 01       	movw	r22, r30
 582:	8a e0       	ldi	r24, 0x0A	; 10
 584:	f9 01       	movw	r30, r18
 586:	09 95       	icall
 588:	89 2b       	or	r24, r25
 58a:	11 f0       	breq	.+4      	; 0x590 <puts+0x5e>
 58c:	cf ef       	ldi	r28, 0xFF	; 255
 58e:	df ef       	ldi	r29, 0xFF	; 255
 590:	ce 01       	movw	r24, r28
 592:	df 91       	pop	r29
 594:	cf 91       	pop	r28
 596:	1f 91       	pop	r17
 598:	0f 91       	pop	r16
 59a:	08 95       	ret

0000059c <vfprintf>:
 59c:	ab e0       	ldi	r26, 0x0B	; 11
 59e:	b0 e0       	ldi	r27, 0x00	; 0
 5a0:	e4 ed       	ldi	r30, 0xD4	; 212
 5a2:	f2 e0       	ldi	r31, 0x02	; 2
 5a4:	0c 94 4d 05 	jmp	0xa9a	; 0xa9a <__prologue_saves__>
 5a8:	3c 01       	movw	r6, r24
 5aa:	2b 01       	movw	r4, r22
 5ac:	5a 01       	movw	r10, r20
 5ae:	fc 01       	movw	r30, r24
 5b0:	17 82       	std	Z+7, r1	; 0x07
 5b2:	16 82       	std	Z+6, r1	; 0x06
 5b4:	83 81       	ldd	r24, Z+3	; 0x03
 5b6:	81 fd       	sbrc	r24, 1
 5b8:	03 c0       	rjmp	.+6      	; 0x5c0 <vfprintf+0x24>
 5ba:	6f ef       	ldi	r22, 0xFF	; 255
 5bc:	7f ef       	ldi	r23, 0xFF	; 255
 5be:	c8 c1       	rjmp	.+912    	; 0x950 <vfprintf+0x3b4>
 5c0:	9a e0       	ldi	r25, 0x0A	; 10
 5c2:	89 2e       	mov	r8, r25
 5c4:	1e 01       	movw	r2, r28
 5c6:	08 94       	sec
 5c8:	21 1c       	adc	r2, r1
 5ca:	31 1c       	adc	r3, r1
 5cc:	f3 01       	movw	r30, r6
 5ce:	23 81       	ldd	r18, Z+3	; 0x03
 5d0:	f2 01       	movw	r30, r4
 5d2:	23 fd       	sbrc	r18, 3
 5d4:	85 91       	lpm	r24, Z+
 5d6:	23 ff       	sbrs	r18, 3
 5d8:	81 91       	ld	r24, Z+
 5da:	2f 01       	movw	r4, r30
 5dc:	88 23       	and	r24, r24
 5de:	09 f4       	brne	.+2      	; 0x5e2 <vfprintf+0x46>
 5e0:	b4 c1       	rjmp	.+872    	; 0x94a <vfprintf+0x3ae>
 5e2:	85 32       	cpi	r24, 0x25	; 37
 5e4:	39 f4       	brne	.+14     	; 0x5f4 <vfprintf+0x58>
 5e6:	23 fd       	sbrc	r18, 3
 5e8:	85 91       	lpm	r24, Z+
 5ea:	23 ff       	sbrs	r18, 3
 5ec:	81 91       	ld	r24, Z+
 5ee:	2f 01       	movw	r4, r30
 5f0:	85 32       	cpi	r24, 0x25	; 37
 5f2:	29 f4       	brne	.+10     	; 0x5fe <vfprintf+0x62>
 5f4:	b3 01       	movw	r22, r6
 5f6:	90 e0       	ldi	r25, 0x00	; 0
 5f8:	0e 94 c3 04 	call	0x986	; 0x986 <fputc>
 5fc:	e7 cf       	rjmp	.-50     	; 0x5cc <vfprintf+0x30>
 5fe:	98 2f       	mov	r25, r24
 600:	dd 24       	eor	r13, r13
 602:	cc 24       	eor	r12, r12
 604:	99 24       	eor	r9, r9
 606:	ff e1       	ldi	r31, 0x1F	; 31
 608:	fd 15       	cp	r31, r13
 60a:	d0 f0       	brcs	.+52     	; 0x640 <vfprintf+0xa4>
 60c:	9b 32       	cpi	r25, 0x2B	; 43
 60e:	69 f0       	breq	.+26     	; 0x62a <vfprintf+0x8e>
 610:	9c 32       	cpi	r25, 0x2C	; 44
 612:	28 f4       	brcc	.+10     	; 0x61e <vfprintf+0x82>
 614:	90 32       	cpi	r25, 0x20	; 32
 616:	59 f0       	breq	.+22     	; 0x62e <vfprintf+0x92>
 618:	93 32       	cpi	r25, 0x23	; 35
 61a:	91 f4       	brne	.+36     	; 0x640 <vfprintf+0xa4>
 61c:	0e c0       	rjmp	.+28     	; 0x63a <vfprintf+0x9e>
 61e:	9d 32       	cpi	r25, 0x2D	; 45
 620:	49 f0       	breq	.+18     	; 0x634 <vfprintf+0x98>
 622:	90 33       	cpi	r25, 0x30	; 48
 624:	69 f4       	brne	.+26     	; 0x640 <vfprintf+0xa4>
 626:	41 e0       	ldi	r20, 0x01	; 1
 628:	24 c0       	rjmp	.+72     	; 0x672 <vfprintf+0xd6>
 62a:	52 e0       	ldi	r21, 0x02	; 2
 62c:	d5 2a       	or	r13, r21
 62e:	84 e0       	ldi	r24, 0x04	; 4
 630:	d8 2a       	or	r13, r24
 632:	28 c0       	rjmp	.+80     	; 0x684 <vfprintf+0xe8>
 634:	98 e0       	ldi	r25, 0x08	; 8
 636:	d9 2a       	or	r13, r25
 638:	25 c0       	rjmp	.+74     	; 0x684 <vfprintf+0xe8>
 63a:	e0 e1       	ldi	r30, 0x10	; 16
 63c:	de 2a       	or	r13, r30
 63e:	22 c0       	rjmp	.+68     	; 0x684 <vfprintf+0xe8>
 640:	d7 fc       	sbrc	r13, 7
 642:	29 c0       	rjmp	.+82     	; 0x696 <vfprintf+0xfa>
 644:	89 2f       	mov	r24, r25
 646:	80 53       	subi	r24, 0x30	; 48
 648:	8a 30       	cpi	r24, 0x0A	; 10
 64a:	70 f4       	brcc	.+28     	; 0x668 <vfprintf+0xcc>
 64c:	d6 fe       	sbrs	r13, 6
 64e:	05 c0       	rjmp	.+10     	; 0x65a <vfprintf+0xbe>
 650:	98 9c       	mul	r9, r8
 652:	90 2c       	mov	r9, r0
 654:	11 24       	eor	r1, r1
 656:	98 0e       	add	r9, r24
 658:	15 c0       	rjmp	.+42     	; 0x684 <vfprintf+0xe8>
 65a:	c8 9c       	mul	r12, r8
 65c:	c0 2c       	mov	r12, r0
 65e:	11 24       	eor	r1, r1
 660:	c8 0e       	add	r12, r24
 662:	f0 e2       	ldi	r31, 0x20	; 32
 664:	df 2a       	or	r13, r31
 666:	0e c0       	rjmp	.+28     	; 0x684 <vfprintf+0xe8>
 668:	9e 32       	cpi	r25, 0x2E	; 46
 66a:	29 f4       	brne	.+10     	; 0x676 <vfprintf+0xda>
 66c:	d6 fc       	sbrc	r13, 6
 66e:	6d c1       	rjmp	.+730    	; 0x94a <vfprintf+0x3ae>
 670:	40 e4       	ldi	r20, 0x40	; 64
 672:	d4 2a       	or	r13, r20
 674:	07 c0       	rjmp	.+14     	; 0x684 <vfprintf+0xe8>
 676:	9c 36       	cpi	r25, 0x6C	; 108
 678:	19 f4       	brne	.+6      	; 0x680 <vfprintf+0xe4>
 67a:	50 e8       	ldi	r21, 0x80	; 128
 67c:	d5 2a       	or	r13, r21
 67e:	02 c0       	rjmp	.+4      	; 0x684 <vfprintf+0xe8>
 680:	98 36       	cpi	r25, 0x68	; 104
 682:	49 f4       	brne	.+18     	; 0x696 <vfprintf+0xfa>
 684:	f2 01       	movw	r30, r4
 686:	23 fd       	sbrc	r18, 3
 688:	95 91       	lpm	r25, Z+
 68a:	23 ff       	sbrs	r18, 3
 68c:	91 91       	ld	r25, Z+
 68e:	2f 01       	movw	r4, r30
 690:	99 23       	and	r25, r25
 692:	09 f0       	breq	.+2      	; 0x696 <vfprintf+0xfa>
 694:	b8 cf       	rjmp	.-144    	; 0x606 <vfprintf+0x6a>
 696:	89 2f       	mov	r24, r25
 698:	85 54       	subi	r24, 0x45	; 69
 69a:	83 30       	cpi	r24, 0x03	; 3
 69c:	18 f0       	brcs	.+6      	; 0x6a4 <vfprintf+0x108>
 69e:	80 52       	subi	r24, 0x20	; 32
 6a0:	83 30       	cpi	r24, 0x03	; 3
 6a2:	38 f4       	brcc	.+14     	; 0x6b2 <vfprintf+0x116>
 6a4:	44 e0       	ldi	r20, 0x04	; 4
 6a6:	50 e0       	ldi	r21, 0x00	; 0
 6a8:	a4 0e       	add	r10, r20
 6aa:	b5 1e       	adc	r11, r21
 6ac:	5f e3       	ldi	r21, 0x3F	; 63
 6ae:	59 83       	std	Y+1, r21	; 0x01
 6b0:	0f c0       	rjmp	.+30     	; 0x6d0 <vfprintf+0x134>
 6b2:	93 36       	cpi	r25, 0x63	; 99
 6b4:	31 f0       	breq	.+12     	; 0x6c2 <vfprintf+0x126>
 6b6:	93 37       	cpi	r25, 0x73	; 115
 6b8:	79 f0       	breq	.+30     	; 0x6d8 <vfprintf+0x13c>
 6ba:	93 35       	cpi	r25, 0x53	; 83
 6bc:	09 f0       	breq	.+2      	; 0x6c0 <vfprintf+0x124>
 6be:	56 c0       	rjmp	.+172    	; 0x76c <vfprintf+0x1d0>
 6c0:	20 c0       	rjmp	.+64     	; 0x702 <vfprintf+0x166>
 6c2:	f5 01       	movw	r30, r10
 6c4:	80 81       	ld	r24, Z
 6c6:	89 83       	std	Y+1, r24	; 0x01
 6c8:	42 e0       	ldi	r20, 0x02	; 2
 6ca:	50 e0       	ldi	r21, 0x00	; 0
 6cc:	a4 0e       	add	r10, r20
 6ce:	b5 1e       	adc	r11, r21
 6d0:	71 01       	movw	r14, r2
 6d2:	01 e0       	ldi	r16, 0x01	; 1
 6d4:	10 e0       	ldi	r17, 0x00	; 0
 6d6:	12 c0       	rjmp	.+36     	; 0x6fc <vfprintf+0x160>
 6d8:	f5 01       	movw	r30, r10
 6da:	e0 80       	ld	r14, Z
 6dc:	f1 80       	ldd	r15, Z+1	; 0x01
 6de:	d6 fc       	sbrc	r13, 6
 6e0:	03 c0       	rjmp	.+6      	; 0x6e8 <vfprintf+0x14c>
 6e2:	6f ef       	ldi	r22, 0xFF	; 255
 6e4:	7f ef       	ldi	r23, 0xFF	; 255
 6e6:	02 c0       	rjmp	.+4      	; 0x6ec <vfprintf+0x150>
 6e8:	69 2d       	mov	r22, r9
 6ea:	70 e0       	ldi	r23, 0x00	; 0
 6ec:	42 e0       	ldi	r20, 0x02	; 2
 6ee:	50 e0       	ldi	r21, 0x00	; 0
 6f0:	a4 0e       	add	r10, r20
 6f2:	b5 1e       	adc	r11, r21
 6f4:	c7 01       	movw	r24, r14
 6f6:	0e 94 b8 04 	call	0x970	; 0x970 <strnlen>
 6fa:	8c 01       	movw	r16, r24
 6fc:	5f e7       	ldi	r21, 0x7F	; 127
 6fe:	d5 22       	and	r13, r21
 700:	14 c0       	rjmp	.+40     	; 0x72a <vfprintf+0x18e>
 702:	f5 01       	movw	r30, r10
 704:	e0 80       	ld	r14, Z
 706:	f1 80       	ldd	r15, Z+1	; 0x01
 708:	d6 fc       	sbrc	r13, 6
 70a:	03 c0       	rjmp	.+6      	; 0x712 <vfprintf+0x176>
 70c:	6f ef       	ldi	r22, 0xFF	; 255
 70e:	7f ef       	ldi	r23, 0xFF	; 255
 710:	02 c0       	rjmp	.+4      	; 0x716 <vfprintf+0x17a>
 712:	69 2d       	mov	r22, r9
 714:	70 e0       	ldi	r23, 0x00	; 0
 716:	42 e0       	ldi	r20, 0x02	; 2
 718:	50 e0       	ldi	r21, 0x00	; 0
 71a:	a4 0e       	add	r10, r20
 71c:	b5 1e       	adc	r11, r21
 71e:	c7 01       	movw	r24, r14
 720:	0e 94 ad 04 	call	0x95a	; 0x95a <strnlen_P>
 724:	8c 01       	movw	r16, r24
 726:	50 e8       	ldi	r21, 0x80	; 128
 728:	d5 2a       	or	r13, r21
 72a:	d3 fe       	sbrs	r13, 3
 72c:	07 c0       	rjmp	.+14     	; 0x73c <vfprintf+0x1a0>
 72e:	1a c0       	rjmp	.+52     	; 0x764 <vfprintf+0x1c8>
 730:	b3 01       	movw	r22, r6
 732:	80 e2       	ldi	r24, 0x20	; 32
 734:	90 e0       	ldi	r25, 0x00	; 0
 736:	0e 94 c3 04 	call	0x986	; 0x986 <fputc>
 73a:	ca 94       	dec	r12
 73c:	8c 2d       	mov	r24, r12
 73e:	90 e0       	ldi	r25, 0x00	; 0
 740:	08 17       	cp	r16, r24
 742:	19 07       	cpc	r17, r25
 744:	a8 f3       	brcs	.-22     	; 0x730 <vfprintf+0x194>
 746:	0e c0       	rjmp	.+28     	; 0x764 <vfprintf+0x1c8>
 748:	f7 01       	movw	r30, r14
 74a:	d7 fc       	sbrc	r13, 7
 74c:	85 91       	lpm	r24, Z+
 74e:	d7 fe       	sbrs	r13, 7
 750:	81 91       	ld	r24, Z+
 752:	7f 01       	movw	r14, r30
 754:	b3 01       	movw	r22, r6
 756:	90 e0       	ldi	r25, 0x00	; 0
 758:	0e 94 c3 04 	call	0x986	; 0x986 <fputc>
 75c:	c1 10       	cpse	r12, r1
 75e:	ca 94       	dec	r12
 760:	01 50       	subi	r16, 0x01	; 1
 762:	10 40       	sbci	r17, 0x00	; 0
 764:	01 15       	cp	r16, r1
 766:	11 05       	cpc	r17, r1
 768:	79 f7       	brne	.-34     	; 0x748 <vfprintf+0x1ac>
 76a:	ec c0       	rjmp	.+472    	; 0x944 <vfprintf+0x3a8>
 76c:	94 36       	cpi	r25, 0x64	; 100
 76e:	11 f0       	breq	.+4      	; 0x774 <vfprintf+0x1d8>
 770:	99 36       	cpi	r25, 0x69	; 105
 772:	71 f5       	brne	.+92     	; 0x7d0 <vfprintf+0x234>
 774:	d7 fe       	sbrs	r13, 7
 776:	08 c0       	rjmp	.+16     	; 0x788 <vfprintf+0x1ec>
 778:	f5 01       	movw	r30, r10
 77a:	e0 80       	ld	r14, Z
 77c:	f1 80       	ldd	r15, Z+1	; 0x01
 77e:	02 81       	ldd	r16, Z+2	; 0x02
 780:	13 81       	ldd	r17, Z+3	; 0x03
 782:	44 e0       	ldi	r20, 0x04	; 4
 784:	50 e0       	ldi	r21, 0x00	; 0
 786:	0a c0       	rjmp	.+20     	; 0x79c <vfprintf+0x200>
 788:	f5 01       	movw	r30, r10
 78a:	80 81       	ld	r24, Z
 78c:	91 81       	ldd	r25, Z+1	; 0x01
 78e:	7c 01       	movw	r14, r24
 790:	00 27       	eor	r16, r16
 792:	f7 fc       	sbrc	r15, 7
 794:	00 95       	com	r16
 796:	10 2f       	mov	r17, r16
 798:	42 e0       	ldi	r20, 0x02	; 2
 79a:	50 e0       	ldi	r21, 0x00	; 0
 79c:	a4 0e       	add	r10, r20
 79e:	b5 1e       	adc	r11, r21
 7a0:	5f e6       	ldi	r21, 0x6F	; 111
 7a2:	d5 22       	and	r13, r21
 7a4:	17 ff       	sbrs	r17, 7
 7a6:	0a c0       	rjmp	.+20     	; 0x7bc <vfprintf+0x220>
 7a8:	10 95       	com	r17
 7aa:	00 95       	com	r16
 7ac:	f0 94       	com	r15
 7ae:	e0 94       	com	r14
 7b0:	e1 1c       	adc	r14, r1
 7b2:	f1 1c       	adc	r15, r1
 7b4:	01 1d       	adc	r16, r1
 7b6:	11 1d       	adc	r17, r1
 7b8:	80 e8       	ldi	r24, 0x80	; 128
 7ba:	d8 2a       	or	r13, r24
 7bc:	2a e0       	ldi	r18, 0x0A	; 10
 7be:	30 e0       	ldi	r19, 0x00	; 0
 7c0:	a1 01       	movw	r20, r2
 7c2:	c8 01       	movw	r24, r16
 7c4:	b7 01       	movw	r22, r14
 7c6:	0e 94 ef 04 	call	0x9de	; 0x9de <__ultoa_invert>
 7ca:	f8 2e       	mov	r15, r24
 7cc:	f2 18       	sub	r15, r2
 7ce:	40 c0       	rjmp	.+128    	; 0x850 <vfprintf+0x2b4>
 7d0:	95 37       	cpi	r25, 0x75	; 117
 7d2:	29 f4       	brne	.+10     	; 0x7de <vfprintf+0x242>
 7d4:	1d 2d       	mov	r17, r13
 7d6:	1f 7e       	andi	r17, 0xEF	; 239
 7d8:	2a e0       	ldi	r18, 0x0A	; 10
 7da:	30 e0       	ldi	r19, 0x00	; 0
 7dc:	1d c0       	rjmp	.+58     	; 0x818 <vfprintf+0x27c>
 7de:	1d 2d       	mov	r17, r13
 7e0:	19 7f       	andi	r17, 0xF9	; 249
 7e2:	9f 36       	cpi	r25, 0x6F	; 111
 7e4:	61 f0       	breq	.+24     	; 0x7fe <vfprintf+0x262>
 7e6:	90 37       	cpi	r25, 0x70	; 112
 7e8:	20 f4       	brcc	.+8      	; 0x7f2 <vfprintf+0x256>
 7ea:	98 35       	cpi	r25, 0x58	; 88
 7ec:	09 f0       	breq	.+2      	; 0x7f0 <vfprintf+0x254>
 7ee:	ad c0       	rjmp	.+346    	; 0x94a <vfprintf+0x3ae>
 7f0:	0f c0       	rjmp	.+30     	; 0x810 <vfprintf+0x274>
 7f2:	90 37       	cpi	r25, 0x70	; 112
 7f4:	39 f0       	breq	.+14     	; 0x804 <vfprintf+0x268>
 7f6:	98 37       	cpi	r25, 0x78	; 120
 7f8:	09 f0       	breq	.+2      	; 0x7fc <vfprintf+0x260>
 7fa:	a7 c0       	rjmp	.+334    	; 0x94a <vfprintf+0x3ae>
 7fc:	04 c0       	rjmp	.+8      	; 0x806 <vfprintf+0x26a>
 7fe:	28 e0       	ldi	r18, 0x08	; 8
 800:	30 e0       	ldi	r19, 0x00	; 0
 802:	0a c0       	rjmp	.+20     	; 0x818 <vfprintf+0x27c>
 804:	10 61       	ori	r17, 0x10	; 16
 806:	14 fd       	sbrc	r17, 4
 808:	14 60       	ori	r17, 0x04	; 4
 80a:	20 e1       	ldi	r18, 0x10	; 16
 80c:	30 e0       	ldi	r19, 0x00	; 0
 80e:	04 c0       	rjmp	.+8      	; 0x818 <vfprintf+0x27c>
 810:	14 fd       	sbrc	r17, 4
 812:	16 60       	ori	r17, 0x06	; 6
 814:	20 e1       	ldi	r18, 0x10	; 16
 816:	32 e0       	ldi	r19, 0x02	; 2
 818:	17 ff       	sbrs	r17, 7
 81a:	08 c0       	rjmp	.+16     	; 0x82c <vfprintf+0x290>
 81c:	f5 01       	movw	r30, r10
 81e:	60 81       	ld	r22, Z
 820:	71 81       	ldd	r23, Z+1	; 0x01
 822:	82 81       	ldd	r24, Z+2	; 0x02
 824:	93 81       	ldd	r25, Z+3	; 0x03
 826:	44 e0       	ldi	r20, 0x04	; 4
 828:	50 e0       	ldi	r21, 0x00	; 0
 82a:	08 c0       	rjmp	.+16     	; 0x83c <vfprintf+0x2a0>
 82c:	f5 01       	movw	r30, r10
 82e:	80 81       	ld	r24, Z
 830:	91 81       	ldd	r25, Z+1	; 0x01
 832:	bc 01       	movw	r22, r24
 834:	80 e0       	ldi	r24, 0x00	; 0
 836:	90 e0       	ldi	r25, 0x00	; 0
 838:	42 e0       	ldi	r20, 0x02	; 2
 83a:	50 e0       	ldi	r21, 0x00	; 0
 83c:	a4 0e       	add	r10, r20
 83e:	b5 1e       	adc	r11, r21
 840:	a1 01       	movw	r20, r2
 842:	0e 94 ef 04 	call	0x9de	; 0x9de <__ultoa_invert>
 846:	f8 2e       	mov	r15, r24
 848:	f2 18       	sub	r15, r2
 84a:	8f e7       	ldi	r24, 0x7F	; 127
 84c:	d8 2e       	mov	r13, r24
 84e:	d1 22       	and	r13, r17
 850:	d6 fe       	sbrs	r13, 6
 852:	0b c0       	rjmp	.+22     	; 0x86a <vfprintf+0x2ce>
 854:	5e ef       	ldi	r21, 0xFE	; 254
 856:	d5 22       	and	r13, r21
 858:	f9 14       	cp	r15, r9
 85a:	38 f4       	brcc	.+14     	; 0x86a <vfprintf+0x2ce>
 85c:	d4 fe       	sbrs	r13, 4
 85e:	07 c0       	rjmp	.+14     	; 0x86e <vfprintf+0x2d2>
 860:	d2 fc       	sbrc	r13, 2
 862:	05 c0       	rjmp	.+10     	; 0x86e <vfprintf+0x2d2>
 864:	8f ee       	ldi	r24, 0xEF	; 239
 866:	d8 22       	and	r13, r24
 868:	02 c0       	rjmp	.+4      	; 0x86e <vfprintf+0x2d2>
 86a:	1f 2d       	mov	r17, r15
 86c:	01 c0       	rjmp	.+2      	; 0x870 <vfprintf+0x2d4>
 86e:	19 2d       	mov	r17, r9
 870:	d4 fe       	sbrs	r13, 4
 872:	0d c0       	rjmp	.+26     	; 0x88e <vfprintf+0x2f2>
 874:	fe 01       	movw	r30, r28
 876:	ef 0d       	add	r30, r15
 878:	f1 1d       	adc	r31, r1
 87a:	80 81       	ld	r24, Z
 87c:	80 33       	cpi	r24, 0x30	; 48
 87e:	19 f4       	brne	.+6      	; 0x886 <vfprintf+0x2ea>
 880:	99 ee       	ldi	r25, 0xE9	; 233
 882:	d9 22       	and	r13, r25
 884:	08 c0       	rjmp	.+16     	; 0x896 <vfprintf+0x2fa>
 886:	1f 5f       	subi	r17, 0xFF	; 255
 888:	d2 fe       	sbrs	r13, 2
 88a:	05 c0       	rjmp	.+10     	; 0x896 <vfprintf+0x2fa>
 88c:	03 c0       	rjmp	.+6      	; 0x894 <vfprintf+0x2f8>
 88e:	8d 2d       	mov	r24, r13
 890:	86 78       	andi	r24, 0x86	; 134
 892:	09 f0       	breq	.+2      	; 0x896 <vfprintf+0x2fa>
 894:	1f 5f       	subi	r17, 0xFF	; 255
 896:	0d 2d       	mov	r16, r13
 898:	d3 fc       	sbrc	r13, 3
 89a:	14 c0       	rjmp	.+40     	; 0x8c4 <vfprintf+0x328>
 89c:	d0 fe       	sbrs	r13, 0
 89e:	0f c0       	rjmp	.+30     	; 0x8be <vfprintf+0x322>
 8a0:	1c 15       	cp	r17, r12
 8a2:	10 f0       	brcs	.+4      	; 0x8a8 <vfprintf+0x30c>
 8a4:	9f 2c       	mov	r9, r15
 8a6:	0b c0       	rjmp	.+22     	; 0x8be <vfprintf+0x322>
 8a8:	9f 2c       	mov	r9, r15
 8aa:	9c 0c       	add	r9, r12
 8ac:	91 1a       	sub	r9, r17
 8ae:	1c 2d       	mov	r17, r12
 8b0:	06 c0       	rjmp	.+12     	; 0x8be <vfprintf+0x322>
 8b2:	b3 01       	movw	r22, r6
 8b4:	80 e2       	ldi	r24, 0x20	; 32
 8b6:	90 e0       	ldi	r25, 0x00	; 0
 8b8:	0e 94 c3 04 	call	0x986	; 0x986 <fputc>
 8bc:	1f 5f       	subi	r17, 0xFF	; 255
 8be:	1c 15       	cp	r17, r12
 8c0:	c0 f3       	brcs	.-16     	; 0x8b2 <vfprintf+0x316>
 8c2:	04 c0       	rjmp	.+8      	; 0x8cc <vfprintf+0x330>
 8c4:	1c 15       	cp	r17, r12
 8c6:	10 f4       	brcc	.+4      	; 0x8cc <vfprintf+0x330>
 8c8:	c1 1a       	sub	r12, r17
 8ca:	01 c0       	rjmp	.+2      	; 0x8ce <vfprintf+0x332>
 8cc:	cc 24       	eor	r12, r12
 8ce:	04 ff       	sbrs	r16, 4
 8d0:	10 c0       	rjmp	.+32     	; 0x8f2 <vfprintf+0x356>
 8d2:	b3 01       	movw	r22, r6
 8d4:	80 e3       	ldi	r24, 0x30	; 48
 8d6:	90 e0       	ldi	r25, 0x00	; 0
 8d8:	0e 94 c3 04 	call	0x986	; 0x986 <fputc>
 8dc:	02 ff       	sbrs	r16, 2
 8de:	1e c0       	rjmp	.+60     	; 0x91c <vfprintf+0x380>
 8e0:	01 fd       	sbrc	r16, 1
 8e2:	03 c0       	rjmp	.+6      	; 0x8ea <vfprintf+0x34e>
 8e4:	88 e7       	ldi	r24, 0x78	; 120
 8e6:	90 e0       	ldi	r25, 0x00	; 0
 8e8:	02 c0       	rjmp	.+4      	; 0x8ee <vfprintf+0x352>
 8ea:	88 e5       	ldi	r24, 0x58	; 88
 8ec:	90 e0       	ldi	r25, 0x00	; 0
 8ee:	b3 01       	movw	r22, r6
 8f0:	0c c0       	rjmp	.+24     	; 0x90a <vfprintf+0x36e>
 8f2:	80 2f       	mov	r24, r16
 8f4:	86 78       	andi	r24, 0x86	; 134
 8f6:	91 f0       	breq	.+36     	; 0x91c <vfprintf+0x380>
 8f8:	01 ff       	sbrs	r16, 1
 8fa:	02 c0       	rjmp	.+4      	; 0x900 <vfprintf+0x364>
 8fc:	8b e2       	ldi	r24, 0x2B	; 43
 8fe:	01 c0       	rjmp	.+2      	; 0x902 <vfprintf+0x366>
 900:	80 e2       	ldi	r24, 0x20	; 32
 902:	d7 fc       	sbrc	r13, 7
 904:	8d e2       	ldi	r24, 0x2D	; 45
 906:	b3 01       	movw	r22, r6
 908:	90 e0       	ldi	r25, 0x00	; 0
 90a:	0e 94 c3 04 	call	0x986	; 0x986 <fputc>
 90e:	06 c0       	rjmp	.+12     	; 0x91c <vfprintf+0x380>
 910:	b3 01       	movw	r22, r6
 912:	80 e3       	ldi	r24, 0x30	; 48
 914:	90 e0       	ldi	r25, 0x00	; 0
 916:	0e 94 c3 04 	call	0x986	; 0x986 <fputc>
 91a:	9a 94       	dec	r9
 91c:	f9 14       	cp	r15, r9
 91e:	c0 f3       	brcs	.-16     	; 0x910 <vfprintf+0x374>
 920:	fa 94       	dec	r15
 922:	f1 01       	movw	r30, r2
 924:	ef 0d       	add	r30, r15
 926:	f1 1d       	adc	r31, r1
 928:	b3 01       	movw	r22, r6
 92a:	80 81       	ld	r24, Z
 92c:	90 e0       	ldi	r25, 0x00	; 0
 92e:	0e 94 c3 04 	call	0x986	; 0x986 <fputc>
 932:	ff 20       	and	r15, r15
 934:	a9 f7       	brne	.-22     	; 0x920 <vfprintf+0x384>
 936:	06 c0       	rjmp	.+12     	; 0x944 <vfprintf+0x3a8>
 938:	b3 01       	movw	r22, r6
 93a:	80 e2       	ldi	r24, 0x20	; 32
 93c:	90 e0       	ldi	r25, 0x00	; 0
 93e:	0e 94 c3 04 	call	0x986	; 0x986 <fputc>
 942:	ca 94       	dec	r12
 944:	cc 20       	and	r12, r12
 946:	c1 f7       	brne	.-16     	; 0x938 <vfprintf+0x39c>
 948:	41 ce       	rjmp	.-894    	; 0x5cc <vfprintf+0x30>
 94a:	f3 01       	movw	r30, r6
 94c:	66 81       	ldd	r22, Z+6	; 0x06
 94e:	77 81       	ldd	r23, Z+7	; 0x07
 950:	cb 01       	movw	r24, r22
 952:	2b 96       	adiw	r28, 0x0b	; 11
 954:	e2 e1       	ldi	r30, 0x12	; 18
 956:	0c 94 69 05 	jmp	0xad2	; 0xad2 <__epilogue_restores__>

0000095a <strnlen_P>:
 95a:	fc 01       	movw	r30, r24
 95c:	05 90       	lpm	r0, Z+
 95e:	61 50       	subi	r22, 0x01	; 1
 960:	70 40       	sbci	r23, 0x00	; 0
 962:	01 10       	cpse	r0, r1
 964:	d8 f7       	brcc	.-10     	; 0x95c <strnlen_P+0x2>
 966:	80 95       	com	r24
 968:	90 95       	com	r25
 96a:	8e 0f       	add	r24, r30
 96c:	9f 1f       	adc	r25, r31
 96e:	08 95       	ret

00000970 <strnlen>:
 970:	fc 01       	movw	r30, r24
 972:	61 50       	subi	r22, 0x01	; 1
 974:	70 40       	sbci	r23, 0x00	; 0
 976:	01 90       	ld	r0, Z+
 978:	01 10       	cpse	r0, r1
 97a:	d8 f7       	brcc	.-10     	; 0x972 <strnlen+0x2>
 97c:	80 95       	com	r24
 97e:	90 95       	com	r25
 980:	8e 0f       	add	r24, r30
 982:	9f 1f       	adc	r25, r31
 984:	08 95       	ret

00000986 <fputc>:
 986:	0f 93       	push	r16
 988:	1f 93       	push	r17
 98a:	cf 93       	push	r28
 98c:	df 93       	push	r29
 98e:	8c 01       	movw	r16, r24
 990:	eb 01       	movw	r28, r22
 992:	8b 81       	ldd	r24, Y+3	; 0x03
 994:	81 ff       	sbrs	r24, 1
 996:	1b c0       	rjmp	.+54     	; 0x9ce <fputc+0x48>
 998:	82 ff       	sbrs	r24, 2
 99a:	0d c0       	rjmp	.+26     	; 0x9b6 <fputc+0x30>
 99c:	2e 81       	ldd	r18, Y+6	; 0x06
 99e:	3f 81       	ldd	r19, Y+7	; 0x07
 9a0:	8c 81       	ldd	r24, Y+4	; 0x04
 9a2:	9d 81       	ldd	r25, Y+5	; 0x05
 9a4:	28 17       	cp	r18, r24
 9a6:	39 07       	cpc	r19, r25
 9a8:	64 f4       	brge	.+24     	; 0x9c2 <fputc+0x3c>
 9aa:	e8 81       	ld	r30, Y
 9ac:	f9 81       	ldd	r31, Y+1	; 0x01
 9ae:	01 93       	st	Z+, r16
 9b0:	f9 83       	std	Y+1, r31	; 0x01
 9b2:	e8 83       	st	Y, r30
 9b4:	06 c0       	rjmp	.+12     	; 0x9c2 <fputc+0x3c>
 9b6:	e8 85       	ldd	r30, Y+8	; 0x08
 9b8:	f9 85       	ldd	r31, Y+9	; 0x09
 9ba:	80 2f       	mov	r24, r16
 9bc:	09 95       	icall
 9be:	89 2b       	or	r24, r25
 9c0:	31 f4       	brne	.+12     	; 0x9ce <fputc+0x48>
 9c2:	8e 81       	ldd	r24, Y+6	; 0x06
 9c4:	9f 81       	ldd	r25, Y+7	; 0x07
 9c6:	01 96       	adiw	r24, 0x01	; 1
 9c8:	9f 83       	std	Y+7, r25	; 0x07
 9ca:	8e 83       	std	Y+6, r24	; 0x06
 9cc:	02 c0       	rjmp	.+4      	; 0x9d2 <fputc+0x4c>
 9ce:	0f ef       	ldi	r16, 0xFF	; 255
 9d0:	1f ef       	ldi	r17, 0xFF	; 255
 9d2:	c8 01       	movw	r24, r16
 9d4:	df 91       	pop	r29
 9d6:	cf 91       	pop	r28
 9d8:	1f 91       	pop	r17
 9da:	0f 91       	pop	r16
 9dc:	08 95       	ret

000009de <__ultoa_invert>:
 9de:	fa 01       	movw	r30, r20
 9e0:	aa 27       	eor	r26, r26
 9e2:	28 30       	cpi	r18, 0x08	; 8
 9e4:	51 f1       	breq	.+84     	; 0xa3a <__ultoa_invert+0x5c>
 9e6:	20 31       	cpi	r18, 0x10	; 16
 9e8:	81 f1       	breq	.+96     	; 0xa4a <__ultoa_invert+0x6c>
 9ea:	e8 94       	clt
 9ec:	6f 93       	push	r22
 9ee:	6e 7f       	andi	r22, 0xFE	; 254
 9f0:	6e 5f       	subi	r22, 0xFE	; 254
 9f2:	7f 4f       	sbci	r23, 0xFF	; 255
 9f4:	8f 4f       	sbci	r24, 0xFF	; 255
 9f6:	9f 4f       	sbci	r25, 0xFF	; 255
 9f8:	af 4f       	sbci	r26, 0xFF	; 255
 9fa:	b1 e0       	ldi	r27, 0x01	; 1
 9fc:	3e d0       	rcall	.+124    	; 0xa7a <__ultoa_invert+0x9c>
 9fe:	b4 e0       	ldi	r27, 0x04	; 4
 a00:	3c d0       	rcall	.+120    	; 0xa7a <__ultoa_invert+0x9c>
 a02:	67 0f       	add	r22, r23
 a04:	78 1f       	adc	r23, r24
 a06:	89 1f       	adc	r24, r25
 a08:	9a 1f       	adc	r25, r26
 a0a:	a1 1d       	adc	r26, r1
 a0c:	68 0f       	add	r22, r24
 a0e:	79 1f       	adc	r23, r25
 a10:	8a 1f       	adc	r24, r26
 a12:	91 1d       	adc	r25, r1
 a14:	a1 1d       	adc	r26, r1
 a16:	6a 0f       	add	r22, r26
 a18:	71 1d       	adc	r23, r1
 a1a:	81 1d       	adc	r24, r1
 a1c:	91 1d       	adc	r25, r1
 a1e:	a1 1d       	adc	r26, r1
 a20:	20 d0       	rcall	.+64     	; 0xa62 <__ultoa_invert+0x84>
 a22:	09 f4       	brne	.+2      	; 0xa26 <__ultoa_invert+0x48>
 a24:	68 94       	set
 a26:	3f 91       	pop	r19
 a28:	2a e0       	ldi	r18, 0x0A	; 10
 a2a:	26 9f       	mul	r18, r22
 a2c:	11 24       	eor	r1, r1
 a2e:	30 19       	sub	r19, r0
 a30:	30 5d       	subi	r19, 0xD0	; 208
 a32:	31 93       	st	Z+, r19
 a34:	de f6       	brtc	.-74     	; 0x9ec <__ultoa_invert+0xe>
 a36:	cf 01       	movw	r24, r30
 a38:	08 95       	ret
 a3a:	46 2f       	mov	r20, r22
 a3c:	47 70       	andi	r20, 0x07	; 7
 a3e:	40 5d       	subi	r20, 0xD0	; 208
 a40:	41 93       	st	Z+, r20
 a42:	b3 e0       	ldi	r27, 0x03	; 3
 a44:	0f d0       	rcall	.+30     	; 0xa64 <__ultoa_invert+0x86>
 a46:	c9 f7       	brne	.-14     	; 0xa3a <__ultoa_invert+0x5c>
 a48:	f6 cf       	rjmp	.-20     	; 0xa36 <__ultoa_invert+0x58>
 a4a:	46 2f       	mov	r20, r22
 a4c:	4f 70       	andi	r20, 0x0F	; 15
 a4e:	40 5d       	subi	r20, 0xD0	; 208
 a50:	4a 33       	cpi	r20, 0x3A	; 58
 a52:	18 f0       	brcs	.+6      	; 0xa5a <__ultoa_invert+0x7c>
 a54:	49 5d       	subi	r20, 0xD9	; 217
 a56:	31 fd       	sbrc	r19, 1
 a58:	40 52       	subi	r20, 0x20	; 32
 a5a:	41 93       	st	Z+, r20
 a5c:	02 d0       	rcall	.+4      	; 0xa62 <__ultoa_invert+0x84>
 a5e:	a9 f7       	brne	.-22     	; 0xa4a <__ultoa_invert+0x6c>
 a60:	ea cf       	rjmp	.-44     	; 0xa36 <__ultoa_invert+0x58>
 a62:	b4 e0       	ldi	r27, 0x04	; 4
 a64:	a6 95       	lsr	r26
 a66:	97 95       	ror	r25
 a68:	87 95       	ror	r24
 a6a:	77 95       	ror	r23
 a6c:	67 95       	ror	r22
 a6e:	ba 95       	dec	r27
 a70:	c9 f7       	brne	.-14     	; 0xa64 <__ultoa_invert+0x86>
 a72:	00 97       	sbiw	r24, 0x00	; 0
 a74:	61 05       	cpc	r22, r1
 a76:	71 05       	cpc	r23, r1
 a78:	08 95       	ret
 a7a:	9b 01       	movw	r18, r22
 a7c:	ac 01       	movw	r20, r24
 a7e:	0a 2e       	mov	r0, r26
 a80:	06 94       	lsr	r0
 a82:	57 95       	ror	r21
 a84:	47 95       	ror	r20
 a86:	37 95       	ror	r19
 a88:	27 95       	ror	r18
 a8a:	ba 95       	dec	r27
 a8c:	c9 f7       	brne	.-14     	; 0xa80 <__ultoa_invert+0xa2>
 a8e:	62 0f       	add	r22, r18
 a90:	73 1f       	adc	r23, r19
 a92:	84 1f       	adc	r24, r20
 a94:	95 1f       	adc	r25, r21
 a96:	a0 1d       	adc	r26, r0
 a98:	08 95       	ret

00000a9a <__prologue_saves__>:
 a9a:	2f 92       	push	r2
 a9c:	3f 92       	push	r3
 a9e:	4f 92       	push	r4
 aa0:	5f 92       	push	r5
 aa2:	6f 92       	push	r6
 aa4:	7f 92       	push	r7
 aa6:	8f 92       	push	r8
 aa8:	9f 92       	push	r9
 aaa:	af 92       	push	r10
 aac:	bf 92       	push	r11
 aae:	cf 92       	push	r12
 ab0:	df 92       	push	r13
 ab2:	ef 92       	push	r14
 ab4:	ff 92       	push	r15
 ab6:	0f 93       	push	r16
 ab8:	1f 93       	push	r17
 aba:	cf 93       	push	r28
 abc:	df 93       	push	r29
 abe:	cd b7       	in	r28, 0x3d	; 61
 ac0:	de b7       	in	r29, 0x3e	; 62
 ac2:	ca 1b       	sub	r28, r26
 ac4:	db 0b       	sbc	r29, r27
 ac6:	0f b6       	in	r0, 0x3f	; 63
 ac8:	f8 94       	cli
 aca:	de bf       	out	0x3e, r29	; 62
 acc:	0f be       	out	0x3f, r0	; 63
 ace:	cd bf       	out	0x3d, r28	; 61
 ad0:	09 94       	ijmp

00000ad2 <__epilogue_restores__>:
 ad2:	2a 88       	ldd	r2, Y+18	; 0x12
 ad4:	39 88       	ldd	r3, Y+17	; 0x11
 ad6:	48 88       	ldd	r4, Y+16	; 0x10
 ad8:	5f 84       	ldd	r5, Y+15	; 0x0f
 ada:	6e 84       	ldd	r6, Y+14	; 0x0e
 adc:	7d 84       	ldd	r7, Y+13	; 0x0d
 ade:	8c 84       	ldd	r8, Y+12	; 0x0c
 ae0:	9b 84       	ldd	r9, Y+11	; 0x0b
 ae2:	aa 84       	ldd	r10, Y+10	; 0x0a
 ae4:	b9 84       	ldd	r11, Y+9	; 0x09
 ae6:	c8 84       	ldd	r12, Y+8	; 0x08
 ae8:	df 80       	ldd	r13, Y+7	; 0x07
 aea:	ee 80       	ldd	r14, Y+6	; 0x06
 aec:	fd 80       	ldd	r15, Y+5	; 0x05
 aee:	0c 81       	ldd	r16, Y+4	; 0x04
 af0:	1b 81       	ldd	r17, Y+3	; 0x03
 af2:	aa 81       	ldd	r26, Y+2	; 0x02
 af4:	b9 81       	ldd	r27, Y+1	; 0x01
 af6:	ce 0f       	add	r28, r30
 af8:	d1 1d       	adc	r29, r1
 afa:	0f b6       	in	r0, 0x3f	; 63
 afc:	f8 94       	cli
 afe:	de bf       	out	0x3e, r29	; 62
 b00:	0f be       	out	0x3f, r0	; 63
 b02:	cd bf       	out	0x3d, r28	; 61
 b04:	ed 01       	movw	r28, r26
 b06:	08 95       	ret

00000b08 <_exit>:
 b08:	f8 94       	cli

00000b0a <__stop_program>:
 b0a:	ff cf       	rjmp	.-2      	; 0xb0a <__stop_program>
