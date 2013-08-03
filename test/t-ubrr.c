#include <muc/usart_gen.h>
#include <stdio.h>
#include <stdlib.h>

unsigned long bauds [] {
	300,
	600,
	1200,
	2400,
	4800,
	9600,
	19200,
	38400,
	57600,
	115200
};

int main(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "usage: %s buad f_cpu\n",
				argc?argv[0]:"t-ubrr");
		return 1;
	}

	unsigned long baud = strtol(argv[1], NULL, 10);
	unsigned long f_cpu = strtol(argv[2], NULL, 10);
	bool use_2x = USART_USE_2X(f_cpu, baud);
	unsigned long ubrr_1 = BAUD_TO_UBRR_1X(f_cpu, baud);
	unsigned long real_baud_1 = UBRR_TO_BAUD_1X(f_cpu, ubrr_1);
	unsigned long ubrr_2 = BAUD_TO_UBRR_2X(f_cpu, baud);
	unsigned long real_baud_2 = UBRR_TO_BAUD_2X(f_cpu, ubrr_2);


	printf("BAUD = %lu ; F_CPU = %lu\n"
	       "1X[%c]: UBRR = %lu ; Actual Baud = %lu\n"
	       "2X[%c]: UBRR = %lu ; Actual Baud = %lu\n",
	       baud, f_cpu,
	       use_2x ? 'n' : 'Y', ubrr_1, real_baud_1,
	       use_2x ? 'Y' : 'n', ubrr_2, real_baud_2);
	return 0;
}

