#ifndef _UTIL_H_
#define _UTIL_H_

int cmp_16(const void * a, const void * b);
void memset_16(uint16_t * dest, uint16_t val, uint8_t len);

#define CLICKS_MS(_ms_) ( (uint16_t) ( F_CPU * (_ms_) / 1000 ) )
#define CLICKS_US(_us_) ( (uint16_t) ( F_CPU / 1000000 * (_us_) ) ) // 1000000 = 1e6

#endif
