/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2021 MediaTek Inc.
*/

#include <generated/autoconf.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/device.h>
#include <linux/pm_wakeup.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/writeback.h>
#include <linux/seq_file.h>
#include <linux/power_supply.h>
#include <linux/time.h>
#include <linux/uaccess.h>
#include <linux/reboot.h>

#include <linux/of.h>
#include <linux/extcon.h>

#include <mt-plat/upmu_common.h>
#include <mach/upmu_sw.h>
#include <mach/upmu_hw.h>
#include <mt-plat/mtk_boot.h>
#include <mt-plat/v1/charger_type.h>
#include <mt-plat/v1/mtk_charger.h>
#include <pmic.h>
#include <tcpm.h>

#include "mtk_charger_intf.h"

int check_cable_in;
struct tag_bootmode {
	u32 size;
	u32 tag;
	u32 bootmode;
	u32 boottype;
};

#ifdef CONFIG_EXTCON_USB_CHG
struct usb_extcon_info {
	struct device *dev;
	struct extcon_dev *edev;

	unsigned int vbus_state;
	unsigned long debounce_jiffies;
	struct delayed_work wq_detcable;
};

static const unsigned int usb_extcon_cable[] = {
	EXTCON_USB,
	EXTCON_USB_HOST,
	EXTCON_NONE,
};
#endif

void __attribute__((weak)) fg_charger_in_handler(void)
{
	pr_notice("%s not defined\n", __func__);
}

struct chg_type_info {
	struct device *dev;
	struct charger_consumer *chg_consumer;
	struct tcpc_device *tcpc;
	struct notifier_block pd_nb;
	bool tcpc_kpoc;
	/* Charger Detection */
	struct mutex chgdet_lock;
	bool chgdet_en;
	atomic_t chgdet_cnt;
	wait_queue_head_t waitq;
	struct task_struct *chgdet_task;
	struct workqueue_struct *pwr_off_wq;
	struct work_struct pwr_off_work;
	struct workqueue_struct *chg_in_wq;
	struct work_struct chg_in_work;
	bool ignore_usb;
	bool plugin;
	bool bypass_chgdet;
#ifdef CONFIG_MACH_MT6771
	struct power_supply *chr_psy;
	struct notifier_block psy_nb;
#endif
};

#ifdef CONFIG_FPGA_EARLY_PORTING
/*  FPGA */
int hw_charging_get_charger_type(void)
{
	return STANDARD_HOST;
}

#else

/* EVB / Phone */
static const char * const mtk_chg_type_name[] = {
	"Charger Unknown",
	"Standard USB Host",
	"Charging USB Host",
	"Non-standard Charger",
	"Standard Charger",
	"Apple 2.1A Charger",
	"Apple 1.0A Charger",
	"Apple 0.5A Charger",
	"Wireless Charger",
};

static void dump_charger_name(enum charger_type type)
{
	switch (type) {
	case CHARGER_UNKNOWN:
	case STANDARD_HOST:
	case CHARGING_HOST:
	case NONSTANDARD_CHARGER:
	case STANDARD_CHARGER:
	case APPLE_2_1A_CHARGER:
	case APPLE_1_0A_CHARGER:
	case APPLE_0_5A_CHARGER:
		pr_info("%s: charger type: %d, %s\n", __func__, type,
			mtk_chg_type_name[type]);
		break;
	default:
		pr_info("%s: charger type: %d, Not Defined!!!\n", __func__,
			type);
		break;
	}
}

/* Power Supply */
struct mt_charger {
	struct device *dev;
	struct power_supply_desc chg_desc;
	struct power_supply_config chg_cfg;
	struct power_supply *chg_psy;
	struct power_supply_desc ac_desc;
	struct power_supply_config ac_cfg;
	struct power_supply *ac_psy;
	struct power_supply_desc usb_desc;
	struct power_supply_config usb_cfg;
	struct power_supply *usb_psy;
	struct chg_type_info *cti;
	#ifdef CONFIG_EXTCON_USB_CHG
	struct usb_extcon_info *extcon_info;
	struct delayed_work extcon_work;
	#endif
	bool chg_online; /* Has charger in or not */
	enum charger_type chg_type;
	int typec_cc_orientation;
};

void kpoc_wait_power_off(void)
{
	int i = 0;

	for (i = 0; i < 10; i++) {
		pr_info("%s, wait %d\n", __func__, i);
		msleep(1000);
		if (mt_charger_plugin())
			return;
	}

	kernel_power_off();
}

static int mt_charger_online(struct mt_charger *mtk_chg)
{
	int ret = 0;
	struct device *dev = NULL;
	struct device_node *boot_node = NULL;
	struct tag_bootmode *tag = NULL;
	int boot_mode = 11;//UNKNOWN_BOOT
	dev = mtk_chg->dev;
	if (dev != NULL){
		boot_node = of_parse_phandle(dev->of_node, "bootmode", 0);
		if (!boot_node){
			chr_err("%s: failed to get boot mode phandle\n", __func__);
		}
		else {
			tag = (struct tag_bootmode *)of_get_property(boot_node,
								"atag,boot", NULL);
			if (!tag){
				chr_err("%s: failed to get atag,boot\n", __func__);
			}
			else
				boot_mode = tag->bootmode;
		}
	}

	if (!mtk_chg->chg_online) {
// workaround for mt6768
		//boot_mode = get_boot_mode();
		if (boot_mode == KERNEL_POWER_OFF_CHARGING_BOOT ||
		    boot_mode == LOW_POWER_OFF_CHARGING_BOOT) {
			pr_notice("%s: Unplug Charger/USB\n", __func__);
			pr_notice("%s: system_state=%d\n", __func__,
				system_state);
			if (system_state != SYSTEM_POWER_OFF) {
				kpoc_wait_power_off();
			}
		}
	}

	return ret;
}

int mt_check_cable_in(void)
{
	return check_cable_in;
}

/* Power Supply Functions */
static int mt_charger_get_property(struct power_supply *psy,
	enum power_supply_property psp, union power_supply_propval *val)
{
	struct mt_charger *mtk_chg = power_supply_get_drvdata(psy);

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = 0;
		/* Force to 1 in all charger type */
		if (mtk_chg->chg_type != CHARGER_UNKNOWN)
			val->intval = 1;
		break;
	case POWER_SUPPLY_PROP_CHARGE_TYPE:
		val->intval = mtk_chg->chg_type;
		break;
	case POWER_SUPPLY_PROP_SHIP_MODE:
		val->intval = 0;
		break;
	case POWER_SUPPLY_PROP_USB_TYPE:
		switch (mtk_chg->chg_type) {
		case STANDARD_HOST:
			val->intval = POWER_SUPPLY_USB_TYPE_SDP;
			pr_info("%s: Charger Type: STANDARD_HOST\n", __func__);
			break;
		case NONSTANDARD_CHARGER:
			val->intval = POWER_SUPPLY_USB_TYPE_DCP;
			pr_info("%s: Charger Type: NONSTANDARD_CHARGER\n", __func__);
			break;
		case CHARGING_HOST:
			val->intval = POWER_SUPPLY_USB_TYPE_CDP;
			pr_info("%s: Charger Type: CHARGING_HOST\n", __func__);
			break;
		case STANDARD_CHARGER:
			val->intval = POWER_SUPPLY_USB_TYPE_DCP;
			pr_info("%s: Charger Type: STANDARD_CHARGER\n", __func__);
			break;
		case CHARGER_UNKNOWN:
			val->intval = POWER_SUPPLY_USB_TYPE_UNKNOWN;
			pr_info("%s: Charger Type: CHARGER_UNKNOWN\n", __func__);
			break;
		default:
		break;
	}
	default:
		return -EINVAL;
	}

	return 0;
}

static int mt_charger_property_is_writeable(struct power_supply *psy,
	enum power_supply_property psp)
{
	int ret = 0;

	switch (psp) {
	case POWER_SUPPLY_PROP_SHIP_MODE:
		ret = 1;
		break;
	default:
		break;
	}

	return ret;
}

#ifdef CONFIG_EXTCON_USB_CHG
static void usb_extcon_detect_cable(struct work_struct *work)
{
	struct usb_extcon_info *info = container_of(to_delayed_work(work),
						struct usb_extcon_info,
						wq_detcable);

	/* check and update cable state */
	if (info->vbus_state)
		extcon_set_state_sync(info->edev, EXTCON_USB, true);
	else
		extcon_set_state_sync(info->edev, EXTCON_USB, false);
}
#endif

extern int charger_dev_enable_shipping_mode(struct charger_device *chg_dev, bool en);
static int mt_charger_set_property(struct power_supply *psy,
	enum power_supply_property psp, const union power_supply_propval *val)
{
	struct mt_charger *mtk_chg = power_supply_get_drvdata(psy);
	struct chg_type_info *cti = NULL;
	struct charger_device *chg_dev, *chg_dev1;
	#ifdef CONFIG_EXTCON_USB_CHG
	struct usb_extcon_info *info;
	#endif

	pr_info("%s\n", __func__);

	if (!mtk_chg) {
		pr_notice("%s: no mtk chg data\n", __func__);
		return -EINVAL;
	}

#ifdef CONFIG_EXTCON_USB_CHG
	info = mtk_chg->extcon_info;
#endif

	cti = mtk_chg->cti;
	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		mtk_chg->chg_online = val->intval;
		mt_charger_online(mtk_chg);
		return 0;
	case POWER_SUPPLY_PROP_CHARGE_TYPE:
		mtk_chg->chg_type = val->intval;
		if (!cti->plugin) {
			mtk_chg->chg_type = CHARGER_UNKNOWN;
			pr_warn("[%s] no charger detected, force set "
				" chr_type CHARGER_UNKNOWN!", __func__);
		}
		if (mtk_chg->chg_type != CHARGER_UNKNOWN)
			charger_manager_force_disable_power_path(
				cti->chg_consumer, MAIN_CHARGER, false);
		else if (!cti->tcpc_kpoc)
			charger_manager_force_disable_power_path(
				cti->chg_consumer, MAIN_CHARGER, true);
		break;
	case POWER_SUPPLY_PROP_SHIP_MODE:
		chg_dev = get_charger_by_name("primary_chg");
		chg_dev1 = get_charger_by_name("primary_divider_chg");
		if (chg_dev1)
			charger_dev_enable_shipping_mode(chg_dev1, true);
		if (chg_dev)
			charger_dev_enable_shipping_mode(chg_dev, true);
		return 0;
	default:
		return -EINVAL;
	}

	dump_charger_name(mtk_chg->chg_type);

	if (!cti->ignore_usb) {
		/* usb */
		if ((mtk_chg->chg_type == STANDARD_HOST) ||
//+Bug10578, zhouyusong01.wt, modify, enable type pd 5V to transfer data
			(mtk_chg->chg_type == CHARGER_PD_5V) ||
//-Bug10578, zhouyusong01.wt, modify, enable type pd 5V to transfer data
			(mtk_chg->chg_type == CHARGING_HOST) /*||
			(mtk_chg->chg_type == NONSTANDARD_CHARGER)*/) {
				mt_usb_connect_v1();
			#ifdef CONFIG_EXTCON_USB_CHG
			info->vbus_state = 1;
			#endif
		} else {
			mt_usb_disconnect_v1();
			#ifdef CONFIG_EXTCON_USB_CHG
			info->vbus_state = 0;
			#endif
		}
	}

	queue_work(cti->chg_in_wq, &cti->chg_in_work);
	#ifdef CONFIG_EXTCON_USB_CHG
	if (!IS_ERR(info->edev))
		queue_delayed_work(system_power_efficient_wq,
			&info->wq_detcable, info->debounce_jiffies);
	#endif

#ifdef CONFIG_MACH_MT6771
	power_supply_changed(mtk_chg->chg_psy);
#endif
	power_supply_changed(mtk_chg->ac_psy);
	power_supply_changed(mtk_chg->usb_psy);

	return 0;
}

static int mt_ac_get_property(struct power_supply *psy,
	enum power_supply_property psp, union power_supply_propval *val)
{
	struct mt_charger *mtk_chg = power_supply_get_drvdata(psy);

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		val->intval = 0;
		/* Force to 1 in all charger type */
		if (mtk_chg->chg_type != CHARGER_UNKNOWN)
			val->intval = 1;
		/* Reset to 0 if charger type is USB */
		if ((mtk_chg->chg_type == STANDARD_HOST) ||
			(mtk_chg->chg_type == CHARGING_HOST))
			val->intval = 0;
		break;
	case POWER_SUPPLY_PROP_CURRENT_MAX:
		switch (mtk_chg->chg_type) {
			case CHARGER_PD_5V:
			case CHARGER_PE_5V:
				val->intval = 3000000;
				break;
			case CHARGER_PE_7V:
				val->intval = 2700000;
				break;
			case CHARGER_PE_9V:
			case CHARGER_PD_9V:
				val->intval = 2200000;
				break;
			case CHARGER_PE_12V:
				val->intval =1667000;
				break;
			case CHARGER_NS_1A:
				val->intval = 1000000;
				break;
			case CHARGER_NS_2A:
				val->intval = 2000000;
				break;
			case CHARGER_NS_2_1A:
				val->intval = 2100000;
				break;
			case CHARGER_NS_2_4A:
				val->intval = 2400000;
				break;
			case STANDARD_CHARGER:
				val->intval = 1200000;
				break;
			default:
				val->intval = 500000;
				break;
		}
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_MAX:
		if (mtk_chg->chg_type == CHARGER_PD_9V
			|| mtk_chg->chg_type == CHARGER_PE_9V) {
			val->intval = 9000000;
		} else if (mtk_chg->chg_type == CHARGER_PE_12V) {
			val->intval = 12000000;
		} else if (mtk_chg->chg_type == CHARGER_PE_7V) {
			val->intval = 7000000;
		} else {
			val->intval = 5000000;
		}
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static int mt_usb_set_property(struct power_supply *psy,
	enum power_supply_property psp,const union power_supply_propval *val)
{
	struct mt_charger *mtk_chg = power_supply_get_drvdata(psy);

	switch (psp) {
		case POWER_SUPPLY_PROP_TYPEC_CC_ORIENTATION:
			mtk_chg->typec_cc_orientation = val->intval;
			break;
		default:
			return -EINVAL;
	}

	return 0;
}

static int mt_usb_get_property(struct power_supply *psy,
	enum power_supply_property psp, union power_supply_propval *val)
{
	struct mt_charger *mtk_chg = power_supply_get_drvdata(psy);

	switch (psp) {
	case POWER_SUPPLY_PROP_ONLINE:
		if ((mtk_chg->chg_type == STANDARD_HOST) ||
			(mtk_chg->chg_type == CHARGING_HOST))
			val->intval = 1;
		else
			val->intval = 0;
		break;
	case POWER_SUPPLY_PROP_CURRENT_MAX:
		switch (mtk_chg->chg_type) {
			case STANDARD_HOST:
				val->intval = 500000;
				break;
			case CHARGING_HOST:
				val->intval = 1500000;
				break;
			default:
				val->intval = 0;
				break;
		}
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_MAX:
		val->intval = 5000000;
		break;
	case POWER_SUPPLY_PROP_TYPEC_CC_ORIENTATION:
		val->intval = mtk_chg->typec_cc_orientation;
		break;
	case POWER_SUPPLY_PROP_REAL_TYPE:
		val->intval = mtk_chg->chg_type;
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		val->intval = battery_get_vbus();
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

static enum power_supply_property mt_charger_properties[] = {
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_CHARGE_TYPE,
	POWER_SUPPLY_PROP_SHIP_MODE,
};

static enum power_supply_property mt_ac_properties[] = {
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_CURRENT_MAX,
	POWER_SUPPLY_PROP_VOLTAGE_MAX,
};

static enum power_supply_property mt_usb_properties[] = {
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_CURRENT_MAX,
	POWER_SUPPLY_PROP_VOLTAGE_MAX,
	POWER_SUPPLY_PROP_TYPEC_CC_ORIENTATION,
	POWER_SUPPLY_PROP_REAL_TYPE,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
};

static void tcpc_power_off_work_handler(struct work_struct *work)
{
	pr_info("%s\n", __func__);
	kpoc_wait_power_off();
}

static void charger_in_work_handler(struct work_struct *work)
{
	mtk_charger_int_handler();
	fg_charger_in_handler();
}

#ifdef CONFIG_TCPC_CLASS
static void plug_in_out_handler(struct chg_type_info *cti, bool en, bool ignore)
{
	mutex_lock(&cti->chgdet_lock);
	if (cti->chgdet_en == en)
		goto skip;
	cti->chgdet_en = en;
	cti->ignore_usb = ignore;
	cti->plugin = en;
	check_cable_in = en;
	chr_err("%s, check_cable_in: %d\n", __func__, check_cable_in);
	atomic_inc(&cti->chgdet_cnt);
	wake_up_interruptible(&cti->waitq);
skip:
	mutex_unlock(&cti->chgdet_lock);
}

void access_pd_fixed_current(char ops, int *curr)
{
	static int max_current = -1;

	switch (ops) {
	case 'r':
		*curr = max_current;
		break;
	case 'w':
		max_current = *curr;
		break;
	default:
		break;
	}
}
EXPORT_SYMBOL(access_pd_fixed_current);

static int pd_tcp_notifier_call(struct notifier_block *pnb,
				unsigned long event, void *data)
{
	struct tcp_notify *noti = data;
	struct chg_type_info *cti = container_of(pnb,
		struct chg_type_info, pd_nb);
	int vbus = 0;
	union power_supply_propval propval;
	int tcpm_ret, fixed_curr, select_vol;
	struct tcpm_remote_power_cap pd_cap;
	unsigned char cap_i = 0;
	struct power_supply *usb_psy = power_supply_get_by_name("usb");
	struct power_supply *chrdet_psy = power_supply_get_by_name("charger");

	if (IS_ERR_OR_NULL(usb_psy)) {
		chr_err("%s, fail to get usb_psy\n", __func__);
		return NOTIFY_BAD;
	}

	if (IS_ERR_OR_NULL(chrdet_psy)) {
		chr_err("%s, fail to get charger_psy\n", __func__);
		return NOTIFY_BAD;
	}

	switch (event) {
	case TCP_NOTIFY_SINK_VBUS:
		if (tcpm_inquire_typec_attach_state(cti->tcpc) ==
						   TYPEC_ATTACHED_AUDIO)
			plug_in_out_handler(cti, !!noti->vbus_state.mv, true);
		break;
	case TCP_NOTIFY_TYPEC_STATE:
		if (noti->typec_state.old_state == TYPEC_UNATTACHED &&
		    (noti->typec_state.new_state == TYPEC_ATTACHED_SNK ||
		    noti->typec_state.new_state == TYPEC_ATTACHED_CUSTOM_SRC ||
		    noti->typec_state.new_state == TYPEC_ATTACHED_NORP_SRC)) {
			pr_info("%s USB Plug in, pol = %d\n", __func__,
					noti->typec_state.polarity);
			plug_in_out_handler(cti, true, false);
		} else if ((noti->typec_state.old_state == TYPEC_ATTACHED_SNK ||
		    noti->typec_state.old_state == TYPEC_ATTACHED_CUSTOM_SRC ||
		    noti->typec_state.old_state == TYPEC_ATTACHED_NORP_SRC ||
		    noti->typec_state.old_state == TYPEC_ATTACHED_AUDIO)
			&& noti->typec_state.new_state == TYPEC_UNATTACHED) {
			if (cti->tcpc_kpoc) {
				vbus = battery_get_vbus();
				pr_info("%s KPOC Plug out, vbus = %d\n",
					__func__, vbus);
				plug_in_out_handler(cti, false, false);
				queue_work_on(cpumask_first(cpu_online_mask),
					      cti->pwr_off_wq,
					      &cti->pwr_off_work);
				break;
			}
			pr_info("%s USB Plug out\n", __func__);
			plug_in_out_handler(cti, false, false);
		} else if (noti->typec_state.old_state == TYPEC_ATTACHED_SRC &&
			noti->typec_state.new_state == TYPEC_ATTACHED_SNK) {
			pr_info("%s Source_to_Sink\n", __func__);
			plug_in_out_handler(cti, true, true);
		}  else if (noti->typec_state.old_state == TYPEC_ATTACHED_SNK &&
			noti->typec_state.new_state == TYPEC_ATTACHED_SRC) {
			pr_info("%s Sink_to_Source\n", __func__);
			plug_in_out_handler(cti, false, true);
		}
		if (usb_psy) {
			pr_info("%s TCP_NOTIFY_TYPEC_STATE pol = %d\n", __func__,
					noti->typec_state.polarity);
			if (noti->typec_state.new_state == TYPEC_UNATTACHED ||
					noti->typec_state.new_state == TYPEC_ATTACHED_NORP_SRC)
				propval.intval = 0;
			else
				propval.intval = 1+noti->typec_state.polarity;
			power_supply_set_property(usb_psy,
					POWER_SUPPLY_PROP_TYPEC_CC_ORIENTATION, &propval);
		}
		break;
	case TCP_NOTIFY_PD_STATE:
		switch (noti->pd_state.connected) {
		case PD_CONNECT_PE_READY_SNK:
		case PD_CONNECT_PE_READY_SNK_PD30:
		case PD_CONNECT_PE_READY_SNK_APDO:
			if (!chrdet_psy)
				break;
			tcpm_ret = tcpm_dpm_pd_request(cti->tcpc, 9000, 2200, NULL);
			fixed_curr = 2200;
			if (tcpm_ret) {
				tcpm_ret = tcpm_dpm_pd_request(cti->tcpc, 9000, 2000, NULL);
				fixed_curr = 2000;
			}
			if (!tcpm_ret) {
				access_pd_fixed_current('w', &fixed_curr);
				propval.intval = CHARGER_PD_9V;
				power_supply_set_property(chrdet_psy,
						POWER_SUPPLY_PROP_CHARGE_TYPE, &propval);
				break;
			}
			pd_cap.nr = 0;
			pd_cap.selected_cap_idx = 0;
			tcpm_ret = tcpm_get_remote_power_cap(cti->tcpc, &pd_cap);
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
							tcpm_ret = tcpm_dpm_pd_request(cti->tcpc, select_vol, 3000, NULL);
							fixed_curr = 3000;
						} else {
							tcpm_ret = tcpm_dpm_pd_request(cti->tcpc, select_vol,
									pd_cap.ma[cap_i], NULL);
							fixed_curr = pd_cap.ma[cap_i];
						}
						if (!tcpm_ret) {
							access_pd_fixed_current('w', &fixed_curr);
							propval.intval = CHARGER_PD_5V;
							power_supply_set_property(chrdet_psy,
									POWER_SUPPLY_PROP_CHARGE_TYPE, &propval);
						}
						break;
					}
				}
			}
			pr_info("%s pd state:%d tcpm_ret:%d \n", __func__, noti->pd_state.connected, tcpm_ret);
			break;
		default:
			pr_info("%s pd state:%d\n", __func__, noti->pd_state.connected);
			break;
		}
		break;
	}
	return NOTIFY_OK;
}
#endif

static int chgdet_task_threadfn(void *data)
{
	struct chg_type_info *cti = data;
	bool attach = false, ignore_usb = false;
	int ret = 0;
	struct power_supply *psy = power_supply_get_by_name("charger");
	union power_supply_propval val = {.intval = 0};
	struct device *dev = NULL;
	struct device_node *boot_node = NULL;
	struct tag_bootmode *tag = NULL;
	int boot_mode = 11;//UNKNOWN_BOOT

	if (!psy) {
		pr_notice("%s: power supply get fail\n", __func__);
		return -ENODEV;
	}

	pr_info("%s: ++\n", __func__);
	while (!kthread_should_stop()) {
		ret = wait_event_interruptible(cti->waitq,
					     atomic_read(&cti->chgdet_cnt) > 0);
		if (ret < 0) {
			pr_info("%s: wait event been interrupted(%d)\n",
				__func__, ret);
			continue;
		}

		pm_stay_awake(cti->dev);
		mutex_lock(&cti->chgdet_lock);
		atomic_set(&cti->chgdet_cnt, 0);
		attach = cti->chgdet_en;
		ignore_usb = cti->ignore_usb;
		mutex_unlock(&cti->chgdet_lock);

		if (attach && ignore_usb) {
			cti->bypass_chgdet = true;
			goto bypass_chgdet;
		} else if (!attach && cti->bypass_chgdet) {
			cti->bypass_chgdet = false;
			goto bypass_chgdet;
		}

		dev = cti->dev;
		if (dev != NULL){
			boot_node = of_parse_phandle(dev->of_node, "bootmode", 0);
			if (!boot_node){
				chr_err("%s: failed to get boot mode phandle\n", __func__);
			}
			else {
				tag = (struct tag_bootmode *)of_get_property(boot_node,
									"atag,boot", NULL);
				if (!tag){
					chr_err("%s: failed to get atag,boot\n", __func__);
				}
				else
					boot_mode = tag->bootmode;
			}
		}

		if (1 == boot_mode) { //META_BOOT
			pr_notice("%s: meta mode, force chg_type SDP!\n", __func__);
			val.intval = attach ? STANDARD_HOST : CHARGER_UNKNOWN;
			ret = power_supply_set_property(psy,
					POWER_SUPPLY_PROP_CHARGE_TYPE, &val);
			if (ret < 0)
				pr_notice("%s: power supply set charge type fail(%d)\n",
						__func__, ret);
			goto pm_relax;
		}
#ifdef CONFIG_MTK_EXTERNAL_CHARGER_TYPE_DETECT
		if (cti->chg_consumer)
			charger_manager_enable_chg_type_det(cti->chg_consumer,
							attach);
#else
		mtk_pmic_enable_chr_type_det(attach);
#endif
		goto pm_relax;
bypass_chgdet:
		val.intval = attach;
		ret = power_supply_set_property(psy, POWER_SUPPLY_PROP_ONLINE,
						&val);
		if (ret < 0)
			pr_notice("%s: power supply set online fail(%d)\n",
				  __func__, ret);
		if (tcpm_inquire_typec_attach_state(cti->tcpc) ==
						   TYPEC_ATTACHED_AUDIO)
			val.intval = attach ? NONSTANDARD_CHARGER :
					      CHARGER_UNKNOWN;
		else
			val.intval = attach ? STANDARD_HOST : CHARGER_UNKNOWN;
		ret = power_supply_set_property(psy,
						POWER_SUPPLY_PROP_CHARGE_TYPE,
						&val);
		if (ret < 0)
			pr_notice("%s: power supply set charge type fail(%d)\n",
				  __func__, ret);
pm_relax:
		pm_relax(cti->dev);
	}
	pr_info("%s: --\n", __func__);
	return 0;
}

#ifdef CONFIG_EXTCON_USB_CHG
static void init_extcon_work(struct work_struct *work)
{
	struct delayed_work *dw = to_delayed_work(work);
	struct mt_charger *mt_chg =
		container_of(dw, struct mt_charger, extcon_work);
	struct device_node *node = mt_chg->dev->of_node;
	struct usb_extcon_info *info;

	info = mt_chg->extcon_info;
	if (!info)
		return;

	if (of_property_read_bool(node, "extcon")) {
		info->edev = extcon_get_edev_by_phandle(mt_chg->dev, 0);
		if (IS_ERR(info->edev)) {
			schedule_delayed_work(&mt_chg->extcon_work,
				msecs_to_jiffies(50));
			return;
		}
	}

	INIT_DELAYED_WORK(&info->wq_detcable, usb_extcon_detect_cable);
}
#endif

#ifdef CONFIG_MACH_MT6771
static int mt6370_psy_notifier(struct notifier_block *nb,
				unsigned long event, void *data)
{
	struct power_supply *psy = data;
	struct power_supply *type_psy = NULL;

	struct chg_type_info *cti = container_of(nb,
					struct chg_type_info, psy_nb);

	union power_supply_propval pval;
	int ret;

	if (event != PSY_EVENT_PROP_CHANGED) {
		pr_info("%s, event not equal\n", __func__);
		return NOTIFY_DONE;
	}

	if (IS_ERR_OR_NULL(cti->chr_psy)) {
		cti->chr_psy = power_supply_get_by_name("mt6370_pmu_charger");
		if (IS_ERR_OR_NULL(cti->chr_psy)) {
			pr_info("fail to get chr_psy\n");
			cti->chr_psy = NULL;
			return NOTIFY_DONE;
		}
	} else
		pr_info("%s, get mt6370 psy success, event(%d)\n", __func__, event);

	/*psy is mt6370, type_psy is charger_type psy*/
	if (psy != cti->chr_psy) {
		pr_info("power supply not equal\n");
		return NOTIFY_DONE;
	}

	type_psy = power_supply_get_by_name("charger");
	if (!type_psy) {
		pr_info("%s: get power supply failed\n",
			__func__);
		return NOTIFY_DONE;
	}

	if (event != PSY_EVENT_PROP_CHANGED) {
		pr_info("%s: get event power supply failed\n",
			__func__);
		return NOTIFY_DONE;
	}

	ret = power_supply_get_property(psy,
				POWER_SUPPLY_PROP_ONLINE, &pval);
	if (ret < 0) {
		pr_info("psy failed to get online prop\n");
		return NOTIFY_DONE;
	}

	ret = power_supply_set_property(type_psy, POWER_SUPPLY_PROP_ONLINE,
		&pval);
	if (ret < 0)
		pr_info("%s: type_psy online failed, ret = %d\n",
			__func__, ret);

	ret = power_supply_get_property(psy,
				POWER_SUPPLY_PROP_USB_TYPE, &pval);
	if (ret < 0) {
		pr_info("failed to get usb type prop\n");
		return NOTIFY_DONE;
	}

	switch (pval.intval) {
	case POWER_SUPPLY_USB_TYPE_SDP:
		pval.intval = STANDARD_HOST;
		break;
	case POWER_SUPPLY_USB_TYPE_DCP:
		pval.intval = STANDARD_CHARGER;
		break;
	case POWER_SUPPLY_USB_TYPE_CDP:
		pval.intval = CHARGING_HOST;
		break;
	default:
		pval.intval = CHARGER_UNKNOWN;
		break;
	}

	ret = power_supply_set_property(type_psy, POWER_SUPPLY_PROP_CHARGE_TYPE,
		&pval);
	if (ret < 0)
		pr_info("%s: type_psy type failed, ret = %d\n",
			__func__, ret);

	return NOTIFY_DONE;
}
#endif

static int mt_charger_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct chg_type_info *cti = NULL;
	struct mt_charger *mt_chg = NULL;
	#ifdef CONFIG_EXTCON_USB_CHG
	struct usb_extcon_info *info;
	#endif
	struct device *dev = NULL;
	struct device_node *boot_node = NULL;
	struct tag_bootmode *tag = NULL;
	int boot_mode = 11;//UNKNOWN_BOOT

	check_cable_in = 0;
	chr_err("%s: check_cable_in: %d\n", __func__, check_cable_in);
	pr_info("%s\n", __func__);

	mt_chg = devm_kzalloc(&pdev->dev, sizeof(*mt_chg), GFP_KERNEL);
	if (!mt_chg)
		return -ENOMEM;

	mt_chg->dev = &pdev->dev;
	mt_chg->chg_online = false;
	mt_chg->chg_type = CHARGER_UNKNOWN;
	mt_chg->typec_cc_orientation = 0;

	mt_chg->chg_desc.name = "charger";
	mt_chg->chg_desc.type = POWER_SUPPLY_TYPE_UNKNOWN;
	mt_chg->chg_desc.properties = mt_charger_properties;
	mt_chg->chg_desc.num_properties = ARRAY_SIZE(mt_charger_properties);
	mt_chg->chg_desc.set_property = mt_charger_set_property;
	mt_chg->chg_desc.get_property = mt_charger_get_property;
	mt_chg->chg_desc.property_is_writeable = mt_charger_property_is_writeable;
	mt_chg->chg_cfg.drv_data = mt_chg;

	mt_chg->ac_desc.name = "ac";
	mt_chg->ac_desc.type = POWER_SUPPLY_TYPE_MAINS;
	mt_chg->ac_desc.properties = mt_ac_properties;
	mt_chg->ac_desc.num_properties = ARRAY_SIZE(mt_ac_properties);
	mt_chg->ac_desc.get_property = mt_ac_get_property;
	mt_chg->ac_cfg.drv_data = mt_chg;

	mt_chg->usb_desc.name = "usb";
	mt_chg->usb_desc.type = POWER_SUPPLY_TYPE_USB;
	mt_chg->usb_desc.properties = mt_usb_properties;
	mt_chg->usb_desc.num_properties = ARRAY_SIZE(mt_usb_properties);
	mt_chg->usb_desc.get_property = mt_usb_get_property;
	mt_chg->usb_desc.set_property = mt_usb_set_property;
	mt_chg->usb_cfg.drv_data = mt_chg;

	mt_chg->chg_psy = power_supply_register(&pdev->dev,
		&mt_chg->chg_desc, &mt_chg->chg_cfg);
	if (IS_ERR(mt_chg->chg_psy)) {
		dev_notice(&pdev->dev, "Failed to register power supply: %ld\n",
			PTR_ERR(mt_chg->chg_psy));
		ret = PTR_ERR(mt_chg->chg_psy);
		return ret;
	}

	mt_chg->ac_psy = power_supply_register(&pdev->dev, &mt_chg->ac_desc,
		&mt_chg->ac_cfg);
	if (IS_ERR(mt_chg->ac_psy)) {
		dev_notice(&pdev->dev, "Failed to register power supply: %ld\n",
			PTR_ERR(mt_chg->ac_psy));
		ret = PTR_ERR(mt_chg->ac_psy);
		goto err_ac_psy;
	}

	mt_chg->usb_psy = power_supply_register(&pdev->dev, &mt_chg->usb_desc,
		&mt_chg->usb_cfg);
	if (IS_ERR(mt_chg->usb_psy)) {
		dev_notice(&pdev->dev, "Failed to register power supply: %ld\n",
			PTR_ERR(mt_chg->usb_psy));
		ret = PTR_ERR(mt_chg->usb_psy);
		goto err_usb_psy;
	}

	cti = devm_kzalloc(&pdev->dev, sizeof(*cti), GFP_KERNEL);
	if (!cti) {
		ret = -ENOMEM;
		goto err_no_mem;
	}
	cti->dev = &pdev->dev;

	cti->chg_consumer = charger_manager_get_by_name(cti->dev,
							"charger_port1");
	if (!cti->chg_consumer) {
		pr_info("%s: get charger consumer device failed\n", __func__);
		ret = -EINVAL;
		goto err_get_tcpc_dev;
	}

	dev = &(pdev->dev);
	if (dev != NULL) {
		boot_node = of_parse_phandle(dev->of_node, "bootmode", 0);
		if (!boot_node) {
			chr_err("%s: failed to get boot mode phandle\n", __func__);
		} else {
			tag = (struct tag_bootmode *)of_get_property(boot_node, "atag,boot", NULL);
			if (!tag)
				chr_err("%s: failed to get atag,boot\n", __func__);
			else
				boot_mode = tag->bootmode;
		}
	}
	//ret = get_boot_mode();
	if (boot_mode == KERNEL_POWER_OFF_CHARGING_BOOT ||
	    boot_mode == LOW_POWER_OFF_CHARGING_BOOT)
		cti->tcpc_kpoc = true;
	pr_info("%s KPOC(%d)\n", __func__, cti->tcpc_kpoc);

	/* Init Charger Detection */
	mutex_init(&cti->chgdet_lock);
	atomic_set(&cti->chgdet_cnt, 0);

	init_waitqueue_head(&cti->waitq);
	cti->chgdet_task = kthread_run(
				chgdet_task_threadfn, cti, "chgdet_thread");
	ret = PTR_ERR_OR_ZERO(cti->chgdet_task);
	if (ret < 0) {
		pr_info("%s: create chg det work fail\n", __func__);
		return ret;
	}

	/* Init power off work */
	cti->pwr_off_wq = create_singlethread_workqueue("tcpc_power_off");
	INIT_WORK(&cti->pwr_off_work, tcpc_power_off_work_handler);

	cti->chg_in_wq = create_singlethread_workqueue("charger_in");
	INIT_WORK(&cti->chg_in_work, charger_in_work_handler);

	mt_chg->cti = cti;
	platform_set_drvdata(pdev, mt_chg);
	device_init_wakeup(&pdev->dev, true);

#ifdef CONFIG_MACH_MT6771
	cti->psy_nb.notifier_call = mt6370_psy_notifier;
	ret = power_supply_reg_notifier(&cti->psy_nb);
	if (ret)
		pr_info("fail to register notifer\n");

	cti->chr_psy = power_supply_get_by_name("mt6370_pmu_charger");
	if (IS_ERR_OR_NULL(cti->chr_psy))
		pr_info("%s, fail to get chr_psy\n", __func__);
#endif

	#ifdef CONFIG_EXTCON_USB_CHG
	info = devm_kzalloc(mt_chg->dev, sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	info->dev = mt_chg->dev;
	mt_chg->extcon_info = info;

	INIT_DELAYED_WORK(&mt_chg->extcon_work, init_extcon_work);
	schedule_delayed_work(&mt_chg->extcon_work, 0);
	#endif

	pr_info("%s done\n", __func__);
	return 0;

err_get_tcpc_dev:
	devm_kfree(&pdev->dev, cti);
err_no_mem:
	power_supply_unregister(mt_chg->usb_psy);
err_usb_psy:
	power_supply_unregister(mt_chg->ac_psy);
err_ac_psy:
	power_supply_unregister(mt_chg->chg_psy);
	return ret;
}

static int mt_charger_remove(struct platform_device *pdev)
{
	struct mt_charger *mt_charger = platform_get_drvdata(pdev);
	struct chg_type_info *cti = mt_charger->cti;

	power_supply_unregister(mt_charger->chg_psy);
	power_supply_unregister(mt_charger->ac_psy);
	power_supply_unregister(mt_charger->usb_psy);

	pr_info("%s\n", __func__);
	if (cti->chgdet_task) {
		kthread_stop(cti->chgdet_task);
		atomic_inc(&cti->chgdet_cnt);
		wake_up_interruptible(&cti->waitq);
	}

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int mt_charger_suspend(struct device *dev)
{
	/* struct mt_charger *mt_charger = dev_get_drvdata(dev); */
	return 0;
}

static int mt_charger_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct mt_charger *mt_charger = platform_get_drvdata(pdev);

	if (!mt_charger) {
		pr_info("%s: get mt_charger failed\n", __func__);
		return -ENODEV;
	}

	power_supply_changed(mt_charger->chg_psy);
	power_supply_changed(mt_charger->ac_psy);
	power_supply_changed(mt_charger->usb_psy);

	return 0;
}
#endif

static SIMPLE_DEV_PM_OPS(mt_charger_pm_ops, mt_charger_suspend,
	mt_charger_resume);

static const struct of_device_id mt_charger_match[] = {
	{ .compatible = "mediatek,mt-charger", },
	{ },
};
static struct platform_driver mt_charger_driver = {
	.probe = mt_charger_probe,
	.remove = mt_charger_remove,
	.driver = {
		.name = "mt-charger-det",
		.owner = THIS_MODULE,
		.pm = &mt_charger_pm_ops,
		.of_match_table = mt_charger_match,
	},
};

/* Legacy api to prevent build error */
bool upmu_is_chr_det(void)
{
	struct mt_charger *mtk_chg = NULL;
	struct power_supply *psy = power_supply_get_by_name("charger");

	if (!psy) {
		pr_info("%s: get power supply failed\n", __func__);
		return -EINVAL;
	}
	mtk_chg = power_supply_get_drvdata(psy);
	return mtk_chg->chg_online;
}

/* Legacy api to prevent build error */
bool pmic_chrdet_status(void)
{
	if (upmu_is_chr_det())
		return true;

	pr_notice("%s: No charger\n", __func__);
	return false;
}

enum charger_type mt_get_charger_type(void)
{
	struct mt_charger *mtk_chg = NULL;
	struct power_supply *psy = power_supply_get_by_name("charger");

	if (!psy) {
		pr_info("%s: get power supply failed\n", __func__);
		return -EINVAL;
	}
	mtk_chg = power_supply_get_drvdata(psy);
	return mtk_chg->chg_type;
}

bool mt_charger_plugin(void)
{
	struct mt_charger *mtk_chg = NULL;
	struct power_supply *psy = power_supply_get_by_name("charger");
	struct chg_type_info *cti = NULL;

	if (!psy) {
		pr_info("%s: get power supply failed\n", __func__);
		return -EINVAL;
	}
	mtk_chg = power_supply_get_drvdata(psy);
	cti = mtk_chg->cti;
	pr_info("%s plugin:%d\n", __func__, cti->plugin);

	return cti->plugin;
}

static s32 __init mt_charger_det_init(void)
{
	return platform_driver_register(&mt_charger_driver);
}

static void __exit mt_charger_det_exit(void)
{
	platform_driver_unregister(&mt_charger_driver);
}

device_initcall(mt_charger_det_init);
module_exit(mt_charger_det_exit);

#ifdef CONFIG_TCPC_CLASS
static int __init mt_charger_det_notifier_call_init(void)
{
	int ret = 0;
	struct power_supply *psy = power_supply_get_by_name("charger");
	struct mt_charger *mt_chg = NULL;
	struct chg_type_info *cti = NULL;

	if (!psy) {
		pr_notice("%s: get power supply fail\n", __func__);
		return -ENODEV;
	}
	mt_chg = power_supply_get_drvdata(psy);
	cti = mt_chg->cti;

	cti->tcpc = tcpc_dev_get_by_name("type_c_port0");
	if (cti->tcpc == NULL) {
		pr_notice("%s: get tcpc dev fail\n", __func__);
		ret = -ENODEV;
		goto out;
	}
	cti->pd_nb.notifier_call = pd_tcp_notifier_call;
	ret = register_tcp_dev_notifier(cti->tcpc,
		&cti->pd_nb, TCP_NOTIFY_TYPE_ALL);
	if (ret < 0) {
		pr_notice("%s: register tcpc notifier fail(%d)\n",
			  __func__, ret);
		goto out;
	}
	pr_info("%s done\n", __func__);
out:
	power_supply_put(psy);
	return ret;
}
late_initcall(mt_charger_det_notifier_call_init);
#endif

MODULE_DESCRIPTION("mt-charger-detection");
MODULE_AUTHOR("MediaTek");
MODULE_LICENSE("GPL v2");

#endif /* CONFIG_MTK_FPGA */
