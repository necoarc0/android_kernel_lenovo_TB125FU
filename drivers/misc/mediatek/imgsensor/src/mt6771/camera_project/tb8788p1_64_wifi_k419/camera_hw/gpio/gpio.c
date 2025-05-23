// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2017 MediaTek Inc.
 */

#include "gpio.h"

struct GPIO_PINCTRL gpio_pinctrl_list[GPIO_CTRL_STATE_MAX_NUM] = {
	/* Main */
	{"cam0_pnd1"},
	{"cam0_pnd0"},
	{"cam0_rst1"},
	{"cam0_rst0"},
	{"cam_ldo_vcama_1"},
	{"cam_ldo_vcama_0"},
	{"cam_ldo_vcamd_1"},
	{"cam_ldo_vcamd_0"},
	{"cam_ldo_vcamio_1"},
	{"cam_ldo_vcamio_0"},
	/* Sub */
	{"cam1_pnd1"},
	{"cam1_pnd0"},
	{"cam1_rst1"},
	{"cam1_rst0"},
	{"cam_ldo_sub_vcama_1"},
	{"cam_ldo_sub_vcama_0"},
	{"cam_ldo_sub_vcamd_1"},
	{"cam_ldo_sub_vcamd_0"},
	{"cam_ldo_sub_vcamio_1"},
	{"cam_ldo_sub_vcamio_0"},
	/* Main2 */
	{"cam2_pnd1"},
	{"cam2_pnd0"},
	{"cam2_rst1"},
	{"cam2_rst0"},
	{"cam_ldo_main2_vcama_1"},
	{"cam_ldo_main2_vcama_0"},
	{"cam_ldo_main2_vcamd_1"},
	{"cam_ldo_main2_vcamd_0"},
	{"cam_ldo_main2_vcamio_1"},
	{"cam_ldo_main2_vcamio_0"},
	/* Sub2 */
	{"cam3_pnd1"},
	{"cam3_pnd0"},
	{"cam3_rst1"},
	{"cam3_rst0"},
	{"cam_ldo_sub2_vcama_1"},
	{"cam_ldo_sub2_vcama_0"},
	{"cam_ldo_sub2_vcamd_1"},
	{"cam_ldo_sub2_vcamd_0"},
	{"cam_ldo_sub2_vcamio_1"},
	{"cam_ldo_sub2_vcamio_0"},

#ifdef MIPI_SWITCH
	{"cam_mipi_switch_en_1"},
	{"cam_mipi_switch_en_0"},
	{"cam_mipi_switch_sel_1"},
	{"cam_mipi_switch_sel_0"}
#endif
};

static struct GPIO gpio_instance;

static enum IMGSENSOR_RETURN gpio_init(
	void *pinstance,
	struct IMGSENSOR_HW_DEVICE_COMMON *pcommon)
{
	int    i;
	struct GPIO            *pgpio            = (struct GPIO *)pinstance;
	struct GPIO_PINCTRL    *pgpio_pinctrl    = gpio_pinctrl_list;
	enum   IMGSENSOR_RETURN ret              = IMGSENSOR_RETURN_SUCCESS;

	pgpio->pgpio_mutex = &pcommon->pinctrl_mutex;

	pgpio->ppinctrl = devm_pinctrl_get(&pcommon->pplatform_device->dev);
	if (IS_ERR(pgpio->ppinctrl)) {
		PK_INFO("%s : Cannot find camera pinctrl!", __func__);
		ret = IMGSENSOR_RETURN_ERROR;
	}

	for (i = 0; i < GPIO_CTRL_STATE_MAX_NUM; i++, pgpio_pinctrl++) {
		if (pgpio_pinctrl->ppinctrl_lookup_names)
			pgpio->ppinctrl_state[i] =
				pinctrl_lookup_state(pgpio->ppinctrl,
					pgpio_pinctrl->ppinctrl_lookup_names);

		if (pgpio->ppinctrl_state[i] == NULL ||
			IS_ERR(pgpio->ppinctrl_state[i])) {
			PK_INFO("%s : pinctrl err, %s\n", __func__,
				pgpio_pinctrl->ppinctrl_lookup_names);
			ret = IMGSENSOR_RETURN_ERROR;
		}
	}

	return ret;
}

static enum IMGSENSOR_RETURN gpio_release(void *pinstance)
{
	return IMGSENSOR_RETURN_SUCCESS;
}

static enum IMGSENSOR_RETURN gpio_set(
	void *pinstance,
	enum IMGSENSOR_SENSOR_IDX   sensor_idx,
	enum IMGSENSOR_HW_PIN       pin,
	enum IMGSENSOR_HW_PIN_STATE pin_state)
{
	struct pinctrl_state  *ppinctrl_state;
	struct GPIO           *pgpio = (struct GPIO *)pinstance;
	enum   GPIO_STATE      gpio_state;
	enum   GPIO_CTRL_STATE ctrl_state_offset;

	if (pin < IMGSENSOR_HW_PIN_PDN ||
#ifdef MIPI_SWITCH
	    pin > IMGSENSOR_HW_PIN_MIPI_SWITCH_SEL ||
#else
	   pin > IMGSENSOR_HW_PIN_DOVDD ||
#endif
	   pin_state < IMGSENSOR_HW_PIN_STATE_LEVEL_0 ||
	   pin_state > IMGSENSOR_HW_PIN_STATE_LEVEL_HIGH)
		return IMGSENSOR_RETURN_ERROR;

	gpio_state = (pin_state > IMGSENSOR_HW_PIN_STATE_LEVEL_0) ?
		GPIO_STATE_H : GPIO_STATE_L;

#ifdef MIPI_SWITCH
	if (pin == IMGSENSOR_HW_PIN_MIPI_SWITCH_EN)
		ppinctrl_state = pgpio->ppinctrl_state[
			GPIO_CTRL_STATE_MIPI_SWITCH_EN_H + gpio_state];
	else if (pin == IMGSENSOR_HW_PIN_MIPI_SWITCH_SEL)
		ppinctrl_state = pgpio->ppinctrl_state[
			GPIO_CTRL_STATE_MIPI_SWITCH_SEL_H + gpio_state];
	else
#endif
	{
		ctrl_state_offset =
		(sensor_idx == IMGSENSOR_SENSOR_IDX_MAIN)  ?
			GPIO_CTRL_STATE_CAM0_PDN_H :
		(sensor_idx == IMGSENSOR_SENSOR_IDX_SUB)   ?
			GPIO_CTRL_STATE_CAM1_PDN_H :
		(sensor_idx == IMGSENSOR_SENSOR_IDX_MAIN2) ?
			GPIO_CTRL_STATE_CAM2_PDN_H :
		GPIO_CTRL_STATE_CAM3_PDN_H;

		ppinctrl_state = pgpio->ppinctrl_state[ctrl_state_offset +
			((pin - IMGSENSOR_HW_PIN_PDN) << 1) + gpio_state];
	}

	mutex_lock(pgpio->pgpio_mutex);

	if (ppinctrl_state != NULL && !IS_ERR(ppinctrl_state))
		pinctrl_select_state(pgpio->ppinctrl, ppinctrl_state);
	else
		pr_info("%s : pinctrl err, PinIdx %d, Val %d\n",
			__func__, pin, pin_state);

	mutex_unlock(pgpio->pgpio_mutex);

	return IMGSENSOR_RETURN_SUCCESS;
}

static struct IMGSENSOR_HW_DEVICE device = {
	.id        = IMGSENSOR_HW_ID_GPIO,
	.pinstance = (void *)&gpio_instance,
	.init      = gpio_init,
	.set       = gpio_set,
	.release   = gpio_release
};

enum IMGSENSOR_RETURN imgsensor_hw_gpio_open(
	struct IMGSENSOR_HW_DEVICE **pdevice)
{
	*pdevice = &device;
	return IMGSENSOR_RETURN_SUCCESS;
}
