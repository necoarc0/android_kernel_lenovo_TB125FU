/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2021 MediaTek Inc.
*/

/*
 *
 * Filename:
 * ---------
 *    mtk_switch_charging.c
 *
 * Project:
 * --------
 *   Android_Software
 *
 * Description:
 * ------------
 *   This Module defines functions of Battery charging
 *
 * Author:
 * -------
 * Wy Chuang
 *
 */
#include <linux/init.h>		/* For init/exit macros */
#include <linux/module.h>	/* For MODULE_ marcros  */
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/power_supply.h>
#include <linux/pm_wakeup.h>
#include <linux/time.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/proc_fs.h>
#include <linux/platform_device.h>
#include <linux/seq_file.h>
#include <linux/scatterlist.h>
#include <linux/suspend.h>
#include <linux/of.h>
#include <tcpm.h>

#include <mt-plat/mtk_boot.h>
#include "mtk_charger_intf.h"
#include "mtk_switch_charging.h"
#include "mtk_intf.h"

struct tag_bootmode {
	u32 size;
	u32 tag;
	u32 bootmode;
	u32 boottype;
};

static int _uA_to_mA(int uA)
{
	if (uA == -1)
		return -1;
	else
		return uA / 1000;
}

extern void access_pd_fixed_current(char ops, int *curr);
static int select_pd_pdo(bool high_vol)
{
	int tcpm_ret, fixed_curr, select_vol;
	unsigned char cap_i = 0;
	struct tcpm_remote_power_cap pd_cap;
	static struct tcpc_device *tcpc = NULL;
	static struct power_supply *chrdet_psy = NULL;
	union power_supply_propval propval;

	if (!tcpc) {
		tcpc = tcpc_dev_get_by_name("type_c_port0");
		if (!tcpc) {
			pr_notice("%s: get tcpc dev fail\n", __func__);
			return -ENODEV;
		}
	}

	if (!chrdet_psy) {
		chrdet_psy = power_supply_get_by_name("charger");
		if (!chrdet_psy) {
			pr_notice("%s: get chrdet_psy power_supply fail\n", __func__);
			return -ENODEV;
		}
	}

	if (high_vol) {
		tcpm_ret = tcpm_dpm_pd_request(tcpc, 9000, 2200, NULL);
		fixed_curr = 2200;
		if (tcpm_ret) {
			tcpm_ret = tcpm_dpm_pd_request(tcpc, 9000, 2000, NULL);
			fixed_curr = 2000;
		}
		if (!tcpm_ret) {
			access_pd_fixed_current('w', &fixed_curr);
			propval.intval = CHARGER_PD_9V;
			power_supply_set_property(chrdet_psy,
				POWER_SUPPLY_PROP_CHARGE_TYPE, &propval);
			return 0;
		}
	}

	pd_cap.nr = 0;
	pd_cap.selected_cap_idx = 0;
	tcpm_ret = tcpm_get_remote_power_cap(tcpc, &pd_cap);
	if (!tcpm_ret && pd_cap.nr!=0) {
		for (cap_i=0;cap_i<pd_cap.nr;cap_i++) {
			pr_info("%s cap[%d](%s) %dmv~%dmv %dma\n", __func__, cap_i,
				pd_cap.type[cap_i]==0?"FIXED":
				pd_cap.type[cap_i]==3?"APDO":"OTHERS",
				pd_cap.min_mv[cap_i], pd_cap.max_mv[cap_i], pd_cap.ma[cap_i]);
			if (pd_cap.min_mv[cap_i]<=6000 && pd_cap.max_mv[cap_i]>=4500) {
				tcpm_ret = -1;
				if (pd_cap.type[cap_i] == 0) {//fixed
					select_vol = pd_cap.min_mv[cap_i];
				} else {//apdo
					if (pd_cap.min_mv[cap_i]<=5000 && pd_cap.max_mv[cap_i]>=5000) {
						select_vol = 5000;
					} else if (pd_cap.max_mv[cap_i] <= 5000) {
						select_vol = pd_cap.max_mv[cap_i];
					} else {
						select_vol = pd_cap.min_mv[cap_i];
					}
				}
				if (pd_cap.ma[cap_i] >= 3000) {
					tcpm_ret = tcpm_dpm_pd_request(tcpc, select_vol, 3000, NULL);
					fixed_curr = 3000;
				} else {
					tcpm_ret = tcpm_dpm_pd_request(tcpc, select_vol,
						pd_cap.ma[cap_i], NULL);
					fixed_curr = pd_cap.ma[cap_i];
				}
				if (!tcpm_ret) {
					access_pd_fixed_current('w', &fixed_curr);
					propval.intval = CHARGER_PD_5V;
					power_supply_set_property(chrdet_psy,
						POWER_SUPPLY_PROP_CHARGE_TYPE, &propval);
				}
				return 0;
			}
		}
	}
	return -EINVAL;
}

static void _disable_all_charging(struct charger_manager *info)
{
	charger_dev_enable(info->chg1_dev, false);
	charger_dev_enable(info->dvchg1_dev, false);

	if (info->chr_type == CHARGER_PD_9V) {
		select_pd_pdo(false);//reset 5V
	}

	if (mtk_pe20_get_is_enable(info)) {
		mtk_pe20_set_is_enable(info, false);
		if (mtk_pe20_get_is_connect(info))
			mtk_pe20_reset_ta_vchr(info);
	}

	if (mtk_pe_get_is_enable(info)) {
		mtk_pe_set_is_enable(info, false);
		if (mtk_pe_get_is_connect(info))
			mtk_pe_reset_ta_vchr(info);
	}

	if (info->enable_pe_5)
		pe50_stop();

	if (info->enable_pe_4)
		pe40_stop();

//	if (pdc_is_ready())
//		pdc_stop();
}

void access_charge_current_total(char ops, int *mA)
{
	static int curr = 2000;

	switch (ops) {
	case 'r':
		*mA = curr;
		break;
	case 'w':
		curr = *mA;
		break;
	default:
		break;
	}
}
EXPORT_SYMBOL(access_charge_current_total);

extern void access_pd_fixed_current(char ops, int *curr);

static void swchg_select_charging_current_limit(struct charger_manager *info)
{
	struct charger_data *pdata = NULL;
	struct charger_data *pdata2 = NULL;
	struct switch_charging_alg_data *swchgalg = info->algorithm_data;
	u32 ichg1_min = 0, aicr1_min = 0;
	struct power_supply *usb_psy;
	union power_supply_propval propval;
	int ret = 0, fixed_curr, total_curr;
	bool tune_done = true;

	struct device *dev = NULL;
	struct device_node *boot_node = NULL;
	struct tag_bootmode *tag = NULL;
	int boot_mode = 11;//UNKNOWN_BOOT

	dev = &(info->pdev->dev);
	if (dev != NULL) {
		boot_node = of_parse_phandle(dev->of_node, "bootmode", 0);
		if (!boot_node) {
			chr_err("%s: failed to get boot mode phandle\n", __func__);
		} else {
			tag = (struct tag_bootmode *)of_get_property(boot_node,
								"atag,boot", NULL);
			if (!tag) {
				chr_err("%s: failed to get atag,boot\n", __func__);
			} else
				boot_mode = tag->bootmode;
		}
	}

	if (info->pe5.online) {
		chr_err("In PE5.0\n");
		return;
	}

	if (battery_get_uisoc() <= 85
			&& info->chr_type == CHARGER_PD_5V) {
		select_pd_pdo(true);
	} else if (battery_get_uisoc() > 85
			&& info->chr_type == CHARGER_PD_9V){
		select_pd_pdo(false);
	}

	access_pd_fixed_current('r', &fixed_curr);
	pdata = &info->chg1_data;
	pdata2 = &info->dvchg1_data;
	mutex_lock(&swchgalg->ichg_aicr_access_mutex);

	charger_dev_is_tune_done(info->chg1_dev, &tune_done);
	if (!info->test_charger_enable || !info->ato_charger_enable
			|| (info->batt_protect_mode == true
					&& battery_get_uisoc() > BPT_CAP_THRESHOLD_H)) {
		chr_err("[%s] test:%d ato:%d batt_protect:%d not all enabled, hiz all charger!\n",
				__func__, info->test_charger_enable, info->ato_charger_enable,
				info->batt_protect_mode);
		charger_dev_enable_hz(info->chg1_dev, true);
		charger_dev_enable_hz(info->dvchg1_dev, true);
	} else {
		charger_dev_enable_hz(info->chg1_dev, false);
	}

	pdata2->input_current_limit = 0;
	pdata2->charging_current_limit = 0;

	/* AICL */
	if (!mtk_pe20_get_is_connect(info) && !mtk_pe_get_is_connect(info) &&
	    !mtk_is_TA_support_pd_pps(info) /*&& !mtk_pdc_check_charger(info)*/) {
		charger_dev_run_aicl(info->chg1_dev,
				&pdata->input_current_limit_by_aicl);
		if (info->enable_dynamic_mivr) {
			if (pdata->input_current_limit_by_aicl >
				info->data.max_dmivr_charger_current)
				pdata->input_current_limit_by_aicl =
					info->data.max_dmivr_charger_current;
		}
	}

	if (pdata->force_charging_current > 0) {

		pdata->charging_current_limit = pdata->force_charging_current;
		if (pdata->force_charging_current <= 450000) {
			pdata->input_current_limit = 500000;
		} else {
			pdata->input_current_limit =
					info->data.ac_charger_input_current;
			pdata->charging_current_limit =
					info->data.ac_charger_current;
		}
		goto done;
	}

	if (info->usb_unlimited) {
		if (pdata->input_current_limit_by_aicl != -1) {
			pdata->input_current_limit =
				pdata->input_current_limit_by_aicl;
		} else {
			pdata->input_current_limit =
				info->data.usb_unlimited_current;
		}
		pdata->charging_current_limit =
			info->data.ac_charger_current;
		goto done;
	}

	if (info->water_detected) {
		pdata->input_current_limit = info->data.usb_charger_current;
		pdata->charging_current_limit = info->data.usb_charger_current;
		goto done;
	}

	if ((boot_mode == META_BOOT) ||
		(boot_mode == ADVMETA_BOOT)) {
		pdata->input_current_limit = 200000; /* 200mA */
		goto done;
	}

	if (info->atm_enabled == true && (info->chr_type == STANDARD_HOST ||
	    info->chr_type == CHARGING_HOST)) {
		pdata->input_current_limit = 100000; /* 100mA */
		goto done;
	}

	if (mtk_is_TA_support_pd_pps(info)) {
		pdata->input_current_limit =
			info->data.pe40_single_charger_input_current;
		pdata->charging_current_limit =
			info->data.pe40_single_charger_current;
#if 0
	if (is_typec_adapter(info)) {
		if (adapter_dev_get_property(info->pd_adapter, TYPEC_RP_LEVEL)
			== 3000) {
			pdata->input_current_limit = 3000000;
			pdata->charging_current_limit = 3000000;
		} else if (adapter_dev_get_property(info->pd_adapter,
			TYPEC_RP_LEVEL) == 1500) {
			pdata->input_current_limit = 1500000;
			pdata->charging_current_limit = 2000000;
		} else {
			chr_err("type-C: inquire rp error\n");
			pdata->input_current_limit = 500000;
			pdata->charging_current_limit = 500000;
		}

		chr_err("type-C:%d current:%d\n",
			info->pd_type,
			adapter_dev_get_property(info->pd_adapter,
				TYPEC_RP_LEVEL));
#endif
	} else if (info->chr_type == STANDARD_HOST) {
		if (IS_ENABLED(CONFIG_USBIF_COMPLIANCE)) {
			if (info->usb_state == USB_SUSPEND)
				pdata->input_current_limit =
					info->data.usb_charger_current_suspend;
			else if (info->usb_state == USB_UNCONFIGURED)
				pdata->input_current_limit =
				info->data.usb_charger_current_unconfigured;
			else if (info->usb_state == USB_CONFIGURED)
				pdata->input_current_limit =
				info->data.usb_charger_current_configured;
			else
				pdata->input_current_limit =
				info->data.usb_charger_current_unconfigured;

			pdata->charging_current_limit =
					pdata->input_current_limit;
		} else {
			pdata->input_current_limit =
					info->data.usb_charger_current;
			/* it can be larger */
			pdata->charging_current_limit =
					info->data.usb_charger_current;
		}
	} else if (info->chr_type == NONSTANDARD_CHARGER) {
		pdata->input_current_limit =
				info->data.non_std_ac_charger_current;
		pdata->charging_current_limit =
				info->data.non_std_ac_charger_current;
	} else if (info->chr_type == STANDARD_CHARGER) {
		pdata->input_current_limit =
				info->data.ac_charger_input_current;
		pdata->charging_current_limit =
				info->data.ac_charger_current;
		if (is_typec_adapter(info)) {
			/*if (adapter_dev_get_property(info->pd_adapter,
						TYPEC_RP_LEVEL) == 1500) {
				pdata->input_current_limit = 1500000;
				pdata->charging_current_limit = 1500000;
			} else if (adapter_dev_get_property(info->pd_adapter,
						TYPEC_RP_LEVEL) != 3000) {
				chr_err("type-C: inquire rp error\n");
				pdata->input_current_limit = 500000;
				pdata->charging_current_limit = 500000;
			}*/

			chr_err("type-C:%d current:%d\n",
					info->pd_type,
					adapter_dev_get_property(info->pd_adapter,
						TYPEC_RP_LEVEL));
		}
		mtk_pe20_set_charging_current(info,
					&pdata->charging_current_limit,
					&pdata->input_current_limit);
		mtk_pe_set_charging_current(info,
					&pdata->charging_current_limit,
					&pdata->input_current_limit);

		//+Bug9450, yangpingao.wt, modify, set input curent to 700mA
		if (tune_done != true) {
			pdata->input_current_limit = 700000;
			pdata->charging_current_limit = 1500000;
		}
		chr_info("%s: tune_done= %d, input_current= %dmA, charging_current= %dmA\n",
					__func__, tune_done, pdata->input_current_limit / 1000,
					pdata->charging_current_limit / 1000);
		//-Bug9450, yangpingao.wt, modify, set input curent to 700mA
	} else if (info->chr_type == CHARGING_HOST) {
		pdata->input_current_limit =
				info->data.charging_host_charger_current;
		pdata->charging_current_limit =
				info->data.charging_host_charger_current;
	} else if (info->chr_type == APPLE_1_0A_CHARGER) {
		pdata->input_current_limit =
				info->data.apple_1_0a_charger_current;
		pdata->charging_current_limit =
				info->data.apple_1_0a_charger_current;
	} else if (info->chr_type == APPLE_2_1A_CHARGER) {
		pdata->input_current_limit =
				info->data.apple_2_1a_charger_current;
		pdata->charging_current_limit =
				info->data.apple_2_1a_charger_current;
	} else if (info->chr_type == CHARGER_PD_5V) {
		if (fixed_curr == 3000) {
			pdata->input_current_limit = 2200000;
			pdata->charging_current_limit = 2160000;
			pdata2->input_current_limit = 800000;
			pdata2->charging_current_limit = 840000;
		} else if (fixed_curr > 2000) {
			pdata->input_current_limit = 2000000;
			pdata->charging_current_limit = 2000000;
			pdata2->input_current_limit = fixed_curr*1000-2000000;
			pdata2->charging_current_limit = fixed_curr*1000-2000000;
		} else {
			pdata->input_current_limit = fixed_curr*1000;
			pdata->charging_current_limit = fixed_curr*1000*5/4;
		}
	} else if (info->chr_type == CHARGER_PE_5V) {
		pdata->input_current_limit = 2200000;
		pdata->charging_current_limit = 2160000;
		pdata2->input_current_limit = 800000;
		pdata2->charging_current_limit = 840000;
	} else if (info->chr_type == CHARGER_PE_7V) {
		pdata->input_current_limit = 1800000;
		pdata->charging_current_limit = 2620000;
		pdata2->input_current_limit = 900000;
		pdata2->charging_current_limit = 1380000;
	} else if (info->chr_type == CHARGER_PD_9V) {
		if (fixed_curr == 2200) {
			pdata->input_current_limit = 1600000;
			pdata->charging_current_limit = 2620000;
			pdata2->input_current_limit = 600000;
			pdata2->charging_current_limit = 1380000;
		} else if (fixed_curr == 2000) {
			pdata->input_current_limit = 1400000;
			pdata->charging_current_limit = 2620000;
			pdata2->input_current_limit = 600000;
			pdata2->charging_current_limit = 1380000;
		}
	} else if (info->chr_type == CHARGER_PE_9V) {
		pdata->input_current_limit = 1600000;
		pdata->charging_current_limit = 2620000;
		pdata2->input_current_limit = 600000;
		pdata2->charging_current_limit = 1380000;
	} else if (info->chr_type == CHARGER_PE_12V) {
		pdata->input_current_limit = 1170000;
		pdata->charging_current_limit = 2620000;
		pdata2->input_current_limit = 500000;
		pdata2->charging_current_limit = 1380000;
	} else if (info->chr_type == CHARGER_NS_1A) {
		pdata->input_current_limit = 1000000;
		pdata->charging_current_limit = 1216000;
	} else if (info->chr_type == CHARGER_NS_2A) {
		pdata->input_current_limit = 2000000;
		pdata->charging_current_limit = 2000000;
	} else if (info->chr_type == CHARGER_NS_2_1A) {
		pdata->input_current_limit = 2100000;
		pdata->charging_current_limit = 2000000;
	} else if (info->chr_type == CHARGER_NS_2_4A) {
		pdata->input_current_limit = 2000000;
		pdata->charging_current_limit = 2000000;
		pdata2->input_current_limit = 400000;
		pdata2->charging_current_limit = 600000;
	} else if (info->chr_type == CHARGER_FLOATING) {
		pdata->input_current_limit = 500000;
		pdata->charging_current_limit = 500000;
	}

	total_curr = (pdata->charging_current_limit + pdata2->charging_current_limit)/1000;
	access_charge_current_total('w', &total_curr);

	if (info->enable_sw_jeita) {
		if (IS_ENABLED(CONFIG_USBIF_COMPLIANCE)
		    && info->chr_type == STANDARD_HOST)
			chr_err("USBIF & STAND_HOST skip current check\n");
		else {
			if (info->sw_jeita.cc <
				(pdata->charging_current_limit + pdata2->charging_current_limit)) {
				if (info->sw_jeita.cc <= 2000000) {
					pdata->charging_current_limit = info->sw_jeita.cc;
					pdata->input_current_limit += pdata2->input_current_limit;
					pdata2->charging_current_limit = 0;
					pdata2->input_current_limit = 0;
				} else {
					pdata->charging_current_limit = info->sw_jeita.cc /
						((pdata->charging_current_limit +
						pdata2->charging_current_limit)/1000) *
						(pdata->charging_current_limit/1000);
					pdata2->charging_current_limit = info->sw_jeita.cc /
						((pdata->charging_current_limit +
						pdata2->charging_current_limit)/1000) *
						(pdata2->charging_current_limit/1000);
				}
			}
		}
	}

	usb_psy = power_supply_get_by_name("usb");
	if (usb_psy) {
		ret = power_supply_get_property(usb_psy,
				POWER_SUPPLY_PROP_TYPEC_CC_ORIENTATION, &propval);
		if (ret)
			propval.intval = 0;
		if (!propval.intval) {
			pr_debug("no cc-line detected, max current 500mA for cc float!\n");
			if (pdata->input_current_limit > 500000)
				pdata->input_current_limit = 500000;
			if (pdata->charging_current_limit > 500000)
				pdata->charging_current_limit = 500000;
			pdata2->input_current_limit = 0;
			pdata2->charging_current_limit = 0;
		}
	}

	if (pdata->thermal_charging_current_limit != -1) {
		if (pdata->thermal_charging_current_limit <
		    pdata->charging_current_limit)
			pdata->charging_current_limit =
					pdata->thermal_charging_current_limit;
	}

	//sc_select_charging_current(info, pdata);

	if (pdata->thermal_input_current_limit != -1) {
		if (pdata->thermal_input_current_limit <
		    pdata->input_current_limit)
			pdata->input_current_limit =
					pdata->thermal_input_current_limit;
	}

	if (mtk_pe40_get_is_connect(info)) {
		if (info->pe4.pe4_input_current_limit != -1 &&
		    info->pe4.pe4_input_current_limit <
		    pdata->input_current_limit)
			pdata->input_current_limit =
				info->pe4.pe4_input_current_limit;

		info->pe4.input_current_limit = pdata->input_current_limit;

		if (info->pe4.pe4_input_current_limit_setting != -1 &&
		    info->pe4.pe4_input_current_limit_setting <
		    pdata->input_current_limit)
			pdata->input_current_limit =
				info->pe4.pe4_input_current_limit_setting;
	}

	if (pdata->input_current_limit_by_aicl != -1 &&
	    !mtk_pe20_get_is_connect(info) && !mtk_pe_get_is_connect(info) &&
	    !mtk_is_TA_support_pd_pps(info)) {
		if (pdata->input_current_limit_by_aicl <
		    pdata->input_current_limit)
			pdata->input_current_limit =
					pdata->input_current_limit_by_aicl;
	}
done:
	ret = charger_dev_get_min_charging_current(info->chg1_dev, &ichg1_min);
	if (ret != -ENOTSUPP && pdata->charging_current_limit < ichg1_min)
		pdata->charging_current_limit = 0;

	ret = charger_dev_get_min_input_current(info->chg1_dev, &aicr1_min);
	if (ret != -ENOTSUPP && pdata->input_current_limit < aicr1_min)
		pdata->input_current_limit = 0;

	if (!info->test_charger_enable || !info->ato_charger_enable) {
		pdata->input_current_limit = 0;
		pdata->charging_current_limit = 0;
		pdata2->input_current_limit = 0;
		pdata2->charging_current_limit = 0;
	}

	if (info->batt_protect_mode == true && info->bpt_status != true
			&& battery_get_uisoc() >= (BPT_CAP_THRESHOLD_H - 3)) {
		pdata->charging_current_limit = 1000000;
		pdata2->charging_current_limit = 0;
	}

	chr_err("force:%d thermal:%d,%d pe4:%d,%d,%d setting:%d %d(%d %d) sc:%d,%d,%d type:%d usb_unlimited:%d usbif:%d usbsm:%d aicl:%d atm:%d enable:%d:%d:%d\n",
		_uA_to_mA(pdata->force_charging_current),
		_uA_to_mA(pdata->thermal_input_current_limit),
		_uA_to_mA(pdata->thermal_charging_current_limit),
		_uA_to_mA(info->pe4.pe4_input_current_limit),
		_uA_to_mA(info->pe4.pe4_input_current_limit_setting),
		_uA_to_mA(info->pe4.input_current_limit),
		_uA_to_mA(pdata->input_current_limit),
		_uA_to_mA(pdata->charging_current_limit),
		_uA_to_mA(pdata2->input_current_limit),
		_uA_to_mA(pdata2->charging_current_limit),
		_uA_to_mA(info->sc.pre_ibat),
		_uA_to_mA(info->sc.sc_ibat),
		info->sc.solution,
		info->chr_type, info->usb_unlimited,
		IS_ENABLED(CONFIG_USBIF_COMPLIANCE), info->usb_state,
		pdata->input_current_limit_by_aicl, info->atm_enabled,
		info->test_charger_enable, info->ato_charger_enable,
		info->bpt_status);

	if (pdata2->charging_current_limit && pdata2->input_current_limit
		&& info->test_charger_enable && info->ato_charger_enable
		&& tune_done) {
		charger_dev_enable_hz(info->dvchg1_dev, false);
	} else {
		charger_dev_enable_hz(info->dvchg1_dev, true);
	}
	charger_dev_set_input_current(info->chg1_dev,
					pdata->input_current_limit);
	charger_dev_set_charging_current(info->chg1_dev,
					pdata->charging_current_limit);
	charger_dev_set_input_current(info->dvchg1_dev,
					pdata2->input_current_limit);
	charger_dev_set_charging_current(info->dvchg1_dev,
					pdata2->charging_current_limit);

	/* If AICR < 300mA, stop PE+/PE+20 */
	if (pdata->input_current_limit < 300000) {
		if (mtk_pe20_get_is_enable(info)) {
			mtk_pe20_set_is_enable(info, false);
			if (mtk_pe20_get_is_connect(info))
				mtk_pe20_reset_ta_vchr(info);
		}

		if (mtk_pe_get_is_enable(info)) {
			mtk_pe_set_is_enable(info, false);
			if (mtk_pe_get_is_connect(info))
				mtk_pe_reset_ta_vchr(info);
		}
	}

	/*
	 * If thermal current limit is larger than charging IC's minimum
	 * current setting, enable the charger immediately
	 */
	if (pdata->input_current_limit > aicr1_min &&
	    pdata->charging_current_limit > ichg1_min && info->can_charging)
		charger_dev_enable(info->chg1_dev, true);
	mutex_unlock(&swchgalg->ichg_aicr_access_mutex);
}

static void swchg_select_safe_charger_voltage(struct charger_manager *info)
{
	u32 mivr_uV;

	if (info->chr_type==CHARGER_PD_9V || info->chr_type==CHARGER_PE_9V) {
		mivr_uV = 8100000;
	} else if (info->chr_type==CHARGER_PE_7V) {
		mivr_uV = 6100000;
	} else if (info->chr_type==CHARGER_PE_12V) {
		mivr_uV = 10500000;
	} else {
		mivr_uV = 4600000;
	}

	charger_dev_set_mivr(info->chg1_dev, mivr_uV);
	charger_dev_set_mivr(info->dvchg1_dev, mivr_uV);
}

#ifdef CONFIG_GAUGE_MM8013
extern int mm8013_get_info(enum power_supply_property info_type, int *val);
#else
int mm8013_get_info(enum power_supply_property info_type, int *val)
{
	return -1;
}
#endif
static void swchg_select_cv(struct charger_manager *info)
{
	u32 constant_voltage;
	int battery_cv;
	int ret = mm8013_get_info(POWER_SUPPLY_PROP_CONSTANT_CHARGE_VOLTAGE_MAX,
		&battery_cv);

	chr_err("[%s] battery_cv:%d, jeita_cv:%d\n", __func__, battery_cv, info->sw_jeita.cv);
	if (info->enable_sw_jeita)
		if (info->sw_jeita.cv != 0) {
			if (!ret && battery_cv < info->sw_jeita.cv) {
				if (battery_cv == 4430000)
					battery_cv = 4432000;
				else if (battery_cv == 4380000)
					battery_cv = 4384000;
				else if (battery_cv == 4200000)
					battery_cv = 4224000;
				else
					battery_cv = 4432000;
				chr_err("[%s] fixed jeita_cv:%d\n", __func__, battery_cv);
				charger_dev_set_constant_voltage(info->chg1_dev, battery_cv);
				charger_dev_set_constant_voltage(info->dvchg1_dev, battery_cv);
				return;
			}
			charger_dev_set_constant_voltage(info->chg1_dev,
							info->sw_jeita.cv);
			charger_dev_set_constant_voltage(info->dvchg1_dev,
							info->sw_jeita.cv);
			return;
		}

	/* dynamic cv*/
	constant_voltage = info->data.battery_cv;
	mtk_get_dynamic_cv(info, &constant_voltage);

	if (!ret && battery_cv < constant_voltage) {
		if (battery_cv == 4430000)
			battery_cv = 4432000;
		else if (battery_cv == 4380000)
			battery_cv = 4384000;
		else if (battery_cv == 4200000)
			battery_cv = 4224000;
		else
			battery_cv = 4432000;
		chr_err("[%s] fixed constant_voltage:%d\n", __func__, battery_cv);
		charger_dev_set_constant_voltage(info->chg1_dev, battery_cv);
		charger_dev_set_constant_voltage(info->dvchg1_dev, battery_cv);
		return;
	}
	charger_dev_set_constant_voltage(info->chg1_dev, constant_voltage);
	charger_dev_set_constant_voltage(info->dvchg1_dev, constant_voltage);
}

static void swchg_turn_on_charging(struct charger_manager *info)
{
	struct switch_charging_alg_data *swchgalg = info->algorithm_data;
	bool charging_enable = true;

	struct device *dev = NULL;
	struct device_node *boot_node = NULL;
	struct tag_bootmode *tag = NULL;
	int boot_mode = 11;//UNKNOWN_BOOT

	dev = &(info->pdev->dev);
	if (dev != NULL) {
		boot_node = of_parse_phandle(dev->of_node, "bootmode", 0);
		if (!boot_node) {
			chr_err("%s: failed to get boot mode phandle\n", __func__);
		} else {
			tag = (struct tag_bootmode *)of_get_property(boot_node,
								"atag,boot", NULL);
			if (!tag)
				chr_err("%s: failed to get atag,boot\n", __func__);
			else
				boot_mode = tag->bootmode;
		}
	}

	if (swchgalg->state == CHR_ERROR) {
		charging_enable = false;
		chr_err("[charger]Charger Error, turn OFF charging !\n");
	} else if ((boot_mode == META_BOOT) ||
			(boot_mode == ADVMETA_BOOT)) {
		charging_enable = false;
		info->chg1_data.input_current_limit = 200000; /* 200mA */
	charger_dev_set_input_current(info->chg1_dev,
					info->chg1_data.input_current_limit);
		chr_err("In meta mode, disable charging and set input current limit to 200mA\n");
	} else {
		mtk_pe20_start_algorithm(info);
		if (mtk_pe20_get_is_connect(info) == false)
			mtk_pe_start_algorithm(info);

		swchg_select_cv(info);
		swchg_select_safe_charger_voltage(info);
		swchg_select_charging_current_limit(info);
		if (info->chg1_data.input_current_limit == 0
		    || info->chg1_data.charging_current_limit == 0) {
			charging_enable = false;
			chr_err("[charger]charging current is set 0mA, turn off charging !\n");
		}
	}

	charger_dev_enable(info->chg1_dev, charging_enable);
}

static int mtk_switch_charging_plug_in(struct charger_manager *info)
{
	struct switch_charging_alg_data *swchgalg = info->algorithm_data;

	swchgalg->state = CHR_CC;
	info->polling_interval = CHARGING_INTERVAL;
	swchgalg->disable_charging = false;
	get_monotonic_boottime(&swchgalg->charging_begin_time);

	return 0;
}

static int mtk_switch_charging_plug_out(struct charger_manager *info)
{
	struct switch_charging_alg_data *swchgalg = info->algorithm_data;

	swchgalg->total_charging_time = 0;

	mtk_pe20_set_is_cable_out_occur(info, true);
	mtk_pe_set_is_cable_out_occur(info, true);
	mtk_pdc_plugout(info);

	if (info->enable_pe_5)
		pe50_stop();

	if (info->enable_pe_4)
		pe40_stop();

	info->leave_pe5 = false;
	info->leave_pe4 = false;
	info->leave_pdc = false;

	return 0;
}

static int mtk_switch_charging_do_charging(struct charger_manager *info,
						bool en)
{
	struct switch_charging_alg_data *swchgalg = info->algorithm_data;

	chr_err("%s: en:%d %s\n", __func__, en, info->algorithm_name);
	if (en) {
		swchgalg->disable_charging = false;
		swchgalg->state = CHR_CC;
		get_monotonic_boottime(&swchgalg->charging_begin_time);
		charger_manager_notifier(info, CHARGER_NOTIFY_NORMAL);
	} else {
		/* disable charging might change state, so call it first */
		_disable_all_charging(info);
		swchgalg->disable_charging = true;
		swchgalg->state = CHR_ERROR;
		charger_manager_notifier(info, CHARGER_NOTIFY_ERROR);
	}

	return 0;
}

static int mtk_switch_chr_pe50_init(struct charger_manager *info)
{
	int ret;

	ret = pe50_init();

	if (ret == 0)
		set_charger_manager(info);
	else
		chr_err("pe50 init fail\n");

	info->leave_pe5 = false;

	return ret;
}

static int mtk_switch_chr_pe50_run(struct charger_manager *info)
{
	struct switch_charging_alg_data *swchgalg = info->algorithm_data;
	/* struct charger_custom_data *pdata = &info->data; */
	/* struct pe50_data *data; */
	int ret = 0;

	if (info->enable_hv_charging == false)
		goto stop;

	ret = pe50_run();

	if (ret == 1) {
		pr_info("retry pe5\n");
		goto retry;
	}

	if (ret == 2) {
		chr_err("leave pe5\n");
		info->leave_pe5 = true;
		swchgalg->state = CHR_CC;
	}

	return 0;

stop:
	pe50_stop();
retry:
	swchgalg->state = CHR_CC;

	return 0;
}


static int mtk_switch_chr_pe40_init(struct charger_manager *info)
{
	int ret;

	ret = pe40_init();

	if (ret == 0)
		set_charger_manager(info);

	info->leave_pe4 = false;

	return 0;
}

static int select_pe40_charging_current_limit(struct charger_manager *info)
{
	struct charger_data *pdata;
	u32 ichg1_min = 0, aicr1_min = 0;
	int ret = 0;

	pdata = &info->chg1_data;

	pdata->input_current_limit =
		info->data.pe40_single_charger_input_current;
	pdata->charging_current_limit =
		info->data.pe40_single_charger_current;

	sc_select_charging_current(info, pdata);

	if (pdata->thermal_input_current_limit != -1) {
		if (pdata->thermal_input_current_limit <
		    pdata->input_current_limit)
			pdata->input_current_limit =
					pdata->thermal_input_current_limit;
	}

	ret = charger_dev_get_min_charging_current(info->chg1_dev, &ichg1_min);
	if (ret != -ENOTSUPP && pdata->charging_current_limit < ichg1_min)
		pdata->charging_current_limit = 0;

	ret = charger_dev_get_min_input_current(info->chg1_dev, &aicr1_min);
	if (ret != -ENOTSUPP && pdata->input_current_limit < aicr1_min)
		pdata->input_current_limit = 0;

	chr_err("force:%d thermal:%d,%d setting:%d %d sc:%d %d %d type:%d usb_unlimited:%d usbif:%d usbsm:%d aicl:%d atm:%d\n",
		_uA_to_mA(pdata->force_charging_current),
		_uA_to_mA(pdata->thermal_input_current_limit),
		_uA_to_mA(pdata->thermal_charging_current_limit),
		_uA_to_mA(pdata->input_current_limit),
		_uA_to_mA(pdata->charging_current_limit),
		info->sc.pre_ibat,
		info->sc.sc_ibat,
		info->sc.solution,
		info->chr_type, info->usb_unlimited,
		IS_ENABLED(CONFIG_USBIF_COMPLIANCE), info->usb_state,
		pdata->input_current_limit_by_aicl, info->atm_enabled);

	return 0;
}

static int mtk_switch_chr_pe40_run(struct charger_manager *info)
{
	struct charger_custom_data *pdata = &info->data;
	struct switch_charging_alg_data *swchgalg = info->algorithm_data;
	struct pe40_data *data = NULL;
	int ret = 0;

	charger_dev_enable(info->chg1_dev, true);
	select_pe40_charging_current_limit(info);

	data = pe40_get_data();
	if (!data) {
		chr_err("%s: data is NULL\n", __func__);
		goto stop;
	}

	data->input_current_limit = info->chg1_data.input_current_limit;
	data->charging_current_limit = info->chg1_data.charging_current_limit;
	data->pe40_max_vbus = pdata->pe40_max_vbus;
	data->high_temp_to_leave_pe40 = pdata->high_temp_to_leave_pe40;
	data->high_temp_to_enter_pe40 = pdata->high_temp_to_enter_pe40;
	data->low_temp_to_leave_pe40 = pdata->low_temp_to_leave_pe40;
	data->low_temp_to_enter_pe40 = pdata->low_temp_to_enter_pe40;
	data->pe40_r_cable_1a_lower = pdata->pe40_r_cable_1a_lower;
	data->pe40_r_cable_2a_lower = pdata->pe40_r_cable_2a_lower;
	data->pe40_r_cable_3a_lower = pdata->pe40_r_cable_3a_lower;

	data->battery_cv = pdata->battery_cv;
	if (info->enable_sw_jeita) {
		if (info->sw_jeita.cv != 0)
			data->battery_cv = info->sw_jeita.cv;
	}

	if (info->enable_hv_charging == false)
		goto stop;
	if (info->pd_reset == true) {
		chr_err("encounter hard reset, stop pe4.0\n");
		info->pd_reset = false;
		goto stop;
	}

	ret = pe40_run();

	if (ret == 1) {
		chr_err("retry pe4\n");
		goto retry;
	}

	if (ret == 2 &&
		info->chg1_data.thermal_charging_current_limit == -1 &&
		info->chg1_data.thermal_input_current_limit == -1) {
		chr_err("leave pe4\n");
		info->leave_pe4 = true;
		swchgalg->state = CHR_CC;
	}

	return 0;

stop:
	pe40_stop();
retry:
	swchgalg->state = CHR_CC;

	return 0;
}


static int mtk_switch_chr_pdc_init(struct charger_manager *info)
{
	int ret;

	ret = pdc_init();

	if (ret == 0)
		set_charger_manager(info);

	info->leave_pdc = false;

	return 0;
}

static int select_pdc_charging_current_limit(struct charger_manager *info)
{
	struct charger_data *pdata;
	u32 ichg1_min = 0, aicr1_min = 0;
	int ret = 0;

	pdata = &info->chg1_data;

	pdata->input_current_limit =
		info->data.pd_charger_current;
	pdata->charging_current_limit =
		info->data.pd_charger_current;

	sc_select_charging_current(info, pdata);

	if (pdata->thermal_input_current_limit != -1) {
		if (pdata->thermal_input_current_limit <
		    pdata->input_current_limit)
			pdata->input_current_limit =
					pdata->thermal_input_current_limit;
	}

	ret = charger_dev_get_min_charging_current(info->chg1_dev, &ichg1_min);
	if (ret != -ENOTSUPP && pdata->charging_current_limit < ichg1_min)
		pdata->charging_current_limit = 0;

	ret = charger_dev_get_min_input_current(info->chg1_dev, &aicr1_min);
	if (ret != -ENOTSUPP && pdata->input_current_limit < aicr1_min)
		pdata->input_current_limit = 0;

	chr_err("force:%d thermal:%d,%d setting:%d %d sc:%d %d %d type:%d usb_unlimited:%d usbif:%d usbsm:%d aicl:%d atm:%d\n",
		_uA_to_mA(pdata->force_charging_current),
		_uA_to_mA(pdata->thermal_input_current_limit),
		_uA_to_mA(pdata->thermal_charging_current_limit),
		_uA_to_mA(pdata->input_current_limit),
		_uA_to_mA(pdata->charging_current_limit),
		info->sc.pre_ibat,
		info->sc.sc_ibat,
		info->sc.solution,
		info->chr_type, info->usb_unlimited,
		IS_ENABLED(CONFIG_USBIF_COMPLIANCE), info->usb_state,
		pdata->input_current_limit_by_aicl, info->atm_enabled);

	return 0;
}

static int mtk_switch_chr_pdc_run(struct charger_manager *info)
{
	struct charger_custom_data *pdata = &info->data;
	struct switch_charging_alg_data *swchgalg = info->algorithm_data;
	struct pdc_data *data = NULL;
	int ret = 0;

	charger_dev_enable(info->chg1_dev, true);
	select_pdc_charging_current_limit(info);

	data = pdc_get_data();

	data->input_current_limit = info->chg1_data.input_current_limit;
	data->charging_current_limit = info->chg1_data.charging_current_limit;
	data->pd_vbus_low_bound = pdata->pd_vbus_low_bound;
	data->pd_vbus_upper_bound = pdata->pd_vbus_upper_bound;

	data->battery_cv = pdata->battery_cv;
	if (info->enable_sw_jeita) {
		if (info->sw_jeita.cv != 0)
			data->battery_cv = info->sw_jeita.cv;
	}

	if (info->enable_hv_charging == false)
		goto stop;
	info->is_pdc_run = true;
	ret = pdc_run();

	if (ret == 2 &&
		info->chg1_data.thermal_charging_current_limit == -1 &&
		info->chg1_data.thermal_input_current_limit == -1) {
		chr_err("leave pdc\n");
		info->leave_pdc = true;
		info->is_pdc_run = false;
		swchgalg->state = CHR_CC;
	}

	return 0;

stop:
	pdc_stop();
	swchgalg->state = CHR_CC;
	info->is_pdc_run = false;
	return 0;
}


/* return false if total charging time exceeds max_charging_time */
static bool mtk_switch_check_charging_time(struct charger_manager *info)
{
	struct switch_charging_alg_data *swchgalg = info->algorithm_data;
	struct timespec time_now;

	if (info->enable_sw_safety_timer) {
		get_monotonic_boottime(&time_now);
		chr_debug("%s: begin: %ld, now: %ld\n", __func__,
			swchgalg->charging_begin_time.tv_sec, time_now.tv_sec);

		if (swchgalg->total_charging_time >=
		    info->data.max_charging_time) {
			chr_err("%s: SW safety timeout: %d sec > %d sec\n",
				__func__, swchgalg->total_charging_time,
				info->data.max_charging_time);
			charger_dev_notify(info->chg1_dev,
					CHARGER_DEV_NOTIFY_SAFETY_TIMEOUT);
			return false;
		}
	}

	return true;
}

static int mtk_switch_chr_cc(struct charger_manager *info)
{
	bool chg_done = false;
	struct switch_charging_alg_data *swchgalg = info->algorithm_data;
	struct timespec time_now, charging_time;
	int tmp = battery_get_bat_temperature();

	/* check bif */
	if (IS_ENABLED(CONFIG_MTK_BIF_SUPPORT)) {
		if (pmic_is_bif_exist() != 1) {
			chr_err("CONFIG_MTK_BIF_SUPPORT but no bif , stop charging\n");
			swchgalg->state = CHR_ERROR;
			charger_manager_notifier(info, CHARGER_NOTIFY_ERROR);
		}
	}

	get_monotonic_boottime(&time_now);
	charging_time = timespec_sub(time_now, swchgalg->charging_begin_time);

	swchgalg->total_charging_time = charging_time.tv_sec;

	chr_err("pe40_ready:%d pps:%d hv:%d thermal:%d,%d tmp:%d,%d,%d\n",
		info->enable_pe_4,
		pe40_is_ready(),
		info->enable_hv_charging,
		info->chg1_data.thermal_charging_current_limit,
		info->chg1_data.thermal_input_current_limit,
		tmp,
		info->data.high_temp_to_enter_pe40,
		info->data.low_temp_to_enter_pe40);

	if (info->enable_pe_5 && pe50_is_ready() && !info->leave_pe5) {
		if (info->enable_hv_charging == true) {
			chr_err("enter PE5.0\n");
			swchgalg->state = CHR_PE50;
			info->pe5.online = true;
			if (mtk_pe20_get_is_enable(info)) {
				mtk_pe20_set_is_enable(info, false);
				if (mtk_pe20_get_is_connect(info))
					mtk_pe20_reset_ta_vchr(info);
			}

			if (mtk_pe_get_is_enable(info)) {
				mtk_pe_set_is_enable(info, false);
				if (mtk_pe_get_is_connect(info))
					mtk_pe_reset_ta_vchr(info);
			}
			return 1;
		}
	}

	if (info->enable_pe_4 &&
		pe40_is_ready() &&
		!info->leave_pe4) {
		if (info->enable_hv_charging == true &&
			info->chg1_data.thermal_charging_current_limit == -1 &&
			info->chg1_data.thermal_input_current_limit == -1) {
			chr_err("enter PE4.0!\n");
			swchgalg->state = CHR_PE40;
			if (mtk_pe20_get_is_enable(info)) {
				mtk_pe20_set_is_enable(info, false);
				if (mtk_pe20_get_is_connect(info))
					mtk_pe20_reset_ta_vchr(info);
			}

			if (mtk_pe_get_is_enable(info)) {
				mtk_pe_set_is_enable(info, false);
				if (mtk_pe_get_is_connect(info))
					mtk_pe_reset_ta_vchr(info);
			}
			return 1;
		}
	}

	if (pdc_is_ready() &&
		!info->leave_pdc) {
		if (info->enable_hv_charging == true) {
			chr_err("enter PDC!\n");
			swchgalg->state = CHR_PDC;
			if (mtk_pe20_get_is_enable(info)) {
				mtk_pe20_set_is_enable(info, false);
				if (mtk_pe20_get_is_connect(info))
					mtk_pe20_reset_ta_vchr(info);
			}

			if (mtk_pe_get_is_enable(info)) {
				mtk_pe_set_is_enable(info, false);
				if (mtk_pe_get_is_connect(info))
					mtk_pe_reset_ta_vchr(info);
			}
			return 1;
		}
	}

	swchg_turn_on_charging(info);

	charger_dev_is_charging_done(info->chg1_dev, &chg_done);
	if (chg_done) {
		swchgalg->state = CHR_BATFULL;
		charger_dev_do_event(info->chg1_dev, EVENT_EOC, 0);
		/* stop dvchg1 */
		charger_dev_enable_hz(info->dvchg1_dev, true);
		chr_err("battery full!\n");
	}

	/* If it is not disabled by throttling,
	 * enable PE+/PE+20, if it is disabled
	 */
	if (info->chg1_data.thermal_input_current_limit != -1 &&
		info->chg1_data.thermal_input_current_limit < 300)
		return 0;

	if (!mtk_pe20_get_is_enable(info)) {
		mtk_pe20_set_is_enable(info, true);
		mtk_pe20_set_to_check_chr_type(info, true);
	}

	if (!mtk_pe_get_is_enable(info)) {
		mtk_pe_set_is_enable(info, true);
		mtk_pe_set_to_check_chr_type(info, true);
	}
	return 0;
}

static int mtk_switch_chr_err(struct charger_manager *info)
{
	struct switch_charging_alg_data *swchgalg = info->algorithm_data;

	if (info->enable_sw_jeita) {
		if ((info->sw_jeita.sm == TEMP_BELOW_T0) ||
			(info->sw_jeita.sm == TEMP_ABOVE_T4))
			info->sw_jeita.error_recovery_flag = false;

		if ((info->sw_jeita.error_recovery_flag == false) &&
			(info->sw_jeita.sm != TEMP_BELOW_T0) &&
			(info->sw_jeita.sm != TEMP_ABOVE_T4)) {
			info->sw_jeita.error_recovery_flag = true;
			swchgalg->state = CHR_CC;
			get_monotonic_boottime(&swchgalg->charging_begin_time);
		}
	}

	swchgalg->total_charging_time = 0;

	_disable_all_charging(info);
	return 0;
}

static int mtk_switch_chr_full(struct charger_manager *info)
{
	bool chg_done = false;
	struct switch_charging_alg_data *swchgalg = info->algorithm_data;

	swchgalg->total_charging_time = 0;

	/* turn off LED */

	/*
	 * If CV is set to lower value by JEITA,
	 * Reset CV to normal value if temperture is in normal zone
	 */
	swchg_select_cv(info);
	info->polling_interval = CHARGING_FULL_INTERVAL;
	charger_dev_is_charging_done(info->chg1_dev, &chg_done);
	if (!chg_done) {
		swchgalg->state = CHR_CC;
		charger_dev_do_event(info->chg1_dev, EVENT_RECHARGE, 0);
		mtk_pe20_set_to_check_chr_type(info, true);
		mtk_pe_set_to_check_chr_type(info, true);
		info->enable_dynamic_cv = true;
		get_monotonic_boottime(&swchgalg->charging_begin_time);
		chr_err("battery recharging!\n");
		info->polling_interval = CHARGING_INTERVAL;
	}

	return 0;
}

static int mtk_switch_charging_current(struct charger_manager *info)
{
	swchg_select_charging_current_limit(info);
	return 0;
}

static int mtk_switch_charging_run(struct charger_manager *info)
{
	struct switch_charging_alg_data *swchgalg = info->algorithm_data;
	int ret = 0;

	chr_err("%s [%d %d], timer=%d\n", __func__, swchgalg->state,
		info->pd_type,
		swchgalg->total_charging_time);

	if (mtk_pdc_check_charger(info) == false &&
	    mtk_is_TA_support_pd_pps(info) == false) {
		mtk_pe20_check_charger(info);
		if (mtk_pe20_get_is_connect(info) == false)
			mtk_pe_check_charger(info);
	}

	do {
		switch (swchgalg->state) {
			chr_err("%s_2 [%d] %d\n", __func__, swchgalg->state,
				info->pd_type);
		case CHR_CC:
			ret = mtk_switch_chr_cc(info);
			break;

		case CHR_PE50:
			ret = mtk_switch_chr_pe50_run(info);
			break;

		case CHR_PE40:
			ret = mtk_switch_chr_pe40_run(info);
			break;

		case CHR_PDC:
			ret = mtk_switch_chr_pdc_run(info);
			break;

		case CHR_BATFULL:
			ret = mtk_switch_chr_full(info);
			break;

		case CHR_ERROR:
			ret = mtk_switch_chr_err(info);
			break;
		}
	} while (ret != 0);
	mtk_switch_check_charging_time(info);

	charger_dev_dump_registers(info->chg1_dev);
	charger_dev_dump_registers(info->dvchg1_dev);
	return 0;
}

static int charger_dev_event(struct notifier_block *nb,
	unsigned long event, void *v)
{
	struct charger_manager *info =
			container_of(nb, struct charger_manager, chg1_nb);
	struct chgdev_notify *data = v;

	chr_info("%s %ld", __func__, event);

	switch (event) {
	case CHARGER_DEV_NOTIFY_EOC:
		if (info->sw_jeita.cv != 0 && info->sw_jeita.cv < 4200000) {
			charger_manager_notifier(info, CHARGER_NOTIFY_STOP_CHARGING);
			pr_info("%s: high temperature end of charge\n", __func__);
		} else {
			charger_manager_notifier(info, CHARGER_NOTIFY_EOC);
			pr_info("%s: end of charge\n", __func__);
		}
		break;
	case CHARGER_DEV_NOTIFY_RECHG:
		charger_manager_notifier(info, CHARGER_NOTIFY_START_CHARGING);
		pr_info("%s: recharge\n", __func__);
		break;
	case CHARGER_DEV_NOTIFY_SAFETY_TIMEOUT:
		info->safety_timeout = true;
		chr_err("%s: safety timer timeout\n", __func__);

		/* If sw safety timer timeout, do not wake up charger thread */
		if (info->enable_sw_safety_timer)
			return NOTIFY_DONE;
		break;
	case CHARGER_DEV_NOTIFY_VBUS_OVP:
		info->vbusov_stat = data->vbusov_stat;
		chr_err("%s: vbus ovp = %d\n", __func__, info->vbusov_stat);
		break;
	default:
		return NOTIFY_DONE;
	}

	if (info->chg1_dev->is_polling_mode == false)
		_wake_up_charger(info);

	return NOTIFY_DONE;
}

static int dvchg1_dev_event(struct notifier_block *nb, unsigned long event,
			    void *data)
{
	struct charger_manager *info =
			container_of(nb, struct charger_manager, dvchg1_nb);

	chr_info("%s %ld", __func__, event);

	return mtk_pe50_notifier_call(info, MTK_PE50_NOTISRC_CHG, event, data);
}

static int dvchg2_dev_event(struct notifier_block *nb, unsigned long event,
			    void *data)
{
	struct charger_manager *info =
			container_of(nb, struct charger_manager, dvchg2_nb);

	chr_info("%s %ld", __func__, event);

	return mtk_pe50_notifier_call(info, MTK_PE50_NOTISRC_CHG, event, data);
}

int mtk_switch_charging_init2(struct charger_manager *info)
{
	struct switch_charging_alg_data *swch_alg;

	swch_alg = devm_kzalloc(&info->pdev->dev,
				sizeof(*swch_alg), GFP_KERNEL);
	if (!swch_alg)
		return -ENOMEM;

	info->chg1_dev = get_charger_by_name("primary_chg");
	if (info->chg1_dev)
		chr_err("Found primary charger [%s]\n",
			info->chg1_dev->props.alias_name);
	else
		chr_err("*** Error : can't find primary charger ***\n");

	info->dvchg1_dev = get_charger_by_name("primary_divider_chg");
	if (info->dvchg1_dev) {
		chr_err("Found primary divider charger [%s]\n",
			info->dvchg1_dev->props.alias_name);
		info->dvchg1_nb.notifier_call = dvchg1_dev_event;
		register_charger_device_notifier(info->dvchg1_dev,
						 &info->dvchg1_nb);
	} else
		chr_err("Can't find primary divider charger\n");
	info->dvchg2_dev = get_charger_by_name("secondary_divider_chg");
	if (info->dvchg2_dev) {
		chr_err("Found secondary divider charger [%s]\n",
			info->dvchg2_dev->props.alias_name);
		info->dvchg2_nb.notifier_call = dvchg2_dev_event;
		register_charger_device_notifier(info->dvchg2_dev,
						 &info->dvchg2_nb);
	} else
		chr_err("Can't find secondary divider charger\n");

	mutex_init(&swch_alg->ichg_aicr_access_mutex);

	info->algorithm_data = swch_alg;
	info->do_algorithm = mtk_switch_charging_run;
	info->plug_in = mtk_switch_charging_plug_in;
	info->plug_out = mtk_switch_charging_plug_out;
	info->do_charging = mtk_switch_charging_do_charging;
	info->do_event = charger_dev_event;
	info->change_current_setting = mtk_switch_charging_current;

	mtk_switch_chr_pe50_init(info);
	mtk_switch_chr_pe40_init(info);
	mtk_switch_chr_pdc_init(info);

	return 0;
}
