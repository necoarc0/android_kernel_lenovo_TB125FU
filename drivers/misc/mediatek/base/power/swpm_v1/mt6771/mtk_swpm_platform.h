/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#ifndef __MTK_SWPM_PLATFORM_H__
#define __MTK_SWPM_PLATFORM_H__

#define MAX_RECORD_CNT                  (64)
#define MAX_APHY_PWR                    (10)
#define DEFAULT_LOG_INTERVAL_MS         (1000)
#define DEFAULT_LOG_MASK                (0x13) /* VPROC2 + VPROC1 + VDRAM */

#define CPU_LKG_NOT_SUPPORT		(1)

#define NR_CPU_OPP                      (16)
#define NR_CPU_L_CORE                   (4)


/* data shared w/ SSPM */
enum profile_point {
	READ_EMI_TIME,
	MON_TIME,
	CALC_TIME,
	REC_TIME,
	TOTAL_TIME,

	NR_PROFILE_POINT
};

enum power_rail {
	VPROC12,
	VPROC11,
	VGPU,
	VCORE,
	VDRAM1,
	VIO18_DDR,
	VIO18_DRAM,

	NR_POWER_RAIL
};

enum power_meter_type {
	CPU_POWER_METER,
	GPU_POWER_METER,
	CORE_POWER_METER,
	MEM_POWER_METER,

	NR_POWER_METER
};

enum ddr_freq {
	DDR_800,
	DDR_1200,
	DDR_1600,
	DDR_1800,

	NR_DDR_FREQ
};

enum aphy_pwr_type {
	APHY_VCORE_0P7V,
	APHY_VDDQ_0P6V,
	APHY_VM_1P1V,
	APHY_VIO_1P8V,

	NR_APHY_PWR_TYPE
};

enum dram_pwr_type {
	DRAM_VDD1_1P8V,
	DRAM_VDD2_1P1V,
	DRAM_VDDQ_0P6V,

	NR_DRAM_PWR_TYPE
};

struct aphy_pwr {
	unsigned short bw[5];
	unsigned short coef[5];
};

/* unit: uW / V^2 */
struct aphy_pwr_data {
	struct aphy_pwr read_pwr[NR_DDR_FREQ];
	struct aphy_pwr write_pwr[NR_DDR_FREQ];
	unsigned short coef_idle[NR_DDR_FREQ];
};

/* unit: uA */
struct dram_pwr_conf {
	unsigned int i_dd0;
	unsigned int i_dd2p;
	unsigned int i_dd2n;
	unsigned int i_dd4r;
	unsigned int i_dd4w;
	unsigned int i_dd5;
	unsigned int i_dd6;
};

struct swpm_rec_data {
	/* 8 bytes */
	unsigned int cur_idx;
	unsigned int profile_enable;

	/* 4(int) * 64(rec_cnt) * 4 = 1KB */
	unsigned int pwr[NR_POWER_METER][MAX_RECORD_CNT];

	/* 8(long) * 5(prof_pt) * 3 = 120 bytes */
	unsigned long long avg_latency[NR_PROFILE_POINT];
	unsigned long long max_latency[NR_PROFILE_POINT];
	unsigned long long prof_cnt[NR_PROFILE_POINT];

	/* 2(short) * 4(pwr_type) * 84 = 672 bytes */
	struct aphy_pwr_data aphy_pwr_tbl[NR_APHY_PWR_TYPE];

	/* 4(int) * 3(pwr_type) * 7 = 84 bytes */
	struct dram_pwr_conf dram_conf[NR_DRAM_PWR_TYPE];

	/* reserv for emi_idx in SSPM = 1024 bytes */
	unsigned char reserv[1024];

	/* remaining size = 148 bytes */
};

extern struct swpm_rec_data *swpm_info_ref;

#endif

