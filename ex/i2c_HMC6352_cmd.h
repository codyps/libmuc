// HMC6352 Data

#define HMC6352_ADDR   0x42 // 8bits, lsb = 0
#define HMC6352_ADDR_W 0x42
#define HMC6352_ADDR_R 0x43

// Commands.
#define HMC6352_C_EWRITE 0x77 //'w'
#define HMC6352_C_EREAD  0x72 //'r'
#define HMC6352_C_MWRITE 'G'  //'G'
#define HMC6352_C_MREAD  'g'  //'g'
#define HMC6352_C_SLEEP  'S'  //'S'
#define HMC6352_C_WAKE   'W'  //'W'
#define HMC6352_C_UPDATE_BRIDGE_OFFSETS  'O'  //'O', XXX:?
#define HMC6352_C_CALIB_ENTER  'C'  //'C'
#define HMC6352_C_CALIB_EXIT   'E'  //'E'
#define HMC6352_C_OP_MODE_SAVE 'L'  //'L'
#define HMC6352_C_GET_DATA  'A'  //'A'

// RAM (memory) layout. 
// These do not appear to be clearly listed in the datasheet.
#define HMC6352_M_OP_MODE 0x72  // = 50
#define HMC6352_M_OUT_MODE 0x4E  // = ?

// EEPROM layout.
#define HMC6352_E_I2C_ADDR 0x00  // = 42
#define HMC6352_E_MAG_X_OFF_MSB 0x01 // = factory calib.
#define HMC6352_E_MAG_X_OFF_LSB 0x02
#define HMC6352_E_MAG_Y_OFF_MSB 0x03 
#define HMC6352_E_MAG_Y_OFF_LSB 0x04
#define HMC6352_E_TIME_D 0x05 // = 1, time delay.
#define HMC6352_E_SUM_CT 0x06 // = 4, summed measurements.
#define HMC6352_E_VER 0x07 // > 1, version number.
#define HMC6352_E_OP_MODE 0x08 // = 50, operational mode.

struct hmc6352_op_mode {
	uint8_t sample_mode:2; // 0 = standby, 1 = query, 2 = continuous.
	uint8_t unused1:2; // = 0.
	uint8_t periodic_reset:1; // 0 = off, 1 = on.
	uint8_t mearsurement_rate:2; // 0 = 1Hz, 1 = 5Hz, 2 = 10Hz, 3 = 20Hz.
	uint8_t unused2:1; // = 0.
};

enum hmc6352_out_mode {
	HMC6352_OM_heading,
	HMC6352_OM_raw_mag_x,
	HMC6352_OM_raw_mag_y,
	HMC6352_OM_mag_x,
	HMC6352_OM_mag_y
};
