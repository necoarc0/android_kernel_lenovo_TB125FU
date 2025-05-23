/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#define VENDOR_CHIP_DRIVER

#include "dpi_dvt_test.h"

#if defined(RDMA_DPI_PATH_SUPPORT) || defined(DPI_DVT_TEST_SUPPORT)

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/time.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/vmalloc.h>
#include <linux/err.h>
#include <linux/switch.h>

#include <linux/file.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_fdt.h>
#include <linux/io.h>

#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/kthread.h>
#include <linux/timer.h>
#include <linux/types.h>
#ifdef CONFIG_MTK_CLKMGR
#include <mach/mt_clkmgr.h>
#endif

#include "m4u.h"
#include "ddp_log.h"
#include "ddp_dump.h"
#include "ddp_info.h"
#include "ddp_clkmgr.h"
#include "ddp_reg.h"
/* #include "ddp_mutex.h" */
#include "ddp_reg_ovl.h"
/* #include "disp_drv_dsi.h" */
#include "ddp_dsi.h"

#include "hdmi_drv.h"

#include "ddp_dpi_ext.h"
#include "dpi_dvt_test.h"
#include "dpi_dvt_platform.h"
#include "DpDataType.h"
#include "disp_lowpower.h"
#include "extd_factory.h"
#include <linux/hrtimer.h>
#include <mt-plat/upmu_common.h>
#define MHL_SII8348


/*
 *
 *To-do list:
 *1.  Open RDMA_DPI_PATH_SUPPORT or DPI_DVT_TEST_SUPPORT;
 *2.  extern picture data structure and modify Makefile,
 *     e.g.  extern unsigned char kara_1280x720[2764800];
 *3.  Modify sii8348 driver;
 */

/******************************************************************************/

/**************************Resource File***************************************/
/*#define BMP_HEADER_SIZE 54*/

/*********************DPI DVT Path Control*************************************/
struct DPI_DVT_CONTEXT DPI_DVT_Context;
bool disconnectFlag = true;
enum PATTERN {
	A256_VERTICAL_GRAY,
	A64_VERTICAL_GRAY,
	A256_HORIZONTAL_GRAY,
	A64_HORIZONTAL_GRAY,
	COLOR_BAR,
	USER_DEFINED_COLOR,
	FRAME_BORDER,
	DOT_MOREI,
	MAX_PATTERN
};
static struct HDMI_DRIVER *hdmi_drv;
static struct disp_ddp_path_config extd_dpi_params;
static struct disp_ddp_path_config extd_rdma_params;
static struct disp_ddp_path_config extd_ovl_params;
static struct golden_setting_context temp_golden;
struct cmdqRecStruct *checksum_cmdq;
cmdqBackupSlotHandle checksum_record;
cmdqBackupSlotHandle checksum_record1;
m4u_client_t *client_ldvt;
int is_timer_inited;

static int checksum[10];
/*TEST_CASE_TYPE test_type;*/
unsigned long int ldvt_data_va[4];
unsigned int ldvt_data_mva[4];
unsigned long int data_va;
unsigned long int out_va;
unsigned int data_mva;
unsigned int out_mva;
unsigned int bypass = 1;
/****************************Basic Function Start*****************************/
#ifdef DPI_DVT_TEST_SUPPORT
enum RDMA_MODE {
	RDMA_MODE_DIRECT_LINK = 0,
	RDMA_MODE_MEMORY = 1,
};

enum DSI_MODE {
	DSI_CMD_MODE_0 = 0,
	DSI_VDO_MODE_SYNC_PLUSE,
	DSI_VDO_MODE_SYNC_EVENT,
	DSI_VDO_MODE_BURST,
	DSI_MODE_MAX
};

enum ACTION {
	INIT,
	POWER_ON,
	POWER_OFF,
	CONFIG,
	TRIGGER,
	BYPASS,
	START,
	STOP,
	DEINIT,
	RESET,
	MAX_ACTION
};

enum PATH_TYPE {
	LONG_PATH0,
	LONG_PATH1,
	SHORT_PATH1,
	SHORT_PATH2,
	WDMA1_PATH,
	LONG_PATH1_DSI1,
	MAX_PATH
};

int dvt_module_action(enum PATH_TYPE test_path, enum ACTION action)
{
	int mod_num = 0;
	int i = 0;
	unsigned int *module = NULL;
	int cur_mod = 0;
	struct DDP_MODULE_DRIVER *m;

	switch (test_path) {
	case LONG_PATH0:
		mod_num = LONG_PATH0_MODULE_NUM;
		module = long_path0_module;
		break;
	case LONG_PATH1:
		mod_num = LONG_PATH1_MODULE_NUM;
		module = long_path1_module;
		break;
	case SHORT_PATH1:
		mod_num = SHORT_PATH_MODULE_NUM;
		module = short_path_module;
		break;
	default:
		break;
	}
	switch (action) {
	case INIT:
		for (i = 0; i < mod_num; i++) {
			cur_mod = module[i];
			if (cur_mod != DISP_MODULE_UNKNOWN
				&& ddp_get_module_driver(cur_mod) != 0) {
				m = ddp_get_module_driver(cur_mod);
				if (m->init != 0)
					m->init(cur_mod, NULL);
			}
		}
		break;
	case POWER_ON:
		for (i = 0; i < mod_num; i++) {
			cur_mod = module[i];
			if (cur_mod != DISP_MODULE_UNKNOWN
				&& ddp_get_module_driver(cur_mod) != 0) {
				m = ddp_get_module_driver(cur_mod);
				if (m->power_on != 0)
					m->power_on(cur_mod, NULL);
			}
		}
		break;
	case POWER_OFF:
		for (i = 0; i < mod_num; i++) {
			cur_mod = module[i];
			if (cur_mod != DISP_MODULE_UNKNOWN
				&& ddp_get_module_driver(cur_mod) != 0) {
				m = ddp_get_module_driver(cur_mod);
				if (m->power_off != 0)
					m->power_off(cur_mod, NULL);
			}
		}
		break;
	case START:
		for (i = 0; i < mod_num; i++) {
			cur_mod = module[i];
			if (cur_mod != DISP_MODULE_UNKNOWN
				&& ddp_get_module_driver(cur_mod) != 0) {
				m = ddp_get_module_driver(cur_mod);
				if (m->start != 0)
					m->start(cur_mod, NULL);
			}
		}
		break;
	case STOP:
		for (i = 0; i < mod_num; i++) {
			cur_mod = module[i];
			if (cur_mod != DISP_MODULE_UNKNOWN
				&& ddp_get_module_driver(cur_mod) != 0) {
				m = ddp_get_module_driver(cur_mod);
				if (m->stop != 0)
					m->stop(cur_mod, NULL);
			}
		}
		break;
	case RESET:
		for (i = 0; i < mod_num; i++) {
			cur_mod = module[i];
			if (cur_mod != DISP_MODULE_UNKNOWN
				&& ddp_get_module_driver(cur_mod) != 0) {
				m = ddp_get_module_driver(cur_mod);
				if (m->reset != 0)
					m->reset(cur_mod, NULL);
			}
		}
		break;
	case BYPASS:
		for (i = 0; i < mod_num; i++) {
			cur_mod = module[i];
			if (cur_mod != DISP_MODULE_UNKNOWN
				&& ddp_get_module_driver(cur_mod) != 0) {
				m = ddp_get_module_driver(cur_mod);
				if (m->bypass != 0)
					m->bypass(cur_mod, 1);
			}
		}
		break;
	case TRIGGER:
		for (i = 0; i < mod_num; i++) {
			cur_mod = module[i];
			if (cur_mod != DISP_MODULE_UNKNOWN
				&& ddp_get_module_driver(cur_mod) != 0) {
				m = ddp_get_module_driver(cur_mod);
				if (m->trigger != 0)
					m->trigger(cur_mod, NULL);
			}
		}
		break;
	case DEINIT:
		for (i = 0; i < mod_num; i++) {
			cur_mod = module[i];
			if (cur_mod != DISP_MODULE_UNKNOWN
				&& ddp_get_module_driver(cur_mod) != 0) {
				m = ddp_get_module_driver(cur_mod);
				if (m->deinit != 0)
					m->deinit(cur_mod, NULL);
			}
		}
		break;
	default:
		break;
	}
	return 0;
}

int dvt_init_OVL_param(unsigned int mode)
{
	memset(&extd_ovl_params, 0, sizeof(extd_ovl_params));
	extd_ovl_params.p_golden_setting_context = get_golden_setting_pgc();
	memcpy(&temp_golden, extd_ovl_params.p_golden_setting_context,
			sizeof(temp_golden));
	if (mode == RDMA_MODE_DIRECT_LINK) {
		extd_ovl_params.ovl_layer_scanned = 0;
		extd_ovl_params.dst_dirty = 1;
		extd_ovl_params.dst_w =
			extd_dpi_params.dispif_config.dpi.width;
		extd_ovl_params.dst_h =
			extd_dpi_params.dispif_config.dpi.height;
		extd_ovl_params.ovl_dirty = 1;

		extd_ovl_params.ovl_config[0].layer_en = 1;
		extd_ovl_params.ovl_config[1].layer_en = 0;
		extd_ovl_params.ovl_config[2].layer_en = 0;
		extd_ovl_params.ovl_config[3].layer_en = 0;

		extd_ovl_params.ovl_config[0].source = OVL_LAYER_SOURCE_MEM;
		extd_ovl_params.ovl_config[0].layer = 0;
		extd_ovl_params.ovl_config[0].fmt = UFMT_BGR888;
		extd_ovl_params.ovl_config[0].addr = (unsigned long)data_mva;
		extd_ovl_params.ovl_config[0].vaddr = data_va;

		extd_ovl_params.ovl_config[0].src_x = 0;
		extd_ovl_params.ovl_config[0].src_y = 0;
		extd_ovl_params.ovl_config[0].src_w =
			extd_dpi_params.dispif_config.dpi.width;
		extd_ovl_params.ovl_config[0].src_h =
			extd_dpi_params.dispif_config.dpi.height;
		extd_ovl_params.ovl_config[0].src_pitch =
			extd_dpi_params.dispif_config.dpi.width * 3;

		extd_ovl_params.ovl_config[0].dst_x = 0;
		extd_ovl_params.ovl_config[0].dst_y = 0;
		extd_ovl_params.ovl_config[0].dst_w =
			extd_dpi_params.dispif_config.dpi.width;
		extd_ovl_params.ovl_config[0].dst_h =
			extd_dpi_params.dispif_config.dpi.height;
		extd_ovl_params.ovl_config[0].ext_sel_layer = -1;

		extd_ovl_params.ovl_config[0].isDirty = 1;

	}

	return 0;
}

int dvt_start_ovl1_2l_to_dpi(unsigned int resolution, unsigned int timeS)
{
	int ret = 0;
	struct ddp_io_golden_setting_arg gset_arg;

	DPI_DVT_LOG_W("top clock on\n");
	dvt_ddp_path_top_clock_on();


	DPI_DVT_LOG_W("set Mutex and connect path\n");
	dvt_connect_ovl1_2l_dpi(NULL);
	dvt_mutex_set_ovl1_2l_dpi(dvt_acquire_mutex(), NULL);


	DPI_DVT_LOG_W("module init\n");
	dvt_module_action(LONG_PATH1, INIT);
	DPI_DVT_LOG_W("module init done\n");

	DPI_DVT_LOG_W("module power on\n");
	dvt_module_action(LONG_PATH1, POWER_ON);

	DPI_DVT_LOG_W("module config\n");
	ddp_driver_dpi.config(DISP_MODULE_DPI, &extd_dpi_params, NULL);
	ret = dvt_allocate_buffer(resolution, M4U_FOR_OVL1_2L);
	if (ret < 0) {
		DDPERR("dvt_allocate_buffer error: ret: %d\n", ret);
		return ret;
	}

	dvt_init_RDMA_param(RDMA_MODE_DIRECT_LINK, resolution);
	ddp_driver_rdma.config(DISP_MODULE_RDMA1, &extd_rdma_params, NULL);

	dvt_init_OVL_param(RDMA_MODE_DIRECT_LINK);
	ddp_driver_ovl.config(DISP_MODULE_OVL1_2L, &extd_ovl_params, NULL);

	memset(&gset_arg, 0, sizeof(gset_arg));
	gset_arg.dst_mod_type = DST_MOD_REAL_TIME;
	gset_arg.is_decouple_mode = 0;
	ddp_driver_ovl.ioctl(DISP_MODULE_OVL1_2L, NULL,
			DDP_OVL_GOLDEN_SETTING, &gset_arg);

	DPI_DVT_LOG_W("module reset\n");
	dvt_mutex_reset(NULL, dvt_acquire_mutex());
	dvt_module_action(LONG_PATH1, RESET);

	DPI_DVT_LOG_W("module start\n");
	dvt_module_action(LONG_PATH1, START);

	DPI_DVT_LOG_W("module trigger\n");
	dvt_mutex_enable(NULL, dvt_acquire_mutex());
	ddp_driver_dpi.trigger(DISP_MODULE_DPI, NULL);

	/* after trigger, we should get&release mutex */
	/* DISP_REG_SET(NULL, DISP_REG_CONFIG_MUTEX_GET(1), 1); */
	/* DISP_REG_SET(NULL, DISP_REG_CONFIG_MUTEX_GET(1), 0); */

	hdmi_drv->video_config(resolution, 0, 0);

	DPI_DVT_LOG_W("module dump_info\n");
	ddp_driver_dpi.dump_info(DISP_MODULE_DPI, 1);
	ddp_dump_analysis(DISP_MODULE_OVL1_2L);
	ddp_dump_reg(DISP_MODULE_OVL1_2L);
	ddp_dump_analysis(DISP_MODULE_RDMA1);
	ddp_dump_reg(DISP_MODULE_RDMA1);
	ddp_dump_analysis(DISP_MODULE_CONFIG);
	ddp_dump_reg(DISP_MODULE_CONFIG);
	ddp_dump_analysis(DISP_MODULE_MUTEX);
	ddp_dump_reg(DISP_MODULE_MUTEX);


	msleep(timeS * 1000);

	DPI_DVT_LOG_W("module stop\n");
	dvt_module_action(LONG_PATH1, STOP);
	dvt_mutex_disenable(NULL, dvt_acquire_mutex());


	DPI_DVT_LOG_W("module reset\n");
	dvt_mutex_reset(NULL, dvt_acquire_mutex());
	dvt_module_action(LONG_PATH1, RESET);

	DPI_DVT_LOG_W("module power off\n");
	dvt_module_action(LONG_PATH1, POWER_OFF);

	DPI_DVT_LOG_W("set Mutex and disconnect path\n");
	dvt_disconnect_ovl1_2l_dpi(NULL);
	dvt_mutex_set(dvt_acquire_mutex(), NULL);


	DPI_DVT_LOG_W("top clock off\n");
	dvt_ddp_path_top_clock_off();
	memcpy(extd_rdma_params.p_golden_setting_context, &temp_golden,
			sizeof(temp_golden));

	if (data_va)
		vfree((void *)data_va);

	return 0;
}


int dpi_dvt_show_pattern(unsigned int pattern, unsigned int timeS)
{
	int val = (int)pattern;

	DPI_DVT_LOG_W("module init\n");
	ddp_driver_dpi.init(DISP_MODULE_DPI, NULL);

	DPI_DVT_LOG_W("module power on\n");
	ddp_driver_dpi.power_on(DISP_MODULE_DPI, NULL);

	ddp_driver_dpi.reset(DISP_MODULE_DPI, NULL);
	ddp_driver_dpi.config(DISP_MODULE_DPI, &extd_dpi_params, NULL);

	DISP_REG_SET_FIELD(NULL, REG_FLD(3, 4), DISPSYS_DPI_BASE + 0xF00, val);
	DISP_REG_SET_FIELD(NULL, REG_FLD(1, 0), DISPSYS_DPI_BASE + 0xF00, 1);

	if (pattern == 5) {
		DISP_REG_SET_FIELD(NULL, REG_FLD(8, 8),
				DISPSYS_DPI_BASE + 0xF00, 0xca);
		DISP_REG_SET_FIELD(NULL, REG_FLD(8, 16),
				DISPSYS_DPI_BASE + 0xF00, 0xbc);
		DISP_REG_SET_FIELD(NULL, REG_FLD(8, 24),
				DISPSYS_DPI_BASE + 0xF00, 0x3e);
	}

	if (timeS == 100)
		enableRGB2YUV(acsYCbCr444);
	else if (timeS == 99) {
		configDpiRepetition();
		configInterlaceMode(HDMI_VIDEO_720x480i_60Hz);
	}

	ddp_driver_dpi.start(DISP_MODULE_DPI, NULL);
	ddp_driver_dpi.trigger(DISP_MODULE_DPI, NULL);
	msleep(50);

	hdmi_drv->video_config(DPI_DVT_Context.output_video_resolution, 0, 0);

	ddp_driver_dpi.dump_info(DISP_MODULE_DPI, 1);

	msleep(timeS * 1000);

	DPI_DisableColorBar();
	DPI_DVT_LOG_W("module stop\n");
	ddp_driver_dpi.stop(DISP_MODULE_DPI, NULL);
	ddp_driver_dpi.reset(DISP_MODULE_DPI, NULL);

	DPI_DVT_LOG_W("module power Off\n");
	ddp_driver_dpi.power_off(DISP_MODULE_DPI, NULL);

	return 0;
}

#endif				/*#ifdef DPI_DVT_TEST_SUPPORT */

int dvt_init_WDMA_param(unsigned int resolution)
{
	memset(&extd_rdma_params, 0, sizeof(extd_rdma_params));
	extd_rdma_params.p_golden_setting_context = get_golden_setting_pgc();
	memcpy(&temp_golden, extd_rdma_params.p_golden_setting_context,
			sizeof(temp_golden));

	extd_rdma_params.dst_dirty = 1;
	extd_rdma_params.wdma_dirty = 1;
	extd_rdma_params.dst_w = 1920;
	extd_rdma_params.dst_h = 1080;
	extd_rdma_params.wdma_config.dstAddress = (unsigned long)out_mva;
	extd_rdma_params.wdma_config.outputFormat = UFMT_RGB888;
	extd_rdma_params.wdma_config.dstPitch = 1920*4;
	extd_rdma_params.wdma_config.srcHeight = 1080;
	extd_rdma_params.wdma_config.srcWidth = 1920;
	extd_rdma_params.wdma_config.clipX = 0;
	extd_rdma_params.wdma_config.clipY = 0;
	extd_rdma_params.wdma_config.clipHeight = 1080;
	extd_rdma_params.wdma_config.clipWidth = 1920;
	extd_rdma_params.wdma_config.security = 0;
	extd_rdma_params.wdma_config.alpha = 0xff;
	extd_rdma_params.wdma_config.useSpecifiedAlpha = 0;

	return 0;
}

int dvt_init_RDMA_param(unsigned int mode, unsigned int resolution)
{
	memset(&extd_rdma_params, 0, sizeof(extd_rdma_params));
	extd_rdma_params.p_golden_setting_context = get_golden_setting_pgc();
	memcpy(&temp_golden, extd_rdma_params.p_golden_setting_context,
			sizeof(temp_golden));
	if (mode == RDMA_MODE_DIRECT_LINK) {
		DPI_DVT_LOG_W("RDMA_MODE_DIRECT_LINK configure\n");
		extd_rdma_params.dst_dirty = 1;
		extd_rdma_params.rdma_dirty = 1;
		extd_rdma_params.dst_w =
			extd_dpi_params.dispif_config.dpi.width;
		extd_rdma_params.dst_h =
			extd_dpi_params.dispif_config.dpi.height;
		extd_rdma_params.rdma_config.address = 0;
		extd_rdma_params.rdma_config.inputFormat = 0;
		extd_rdma_params.rdma_config.pitch = 0;
		extd_rdma_params.rdma_config.dst_x = 0;
		extd_rdma_params.rdma_config.dst_y = 0;
		extd_rdma_params.lcm_bpp = 24;


	} else if (mode == RDMA_MODE_MEMORY) {
		DPI_DVT_LOG_W("RDMA_MODE_MEMORY configure\n");
		extd_rdma_params.dst_dirty = 0;
		extd_rdma_params.rdma_dirty = 1;
		extd_rdma_params.dst_w =
			extd_dpi_params.dispif_config.dpi.width;
		extd_rdma_params.dst_h =
			extd_dpi_params.dispif_config.dpi.height;
		extd_rdma_params.rdma_config.address = (unsigned long)data_mva;
		extd_rdma_params.rdma_config.inputFormat = UFMT_BGR888;
		extd_rdma_params.rdma_config.pitch =
			extd_dpi_params.dispif_config.dpi.width * 3;
		extd_rdma_params.rdma_config.width =
			extd_dpi_params.dispif_config.dpi.width;
		extd_rdma_params.rdma_config.height =
			extd_dpi_params.dispif_config.dpi.height;
		extd_rdma_params.rdma_config.dst_x = 0;
		extd_rdma_params.rdma_config.dst_y = 0;
		extd_rdma_params.rdma_config.dst_w =
			extd_dpi_params.dispif_config.dpi.width;
		extd_rdma_params.rdma_config.dst_h =
			extd_dpi_params.dispif_config.dpi.height;
		extd_rdma_params.lcm_bpp = 24;

	}

	switch (resolution) {
	case HDMI_VIDEO_720x480p_60Hz:
	case HDMI_VIDEO_720x480i_60Hz:
		extd_rdma_params.p_golden_setting_context->ext_dst_height = 480;
		extd_rdma_params.p_golden_setting_context->ext_dst_width = 720;
		extd_rdma_params.p_golden_setting_context->dst_height = 480;
		extd_rdma_params.p_golden_setting_context->dst_width = 720;
		break;
	case HDMI_VIDEO_1280x720p_60Hz:
		extd_rdma_params.p_golden_setting_context->ext_dst_height = 720;
		extd_rdma_params.p_golden_setting_context->ext_dst_width = 1280;
		extd_rdma_params.p_golden_setting_context->dst_height = 720;
		extd_rdma_params.p_golden_setting_context->dst_width = 1280;
		break;
	case HDMI_VIDEO_1920x1080p_30Hz:
	case HDMI_VIDEO_1920x1080p_60Hz:
	case HDMI_VIDEO_1920x1080i_60Hz:
		extd_rdma_params.p_golden_setting_context->ext_dst_height =
			1080;
		extd_rdma_params.p_golden_setting_context->ext_dst_width =
			1920;
		extd_rdma_params.p_golden_setting_context->dst_height = 1080;
		extd_rdma_params.p_golden_setting_context->dst_width = 1920;
		break;
	case HDMI_VIDEO_2160p_DSC_24Hz:
	case HDMI_VIDEO_2160p_DSC_30Hz:
		extd_rdma_params.p_golden_setting_context->ext_dst_height =
			2160;
		extd_rdma_params.p_golden_setting_context->ext_dst_width =
			3840;
		extd_rdma_params.p_golden_setting_context->dst_height = 2160;
		extd_rdma_params.p_golden_setting_context->dst_width = 3840;
		break;
	default:
		break;
	}

	return 0;
}

void dpi_dvt_parameters(unsigned char arg)
{
	enum DPI_POLARITY clk_pol = DPI_POLARITY_RISING,
			  de_pol = DPI_POLARITY_RISING,
			  hsync_pol = DPI_POLARITY_RISING,
			  vsync_pol = DPI_POLARITY_RISING;
	unsigned int dpi_clock = 0;
	unsigned int dpi_clk_div = 0, hsync_pulse_width = 0,
		     hsync_back_porch = 0, hsync_front_porch = 0,
		     vsync_pulse_width = 0, vsync_back_porch = 0,
		     vsync_front_porch = 0;

	DPI_DVT_Context.scaling_factor = 0;
	switch (arg) {
	case HDMI_VIDEO_720x480p_60Hz:
		{
#if defined(MHL_SII8338) || defined(MHL_SII8348)
			clk_pol = HDMI_POLARITY_FALLING;
#else
			clk_pol = HDMI_POLARITY_RISING;
#endif
			de_pol = HDMI_POLARITY_RISING;
			hsync_pol = HDMI_POLARITY_RISING;
			vsync_pol = HDMI_POLARITY_RISING;

			dpi_clk_div = 2;

			hsync_pulse_width = 62;
			hsync_back_porch = 60;
			hsync_front_porch = 16;

			vsync_pulse_width = 6;
			vsync_back_porch = 30;
			vsync_front_porch = 9;

			DPI_DVT_Context.bg_height =
				((480 * DPI_DVT_Context.scaling_factor) /
				 100 >> 2) << 2;
			DPI_DVT_Context.bg_width =
				((720 * DPI_DVT_Context.scaling_factor) /
				 100 >> 2) << 2;
			DPI_DVT_Context.hdmi_width =
				720 - DPI_DVT_Context.bg_width;
			DPI_DVT_Context.hdmi_height =
				480 - DPI_DVT_Context.bg_height;
			DPI_DVT_Context.output_video_resolution =
				HDMI_VIDEO_720x480p_60Hz;
			dpi_clock = HDMI_VIDEO_720x480p_60Hz;
			break;
		}
	case HDMI_VIDEO_720x480i_60Hz:
		{
#if defined(MHL_SII8338) || defined(MHL_SII8348)
			clk_pol = HDMI_POLARITY_FALLING;
#else
			clk_pol = HDMI_POLARITY_RISING;
#endif
			de_pol = HDMI_POLARITY_RISING;
			hsync_pol = HDMI_POLARITY_RISING;
			vsync_pol = HDMI_POLARITY_RISING;

			dpi_clk_div = 2;

			hsync_pulse_width = 124 / 2;
			hsync_back_porch = 114 / 2;
			hsync_front_porch = 38 / 2;

			vsync_pulse_width = 6;
			vsync_back_porch = 30;
			vsync_front_porch = 9;

			DPI_DVT_Context.bg_height =
				((480 * DPI_DVT_Context.scaling_factor) /
				 100 >> 2) << 2;
			DPI_DVT_Context.bg_width =
				((1440 / 2 * DPI_DVT_Context.scaling_factor) /
				 100 >> 2) << 2;
			DPI_DVT_Context.hdmi_width =
				1440 / 2 - DPI_DVT_Context.bg_width;
			DPI_DVT_Context.hdmi_height =
				480 - DPI_DVT_Context.bg_height;
			DPI_DVT_Context.output_video_resolution =
				HDMI_VIDEO_720x480i_60Hz;
			dpi_clock = 27027;
			break;
		}
	case HDMI_VIDEO_1280x720p_60Hz:
		{
			clk_pol = HDMI_POLARITY_RISING;
			de_pol = HDMI_POLARITY_RISING;
#if defined(HDMI_TDA19989)
			hsync_pol = HDMI_POLARITY_FALLING;
#else
			hsync_pol = HDMI_POLARITY_FALLING;
#endif
			vsync_pol = HDMI_POLARITY_FALLING;

#if defined(MHL_SII8338) || defined(MHL_SII8348)
			clk_pol = HDMI_POLARITY_FALLING;
			de_pol = HDMI_POLARITY_RISING;
			hsync_pol = HDMI_POLARITY_FALLING;
			vsync_pol = HDMI_POLARITY_FALLING;
#endif
			dpi_clk_div = 2;

			hsync_pulse_width = 40;
			hsync_back_porch = 220;
			hsync_front_porch = 110;

			vsync_pulse_width = 5;
			vsync_back_porch = 20;
			vsync_front_porch = 5;
			dpi_clock = HDMI_VIDEO_1280x720p_60Hz;

			DPI_DVT_Context.bg_height =
				((720 * DPI_DVT_Context.scaling_factor) /
				 100 >> 2) << 2;
			DPI_DVT_Context.bg_width =
				((1280 * DPI_DVT_Context.scaling_factor) /
				 100 >> 2) << 2;
			DPI_DVT_Context.hdmi_width =
				1280 - DPI_DVT_Context.bg_width;
			DPI_DVT_Context.hdmi_height =
				720 - DPI_DVT_Context.bg_height;

			DPI_DVT_Context.output_video_resolution =
				HDMI_VIDEO_1280x720p_60Hz;
			break;
		}
	case HDMI_VIDEO_1920x1080p_30Hz:
		{
#if defined(MHL_SII8338) || defined(MHL_SII8348)
			clk_pol = HDMI_POLARITY_FALLING;
			de_pol = HDMI_POLARITY_RISING;
			hsync_pol = HDMI_POLARITY_FALLING;
			vsync_pol = HDMI_POLARITY_FALLING;
#else
			clk_pol = HDMI_POLARITY_RISING;
			de_pol = HDMI_POLARITY_RISING;
			hsync_pol = HDMI_POLARITY_FALLING;
			vsync_pol = HDMI_POLARITY_FALLING;
#endif
			dpi_clk_div = 2;

			hsync_pulse_width = 44;
			hsync_back_porch = 148;
			hsync_front_porch = 88;

			vsync_pulse_width = 5;
			vsync_back_porch = 36;
			vsync_front_porch = 4;

			DPI_DVT_Context.bg_height =
				((1080 * DPI_DVT_Context.scaling_factor) /
				 100 >> 2) << 2;
			DPI_DVT_Context.bg_width =
				((1920 * DPI_DVT_Context.scaling_factor) /
				 100 >> 2) << 2;
			DPI_DVT_Context.hdmi_width =
				1920 - DPI_DVT_Context.bg_width;
			DPI_DVT_Context.hdmi_height =
				1080 - DPI_DVT_Context.bg_height;

			DPI_DVT_Context.output_video_resolution =
				HDMI_VIDEO_1920x1080p_30Hz;
			dpi_clock = HDMI_VIDEO_1920x1080p_30Hz;
			break;
		}
	case HDMI_VIDEO_1920x1080p_60Hz:
		{
#if defined(MHL_SII8338) || defined(MHL_SII8348)
			clk_pol = HDMI_POLARITY_FALLING;
			de_pol = HDMI_POLARITY_RISING;
			hsync_pol = HDMI_POLARITY_FALLING;
			vsync_pol = HDMI_POLARITY_FALLING;
#else
			clk_pol = HDMI_POLARITY_RISING;
			de_pol = HDMI_POLARITY_RISING;
			hsync_pol = HDMI_POLARITY_FALLING;
			vsync_pol = HDMI_POLARITY_FALLING;
#endif
			dpi_clk_div = 2;

			hsync_pulse_width = 44;
			hsync_back_porch = 148;
			hsync_front_porch = 88;

			vsync_pulse_width = 5;
			vsync_back_porch = 36;
			vsync_front_porch = 4;

			DPI_DVT_Context.bg_height =
				((1080 * DPI_DVT_Context.scaling_factor) /
				 100 >> 2) << 2;
			DPI_DVT_Context.bg_width =
				((1920 * DPI_DVT_Context.scaling_factor) /
				 100 >> 2) << 2;
			DPI_DVT_Context.hdmi_width =
				1920 - DPI_DVT_Context.bg_width;
			DPI_DVT_Context.hdmi_height =
				1080 - DPI_DVT_Context.bg_height;

			DPI_DVT_Context.output_video_resolution =
				HDMI_VIDEO_1920x1080p_60Hz;
			dpi_clock = HDMI_VIDEO_1920x1080p_60Hz;
			break;
		}
	case HDMI_VIDEO_1920x1080i_60Hz:
		{
#if defined(MHL_SII8338) || defined(MHL_SII8348)
			clk_pol = HDMI_POLARITY_FALLING;
			de_pol = HDMI_POLARITY_RISING;
			hsync_pol = HDMI_POLARITY_FALLING;
			vsync_pol = HDMI_POLARITY_FALLING;
#else
			clk_pol = HDMI_POLARITY_RISING;
			de_pol = HDMI_POLARITY_RISING;
			hsync_pol = HDMI_POLARITY_FALLING;
			vsync_pol = HDMI_POLARITY_FALLING;
#endif
			dpi_clk_div = 2;

			hsync_pulse_width = 44;
			hsync_back_porch = 148;
			hsync_front_porch = 88;

			vsync_pulse_width = 5;
			vsync_back_porch = 36;
			vsync_front_porch = 4;

			DPI_DVT_Context.bg_height =
				((1080 * DPI_DVT_Context.scaling_factor) /
				 100 >> 2) << 2;
			DPI_DVT_Context.bg_width =
				((1920 * DPI_DVT_Context.scaling_factor) /
				 100 >> 2) << 2;
			DPI_DVT_Context.hdmi_width =
				1920 - DPI_DVT_Context.bg_width;
			DPI_DVT_Context.hdmi_height =
				1080 - DPI_DVT_Context.bg_height;

			DPI_DVT_Context.output_video_resolution =
				HDMI_VIDEO_1920x1080i_60Hz;
			dpi_clock = 74250;
			break;
		}

	case HDMI_VIDEO_2160p_DSC_24Hz:
		{
#if defined(MHL_SII8338) || defined(MHL_SII8348)
			clk_pol = HDMI_POLARITY_FALLING;
			de_pol = HDMI_POLARITY_RISING;
			hsync_pol = HDMI_POLARITY_FALLING;
			vsync_pol = HDMI_POLARITY_FALLING;
#else
			clk_pol = HDMI_POLARITY_RISING;
			de_pol = HDMI_POLARITY_RISING;
			hsync_pol = HDMI_POLARITY_FALLING;
			vsync_pol = HDMI_POLARITY_FALLING;
#endif
			hsync_pulse_width = 30;
			hsync_back_porch = 98;
			hsync_front_porch = 242;

			vsync_pulse_width = 10;
			vsync_back_porch = 72;
			vsync_front_porch = 8;

			DPI_DVT_Context.bg_height = 0;
			DPI_DVT_Context.bg_width = 0;
			DPI_DVT_Context.hdmi_width = 1280;
			DPI_DVT_Context.hdmi_height = 2160;

			DPI_DVT_Context.output_video_resolution =
				HDMI_VIDEO_2160p_DSC_24Hz;
			dpi_clock = 89100;

			break;
		}
		case HDMI_VIDEO_2160p_DSC_30Hz:
		{
#if defined(MHL_SII8338) || defined(MHL_SII8348_)
/* #if defined(MHL_SII8338) || defined(MHL_SII8348) */
			clk_pol = HDMI_POLARITY_FALLING;
			de_pol = HDMI_POLARITY_RISING;
			hsync_pol = HDMI_POLARITY_FALLING;
			vsync_pol = HDMI_POLARITY_FALLING;
#else
			clk_pol = HDMI_POLARITY_RISING;
			de_pol = HDMI_POLARITY_RISING;
			hsync_pol = HDMI_POLARITY_FALLING;
			vsync_pol = HDMI_POLARITY_FALLING;
#endif
			hsync_pulse_width = 30;
			hsync_back_porch = 98;
			hsync_front_porch = 66;

			vsync_pulse_width = 10;
			vsync_back_porch = 72;
			vsync_front_porch = 8;

			DPI_DVT_Context.bg_height = 0;
			DPI_DVT_Context.bg_width = 0;
			DPI_DVT_Context.hdmi_width = 1280;
			DPI_DVT_Context.hdmi_height = 2160;

			DPI_DVT_Context.output_video_resolution =
				HDMI_VIDEO_2160p_DSC_30Hz;
			dpi_clock = 99500;

			break;
		}

	default:
		break;
	}

	extd_dpi_params.dispif_config.dpi.width = DPI_DVT_Context.hdmi_width;
	extd_dpi_params.dispif_config.dpi.height = DPI_DVT_Context.hdmi_height;
	extd_dpi_params.dispif_config.dpi.bg_width = DPI_DVT_Context.bg_width;
	extd_dpi_params.dispif_config.dpi.bg_height = DPI_DVT_Context.bg_height;

	extd_dpi_params.dispif_config.dpi.clk_pol = clk_pol;
	extd_dpi_params.dispif_config.dpi.de_pol = de_pol;
	extd_dpi_params.dispif_config.dpi.vsync_pol = vsync_pol;
	extd_dpi_params.dispif_config.dpi.hsync_pol = hsync_pol;

	extd_dpi_params.dispif_config.dpi.hsync_pulse_width = hsync_pulse_width;
	extd_dpi_params.dispif_config.dpi.hsync_back_porch = hsync_back_porch;
	extd_dpi_params.dispif_config.dpi.hsync_front_porch = hsync_front_porch;
	extd_dpi_params.dispif_config.dpi.vsync_pulse_width = vsync_pulse_width;
	extd_dpi_params.dispif_config.dpi.vsync_back_porch = vsync_back_porch;
	extd_dpi_params.dispif_config.dpi.vsync_front_porch = vsync_front_porch;

	extd_dpi_params.dispif_config.dpi.format = 0;
	extd_dpi_params.dispif_config.dpi.rgb_order = 0;
	extd_dpi_params.dispif_config.dpi.i2x_en = true;
	extd_dpi_params.dispif_config.dpi.i2x_edge = 2;
	extd_dpi_params.dispif_config.dpi.embsync = false;
	extd_dpi_params.dispif_config.dpi.dpi_clock = dpi_clock;

	DPI_DVT_LOG_W("%s:%d\n", __func__, arg);
}

/***************************Basic Function End*********************************/

void dvt_dump_ext_dpi_parameters(void)
{
	DPI_DVT_LOG_W("dispif_config.dpi.width:	%d\n",
		      extd_dpi_params.dispif_config.dpi.width);
	DPI_DVT_LOG_W("dispif_config.dpi.height:%d\n",
		      extd_dpi_params.dispif_config.dpi.height);
	DPI_DVT_LOG_W("dispif_config.dpi.bg_width:%d\n",
		      extd_dpi_params.dispif_config.dpi.bg_width);
	DPI_DVT_LOG_W("dispif_config.dpi.bg_height:%d\n",
		      extd_dpi_params.dispif_config.dpi.bg_height);

	DPI_DVT_LOG_W("dispif_config.dpi.clk_pol:%d\n",
		      extd_dpi_params.dispif_config.dpi.clk_pol);
	DPI_DVT_LOG_W("dispif_config.dpi.de_pol:%d\n",
		      extd_dpi_params.dispif_config.dpi.de_pol);
	DPI_DVT_LOG_W("dispif_config.dpi.vsync_pol:%d\n",
		      extd_dpi_params.dispif_config.dpi.vsync_pol);
	DPI_DVT_LOG_W("dispif_config.dpi.hsync_pol:%d\n",
		      extd_dpi_params.dispif_config.dpi.hsync_pol);

	DPI_DVT_LOG_W("dispif_config.dpi.hsync_pulse_width:%d\n",
		      extd_dpi_params.dispif_config.dpi.hsync_pulse_width);
	DPI_DVT_LOG_W("dispif_config.dpi.hsync_back_porch:%d\n",
		      extd_dpi_params.dispif_config.dpi.hsync_back_porch);
	DPI_DVT_LOG_W("dispif_config.dpi.hsync_front_porch:%d\n",
		      extd_dpi_params.dispif_config.dpi.hsync_front_porch);
	DPI_DVT_LOG_W("dispif_config.dpi.vsync_pulse_width:%d\n",
		      extd_dpi_params.dispif_config.dpi.vsync_pulse_width);
	DPI_DVT_LOG_W("dispif_config.dpi.vsync_back_porch:%d\n",
		      extd_dpi_params.dispif_config.dpi.vsync_back_porch);
	DPI_DVT_LOG_W("dispif_config.dpi.vsync_front_porch:%d\n",
		      extd_dpi_params.dispif_config.dpi.vsync_front_porch);

	DPI_DVT_LOG_W("dispif_config.dpi.format:%d\n",
		      extd_dpi_params.dispif_config.dpi.format);
	DPI_DVT_LOG_W("dispif_config.dpi.rgb_order:%d\n",
		      extd_dpi_params.dispif_config.dpi.rgb_order);
	DPI_DVT_LOG_W("dispif_config.dpi.i2x_en:%d\n",
		      extd_dpi_params.dispif_config.dpi.i2x_en);
	DPI_DVT_LOG_W("dispif_config.dpi.i2x_edge:%d\n",
		      extd_dpi_params.dispif_config.dpi.i2x_edge);
	DPI_DVT_LOG_W("dispif_config.dpi.embsync:%d\n",
		      extd_dpi_params.dispif_config.dpi.embsync);
	DPI_DVT_LOG_W("dispif_config.dpi.dpi_clock:%d\n",
		      extd_dpi_params.dispif_config.dpi.dpi_clock);
}

int dvt_copy_file_data(void *ptr, unsigned int resolution)
{
	struct file *fd = NULL;
	mm_segment_t fs;
	loff_t pos = 54;
/* int i; */
/* char *fill = NULL; */

	DPI_DVT_LOG_W("%s, resolution: %d\n", __func__, resolution);

	if (resolution == HDMI_VIDEO_1280x720p_60Hz) {
		fs = get_fs();
		set_fs(KERNEL_DS);
		fd = filp_open("/data/Gene_1280x720.bmp", O_RDONLY, 0);
		if (IS_ERR(fd)) {
			DPI_DVT_LOG_W("open Gene_1280x720.bmp fail! ret%ld\n",
					PTR_ERR(fd));
			set_fs(fs);
			return -1;
			}
		vfs_read(fd, ptr, 2764800, &pos);
		filp_close(fd, NULL);
		fd = NULL;
		set_fs(fs);
	} else if (resolution == HDMI_VIDEO_720x480p_60Hz) {
		fs = get_fs();
		set_fs(KERNEL_DS);
		fd = filp_open("/data/PDA0026_720x480.bmp", O_RDONLY, 0);
		if (IS_ERR(fd)) {
			DPI_DVT_LOG_W("open PDA0026_720x480.bmp fail!ret%ld\n",
					PTR_ERR(fd));
			set_fs(fs);
			return -1;
			}
		vfs_read(fd, ptr, 1036800, &pos);
		filp_close(fd, NULL);
		fd = NULL;
		set_fs(fs);
	} else if (resolution == HDMI_VIDEO_1920x1080p_30Hz ||
			resolution == HDMI_VIDEO_1920x1080p_60Hz) {
		fs = get_fs();
		set_fs(KERNEL_DS);
		fd = filp_open("/data/Venice_1920x1080.bmp", O_RDONLY, 0);
		if (IS_ERR(fd)) {
			DPI_DVT_LOG_W("open Venice_1920x1080.bmp fail!ret%ld\n",
					PTR_ERR(fd));
			set_fs(fs);
			return -1;
		}
		vfs_read(fd, ptr, 6220800, &pos);
		filp_close(fd, NULL);
		fd = NULL;
		set_fs(fs);
	} else if (resolution == HDMI_VIDEO_2160p_DSC_24Hz ||
			resolution == HDMI_VIDEO_2160p_DSC_30Hz) {

		fs = get_fs();
		set_fs(KERNEL_DS);
		fd = filp_open("/data/picture_4k.bmp", O_RDONLY, 0);
		if (IS_ERR(fd)) {
			DPI_DVT_LOG_W("open picture_4k.bmp fail! ret%ld\n",
					PTR_ERR(fd));
			set_fs(fs);
			return -1;
		}
		vfs_read(fd, ptr, 24883200, &pos);
		filp_close(fd, NULL);
		fd = NULL;
		set_fs(fs);
	}
	return 0;
}

int dvt_allocate_buffer(unsigned int resolution, enum HW_MODULE_Type hw_type)
{
	int ret = 0;
	M4U_PORT_STRUCT m4uport;
	m4u_client_t *client = NULL;
	int M4U_PORT = M4U_PORT_UNKNOWN;

	unsigned int hdmiPixelSize = 0;
	unsigned int hdmiDataSize = 0;
	unsigned int hdmiBufferSize = 0;

	if (resolution == HDMI_VIDEO_2160p_DSC_24Hz ||
			resolution == HDMI_VIDEO_2160p_DSC_30Hz) {
		hdmiPixelSize =
			extd_dpi_params.dispif_config.dpi.width * 3 *
			extd_dpi_params.dispif_config.dpi.height;

		hdmiDataSize = hdmiPixelSize * 3;
		hdmiBufferSize = hdmiDataSize;

		DPI_DVT_LOG_W("%s, width: %d, height: %d\n", __func__,
				extd_dpi_params.dispif_config.dpi.width * 3,
				extd_dpi_params.dispif_config.dpi.height);
	} else {
		hdmiPixelSize =
			extd_dpi_params.dispif_config.dpi.width *
			extd_dpi_params.dispif_config.dpi.height;
		hdmiDataSize = hdmiPixelSize * 3;
		hdmiBufferSize = hdmiDataSize;

		DPI_DVT_LOG_W("%s, width: %d, height: %d\n", __func__,
				extd_dpi_params.dispif_config.dpi.width,
				extd_dpi_params.dispif_config.dpi.height);
	}

	data_va = (unsigned long int) vmalloc(hdmiBufferSize);
	if (((void *)data_va) == NULL) {
		DDPERR("vmalloc %d bytes fail\n", hdmiBufferSize);
		return -1;
	}
	memset((void *)data_va, 0, hdmiBufferSize);
	DPI_DVT_LOG_W("resolution %d\n", resolution);
	ret = dvt_copy_file_data((void *)data_va, resolution);

	client = m4u_create_client();
	if (IS_ERR_OR_NULL(client)) {
		DDPERR("create client fail!\n");
		return -1;
	}

	if (hw_type == M4U_FOR_RDMA1)
		M4U_PORT = M4U_PORT_DISP_RDMA1;
#ifdef DPI_DVT_TEST_SUPPORT
	else if (hw_type == M4U_FOR_OVL1_2L)
		M4U_PORT = M4U_PORT_DISP_2L_OVL1_LARB0;
#endif

	DPI_DVT_LOG_W("data_va %lx , client %p\n", data_va, client);

	memset((void *)&m4uport, 0, sizeof(M4U_PORT_STRUCT));
	m4uport.ePortID = M4U_PORT;
	m4uport.Virtuality = 1;
	m4uport.domain = 0;
	m4uport.Security = 0;
	m4uport.Distance = 1;
	m4uport.Direction = 0;
	m4u_config_port(&m4uport);

	m4u_alloc_mva(client, M4U_PORT, (unsigned long int)data_va,
			NULL, hdmiBufferSize,
			M4U_PROT_READ | M4U_PROT_WRITE, 0, &data_mva);
	DPI_DVT_LOG_W("data_mva %x\n", data_mva);

#if 0
	memset((void *)&m4uport, 0, sizeof(M4U_PORT_STRUCT));
	m4uport.ePortID = M4U_PORT_DISP_2L_OVL1_LARB0;
	m4uport.Virtuality = 1;
	m4uport.domain = 0;
	m4uport.Security = 0;
	m4uport.Distance = 1;
	m4uport.Direction = 0;
	m4u_config_port(&m4uport);
#endif
	return ret;
}


int dvt_start_rdma1_to_dpi(unsigned int resolution, unsigned int timeS,
		unsigned int caseNum)
{
	int ret = 0;
	int i;

	DPI_DVT_LOG_W("top clock on\n");
	dvt_ddp_path_top_clock_on();

	DPI_DVT_LOG_W("set Mutex and connect path\n");
	dvt_connect_path(NULL);

	dvt_mutex_set(dvt_acquire_mutex(), NULL);

	DPI_DVT_LOG_W("module init\n");
	dvt_module_action(SHORT_PATH1, INIT);


	DPI_DVT_LOG_W("module power on\n");
	dvt_module_action(SHORT_PATH1, POWER_ON);

	switch (caseNum) {
	case 1:
		ret = dvt_allocate_buffer(resolution, M4U_FOR_RDMA1);
		dvt_init_RDMA_param(RDMA_MODE_MEMORY, resolution);
		/* RGB Swap: last parameter 0--RGB/1--BGR */
		DISP_REG_SET_FIELD(NULL, REG_FLD(1, 1),
				DISPSYS_DPI_BASE + 0x010, 1);
		break;
	case 2:
		ret = dvt_allocate_buffer(HDMI_VIDEO_720x480p_60Hz,
				M4U_FOR_RDMA1);
		dvt_init_RDMA_param(RDMA_MODE_MEMORY, HDMI_VIDEO_720x480p_60Hz);
		/* set background left right top bottom, and enable background*/
		DISP_REG_SET_FIELD(NULL, REG_FLD(1, 0),
				DISPSYS_DPI_BASE + 0x010, 1);
		DISP_REG_SET_FIELD(NULL, REG_FLD(13, 16),
				DISPSYS_DPI_BASE + 0x030, 280);
		DISP_REG_SET_FIELD(NULL, REG_FLD(13, 0),
				DISPSYS_DPI_BASE + 0x030, 280);
		DISP_REG_SET_FIELD(NULL, REG_FLD(13, 16),
				DISPSYS_DPI_BASE + 0x034, 120);
		DISP_REG_SET_FIELD(NULL, REG_FLD(13, 0),
				DISPSYS_DPI_BASE + 0x034, 120);
		DISP_REG_SET_FIELD(NULL, REG_FLD(24, 0),
				DISPSYS_DPI_BASE + 0x038, 0x3ebcca);
		break;
	case 4:
		ret = dvt_allocate_buffer(resolution, M4U_FOR_RDMA1);
		dvt_init_RDMA_param(RDMA_MODE_MEMORY, resolution);
		break;
	case 0xA:
		ret = dvt_allocate_buffer(resolution, M4U_FOR_RDMA1);
		dvt_init_RDMA_param(RDMA_MODE_MEMORY, resolution);
		cmdqRecWait(checksum_cmdq, CMDQ_EVENT_MUTEX1_STREAM_EOF);
		cmdqRecWrite(checksum_cmdq, DPI_PHY_ADDR + 0x048,
				0x80000000, 0x80000000);
		cmdqRecWait(checksum_cmdq, CMDQ_EVENT_MUTEX1_STREAM_EOF);
		cmdqRecPoll(checksum_cmdq, DPI_PHY_ADDR + 0x048,
				0x40000000, 0x40000000);
		cmdqRecBackupRegisterToSlot(checksum_cmdq, checksum_record, 0,
				DPI_PHY_ADDR + 0x048);

		for (i = 1; i < 3; i++) {
			cmdqRecWrite(checksum_cmdq, DPI_PHY_ADDR + 0x048,
					0x80000000, 0x80000000);
			cmdqRecWait(checksum_cmdq,
					CMDQ_EVENT_MUTEX1_STREAM_EOF);
			cmdqRecPoll(checksum_cmdq, DPI_PHY_ADDR +
					0x048, 0x40000000, 0x40000000);
			cmdqRecBackupRegisterToSlot(checksum_cmdq,
					checksum_record, i,
					DPI_PHY_ADDR + 0x048);
		}

		break;
	default:
		break;
	}
	DPI_DVT_LOG_W("module config\n");
	ddp_driver_dpi.config(DISP_MODULE_DPI, &extd_dpi_params, NULL);
	ddp_driver_rdma.config(DISP_MODULE_RDMA1, &extd_rdma_params, NULL);

	ddp_mutex_interrupt_enable(dvt_acquire_mutex(), NULL);


	DPI_DVT_LOG_W("module reset\n");
	dvt_mutex_reset(NULL, dvt_acquire_mutex());
	dvt_module_action(SHORT_PATH1, RESET);

	DPI_DVT_LOG_W("module start\n");
	dvt_module_action(SHORT_PATH1, START);

	DISP_REG_SET(0, DISPSYS_DPI_BASE + 0x48, 0x80000000);

	DPI_DVT_LOG_W("module trigger\n");
	dvt_mutex_enable(NULL, dvt_acquire_mutex());
	ddp_driver_dpi.trigger(DISP_MODULE_DPI, NULL);

	mdelay(30);
	if (caseNum == 0xA)
		cmdqRecFlushAsync(checksum_cmdq);

#ifdef VENDOR_CHIP_DRIVER
	hdmi_drv->video_config(resolution, 0, 0);
#endif

	DPI_DVT_LOG_W("module dump_info\n");
	ddp_driver_dpi.dump_info(DISP_MODULE_DPI, 1);
	ddp_dump_analysis(DISP_MODULE_RDMA1);
	ddp_dump_reg(DISP_MODULE_RDMA1);
	ddp_dump_analysis(DISP_MODULE_CONFIG);
	ddp_dump_reg(DISP_MODULE_CONFIG);
	ddp_dump_analysis(DISP_MODULE_MUTEX);
	ddp_dump_reg(DISP_MODULE_MUTEX);

	msleep(timeS * 1000);

	ddp_driver_dpi.dump_info(DISP_MODULE_DPI, 1);
	ddp_dump_analysis(DISP_MODULE_RDMA1);
	ddp_dump_reg(DISP_MODULE_RDMA1);
	ddp_dump_analysis(DISP_MODULE_CONFIG);
	ddp_dump_reg(DISP_MODULE_CONFIG);
	ddp_dump_analysis(DISP_MODULE_MUTEX);
	ddp_dump_reg(DISP_MODULE_MUTEX);


	DPI_DVT_LOG_W("module stop\n");

	dvt_module_action(SHORT_PATH1, STOP);

	DPI_DVT_LOG_W("module reset\n");
	dvt_module_action(SHORT_PATH1, RESET);

	DPI_DVT_LOG_W("module power off\n");
	dvt_module_action(SHORT_PATH1, POWER_OFF);

	DPI_DVT_LOG_W("set Mutex and disconnect path\n");
	dvt_disconnect_path(NULL);

	dvt_mutex_reset(NULL, dvt_acquire_mutex());
	dvt_mutex_disenable(NULL, dvt_acquire_mutex());

	DPI_DVT_LOG_W("top clock off\n");
	dvt_ddp_path_top_clock_off();

	memcpy(extd_rdma_params.p_golden_setting_context, &temp_golden,
			sizeof(temp_golden));

	if (data_va)
		vfree((void *)data_va);

	return 0;
}


int dvt_start_rdma_to_dpi_for_RGB2YUV(unsigned int resolution,
		unsigned int timeS, enum AviColorSpace_e format)
{
	int ret = 0;

	DPI_DVT_LOG_W("top clock on\n");
	dvt_ddp_path_top_clock_on();

	DPI_DVT_LOG_W("set Mutex and connect path\n");
	dvt_connect_path(NULL);
	dvt_mutex_set(dvt_acquire_mutex(), NULL);

	DPI_DVT_LOG_W("module init\n");
	ddp_driver_dpi.init(DISP_MODULE_DPI, NULL);
	ddp_driver_rdma.init(DISP_MODULE_RDMA1, NULL);

	DPI_DVT_LOG_W("module power on\n");
	ddp_driver_dpi.power_on(DISP_MODULE_DPI, NULL);
	ddp_driver_rdma.power_on(DISP_MODULE_RDMA1, NULL);

	DPI_DVT_LOG_W("module config\n");
	ddp_driver_dpi.config(DISP_MODULE_DPI, &extd_dpi_params, NULL);
	enableRGB2YUV(format);
	ret = dvt_allocate_buffer(resolution, M4U_FOR_RDMA1);
	if (ret < 0) {
		DDPERR("dvt_allocate_buffer error: ret: %d\n", ret);
		return ret;
	}

	dvt_init_RDMA_param(RDMA_MODE_MEMORY, resolution);
	ddp_driver_rdma.config(DISP_MODULE_RDMA1, &extd_rdma_params, NULL);
	/*configRDMASwap(0, 1); */

	DPI_DVT_LOG_W("module reset\n");
	dvt_mutex_reset(NULL, dvt_acquire_mutex());
	ddp_driver_dpi.reset(DISP_MODULE_DPI, NULL);
	ddp_driver_rdma.reset(DISP_MODULE_RDMA1, NULL);

	DPI_DVT_LOG_W("module start\n");
	ddp_driver_rdma.start(DISP_MODULE_RDMA1, NULL);
	ddp_driver_dpi.start(DISP_MODULE_DPI, NULL);

	DPI_DVT_LOG_W("module trigger\n");
	dvt_mutex_enable(NULL, dvt_acquire_mutex());
	ddp_driver_dpi.trigger(DISP_MODULE_DPI, NULL);
	DISP_REG_SET(NULL, DISP_REG_CONFIG_MUTEX_GET(HW_MUTEX), 1);
	DISP_REG_SET(NULL, DISP_REG_CONFIG_MUTEX_GET(HW_MUTEX), 0);

	hdmi_drv->video_config(resolution, 0, 0);

	DPI_DVT_LOG_W("module dump_info\n");
	ddp_driver_dpi.dump_info(DISP_MODULE_DPI, 1);
	ddp_driver_rdma.dump_info(DISP_MODULE_RDMA1, 1);

	msleep(timeS * 1000);

	DPI_DVT_LOG_W("module stop\n");
	dvt_mutex_disenable(NULL, dvt_acquire_mutex());
	DISP_REG_SET(NULL, DISP_REG_CONFIG_MUTEX_GET(HW_MUTEX), 1);
	DISP_REG_SET(NULL, DISP_REG_CONFIG_MUTEX_GET(HW_MUTEX), 0);
	ddp_driver_dpi.stop(DISP_MODULE_DPI, NULL);
	ddp_driver_rdma.stop(DISP_MODULE_RDMA1, NULL);

	DPI_DVT_LOG_W("module reset\n");
	dvt_mutex_reset(NULL, dvt_acquire_mutex());
	ddp_driver_dpi.reset(DISP_MODULE_DPI, NULL);
	ddp_driver_rdma.reset(DISP_MODULE_RDMA1, NULL);

	DPI_DVT_LOG_W("module power off\n");
	ddp_driver_dpi.power_off(DISP_MODULE_DPI, NULL);
	ddp_driver_rdma.power_off(DISP_MODULE_RDMA1, NULL);

	DPI_DVT_LOG_W("set Mutex and disconnect path\n");
	dvt_disconnect_path(NULL);
	DISP_REG_SET(NULL, DISP_REG_CONFIG_MUTEX_GET(HW_MUTEX), 1);
	DISP_REG_SET(NULL, DISP_REG_CONFIG_MUTEX_GET(HW_MUTEX), 0);

	DPI_DVT_LOG_W("top clock off\n");
	dvt_ddp_path_top_clock_off();

	if (data_va)
		vfree((void *)data_va);

	return 0;
}


/******************************Test Case Start********************************/
#include "ddp_hal.h"

static int dpi_dvt_testcase_4_timing(unsigned int resolution)
{
	DPI_DVT_LOG_W("%s, resolution: %d\n",
		      __func__, resolution);
	/* get HDMI Driver from vendor floder */
	hdmi_drv = (struct HDMI_DRIVER *)HDMI_GetDriver();
	if (hdmi_drv == NULL) {
		DPI_DVT_LOG_W("%s, hdmi_init fail, can not get hdmi handle\n",
				  __func__);
		return -1;
	}

	dpi_dvt_parameters(resolution);
	dvt_dump_ext_dpi_parameters();
	dvt_start_rdma1_to_dpi(resolution, 20, 0x4);

	return 0;
}

#ifdef DPI_DVT_TEST_SUPPORT

static int dpi_dvt_testcase_2_BG(unsigned int resolution)
{
	DPI_DVT_LOG_W("dpi_dvt_testcase_2_bg, resolution: %d\n", resolution);
	/* get HDMI Driver from vendor floder */
	hdmi_drv = (struct HDMI_DRIVER *)HDMI_GetDriver();
	if (hdmi_drv == NULL) {
		DPI_DVT_LOG_W("%s, hdmi_init fail, can not get hdmi handle\n",
				  __func__);
		return -1;
	}

	dpi_dvt_parameters(resolution);
	extd_dpi_params.dispif_config.dpi.width = 720;
	extd_dpi_params.dispif_config.dpi.height = 480;
	extd_dpi_params.dispif_config.dpi.bg_width = 560;
	extd_dpi_params.dispif_config.dpi.bg_height = 240;
	dvt_dump_ext_dpi_parameters();
	dvt_start_rdma1_to_dpi(resolution, 20, 0x2);

	return 0;
}


static int dpi_dvt_testcase_3_pattern(enum PATTERN pattern)
{
	/* 0x01 -> 0x71 */
	DPI_DVT_LOG_W("%s, pattern: %d\n", __func__, pattern);

	DPI_DVT_Context.output_video_resolution = HDMI_VIDEO_1280x720p_60Hz;
	dpi_dvt_parameters(DPI_DVT_Context.output_video_resolution);
	dvt_dump_ext_dpi_parameters();
	DPI_DVT_LOG_W("top clock on\n");
	dvt_ddp_path_top_clock_on();

	dpi_dvt_show_pattern(pattern, 20);
	dvt_ddp_path_top_clock_off();

	return 0;
}

static int dpi_dvt_testcase_6_yuv(unsigned int resolution,
				  enum AviColorSpace_e format)
{
	DPI_DVT_LOG_W("dpi_dvt_testcase_6_yuv444, resolution: %d, format: %d\n",
		      resolution, format);

	dpi_dvt_parameters(resolution);
	dvt_dump_ext_dpi_parameters();
	dvt_start_rdma_to_dpi_for_RGB2YUV(resolution, 30, format);

	return 0;
}

static int dpi_dvt_testcase_10_checkSum(unsigned int resolution)
{
	int i;

	DPI_DVT_LOG_W("%s, resolution: %d\n",
		      __func__, resolution);
	cmdqRecCreate(CMDQ_SCENARIO_MHL_DISP, &checksum_cmdq);
	cmdqRecReset(checksum_cmdq);
	init_cmdq_slots(&checksum_record, 3, 0);

	dpi_dvt_parameters(resolution);
	dvt_dump_ext_dpi_parameters();

	if (resolution == HDMI_VIDEO_2160p_DSC_24Hz ||
		resolution == HDMI_VIDEO_2160p_DSC_30Hz)
		;
	else
		dvt_start_rdma1_to_dpi(resolution, 20, 0xA);
	for (i = 0; i < 3; i++)
		cmdqBackupReadSlot(checksum_record, i, &checksum[i]);

	DPI_DVT_LOG_W("checksum[0] = %x", checksum[0] & 0xffffff);
	for (i = 0; i < 2; i++) {
		DPI_DVT_LOG_W("checksum[%d] = %x", i, checksum[i] & 0xffffff);
		if ((checksum[i] & 0xffffff) != (checksum[i+1] & 0xffffff))
			DPI_DVT_LOG_W("Error checksum is not the same!!\n");
	}
	DPI_DVT_LOG_W("checksum[%d] = %x", i, checksum[i] & 0xffffff);
	cmdqBackupFreeSlot(checksum_record);
	cmdqRecDestroy(checksum_cmdq);
	return 0;
}
static int dpi_dvt_testcase_11_ovl1_to_dpi(unsigned int resolution)
{
	DPI_DVT_LOG_W("%s, resolution: %d\n",
		      __func__, resolution);

	dpi_dvt_parameters(resolution);
	dvt_dump_ext_dpi_parameters();

	dvt_start_ovl1_2l_to_dpi(resolution, 30);

	msleep(100);
	return 0;
}

#endif
/* static int dpi_dsc_testcase(unsigned int resolution); */

/*******************************Test Case End**********************************/
int dpi_dvt_run_cases(unsigned int caseNum)
{
	switch (caseNum) {
	case 4:
		{

#if 0
			dpi_dvt_testcase_4_timing(HDMI_VIDEO_1920x1080p_60Hz);
			msleep(500);
#endif
			dpi_dvt_testcase_4_timing(HDMI_VIDEO_1920x1080p_30Hz);
			msleep(500);
			dpi_dvt_testcase_4_timing(HDMI_VIDEO_1280x720p_60Hz);
			msleep(500);
			dpi_dvt_testcase_4_timing(HDMI_VIDEO_720x480p_60Hz);
			msleep(500);
			break;
		}
#ifdef DPI_DVT_TEST_SUPPORT
	case 1:
		{
			dpi_dvt_parameters(HDMI_VIDEO_1280x720p_60Hz);
			dvt_dump_ext_dpi_parameters();
			dvt_start_rdma1_to_dpi(HDMI_VIDEO_1280x720p_60Hz,
					20, 0x1);
			break;
		}
	case 2:
		{
			dpi_dvt_testcase_2_BG(HDMI_VIDEO_1280x720p_60Hz);
			msleep(500);
			break;
		}
	case 3:
		{
			dpi_dvt_testcase_3_pattern(0);
			msleep(500);
			dpi_dvt_testcase_3_pattern(1);
			msleep(500);
			dpi_dvt_testcase_3_pattern(2);
			msleep(500);
			dpi_dvt_testcase_3_pattern(3);
			msleep(500);
			dpi_dvt_testcase_3_pattern(4);
			msleep(500);
			dpi_dvt_testcase_3_pattern(5);
			msleep(500);
			dpi_dvt_testcase_3_pattern(6);
			msleep(500);
			dpi_dvt_testcase_3_pattern(7);
			msleep(500);
			break;
		}
	case 6:
		{
			dpi_dvt_testcase_6_yuv(HDMI_VIDEO_1280x720p_60Hz,
					acsYCbCr444);
			msleep(500);
			break;
		}
	case 10:
		{
/* dpi_dvt_testcase_10_checkSum(HDMI_VIDEO_2160p_DSC_30Hz); */
			dpi_dvt_testcase_10_checkSum(HDMI_VIDEO_1280x720p_60Hz);

			msleep(500);
			break;
		}
	case 11:
		{
			dpi_dvt_testcase_11_ovl1_to_dpi(
					HDMI_VIDEO_1920x1080p_30Hz);
			msleep(500);
			dpi_dvt_testcase_11_ovl1_to_dpi(
					HDMI_VIDEO_1280x720p_60Hz);
			msleep(500);
			dpi_dvt_testcase_11_ovl1_to_dpi(
					HDMI_VIDEO_720x480p_60Hz);
			msleep(500);
			break;
		}
	case 18:
		{
/* dpi_dvt_testcase_18_4K_rdma_to_dpi(HDMI_VIDEO_2160p_DSC_30Hz); */
			msleep(500);
			break;
		}
	case 19:
		{
/* dpi_dvt_parameters(HDMI_VIDEO_1920x1080p_30Hz); */
/* ovl2mem_case19(HDMI_VIDEO_1920x1080p_30Hz); */
			msleep(500);
			break;
		}
	case 20:
		{
/* dpi_ldvt_testcase_20(HDMI_VIDEO_2160p_DSC_30Hz); */
			msleep(500);
			break;
		}
	case 21:
		{
/* dpi_dsc_testcase(HDMI_VIDEO_1080p_DSC_60Hz); */
/* dpi_dsc_testcase(HDMI_VIDEO_2160p_DSC_30Hz); */
/* dpi_dsc_testcase(HDMI_VIDEO_2160p_DSC_24Hz); */
			msleep(500);
			break;
		}
#endif
	default:
		DDPERR("case number is invailed, case: %d\n", caseNum);
	}

	return 0;
}

unsigned int dpi_dvt_ioctl(unsigned int arg)
{
	int ret = 0;

	/* get HDMI Driver from vendor floder */
	hdmi_drv = (struct HDMI_DRIVER *)HDMI_GetDriver();
	if (hdmi_drv == NULL) {
		DPI_DVT_LOG_W("%s, hdmi_init fail, can not get hdmi handle\n",
				  __func__);
		return -1;
	}

	DPI_DVT_LOG_W("testcase: %x\n", arg);
	dpi_dvt_run_cases(arg);

	return ret;
}

#else
unsigned int dpi_dvt_ioctl(unsigned int arg)
{
	return 0;
}
#endif
