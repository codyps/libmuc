#include <muc/usart_gen.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	if (argc != 3) {
		fprintf(stderr, "usage: %s buad f_cpu\n",
				argc?argv[0]:"t-ubrr");
		return 1;
	}

	unsigned long baud = strtol(argv[1], NULL, 10);
	unsigned long f_cpu = strtol(argv[2], NULL, 10);

	printf("BAUD = %lu ; F_CPU = %lu ; UBRR = %lu\n",
			baud, f_cpu, BAUD_TO_UBRR(f_cpu, baud));
	return 0;
}

