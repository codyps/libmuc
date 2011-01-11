
#define _DEF_STRUCT(name, itype, dtype, qual, size) \
	struct name##_s {                           \
		itype head;                         \
		itype tail;                         \
		dtype data[size];                   \
	} qual name

#define _DEF_STORAGE(name, itype, dtype, qual, tx_size, rx_size) \
	_DEF_STRUCT(name##_tx, itype, dtype, qual, tx_size);     \
	_DEF_STRUCT(name##_rx, itype, dtype, qual, rx_size);

#define _DEF_PUTC_WAIT(name, itype)                                 \
	static int name##_putc(char c, FILE *stream)                \
	{                                                           \
		itype nhead = CIRC_NEXT(name##_tx.head,             \
				        sizeof(name##_tx.data));    \
		volatile itype *vtail = &name##_tx.tail;            \
		while(nhead == *vtail)                              \
			;                                           \
		name##_tx.data[name##_tx.head] = c;                 \
		name##_tx.head = nhead;                             \
		return 0;                                           \
	}

#define _DEF_PUTC_EOF(name, itype)                                  \
	static int name##_putc(char c, FILE *stream)                \
	{                                                           \
		itype nhead = CIRC_NEXT(name##_tx.head,             \
				        sizeof(name##_tx.data));    \
		if (nhead == name##_tx.tail)                        \
			return EOF;                                 \
		name##_tx.data[name##_tx.head] = c;                 \
		name##_tx.head = nhead;                             \
		return 0;                                           \
	}

#define _DEF_GETC_WAIT(name, itype, dtype)                    \
	static int name##_getc(FILE *stream)                  \
	{                                                     \
		volatile itype *vhead = &rx.head;             \
		while (*vhead == rx.tail)                     \
			;                                     \
		dtype tmp = rx.data[rx.tail];                 \
		rx.tail = CIRC_NEXT(rx.tail,sizeof(rx.data)); \
		return tmp;                                   \
	}

#define _DEF_GET_EOF(name, itype, dtype)                      \
	static int name##_getc(FILE *stream)                  \
	{                                                     \
		if (rx.head == rx.tail)                       \
			return EOF;                           \
		dtype tmp = rx.data[rx.tail];                 \
		rx.tail = CIRC_NEXT(rx.tail,sizeof(rx.data)); \
		return tmp;                                   \
	}

#define _DEF_ISR_GIVE(name, dtype)                                  \
	static inline void name##_isr_give(dtype z)                 \
	{                                                           \
		name##_rx.data[name##_rx.head] = z;                 \
		name##_rx.head = CIRC_NEXT(name##_rx.head,          \
					   sizeof(name##_rx.data)); \
	}

#define _DEF_ISR_TAKE(name, dtype)                                  \
	static inline dtype name##_isr_take(void)                   \
	{                                                           \
		dtype tmp = tx.data[tx.tail];                       \
		name##_tx.tail = CIRC_NEXT(name##_tx.tail,          \
				           sizeof(name##_tx.data)); \
		return tmp;                                         \
	}

#define _DEF_ISR_TAKE_OK(name)                           \
	static inline bool name##_isr_take_ok(void)      \
	{                                                \
		return name##_tx.head != name##_tx.tail; \
	}

#define _DEF_ISR_GIVE_OK(name)                                          \
	static inline bool name##_isr_give_ok(void)                     \
	{                                                               \
		return CIRC_NEXT(name##_rx.head,sizeof(name##_rx.data)) \
			!= name##_rx.tail;                              \
	}

#define _DEF_ISR_INTERFACE(name, dtype) \
	_DEF_ISR_GIVE(name, dtype)      \
	_DEF_ISR_TAKE(name, dtype)      \
	_DEF_ISR_TAKE_OK(name)          \
	_DEF_ISR_GIVE_OK(name)

#define _DEF_FP_WAIT_INTERFACE(name, itype, dtype) \
	_DEF_GETC_WAIT(name, itype, dtype)         \
	_DEF_PUTC_WAIT(name, itype)

#define _DEF_FP_EOF_INTERFACE(name, itype, dtype)  \
	_DEF_GETC_WAIT(name, itype, dtype)         \
	_DEF_PUTC_WAIT(name, itype)

#define _DEF_FP_GEN(name, putc, getc, flag) \
	static FILE name##_io_ = FDEV_SETUP_STREAM(putc, getc, flag); \
	FILE *name##_io = &name##_io;

#define _DEF_FP_RW(name) \
	_DEF_FP_GEN(name, name##_putc, name##_getc, _FDEV_SETUP_RW)

#define _DEF_FP_W(name) \
	_DEF_FP_GEN(name, name##_putc, 0, _FDEV_SETUP_WRITE)

#define _DEF_FP_R(name) \
	_DEF_FP_GEN(name, 0, name##_getc, _FDEV_SETUP_READ)

#define DEFINE_ISR_IO(name, itype, dtype, tx_size, rx_size) \
	_DEF_STORAGE(name, itype, dtype, static, tx_size, rx_size) \
	_DEF_FP_WAIT_INTERFACE(name, itype, dtype) \
	_DEF_FP_RW(name) \
	_DEF_ISR_INTERFACE(name, dtype)
