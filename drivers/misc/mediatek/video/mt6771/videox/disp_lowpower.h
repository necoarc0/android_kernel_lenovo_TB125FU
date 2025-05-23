/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#ifndef __DISP_LOWPOWER_H__
#define __DISP_LOWPOWER_H__

#define LINE_ACCURACY 1000

unsigned int dsi_phy_get_clk(enum DISP_MODULE_ENUM module);
void primary_display_idlemgr_enter_idle_nolock(void);

struct golden_setting_context *get_golden_setting_pgc(void);
int primary_display_lowpower_init(void);
void primary_display_sodi_rule_init(void);
void kick_logger_dump(char *string);
void kick_logger_dump_reset(void);
char *get_kick_dump(void);
unsigned int get_kick_dump_size(void);
/**
 * return 0: display is not idle trigger now
 * return 1: display is idle
 */
int primary_display_is_idle(void);
void primary_display_idlemgr_kick(const char *source, int need_lock);
void enter_share_sram(enum CMDQ_EVENT_ENUM resourceEvent);
void leave_share_sram(enum CMDQ_EVENT_ENUM resourceEvent);
void set_hrtnum(unsigned int new_hrtnum);
void set_enterulps(unsigned int flag);
void set_is_dc(unsigned int is_dc);
unsigned int set_one_layer(unsigned int is_onelayer);
void set_rdma_width_height(unsigned int width, unsigned int height);
void enable_idlemgr(unsigned int flag);
unsigned int get_idlemgr_flag(void);
unsigned int set_idlemgr(unsigned int flag, int need_lock);
int _blocking_flush(void);
unsigned int get_idlemgr_flag(void);
unsigned int set_idlemgr(unsigned int flag, int need_lock);
unsigned int get_us_perline(unsigned int width);
unsigned int time_to_line(unsigned int ms, unsigned int width);
unsigned int disp_lp_set_idle_check_interval(unsigned int new_interval);

/************************** for met ***********************************/
/**
 * return 0: not enter ultra lowpower state which means mipi pll is enabled
 * return 1: enter ultra lowpower state whicn means mipi pll is disabled
 */
unsigned int is_mipi_enterulps(void);

/* read dsi regs to calculate clk */
unsigned int get_mipi_clk(void);

int primary_display_request_dvfs_perf(int scenario, int req);

#if defined(CONFIG_MTK_DUAL_DISPLAY_SUPPORT) && \
	(CONFIG_MTK_DUAL_DISPLAY_SUPPORT == 2)
int external_display_lowpower_init(void);
void external_display_sodi_rule_init(void);
int external_display_is_idle(void);
void enable_ext_idlemgr(unsigned int flag);
void external_display_idlemgr_kick(const char *source, int need_lock);
#endif

/* functions for anti-latency 2.0 */
int is_wrot_sram_available(void);
void unblock_release_wrot_sram(void);
void set_antilatency_need_repaint(void);

extern atomic_t idle_need_repaint;

#endif /* _DISP_LOWPOWER_H_ */
