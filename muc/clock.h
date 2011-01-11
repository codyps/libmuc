#ifndef _CLOCK_H_
#define _CLOCK_H_

#define MHz(x) ( x * 1000000 )
#define KHz(x) ( x *    1000 )

#define CLICKS_MS TICKS_MS
#define CLICKS_US TICKS_US

#define TICKS_MS(_ms_) ( (uint16_t) ( F_CPU * (_ms_) / 1000 ) )
#define TICKS_US(_us_) ( (uint16_t) ( F_CPU / 1000000 * (_us_) ) ) // 1000000 = 1e6

#define US_TICKS(_ticks_) ( (uint16_t) ( _ticks_ / (F_CPU / 1000 / 1000)  ) )

#endif
