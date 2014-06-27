#ifndef TWI2_CALC_H_
#define TWI2_CALC_H_

#include <penny/math.h> /* pow4 */
#include <penny/penny.h> /* compiletime warnings. */

#define TWI_SCL_FREQ(fcpu, twbr, twps) ((fcpu) / (16 + 2ULL * (twbr) * pow4(twps)))

/* Alternate forms of the same equation.
 *
 * F_CPU - 16 * F_SCL
 * ----------------------   = TWBR
 * 2 * F_SCL * 4 ^ {TWPS}
 *
 * (F_CPU / 2 ) - 8 * F_SCL
 * ------------------------ = TWBR
 * F_SCL * 4 ^ {TWPS}
 */

#define TWI_TWBR TWI_TWBR_c
#define TWI_TWBR_a(fcpu, fscl, twps) (((fcpu) - 16*(fscl)) / (2 * (fscl) * pow4(twps)))
#define TWI_TWBR_b(fcpu, fscl, twps) (((fcpu)/2 - 8*(fscl)) / ((fscl) * pow4(twps)))

/* FIXME: suffers from an off-by-one:
 * TWI_TWBR_c(8M, 100000, 1) -> 9 (should be 8)
 * TWI_TWBR_c(8M, 100001, 1) -> 8
 * TWI_TWBR_c(8M, 111111, 1) -> 8 (should be 7)
 * TWI_TWBR_c(8M, 111112, 1) -> 7
 *
 * This is fixed up by using (fscl + 1) in place of fscl, but the effects have
 * not been studied closely.
 *
 * Idealy, we have 2 possible desired behaviors:
 * 1. choose the TWBR that gets the closest F_SCL
 * 2. choose the TWBR that gets the closest F_SCL that is lower than the
 *    specified one (some devices have maximum SCL frequencies).
 *
 * This function currently _appears_ to provide #2.
 *
 */
typedef long long ll;
#define TWI_TWBR_1(fcpu, fscl, twps) SUB_SAT((ll)(fcpu),16LL * (ll)(fscl + 1))
#define TWI_TWBR_2(fcpu, fscl, twps) (2 * (fscl + 1) * pow4(twps) - 1)
#define TWI_TWBR_c(fcpu, fscl, twps) ({						\
		long long __x__ = TWI_TWBR_1(fcpu, fscl, twps);			\
		long long __y__ = TWI_TWBR_2(fcpu, fscl, twps);			\
		DIV_ROUND_UP(__x__,__y__);					\
	})

#ifdef TWI_CALC_WARN
DEFINE_COMPILETIME_WARNING(twi_fscl_wrong, "generated F_SCL does not match desired F_SCL")
DEFINE_COMPILETIME_WARNING(twi_twbr_not_a_constant, "F_SCL calculation is not a compile time constant");
#else
static inline void twi_fscl_wrong(void) {}
static inline void twi_twbr_not_a_constant(void) {}
#endif

#define TWI_FIXED_TWPS 1
#define TWI_TWPS_FROM_FREQ(fcpu, fscl) TWI_FIXED_TWPS
#define TWI_TWBR_FROM_FREQ(fcpu, fscl) ({				\
		uint8_t __res__ = TWI_TWBR(fcpu, fscl, TWI_FIXED_TWPS); \
		if (!is_constant(__res__))				\
			twi_twbr_not_a_constant();			\
		else if (fscl != TWI_SCL_FREQ(fcpu, __res__, TWI_FIXED_TWPS))\
			twi_fscl_wrong();				\
		__res__;						\
		})

#endif
