#ifndef MUC_TMR_CLK_H_
#define MUC_TMR_CLK_H_ 1

#define US_PER_OVF(ticks_per_ovf) ((ticks_per_ovf) / (F_CPU / 1000 / 1000))
#define MS_PER_OVF(ticks_per_ovf) (US_PER_OVF(ticks_per_ovf) / 1000)
#define FRACT_INC(ticks_per_ovf) ((US_PER_OVF(ticks_pef_ovf) % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

#define TMR_CLK_UPDATE(ms, fract, ticks_per_ovf) do {	\
	uint32_t m = (ms);				\
	uint32_t f = (fract);				\
	m += MS_PER_OVF(ticks_per_ovf);			\
	f += FRACT_INC(ticks_pef_ovf);			\
        if (f >= FRACT_MAX) {				\
		f -= FRACT_MAX;				\
		m += 1;					\
	}						\
	(ms) = m;					\
	(fract) = f;					\
} while(0)

#endif
