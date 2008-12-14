#ifndef _DEFINES_H_
#define _DEFINES_H_ 

#define F_CPU 8000000

#define DEBUG 1


/* Debuging */

#if (DEBUG==1)
	#define dpf(__A) printf(__A)
	#define dpf_P(__A) printf_P(__A)
#else
	#define dpf(__A)
	#define dpf_P(__A)
#endif
#if (DEBUG==2)
	#define dpfv(__A) printf(__A)
	#define dpfv_P(__A) printf_P(__A)
#else
	#define dpfv(__A)
	#define dpfv_P(__A)
#endif
#if (DEBUG==3)
	#define dpfV(__A) printf(__A)
	#define dpfV_P(__A) printf_P(__A)
#else
	#define dpfV(__A)
	#define dpfV_P(__A)
#endif

/* LED Ctrl */


#define DWELL_TOP	30
#define DWELL_BTM	5

#define DWELL_TOP_A	DWELL_TOP
#define DWELL_TOP_B	DWELL_TOP

#define DWELL_BTM_A	DWELL_BTM
#define DWELL_BTM_B	DWELL_BTM

#endif
