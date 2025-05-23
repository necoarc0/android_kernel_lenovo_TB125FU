/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2020 MediaTek Inc.
 */

#ifndef __MTK_CPUFREQ_PLATFORM_H__
#define __MTK_CPUFREQ_PLATFORM_H__

#include "mtk_cpufreq_internal.h"

#ifdef CONFIG_MTK_TINYSYS_SSPM_SUPPORT
#define CONFIG_HYBRID_CPU_DVFS	1
/* #define PPM_AP_SIDE	1 */
#define EEM_AP_SIDE	1
/* #define CPU_DVFS_NOT_READY	1 */
#define DRCC_SUPPORT 1
#define REPORT_IDLE_FREQ	1
#else
#define SUPPORT_VOLT_HW_AUTO_TRACK 1
#define CPU_DVFS_NOT_READY	1
#endif

#define VBOOT_VOLT 80000
#define SINGLE_CLUSTER 1
/* buck ctrl configs */
#define NORMAL_DIFF_VRSAM_VPROC		10000
#define MAX_DIFF_VSRAM_VPROC		26250
#define MIN_VSRAM_VOLT			85000
#define MAX_VSRAM_VOLT			112000
#define MIN_VPROC_VOLT			60000
#define MAX_VPROC_VOLT			112000

#define UP_SRATE	1000
#define DOWN_SRATE	750
#define PMIC_CMD_DELAY_TIME	5
#define MIN_PMIC_SETTLE_TIME	5

#define PLL_SETTLE_TIME		20
#define POS_SETTLE_TIME		1

#define DVFSP_DT_NODE		"mediatek,mt6771-dvfsp"

#define CSRAM_BASE		0x0011bc00
#define CSRAM_SIZE		0x1400		/* 5K bytes */

#define DVFS_LOG_NUM		150
#define ENTRY_EACH_LOG		5

extern struct mt_cpu_dvfs cpu_dvfs[NR_MT_CPU_DVFS];
extern struct buck_ctrl_t buck_ctrl[NR_MT_BUCK];
extern struct pll_ctrl_t pll_ctrl[NR_MT_PLL];
extern struct hp_action_tbl cpu_dvfs_hp_action[];
extern unsigned int nr_hp_action;

extern void prepare_pll_addr(enum mt_cpu_dvfs_pll_id pll_id);
extern void prepare_pmic_config(struct mt_cpu_dvfs *p);
extern unsigned int _cpu_dds_calc(unsigned int khz);
extern unsigned int get_cur_phy_freq(struct pll_ctrl_t *pll_p);
extern unsigned char get_clkdiv(struct pll_ctrl_t *pll_p);
extern unsigned char get_posdiv(struct pll_ctrl_t *pll_p);

#ifdef ENABLE_TURBO_MODE_AP
extern void mt_cpufreq_turbo_action(unsigned long action,
	unsigned int *cpus, enum mt_cpu_dvfs_id cluster_id);
#endif
extern int mt_cpufreq_turbo_config(enum mt_cpu_dvfs_id id,
	unsigned int turbo_f, unsigned int turbo_v);


extern int mt_cpufreq_regulator_map(struct platform_device *pdev);
extern int mt_cpufreq_dts_map(void);
extern unsigned int _mt_cpufreq_get_cpu_level(void);

/* CPU mask related */
extern unsigned int cpufreq_get_nr_clusters(void);
extern void cpufreq_get_cluster_cpus(struct cpumask *cpu_mask,
	unsigned int cid);
extern unsigned int cpufreq_get_cluster_id(unsigned int cpu_id);

#define TOTAL_CORE_NUM  (CORE_NUM_L+CORE_NUM_B)
#define CORE_NUM_L      (4)
#define CORE_NUM_B      (4)

static inline void arch_get_cluster_cpus(struct cpumask *cpus, int cluster_id)
{
	int cpu = 0;

	cpumask_clear(cpus);

	if (cluster_id == 0) {
		cpu = 0;

		while (cpu < CORE_NUM_L) {
			cpumask_set_cpu(cpu, cpus);
			cpu++;
		}
	} else {
		cpu = CORE_NUM_L;

		while (cpu < TOTAL_CORE_NUM) {
			cpumask_set_cpu(cpu, cpus);
			cpu++;
		}
	}
}

static inline int arch_get_cluster_id(unsigned int cpu)
{
	return cpu < 4 ? 0:1;
}

static inline int arch_get_nr_clusters(void)
{
	return 2;
}
#endif	/* __MTK_CPUFREQ_PLATFORM_H__ */
