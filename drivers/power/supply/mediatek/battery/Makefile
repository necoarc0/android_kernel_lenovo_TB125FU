#
# Copyright (C) 2015 MediaTek Inc.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#


MTK_PLATFORM := $(subst ",,$(CONFIG_MTK_PLATFORM))
ccflags-y += -I$(srctree)/drivers/misc/mediatek/pmic/include
ccflags-y += -I$(srctree)/drivers/power/supply/mediatek/misc
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/mt6768/include
ifeq ($(CONFIG_MTK_GAUGE_VERSION),30)
	obj-y += mtk_battery.o mtk_power_misc.o mtk_gauge_coulomb_service.o mtk_gauge_class.o mtk_battery_recovery.o mtk_battery_core.o
	obj-$(CONFIG_GAUGE_MM8013) += mm8013.o
endif
