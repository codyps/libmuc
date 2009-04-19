
//FIXME: Currently the following line does nothing, had to place the '5' directly in the S_# macros
#define S_TIMER	5

#define S_A(_A)	COMB2(_A, 5 )
#define S_I(_A,_B) COMB3(_A, 5 ,_B)

#define COMB3(_A,_num,_C) _A ## _num ## _C
#define COMB2(_A,_num) _A ## _num



// Registers
//  Bits
#define SERVO_TCCRB	S_I(TCCR,B)
 #define WGM3		S_I(WGM,3)
 #define WGM2		S_I(WGM,2)

#define SERVO_TCCRA	S_I(TCCR,A)
 #define WGM1		S_I(WGM,1)
 #define WGM0		S_I(WGM,0)
 #define CS2		S_I(CS,2)
 #define CS1		S_I(CS,1)
 #define CS0		S_I(CS,0)

#define SERVO_TIMSK	S_A(TIMSK)
 #define OCIEA		S_I(OCIE,A)
 #define OCIEB		S_I(OCIE,B)
 #define OCIEC		S_I(OCIE,C)
 #define TOIE		S_A(TOIE)

#define SERVO_ICR	S_A(ICR)
#define SERVO_TCNT	S_A(TCNT)

#define SERVO_OCRA	S_I(OCR,A)
#define SERVO_OCRB	S_I(OCR,B)
#define SERVO_OCRC	S_I(OCR,C)

// Interrupts
#define TIMER_S_OVF_vect S_I(TIMER,_OVF_vect)

#define TIMER_S_COMPA_vect S_I(TIMER,_COMPA_vect)
#define TIMER_S_COMPB_vect S_I(TIMER,_COMPB_vect)
#define TIMER_S_COMPC_vect S_I(TIMER,_COMPC_vect)

// Power macro
#define power_timer_S_enable S_I(power_timer,_enable)