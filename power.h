#ifndef MUC_POWER_H_
#define MUC_POWER_H_

#include <avr/power.h>

#ifndef power_all_enable
#define power_all_enable() do {} while(0)
#endif

#ifndef power_all_disable
#define power_all_disable() do {} while(0)
#endif

#endif
