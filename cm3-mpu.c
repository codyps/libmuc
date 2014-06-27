/*
 * based on http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0179b/CHDFDFIG.html
 */

/* ro */
#define MPU_TYPE		(*(struct mpu_type *)0xE000ED90)
/* number of supported regions. 8 or 0 */
#define MPU_TYPE_DREGION(a)	B_(a, 15, 8)
/* always 0 on cm3 */
#define MPU_TYPE_IREGION(a)	B_(a, 23, 16)
#define MPU_TYPE_SEPERATE(a)	B_(a, 0,  0)

#define MPU_CTRL		(*(uint32_t *)0xE000ED94)
#define MPU_CTRL_ENABLE(a)	B_(a, 0, 0)
#define MPU_CTRL_HFNMIENA(a)	B_(a, 1, 1)
#define MPU_CTRL_PRIVDEFNA(a)	B_(a, 2, 2)


#define MPU_REGION_NUM		(*(uint32_t *)0xE000ED98)
#define MPU_REGION_NUM_REGION(a)	B_(a, 7, 0)

#define MPU_REGION_BASE_ADDR	(*(uint32_t *)0xE000ED9C)
#define MPU_REGION_BASE_ADDR_ADDR(a)	B_(a, 31, 5)
#define MPU_REGION_BASE_ADDR_VALID(a)	B_(a, 4, 4)
#define MPU_REGION_BASE_ADDR_REGION(a)	B_(a, 3, 0)

struct mpu_region_attr_and_size {
	uint32_t res_1:3,	/* [31:29] */
		 xn:1		/* [28] */
		 res_2:1	/* [27] */
		 ap:3		/* [26:24] */
		 res_3:2	/* [23:22] */
		 tex:3		/* [21:19] */
		 s:1		/* [18] */
		 c:1		/* [17] */
		 b:1		/* [16] */
		 srd:8		/* [15:8] */
		 res_4:2	/* [7:6] */
		 size:5		/* [5:1] */
		 enable:1;	/* [0] */
};
#define MPU_REGION_ATTR		(*(struct mpu_region_attr_and_size *)0xE000EDA0)

/*
 * [28] XN
 * [26:24] AP
 * [21:19] TEX
 * [18] S
 * [17] C
 * [16] B
 * [15:8] SRD
 * [5:1] SIZE
 * [0]	ENABLE
 */

#define bit_width_max(bits) ((1ull << ((bits) - 1) << 1) - 1)
#define B_(n, h, l) (((n) >> l) & bit_width_max((h) - (l) + 1)

int mpu_region_ct(void)
{
	/* extract [15:8] */
	return B_(MPU_TYPE, 15, 8);
}

