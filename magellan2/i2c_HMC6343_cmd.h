// HMC6343 Data

#define HMC6343_ADDR		0x32<<1 //(?)
#define HMC6343_ADDR_W		0x32 //(?)
#define HMC6343_ADDR_R		0x33 //(?)

#define HMC6343_T_START		500 // ms, time from reset to run.
#define HMC6343_T_SLEEP		20 // ms, time from sleep to standby

// Head, Pitch, Roll: 10ths of deg, Pitch and roll: (+-900), Head: (0 to 3600) 
#define HMC6343_POST_ACCEL	0x40 //6: Ax[2],Ay[2],Az[2]
#define HMC6343_POST_MAG		0x45 //6: Mx...
#define HMC6343_POST_HEAD	0x50 //6: Head[2], Pitch[2], Roll[2]
#define HMC6343_POST_TILT	0x55 //6: Pitch[2], Roll[2], Temp[2]
#define HMC6343_POST_OP_MODE	0x65

#define HMC6343_ENTER_USER_CALIB	0x71
#define HMC6343_LEVEL_XpZ	0x72
#define HMC6343_LEVEL_YpZ	0x73
#define HMC6343_LEVEL_ZnX	0x74
#define HMC6343_EXIT_USER_CALIB	0x7E

#define HMC6343_RESET		0x82

#define HMC6343_RUN_MODE		0x75 // from Standby
#define HMC6343_STANDBY_MODE	0x76 // from Run
#define HMC6343_ENTER_SLEEP_MODE	0x83 // from Run
#define HMC6343_EXIT_SLEEP_MODE	0x84 // to Standby

#define HMC6343_EEPROM_READ	0xE1
#define HMC6343_EEPROM_WRITE	0xF1

// EEPROM locations
#define HMC6343_E_ADDR		0x00
#define HMC6343_E_SW_VER		0x02

#define HMC6343_E_OMR_1		0x04 //
#define HMC6343_E_OMR_2		0x05 // 1,0 set measure rate: 0,0=1Hz; 0,1=5Hz; 1,0=10Hz

#define HMC6343_E_SN_LSB		0x06
#define HMC6343_E_SN_MSB		0x07

#define HMC6343_E_DATE_Y		0x08
#define HMC6343_E_DATE_W		0x09

// Data Structures
struct heading_data_t {
	union {
		uint16_t head;
		struct {
		uint8_t head_lsb;
		uint8_t head_msb;
		};
	};
	union {
		int16_t pitch;
		struct {
		uint8_t pitch_lsb;
		uint8_t pitch_msb;
		};
	};
	union {
		int16_t roll;
		struct {
		uint8_t roll_lsb;
		uint8_t roll_msb;
		};
	};
};
