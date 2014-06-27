#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define TWI_CALC_NO_WARN 1
#include "../twi_calc.h"

#include <errno.h>
#include <string.h>

typedef unsigned long long ull;
typedef unsigned long ul;

static unsigned long long unit_to_mult(char c)
{
	switch(c) {
	case 'M': return 1000000;
	case 'K': return 1000;
	default: errno = EINVAL; /* fall through */
	case '\0': return 1;
	}
}

#define HUMAN_USE_SSCANF
static unsigned long long human_to_ull(const char *str)
{
#ifdef HUMAN_USE_SSCANF
	char unit = '\0';
	unsigned long long res;
	const char *fmt;
	if (*str == '0') {
		if (str[1] == 'x') {
			fmt = "%llx%c";
			str = str + 2;
		} else {
			/* octal?? */
			fmt = "%llu%c";
		}
	} else {
		fmt = "%llu%c";
	}

	int ret = sscanf(str, fmt, &res, &unit);
	if (ret == 1)
		return res;
	else if (ret == 2)
		return res * unit_to_mult(unit);
	else {
		fprintf(stderr, "error.\n");
		return 0;
	}
#else
	char *end;
	errno = 0;
	unsigned long long res = strtoull(str, &end, 0);
	if (errno)
		fprintf(stderr, "error? %d: %s\n", errno, strerror(errno));
	if (*end != '\0')
		return res * unit_to_mult(*end);
	else
		return res;
#endif
}

static void usage(int argc, char **argv)
{
	const char *p = argc?argv[0]:"twi_baud";
	printf("usage: %s a <fcpu> <scl freq>\n"
	       "       %s b <fcpu> <twbr> <twps>\n",
			p, p);
	exit(1);
}
int main(int argc, char **argv)
{
	if (argc < 4) {
		usage(argc, argv);
	}

	ul f_cpu = human_to_ull(argv[2]);
	if (*argv[1] == 'a') {
		ul f_scl = human_to_ull(argv[3]);
		uint8_t twbr = TWI_TWBR_FROM_FREQ(f_cpu, f_scl);
		uint8_t twps = TWI_TWPS_FROM_FREQ(f_cpu, f_scl);

		printf("F_SCL = %lu ; F_CPU = %lu\n"
		       "TWBR = %lu ;  TWPS = %lu\n"
		       "Calculated F_SCL = %Lu\n",
		       f_scl, f_cpu,
		       (ul)twbr, (ul)twps,
		       (ull)TWI_SCL_FREQ(f_cpu, twbr, twps));
	} else if (*argv[1] == 'b') {
		if (argc != 5)
			usage(argc, argv);
		uint8_t twbr = human_to_ull(argv[3]) & 0xFF;
		uint8_t twps =  human_to_ull(argv[4]) & 0xFF;
		ul f_scl = TWI_SCL_FREQ(f_cpu, twbr, twps);

		printf("F_SCL = %lu ; F_CPU = %lu\n"
		       "TWBR = %lu ;  TWPS = %lu\n",
		       f_scl, f_cpu,
		       (ul)twbr, (ul)twps);
	}
	return 0;
}
