/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2019 MediaTek Inc.
 */

#ifndef _MT_GPUFREQ_CORE_H_
#define _MT_GPUFREQ_CORE_H_

/**************************************************
 * MT6771 segment_1 : GPU DVFS OPP table Setting
 **************************************************/
#define SEG1_GPU_DVFS_FREQ0					(900000)	/* KHz */
#define SEG1_GPU_DVFS_FREQ1					(850000)	/* KHz */
#define SEG1_GPU_DVFS_FREQ2					(800000)	/* KHz */
#define SEG1_GPU_DVFS_FREQ3					(743000)	/* KHz */
#define SEG1_GPU_DVFS_FREQ4					(698000)	/* KHz */
#define SEG1_GPU_DVFS_FREQ5					(653000)	/* KHz */
#define SEG1_GPU_DVFS_FREQ6					(620000)	/* KHz */
#define SEG1_GPU_DVFS_FREQ7					(580000)	/* KHz */
#define SEG1_GPU_DVFS_FREQ8					(540000)	/* KHz */
#define SEG1_GPU_DVFS_FREQ9					(500000)	/* KHz */
#define SEG1_GPU_DVFS_FREQ10				(460000)	/* KHz */
#define SEG1_GPU_DVFS_FREQ11				(420000)	/* KHz */
#define SEG1_GPU_DVFS_FREQ12				(380000)	/* KHz */
#define SEG1_GPU_DVFS_FREQ13				(340000)	/* KHz */
#define SEG1_GPU_DVFS_FREQ14				(320000)	/* KHz */
#define SEG1_GPU_DVFS_FREQ15				(300000)	/* KHz */

#define SEG1_GPU_DVFS_VOLT0					(90000)		/* mV x 100 */
#define SEG1_GPU_DVFS_VOLT1					(86250)		/* mV x 100 */
#define SEG1_GPU_DVFS_VOLT2					(82500)		/* mV x 100 */
#define SEG1_GPU_DVFS_VOLT3					(79375)		/* mV x 100 */
#define SEG1_GPU_DVFS_VOLT4					(76875)		/* mV x 100 */
#define SEG1_GPU_DVFS_VOLT5					(74375)		/* mV x 100 */
#define SEG1_GPU_DVFS_VOLT6					(72500)		/* mV x 100 */
#define SEG1_GPU_DVFS_VOLT7					(71250)		/* mV x 100 */
#define SEG1_GPU_DVFS_VOLT8					(70000)		/* mV x 100 */
#define SEG1_GPU_DVFS_VOLT9					(68750)		/* mV x 100 */
#define SEG1_GPU_DVFS_VOLT10				(67500)		/* mV x 100 */
#define SEG1_GPU_DVFS_VOLT11				(66250)		/* mV x 100 */
#define SEG1_GPU_DVFS_VOLT12				(65000)		/* mV x 100 */
#define SEG1_GPU_DVFS_VOLT13				(63750)		/* mV x 100 */
#define SEG1_GPU_DVFS_VOLT14				(63125)		/* mV x 100 */
#define SEG1_GPU_DVFS_VOLT15				(62500)		/* mV x 100 */

#define SEG1_GPU_DVFS_VSRAM0				(100000)	/* mV x 100 */
#define SEG1_GPU_DVFS_VSRAM1				(96250)		/* mV x 100 */
#define SEG1_GPU_DVFS_VSRAM2				(92500)		/* mV x 100 */
#define SEG1_GPU_DVFS_VSRAM3				(89375)		/* mV x 100 */
#define SEG1_GPU_DVFS_VSRAM4				(86875)		/* mV x 100 */
#define SEG1_GPU_DVFS_VSRAM5				(85000)		/* mV x 100 */
#define SEG1_GPU_DVFS_VSRAM6				(85000)		/* mV x 100 */
#define SEG1_GPU_DVFS_VSRAM7				(85000)		/* mV x 100 */
#define SEG1_GPU_DVFS_VSRAM8				(85000)		/* mV x 100 */
#define SEG1_GPU_DVFS_VSRAM9				(85000)		/* mV x 100 */
#define SEG1_GPU_DVFS_VSRAM10				(85000)		/* mV x 100 */
#define SEG1_GPU_DVFS_VSRAM11				(85000)		/* mV x 100 */
#define SEG1_GPU_DVFS_VSRAM12				(85000)		/* mV x 100 */
#define SEG1_GPU_DVFS_VSRAM13				(85000)		/* mV x 100 */
#define SEG1_GPU_DVFS_VSRAM14				(85000)		/* mV x 100 */
#define SEG1_GPU_DVFS_VSRAM15				(85000)		/* mV x 100 */

/**************************************************
 * MT6771 segment_2 : GPU DVFS OPP table Setting
 **************************************************/
#define SEG2_GPU_DVFS_FREQ0					(800000)	/* KHz */
#define SEG2_GPU_DVFS_FREQ1					(743000)	/* KHz */
#define SEG2_GPU_DVFS_FREQ2					(698000)	/* KHz */
#define SEG2_GPU_DVFS_FREQ3					(653000)	/* KHz */
#define SEG2_GPU_DVFS_FREQ4					(620000)	/* KHz */
#define SEG2_GPU_DVFS_FREQ5					(580000)	/* KHz */
#define SEG2_GPU_DVFS_FREQ6					(540000)	/* KHz */
#define SEG2_GPU_DVFS_FREQ7					(500000)	/* KHz */
#define SEG2_GPU_DVFS_FREQ8					(460000)	/* KHz */
#define SEG2_GPU_DVFS_FREQ9					(420000)	/* KHz */
#define SEG2_GPU_DVFS_FREQ10				(400000)	/* KHz */
#define SEG2_GPU_DVFS_FREQ11				(380000)	/* KHz */
#define SEG2_GPU_DVFS_FREQ12				(360000)	/* KHz */
#define SEG2_GPU_DVFS_FREQ13				(340000)	/* KHz */
#define SEG2_GPU_DVFS_FREQ14				(320000)	/* KHz */
#define SEG2_GPU_DVFS_FREQ15				(300000)	/* KHz */

#define SEG2_GPU_DVFS_VOLT0					(82500)		/* mV x 100 */
#define SEG2_GPU_DVFS_VOLT1					(79375)		/* mV x 100 */
#define SEG2_GPU_DVFS_VOLT2					(76875)		/* mV x 100 */
#define SEG2_GPU_DVFS_VOLT3					(74375)		/* mV x 100 */
#define SEG2_GPU_DVFS_VOLT4					(72500)		/* mV x 100 */
#define SEG2_GPU_DVFS_VOLT5					(71250)		/* mV x 100 */
#define SEG2_GPU_DVFS_VOLT6					(70000)		/* mV x 100 */
#define SEG2_GPU_DVFS_VOLT7					(68750)		/* mV x 100 */
#define SEG2_GPU_DVFS_VOLT8					(67500)		/* mV x 100 */
#define SEG2_GPU_DVFS_VOLT9					(66250)		/* mV x 100 */
#define SEG2_GPU_DVFS_VOLT10				(65625)		/* mV x 100 */
#define SEG2_GPU_DVFS_VOLT11				(65000)		/* mV x 100 */
#define SEG2_GPU_DVFS_VOLT12				(64375)		/* mV x 100 */
#define SEG2_GPU_DVFS_VOLT13				(63750)		/* mV x 100 */
#define SEG2_GPU_DVFS_VOLT14				(63125)		/* mV x 100 */
#define SEG2_GPU_DVFS_VOLT15				(62500)		/* mV x 100 */

#define SEG2_GPU_DVFS_VSRAM0				(92500)		/* mV x 100 */
#define SEG2_GPU_DVFS_VSRAM1				(89375)		/* mV x 100 */
#define SEG2_GPU_DVFS_VSRAM2				(86875)		/* mV x 100 */
#define SEG2_GPU_DVFS_VSRAM3				(85000)		/* mV x 100 */
#define SEG2_GPU_DVFS_VSRAM4				(85000)		/* mV x 100 */
#define SEG2_GPU_DVFS_VSRAM5				(85000)		/* mV x 100 */
#define SEG2_GPU_DVFS_VSRAM6				(85000)		/* mV x 100 */
#define SEG2_GPU_DVFS_VSRAM7				(85000)		/* mV x 100 */
#define SEG2_GPU_DVFS_VSRAM8				(85000)		/* mV x 100 */
#define SEG2_GPU_DVFS_VSRAM9				(85000)		/* mV x 100 */
#define SEG2_GPU_DVFS_VSRAM10				(85000)		/* mV x 100 */
#define SEG2_GPU_DVFS_VSRAM11				(85000)		/* mV x 100 */
#define SEG2_GPU_DVFS_VSRAM12				(85000)		/* mV x 100 */
#define SEG2_GPU_DVFS_VSRAM13				(85000)		/* mV x 100 */
#define SEG2_GPU_DVFS_VSRAM14				(85000)		/* mV x 100 */
#define SEG2_GPU_DVFS_VSRAM15				(85000)		/* mV x 100 */

/**************************************************
 * MT6771 segment_3 : GPU DVFS OPP table Setting
 **************************************************/
#define SEG3_GPU_DVFS_FREQ0					(640000)	/* KHz */
#define SEG3_GPU_DVFS_FREQ1					(600000)	/* KHz */
#define SEG3_GPU_DVFS_FREQ2					(560000)	/* KHz */
#define SEG3_GPU_DVFS_FREQ3					(540000)	/* KHz */
#define SEG3_GPU_DVFS_FREQ4					(520000)	/* KHz */
#define SEG3_GPU_DVFS_FREQ5					(500000)	/* KHz */
#define SEG3_GPU_DVFS_FREQ6					(480000)	/* KHz */
#define SEG3_GPU_DVFS_FREQ7					(460000)	/* KHz */
#define SEG3_GPU_DVFS_FREQ8					(440000)	/* KHz */
#define SEG3_GPU_DVFS_FREQ9					(420000)	/* KHz */
#define SEG3_GPU_DVFS_FREQ10				(400000)	/* KHz */
#define SEG3_GPU_DVFS_FREQ11				(380000)	/* KHz */
#define SEG3_GPU_DVFS_FREQ12				(360000)	/* KHz */
#define SEG3_GPU_DVFS_FREQ13				(340000)	/* KHz */
#define SEG3_GPU_DVFS_FREQ14				(320000)	/* KHz */
#define SEG3_GPU_DVFS_FREQ15				(300000)	/* KHz */

#define SEG3_GPU_DVFS_VOLT0					(73750)		/* mV x 100 */
#define SEG3_GPU_DVFS_VOLT1					(71875)		/* mV x 100 */
#define SEG3_GPU_DVFS_VOLT2					(70625)		/* mV x 100 */
#define SEG3_GPU_DVFS_VOLT3					(70000)		/* mV x 100 */
#define SEG3_GPU_DVFS_VOLT4					(69375)		/* mV x 100 */
#define SEG3_GPU_DVFS_VOLT5					(68750)		/* mV x 100 */
#define SEG3_GPU_DVFS_VOLT6					(68125)		/* mV x 100 */
#define SEG3_GPU_DVFS_VOLT7					(67500)		/* mV x 100 */
#define SEG3_GPU_DVFS_VOLT8					(66875)		/* mV x 100 */
#define SEG3_GPU_DVFS_VOLT9					(66250)		/* mV x 100 */
#define SEG3_GPU_DVFS_VOLT10				(65625)		/* mV x 100 */
#define SEG3_GPU_DVFS_VOLT11				(65000)		/* mV x 100 */
#define SEG3_GPU_DVFS_VOLT12				(64375)		/* mV x 100 */
#define SEG3_GPU_DVFS_VOLT13				(63750)		/* mV x 100 */
#define SEG3_GPU_DVFS_VOLT14				(63125)		/* mV x 100 */
#define SEG3_GPU_DVFS_VOLT15				(62500)		/* mV x 100 */

#define SEG3_GPU_DVFS_VSRAM0				(85000)		/* mV x 100 */
#define SEG3_GPU_DVFS_VSRAM1				(85000)		/* mV x 100 */
#define SEG3_GPU_DVFS_VSRAM2				(85000)		/* mV x 100 */
#define SEG3_GPU_DVFS_VSRAM3				(85000)		/* mV x 100 */
#define SEG3_GPU_DVFS_VSRAM4				(85000)		/* mV x 100 */
#define SEG3_GPU_DVFS_VSRAM5				(85000)		/* mV x 100 */
#define SEG3_GPU_DVFS_VSRAM6				(85000)		/* mV x 100 */
#define SEG3_GPU_DVFS_VSRAM7				(85000)		/* mV x 100 */
#define SEG3_GPU_DVFS_VSRAM8				(85000)		/* mV x 100 */
#define SEG3_GPU_DVFS_VSRAM9				(85000)		/* mV x 100 */
#define SEG3_GPU_DVFS_VSRAM10				(85000)		/* mV x 100 */
#define SEG3_GPU_DVFS_VSRAM11				(85000)		/* mV x 100 */
#define SEG3_GPU_DVFS_VSRAM12				(85000)		/* mV x 100 */
#define SEG3_GPU_DVFS_VSRAM13				(85000)		/* mV x 100 */
#define SEG3_GPU_DVFS_VSRAM14				(85000)		/* mV x 100 */
#define SEG3_GPU_DVFS_VSRAM15				(85000)		/* mV x 100 */

/**************************************************
 * MT6771 segment_4 : GPU DVFS OPP table Setting
 **************************************************/
#define SEG4_GPU_DVFS_FREQ0					(900000)	/* KHz */
#define SEG4_GPU_DVFS_FREQ1					(850000)	/* KHz */
#define SEG4_GPU_DVFS_FREQ2					(800000)	/* KHz */
#define SEG4_GPU_DVFS_FREQ3					(743000)	/* KHz */
#define SEG4_GPU_DVFS_FREQ4					(698000)	/* KHz */
#define SEG4_GPU_DVFS_FREQ5					(653000)	/* KHz */
#define SEG4_GPU_DVFS_FREQ6					(620000)	/* KHz */
#define SEG4_GPU_DVFS_FREQ7					(580000)	/* KHz */
#define SEG4_GPU_DVFS_FREQ8					(540000)	/* KHz */
#define SEG4_GPU_DVFS_FREQ9					(500000)	/* KHz */
#define SEG4_GPU_DVFS_FREQ10				(460000)	/* KHz */
#define SEG4_GPU_DVFS_FREQ11				(420000)	/* KHz */
#define SEG4_GPU_DVFS_FREQ12				(380000)	/* KHz */
#define SEG4_GPU_DVFS_FREQ13				(340000)	/* KHz */
#define SEG4_GPU_DVFS_FREQ14				(320000)	/* KHz */
#define SEG4_GPU_DVFS_FREQ15				(300000)	/* KHz */

#define SEG4_GPU_DVFS_VOLT0					(90000)		/* mV x 100 */
#define SEG4_GPU_DVFS_VOLT1					(86250)		/* mV x 100 */
#define SEG4_GPU_DVFS_VOLT2					(82500)		/* mV x 100 */
#define SEG4_GPU_DVFS_VOLT3					(79375)		/* mV x 100 */
#define SEG4_GPU_DVFS_VOLT4					(76875)		/* mV x 100 */
#define SEG4_GPU_DVFS_VOLT5					(74375)		/* mV x 100 */
#define SEG4_GPU_DVFS_VOLT6					(72500)		/* mV x 100 */
#define SEG4_GPU_DVFS_VOLT7					(71250)		/* mV x 100 */
#define SEG4_GPU_DVFS_VOLT8					(70000)		/* mV x 100 */
#define SEG4_GPU_DVFS_VOLT9					(68750)		/* mV x 100 */
#define SEG4_GPU_DVFS_VOLT10				(67500)		/* mV x 100 */
#define SEG4_GPU_DVFS_VOLT11				(66250)		/* mV x 100 */
#define SEG4_GPU_DVFS_VOLT12				(65000)		/* mV x 100 */
#define SEG4_GPU_DVFS_VOLT13				(63750)		/* mV x 100 */
#define SEG4_GPU_DVFS_VOLT14				(63125)		/* mV x 100 */
#define SEG4_GPU_DVFS_VOLT15				(62500)		/* mV x 100 */

#define SEG4_GPU_DVFS_VSRAM0				(100000)		/* mV x 100 */
#define SEG4_GPU_DVFS_VSRAM1				(96250)		/* mV x 100 */
#define SEG4_GPU_DVFS_VSRAM2				(92500)		/* mV x 100 */
#define SEG4_GPU_DVFS_VSRAM3				(89375)		/* mV x 100 */
#define SEG4_GPU_DVFS_VSRAM4				(86875)		/* mV x 100 */
#define SEG4_GPU_DVFS_VSRAM5				(85000)		/* mV x 100 */
#define SEG4_GPU_DVFS_VSRAM6				(85000)		/* mV x 100 */
#define SEG4_GPU_DVFS_VSRAM7				(85000)		/* mV x 100 */
#define SEG4_GPU_DVFS_VSRAM8				(85000)		/* mV x 100 */
#define SEG4_GPU_DVFS_VSRAM9				(85000)		/* mV x 100 */
#define SEG4_GPU_DVFS_VSRAM10				(85000)		/* mV x 100 */
#define SEG4_GPU_DVFS_VSRAM11				(85000)		/* mV x 100 */
#define SEG4_GPU_DVFS_VSRAM12				(85000)		/* mV x 100 */
#define SEG4_GPU_DVFS_VSRAM13				(85000)		/* mV x 100 */
#define SEG4_GPU_DVFS_VSRAM14				(85000)		/* mV x 100 */
#define SEG4_GPU_DVFS_VSRAM15				(85000)		/* mV x 100 */

/**************************************************
 * PMIC Setting
 **************************************************/
#define VGPU_MAX_VOLT					(SEG1_GPU_DVFS_VOLT0)
#define VSRAM_GPU_MAX_VOLT				(SEG1_GPU_DVFS_VSRAM0)
#define DELAY_FACTOR					(625)
#define PMIC_SRCLKEN_HIGH_TIME_US		(1000)		/* spec is 1(ms) */
#define BUCK_VARIATION_MAX				(25000)		/* mV x 100 */
#define BUCK_VARIATION_MIN				(10000)		/* mV x 100 */

/**************************************************
 * efuse Setting
 **************************************************/
#define GPUFREQ_EFUSE_INDEX				(8)
#define EFUSE_MFG_SPD_BOND_SHIFT		(8)
#define EFUSE_MFG_SPD_BOND_MASK			(0xF)
#define FUNC_CODE_EFUSE_INDEX			(22)
#define EFUSE_FIELD_CNT 1

/**************************************************
 * Clock Setting
 **************************************************/
#define POST_DIV_2_MAX_FREQ				(1900000)
#define POST_DIV_2_MIN_FREQ				(750000)
#define POST_DIV_4_MAX_FREQ				(950000)
#define POST_DIV_4_MIN_FREQ				(375000)
#define POST_DIV_8_MAX_FREQ				(475000)
#define POST_DIV_8_MIN_FREQ				(187500)
#define POST_DIV_16_MAX_FREQ			(237500)
#define POST_DIV_16_MIN_FREQ			(93750)
#define POST_DIV_MASK					(0x70000000)
#define POST_DIV_SHIFT					(24)
#define TO_MHz_HEAD						(100)
#define TO_MHz_TAIL						(10)
#define ROUNDING_VALUE					(5)
#define DDS_SHIFT						(14)
#define GPUPLL_FIN						(26)
#define GPUPLL_CON0						(g_apmixed_base + 0x240)
#define GPUPLL_CON1						(g_apmixed_base + 0x244)
#define GPUPLL_PWR_CON0					(g_apmixed_base + 0x24C)

/**************************************************
 * Reference Power Setting
 **************************************************/
#define GPU_ACT_REF_POWER				(1285)		/* mW  */
#define GPU_ACT_REF_FREQ				(900000)	/* KHz */
#define GPU_ACT_REF_VOLT				(90000)		/* mV x 100 */
#define GPU_DVFS_PTPOD_DISABLE_VOLT		(80000)		/* mV x 100 */

/**************************************************
 * Log Setting
 **************************************************/
#define GPUFERQ_TAG							"[GPU/DVFS]"
#define gpufreq_pr_err(fmt, args...)		pr_err(GPUFERQ_TAG"[ERROR]"fmt, ##args)
#define gpufreq_pr_warn(fmt, args...)		pr_warn(GPUFERQ_TAG"[WARNING]"fmt, ##args)
#define gpufreq_pr_info(fmt, args...)		pr_info(GPUFERQ_TAG"[INFO]"fmt, ##args)
#define gpufreq_pr_debug(fmt, args...)		pr_debug(GPUFERQ_TAG"[DEBUG]"fmt, ##args)

/**************************************************
 * Condition Setting
 **************************************************/
#define MT_GPUFREQ_OPP_STRESS_TEST
//#define MT_GPUFREQ_STATIC_PWR_READY2USE    //to-do
#define MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
#define MT_GPUFREQ_BATT_PERCENT_PROTECT /* todo: disable it */
#define MT_GPUFREQ_BATT_OC_PROTECT
#define MT_GPUFREQ_DYNAMIC_POWER_TABLE_UPDATE

/**************************************************
 * Battery Over Current Protect
 **************************************************/
#ifdef MT_GPUFREQ_BATT_OC_PROTECT
#define MT_GPUFREQ_BATT_OC_LIMIT_FREQ				(485000)	/* KHz */
#endif

/**************************************************
 * Battery Percentage Protect
 **************************************************/
#ifdef MT_GPUFREQ_BATT_PERCENT_PROTECT
#define MT_GPUFREQ_BATT_PERCENT_LIMIT_FREQ			(900000)	/* KHz */
#endif

/**************************************************
 * Low Battery Volume Protect
 **************************************************/
#ifdef MT_GPUFREQ_LOW_BATT_VOLT_PROTECT
#define MT_GPUFREQ_LOW_BATT_VOLT_LIMIT_FREQ			(485000)	/* KHz */
#endif

/**************************************************
 * Proc Node Definition
 **************************************************/
#ifdef CONFIG_PROC_FS
#define PROC_FOPS_RW(name)	\
	static int mt_ ## name ## _proc_open(struct inode *inode, struct file *file)	\
	{	\
		return single_open(file, mt_ ## name ## _proc_show, PDE_DATA(inode));	\
	}	\
	static const struct file_operations mt_ ## name ## _proc_fops = {	\
		.owner = THIS_MODULE,	\
		.open = mt_ ## name ## _proc_open,	\
		.read = seq_read,	\
		.llseek = seq_lseek,	\
		.release = single_release,	\
		.write = mt_ ## name ## _proc_write,	\
	}
#define PROC_FOPS_RO(name)	\
	static int mt_ ## name ## _proc_open(struct inode *inode, struct file *file)	\
	{	\
		return single_open(file, mt_ ## name ## _proc_show, PDE_DATA(inode));	\
	}	\
	static const struct file_operations mt_ ## name ## _proc_fops =	\
	{	\
		.owner = THIS_MODULE,	\
		.open = mt_ ## name ## _proc_open,	\
		.read = seq_read,	\
		.llseek = seq_lseek,	\
		.release = single_release,	\
	}
#define PROC_ENTRY(name) \
	{__stringify(name), &mt_ ## name ## _proc_fops}
#endif

/**************************************************
 * Operation Definition
 **************************************************/
#define VOLT_NORMALIZATION(volt)		((volt % 625) ? (volt - (volt % 625) + 625) : volt)
#ifndef MIN
#define MIN(x, y)						(((x) < (y)) ? (x) : (y))
#endif
#define GPUOP(khz, volt, vsram, idx)	\
	{	\
		.gpufreq_khz = khz,	\
		.gpufreq_volt = volt,	\
		.gpufreq_vsram = vsram,	\
		.gpufreq_idx = idx,	\
	}

/**************************************************
 * Enumerations
 **************************************************/
enum g_segment_id_enum {
	MT6771_SEGMENT_1 = 1,
	MT6771_SEGMENT_2,
	MT6771_SEGMENT_3,
	MT6771_SEGMENT_4,
};
enum g_post_divider_power_enum  {
	POST_DIV2 = 1,
	POST_DIV4,
	POST_DIV8,
	POST_DIV16,
};
enum g_clock_source_enum  {
	CLOCK_MAIN = 0,
	CLOCK_SUB,
};
enum g_limited_idx_enum {
	IDX_THERMAL_PROTECT_LIMITED = 0,
	IDX_LOW_BATT_LIMITED,
	IDX_BATT_PERCENT_LIMITED,
	IDX_BATT_OC_LIMITED,
	IDX_PBM_LIMITED,
	NUMBER_OF_LIMITED_IDX,
};
enum g_volt_switch_enum {
	VOLT_FALLING = 0,
	VOLT_RISING,
};

/**************************************************
 * Structures
 **************************************************/
struct g_opp_table_info {
	unsigned int gpufreq_khz;
	unsigned int gpufreq_volt;
	unsigned int gpufreq_vsram;
	unsigned int gpufreq_idx;
};
struct g_clk_info {
	struct clk *clk_mux;			/* main clock for mfg setting */
	struct clk *clk_main_parent;	/* substitution clock for mfg transient mux setting */
	struct clk *clk_sub_parent;		/* substitution clock for mfg transient parent setting */
	struct clk *subsys_mfg_cg;		/* clock gating */
	struct clk *mtcmos_mfg_async;	/* */
	struct clk *mtcmos_mfg;			/* dependent on mtcmos_mfg_async */
	struct clk *mtcmos_mfg_core0;	/* dependent on mtcmos_mfg */
	struct clk *mtcmos_mfg_core1;	/* dependent on mtcmos_mfg */
	struct clk *mtcmos_mfg_core2;	/* dependent on mtcmos_mfg */
};
struct g_pmic_info {
	struct regulator *reg_vgpu;
	struct regulator *reg_vsram_gpu;
};

/**************************************************
 * External functions declaration
 **************************************************/
extern bool mtk_get_gpu_loading(unsigned int *pLoading);
extern unsigned int mt_get_ckgen_freq(unsigned int);

#endif /* _MT_GPUFREQ_CORE_H_ */
