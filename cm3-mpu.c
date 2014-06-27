/*
 * based on http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0179b/CHDFDFIG.html
 */

#include <stdint.h>

struct mpu_type {
	uint32_t res_1:8,	/* [31:24] */
		 iregion:8,	/* [23:16] */
		 dregion:8,	/* [15:8] */
		 res_2:7,	/* [7:1] */
		 seperate:1;	/* [0] */
};
#define MPU_TYPE		(*(struct mpu_type *)0xE000ED90)

struct mpu_ctrl {
	uint32_t res_1:29,	/* [31:3] */
		 privdefna:1,	/* [2] */
		 hfnmiena:1,	/* [1] */
		 enable:1;	/* [0] */
};
#define MPU_CTRL		(*(struct mpu_ctrl *)0xE000ED94)

struct mpu_region_num {
	uint32_t res_1:24,	/* 31:8 */
		 region:8;	/* 7:0 */
};
#define MPU_REGION_NUM		(*(struct mpu_region_num *)0xE000ED98)

struct mpu_region_base_addr {
	uint32_t addr:27,	/* [31:N(5)] */
		 valid:1,	/* [4] */
		 region:4;	/* [3:0] */
};
#define MPU_REGION_BASE_ADDR	(*(struct mpu_region_base_addr *)0xE000ED9C)

struct mpu_region_attr_and_size {
	uint32_t res_1:3,	/* [31:29] */
		 xn:1,		/* [28] */
		 res_2:1,	/* [27] */
		 ap:3,		/* [26:24] */
		 res_3:2,	/* [23:22] */
		 tex:3,		/* [21:19] */
		 s:1,		/* [18] */
		 c:1,		/* [17] */
		 b:1,		/* [16] */
		 srd:8,		/* [15:8] */
		 res_4:2,	/* [7:6] */
		 size:5,		/* [5:1] */
		 enable:1;	/* [0] */
};
#define MPU_REGION_ATTR		(*(struct mpu_region_attr_and_size *)0xE000EDA0)

#define MPU_AP_PRIV_RO (1 << 2)
#define MPU_AP_PRIV_RW (0 << 2)
#define MPU_AP_USER_NA 1
#define MPU_AP_USER_RO 2
#define MPU_AP_USER_RW 3

#define bit_width_max(bits) ((1ull << ((bits) - 1) << 1) - 1)
#define B_(n, h, l) (((n) >> l) & bit_width_max((h) - (l) + 1)
#define ACCESS_ONCE(v) (*(volatile typeof(v) *)(&(v)))

int mpu_region_ct(void)
{
	/* extract [15:8] */
	return ACCESS_ONCE(MPU_TYPE).dregion;
}

