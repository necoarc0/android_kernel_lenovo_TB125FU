# SPDX-License-Identifier: GPL-2.0 */
#
# Copyright (c) 2022 MediaTek Inc.
#

SSPM_VER := v1

ifneq ($(wildcard $(srctree)/drivers/misc/mediatek/met_drv/),)
ccflags-y += -DMET_SUPPORT
ccflags-y += -DMET_USER_EVENT_SUPPORT
endif

ccflags-y += \
	-I$(CPUFREQ_ROOT_DIR)/inc \
	-I$(CPUFREQ_ROOT_DIR)/src/mach/$(CONFIG_MTK_PLATFORM) \
	-I$(srctree)/drivers/devfreq \
	-I$(srctree)/drivers/misc/mediatek/base/power/$(CONFIG_MTK_PLATFORM) \
	-I$(srctree)/drivers/misc/mediatek/base/power/cm_mgr_v1/ \
	-I$(srctree)/drivers/misc/mediatek/base/power/cm_mgr_v1/$(CONFIG_MTK_PLATFORM) \
	-I$(srctree)/drivers/misc/mediatek/base/power/cpufreq_v1/inc \
	-I$(srctree)/drivers/misc/mediatek/base/power/cpufreq_v1/src/mach/$(CONFIG_MTK_PLATFORM) \
	-I$(srctree)/drivers/misc/mediatek/base/power/include \
	-I$(srctree)/drivers/misc/mediatek/dramc/$(CONFIG_MTK_PLATFORM) \
	-I$(srctree)/drivers/misc/mediatek/include \
	-I$(srctree)/drivers/misc/mediatek/include/mt-plat \
	-I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(CONFIG_MTK_PLATFORM)/include \
	-I$(srctree)/drivers/misc/mediatek/include/mt-plat/$(CONFIG_MTK_PLATFORM)/include/mach \
	-I$(srctree)/drivers/misc/mediatek/sspm \
	-I$(srctree)/drivers/misc/mediatek/sspm/$(CONFIG_MTK_PLATFORM) \
	-I$(srctree)/include \
	-I$(srctree)/include/memory/mediatek/ \

# main
obj-y += mtk_cm_mgr_platform.o

