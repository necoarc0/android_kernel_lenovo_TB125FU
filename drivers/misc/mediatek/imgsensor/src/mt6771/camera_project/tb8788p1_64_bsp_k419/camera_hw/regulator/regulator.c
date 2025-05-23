// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (c) 2017 MediaTek Inc.
 *
 */

#include "regulator.h"


static const int regulator_voltage[] = {
	REGULATOR_VOLTAGE_0,
	REGULATOR_VOLTAGE_1000,
	REGULATOR_VOLTAGE_1100,
	REGULATOR_VOLTAGE_1200,
	REGULATOR_VOLTAGE_1210,
	REGULATOR_VOLTAGE_1220,
	REGULATOR_VOLTAGE_1500,
	REGULATOR_VOLTAGE_1800,
	REGULATOR_VOLTAGE_2500,
	REGULATOR_VOLTAGE_2800,
	REGULATOR_VOLTAGE_2900,
};

struct REGULATOR_CTRL regulator_control[REGULATOR_TYPE_MAX_NUM] = {
	{"vcama"},
	{"vcamd"},
	{"vcamio"},
	{"vcama_sub"},
	{"vcamd_sub"},
	{"vcamio_sub"},
	{"vcama_main2"},
	{"vcamd_main2"},
	{"vcamio_main2"},
	{"vcama_sub2"},
	{"vcamd_sub2"},
	{"vcamio_sub2"}
};

static struct REGULATOR reg_instance;

static enum IMGSENSOR_RETURN regulator_init(
	void *pinstance,
	struct IMGSENSOR_HW_DEVICE_COMMON *pcommon)
{
	struct REGULATOR      *preg            = (struct REGULATOR *)pinstance;
	struct REGULATOR_CTRL *pregulator_ctrl = regulator_control;
	int i;

	for (i = 0; i < REGULATOR_TYPE_MAX_NUM; i++, pregulator_ctrl++) {
		preg->pregulator[i] = regulator_get(
				&pcommon->pplatform_device->dev,
				pregulator_ctrl->pregulator_type);
		if (preg->pregulator[i] == NULL)
			PK_INFO("regulator[%d] %s fail!\n",
				i, pregulator_ctrl->pregulator_type);
		atomic_set(&preg->enable_cnt[i], 0);
	}

	return IMGSENSOR_RETURN_SUCCESS;
}

static enum IMGSENSOR_RETURN regulator_release(void *pinstance)
{
	struct REGULATOR *preg = (struct REGULATOR *)pinstance;
	int i;

	for (i = 0; i < REGULATOR_TYPE_MAX_NUM; i++) {
		if (preg->pregulator[i] != NULL) {
			for (; atomic_read(&preg->enable_cnt[i]) > 0; ) {
				regulator_disable(preg->pregulator[i]);
				atomic_dec(&preg->enable_cnt[i]);
			}
		}
	}
	return IMGSENSOR_RETURN_SUCCESS;
}

static enum IMGSENSOR_RETURN regulator_set(
	void *pinstance,
	enum IMGSENSOR_SENSOR_IDX   sensor_idx,
	enum IMGSENSOR_HW_PIN       pin,
	enum IMGSENSOR_HW_PIN_STATE pin_state)
{
	struct regulator     *pregulator;
	struct REGULATOR     *preg = (struct REGULATOR *)pinstance;
	enum   REGULATOR_TYPE reg_type_offset;
	atomic_t             *enable_cnt;


	if (pin > IMGSENSOR_HW_PIN_DOVDD   ||
	    pin < IMGSENSOR_HW_PIN_AVDD    ||
	    pin_state < IMGSENSOR_HW_PIN_STATE_LEVEL_0 ||
	    pin_state >= IMGSENSOR_HW_PIN_STATE_LEVEL_HIGH)
		return IMGSENSOR_RETURN_ERROR;

	reg_type_offset =
		(sensor_idx == IMGSENSOR_SENSOR_IDX_MAIN)  ?
			REGULATOR_TYPE_MAIN_VCAMA :
		(sensor_idx == IMGSENSOR_SENSOR_IDX_SUB)   ?
			REGULATOR_TYPE_SUB_VCAMA :
		(sensor_idx == IMGSENSOR_SENSOR_IDX_MAIN2) ?
			REGULATOR_TYPE_MAIN2_VCAMA :
		REGULATOR_TYPE_SUB2_VCAMA;

	pregulator = preg->pregulator[reg_type_offset + pin -
		IMGSENSOR_HW_PIN_AVDD];
	enable_cnt = preg->enable_cnt + (reg_type_offset + pin -
		IMGSENSOR_HW_PIN_AVDD);

	if (pregulator) {
		if (pin_state != IMGSENSOR_HW_PIN_STATE_LEVEL_0) {
			if (regulator_set_voltage(pregulator,
				regulator_voltage[pin_state -
				IMGSENSOR_HW_PIN_STATE_LEVEL_0],
				regulator_voltage[pin_state -
				IMGSENSOR_HW_PIN_STATE_LEVEL_0])) {

				pr_info("[regulator]fail to regulator_set_voltage, powertype:%d powerId:%d\n",
					pin,
					regulator_voltage[pin_state -
					IMGSENSOR_HW_PIN_STATE_LEVEL_0]);
			}
			if (regulator_enable(pregulator)) {
				pr_info("[regulator]fail to regulator_enable, powertype:%d powerId:%d\n",
					pin,
					regulator_voltage[pin_state -
					IMGSENSOR_HW_PIN_STATE_LEVEL_0]);
				return IMGSENSOR_RETURN_ERROR;
			}
			atomic_inc(enable_cnt);
		} else {
			if (regulator_is_enabled(pregulator))
				PK_DBG("[regulator]%d is enabled\n", pin);

			if (regulator_disable(pregulator)) {
				pr_info("[regulator]fail to regulator_disable, powertype: %d\n",
					pin);
				return IMGSENSOR_RETURN_ERROR;
			}
			atomic_dec(enable_cnt);
		}
	} else {
		PK_INFO("regulator == NULL %d %d %d\n",
				reg_type_offset,
				pin,
				IMGSENSOR_HW_PIN_AVDD);
	}

	return IMGSENSOR_RETURN_SUCCESS;
}

static struct IMGSENSOR_HW_DEVICE device = {
	.id        = IMGSENSOR_HW_ID_REGULATOR,
	.pinstance = (void *)&reg_instance,
	.init      = regulator_init,
	.set       = regulator_set,
	.release   = regulator_release
};

enum IMGSENSOR_RETURN imgsensor_hw_regulator_open(
	struct IMGSENSOR_HW_DEVICE **pdevice)
{
	*pdevice = &device;
	return IMGSENSOR_RETURN_SUCCESS;
}
