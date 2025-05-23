// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 MediaTek Inc.
 *
 * Author: Guodong Liu <guodong.liu@mediatek.com>
 *
 */

#include "pinctrl-mtk-mt6771.h"
#include "pinctrl-paris.h"

/* MT6771 have multiple bases to program pin configuration listed as the below:
 * gpio:0x10005000,    iocfg_0:0x11F20000, iocfg_1:0x11E80000,
 * iocfg_2:0x11E70000, iocfg_4:0x11E90000, iocfg_5:0x11D30000,
 * iocfg_6:0x11D20000, iocfg_7:0x11C50000
 * _i_based could be used to indicate what base the pin should be mapped into.
 */

#define PIN_FIELD_BASE(s_pin, e_pin, i_base, s_addr, x_addrs, s_bit, x_bits) \
	PIN_FIELD_CALC(s_pin, e_pin, i_base, s_addr, x_addrs, s_bit, x_bits, \
		       32, 0)

#define PINS_FIELD_BASE(s_pin, e_pin, i_base, s_addr, x_addrs, s_bit, x_bits) \
	PIN_FIELD_CALC(s_pin, e_pin, i_base, s_addr, x_addrs, s_bit, x_bits,  \
		       32, 1)

static const struct mtk_pin_field_calc mt6771_pin_mode_range[] = {
	PIN_FIELD(0, 192, 0x300, 0x10, 0, 4),
};

static const struct mtk_pin_field_calc mt6771_pin_dir_range[] = {
	PIN_FIELD(0, 192, 0x0, 0x10, 0, 1),
};

static const struct mtk_pin_field_calc mt6771_pin_di_range[] = {
	PIN_FIELD(0, 192, 0x200, 0x10, 0, 1),
};

static const struct mtk_pin_field_calc mt6771_pin_do_range[] = {
	PIN_FIELD(0, 192, 0x100, 0x10, 0, 1),
};

static const struct mtk_pin_field_calc mt6771_pin_ies_range[] = {
	PINS_FIELD_BASE(0, 3, 6, 0x000, 0x10, 3, 1),
	PINS_FIELD_BASE(4, 7, 6, 0x000, 0x10, 5, 1),
	PIN_FIELD_BASE(8, 8, 6, 0x000, 0x10, 0, 1),
	PINS_FIELD_BASE(9, 10, 6, 0x000, 0x10, 12, 1),
	PIN_FIELD_BASE(11, 11, 1, 0x000, 0x10, 3, 1),
	PIN_FIELD_BASE(12, 12, 1, 0x000, 0x10, 7, 1),
	PINS_FIELD_BASE(13, 16, 2, 0x000, 0x10, 2, 1),
	PINS_FIELD_BASE(17, 20, 2, 0x000, 0x10, 3, 1),
	PINS_FIELD_BASE(21, 24, 2, 0x000, 0x10, 4, 1),
	PINS_FIELD_BASE(25, 28, 2, 0x000, 0x10, 5, 1),
	PIN_FIELD_BASE(29, 29, 2, 0x000, 0x10, 6, 1),
	PIN_FIELD_BASE(30, 30, 2, 0x000, 0x10, 7, 1),
	PINS_FIELD_BASE(31, 31, 2, 0x000, 0x10, 8, 1),
	PINS_FIELD_BASE(32, 34, 2, 0x000, 0x10, 7, 1),
	PINS_FIELD_BASE(35, 37, 3, 0x000, 0x10, 0, 1),
	PINS_FIELD_BASE(38, 40, 3, 0x000, 0x10, 1, 1),
	PINS_FIELD_BASE(41, 42, 3, 0x000, 0x10, 2, 1),
	PINS_FIELD_BASE(43, 45, 3, 0x000, 0x10, 3, 1),
	PINS_FIELD_BASE(46, 47, 3, 0x000, 0x10, 4, 1),
	PINS_FIELD_BASE(48, 49, 3, 0x000, 0x10, 5, 1),
	PINS_FIELD_BASE(50, 51, 4, 0x000, 0x10, 0, 1),
	PINS_FIELD_BASE(52, 57, 4, 0x000, 0x10, 1, 1),
	PINS_FIELD_BASE(58, 60, 4, 0x000, 0x10, 2, 1),
	PINS_FIELD_BASE(61, 64, 5, 0x000, 0x10, 0, 1),
	PINS_FIELD_BASE(65, 66, 5, 0x000, 0x10, 1, 1),
	PINS_FIELD_BASE(67, 68, 5, 0x000, 0x10, 2, 1),
	PINS_FIELD_BASE(69, 71, 5, 0x000, 0x10, 3, 1),
	PINS_FIELD_BASE(72, 76, 5, 0x000, 0x10, 4, 1),
	PINS_FIELD_BASE(77, 80, 5, 0x000, 0x10, 5, 1),
	PIN_FIELD_BASE(81, 81, 5, 0x000, 0x10, 6, 1),
	PINS_FIELD_BASE(82, 83, 5, 0x000, 0x10, 7, 1),
	PIN_FIELD_BASE(84, 84, 5, 0x000, 0x10, 6, 1),
	PINS_FIELD_BASE(85, 88, 5, 0x000, 0x10, 8, 1),
	PIN_FIELD_BASE(89, 89, 6, 0x000, 0x10, 11, 1),
	PIN_FIELD_BASE(90, 90, 6, 0x000, 0x10, 1, 1),
	PINS_FIELD_BASE(91, 94, 6, 0x000, 0x10, 2, 1),
	PINS_FIELD_BASE(95, 96, 6, 0x000, 0x10, 6, 1),
	PINS_FIELD_BASE(97, 98, 6, 0x000, 0x10, 7, 1),
	PIN_FIELD_BASE(99, 99, 6, 0x000, 0x10, 8, 1),
	PIN_FIELD_BASE(100, 100, 6, 0x000, 0x10, 9, 1),
	PINS_FIELD_BASE(101, 102, 6, 0x000, 0x10, 10, 1),
	PINS_FIELD_BASE(103, 104, 6, 0x000, 0x10, 13, 1),
	PINS_FIELD_BASE(105, 106, 6, 0x000, 0x10, 14, 1),
	PIN_FIELD_BASE(107, 107, 7, 0x000, 0x10, 0, 1),
	PIN_FIELD_BASE(108, 108, 7, 0x000, 0x10, 1, 1),
	PIN_FIELD_BASE(109, 109, 7, 0x000, 0x10, 2, 1),
	PIN_FIELD_BASE(110, 110, 7, 0x000, 0x10, 0, 1),
	PIN_FIELD_BASE(111, 111, 7, 0x000, 0x10, 3, 1),
	PIN_FIELD_BASE(112, 112, 7, 0x000, 0x10, 2, 1),
	PIN_FIELD_BASE(113, 113, 7, 0x000, 0x10, 4, 1),
	PIN_FIELD_BASE(114, 114, 7, 0x000, 0x10, 5, 1),
	PIN_FIELD_BASE(115, 115, 7, 0x000, 0x10, 6, 1),
	PIN_FIELD_BASE(116, 116, 7, 0x000, 0x10, 7, 1),
	PIN_FIELD_BASE(117, 117, 7, 0x000, 0x10, 8, 1),
	PIN_FIELD_BASE(118, 118, 7, 0x000, 0x10, 9, 1),
	PIN_FIELD_BASE(119, 119, 7, 0x000, 0x10, 10, 1),
	PIN_FIELD_BASE(120, 120, 7, 0x000, 0x10, 11, 1),
	PIN_FIELD_BASE(121, 121, 7, 0x000, 0x10, 12, 1),
	PIN_FIELD_BASE(122, 122, 8, 0x000, 0x10, 0, 1),
	PIN_FIELD_BASE(123, 123, 8, 0x000, 0x10, 1, 1),
	PIN_FIELD_BASE(124, 124, 8, 0x000, 0x10, 2, 1),
	PINS_FIELD_BASE(125, 130, 8, 0x000, 0x10, 1, 1),
	PIN_FIELD_BASE(131, 131, 8, 0x000, 0x10, 3, 1),
	PIN_FIELD_BASE(132, 132, 8, 0x000, 0x10, 1, 1),
	PIN_FIELD_BASE(133, 133, 8, 0x000, 0x10, 4, 1),
	PIN_FIELD_BASE(134, 134, 1, 0x000, 0x10, 0, 1),
	PIN_FIELD_BASE(135, 135, 1, 0x000, 0x10, 1, 1),
	PINS_FIELD_BASE(136, 143, 1, 0x000, 0x10, 2, 1),
	PINS_FIELD_BASE(144, 147, 1, 0x000, 0x10, 4, 1),
	PIN_FIELD_BASE(148, 148, 1, 0x000, 0x10, 5, 1),
	PIN_FIELD_BASE(149, 149, 1, 0x000, 0x10, 6, 1),
	PINS_FIELD_BASE(150, 153, 1, 0x000, 0x10, 8, 1),
	PIN_FIELD_BASE(154, 154, 1, 0x000, 0x10, 9, 1),
	PINS_FIELD_BASE(155, 157, 1, 0x000, 0x10, 10, 1),
	PINS_FIELD_BASE(158, 160, 1, 0x000, 0x10, 8, 1),
	PINS_FIELD_BASE(161, 164, 2, 0x000, 0x10, 0, 1),
	PINS_FIELD_BASE(165, 166, 2, 0x000, 0x10, 1, 1),
	PINS_FIELD_BASE(167, 168, 4, 0x000, 0x10, 2, 1),
	PIN_FIELD_BASE(169, 169, 4, 0x000, 0x10, 3, 1),
	PINS_FIELD_BASE(170, 174, 4, 0x000, 0x10, 4, 1),
	PINS_FIELD_BASE(175, 176, 4, 0x000, 0x10, 3, 1),
	PINS_FIELD_BASE(177, 179, 6, 0x000, 0x10, 4, 1),
};

static const struct mtk_pin_field_calc mt6771_pin_smt_range[] = {
	PINS_FIELD_BASE(0, 3, 6, 0x010, 0x10, 3, 1),
	PINS_FIELD_BASE(4, 7, 6, 0x010, 0x10, 5, 1),
	PIN_FIELD_BASE(8, 8, 6, 0x010, 0x10, 0, 1),
	PINS_FIELD_BASE(9, 10, 6, 0x010, 0x10, 12, 1),
	PIN_FIELD_BASE(11, 11, 1, 0x010, 0x10, 3, 1),
	PIN_FIELD_BASE(12, 12, 1, 0x010, 0x10, 7, 1),
	PINS_FIELD_BASE(13, 16, 2, 0x010, 0x10, 2, 1),
	PINS_FIELD_BASE(17, 20, 2, 0x010, 0x10, 3, 1),
	PINS_FIELD_BASE(21, 24, 2, 0x010, 0x10, 4, 1),
	PINS_FIELD_BASE(25, 28, 2, 0x010, 0x10, 5, 1),
	PIN_FIELD_BASE(29, 29, 2, 0x010, 0x10, 6, 1),
	PIN_FIELD_BASE(30, 30, 2, 0x010, 0x10, 7, 1),
	PINS_FIELD_BASE(31, 31, 2, 0x010, 0x10, 8, 1),
	PINS_FIELD_BASE(32, 34, 2, 0x010, 0x10, 7, 1),
	PINS_FIELD_BASE(35, 37, 3, 0x010, 0x10, 0, 1),
	PINS_FIELD_BASE(38, 40, 3, 0x010, 0x10, 1, 1),
	PINS_FIELD_BASE(41, 42, 3, 0x010, 0x10, 2, 1),
	PINS_FIELD_BASE(43, 45, 3, 0x010, 0x10, 3, 1),
	PINS_FIELD_BASE(46, 47, 3, 0x010, 0x10, 4, 1),
	PINS_FIELD_BASE(48, 49, 3, 0x010, 0x10, 5, 1),
	PINS_FIELD_BASE(50, 51, 4, 0x010, 0x10, 0, 1),
	PINS_FIELD_BASE(52, 57, 4, 0x010, 0x10, 1, 1),
	PINS_FIELD_BASE(58, 60, 4, 0x010, 0x10, 2, 1),
	PINS_FIELD_BASE(61, 64, 5, 0x010, 0x10, 0, 1),
	PINS_FIELD_BASE(65, 66, 5, 0x010, 0x10, 1, 1),
	PINS_FIELD_BASE(67, 68, 5, 0x010, 0x10, 2, 1),
	PINS_FIELD_BASE(69, 71, 5, 0x010, 0x10, 3, 1),
	PINS_FIELD_BASE(72, 76, 5, 0x010, 0x10, 4, 1),
	PINS_FIELD_BASE(77, 80, 5, 0x010, 0x10, 5, 1),
	PIN_FIELD_BASE(81, 81, 5, 0x010, 0x10, 6, 1),
	PINS_FIELD_BASE(82, 83, 5, 0x010, 0x10, 7, 1),
	PIN_FIELD_BASE(84, 84, 5, 0x010, 0x10, 6, 1),
	PINS_FIELD_BASE(85, 88, 5, 0x010, 0x10, 8, 1),
	PIN_FIELD_BASE(89, 89, 6, 0x010, 0x10, 11, 1),
	PIN_FIELD_BASE(90, 90, 6, 0x010, 0x10, 1, 1),
	PINS_FIELD_BASE(91, 94, 6, 0x010, 0x10, 2, 1),
	PINS_FIELD_BASE(95, 96, 6, 0x010, 0x10, 6, 1),
	PINS_FIELD_BASE(97, 98, 6, 0x010, 0x10, 7, 1),
	PIN_FIELD_BASE(99, 99, 6, 0x010, 0x10, 8, 1),
	PIN_FIELD_BASE(100, 100, 6, 0x010, 0x10, 9, 1),
	PINS_FIELD_BASE(101, 102, 6, 0x010, 0x10, 10, 1),
	PINS_FIELD_BASE(103, 104, 6, 0x010, 0x10, 13, 1),
	PINS_FIELD_BASE(105, 106, 6, 0x010, 0x10, 14, 1),
	PIN_FIELD_BASE(107, 107, 7, 0x010, 0x10, 0, 1),
	PIN_FIELD_BASE(108, 108, 7, 0x010, 0x10, 1, 1),
	PIN_FIELD_BASE(109, 109, 7, 0x010, 0x10, 2, 1),
	PIN_FIELD_BASE(110, 110, 7, 0x010, 0x10, 0, 1),
	PIN_FIELD_BASE(111, 111, 7, 0x010, 0x10, 3, 1),
	PIN_FIELD_BASE(112, 112, 7, 0x010, 0x10, 2, 1),
	PIN_FIELD_BASE(113, 113, 7, 0x010, 0x10, 4, 1),
	PIN_FIELD_BASE(114, 114, 7, 0x010, 0x10, 5, 1),
	PIN_FIELD_BASE(115, 115, 7, 0x010, 0x10, 6, 1),
	PIN_FIELD_BASE(116, 116, 7, 0x010, 0x10, 7, 1),
	PIN_FIELD_BASE(117, 117, 7, 0x010, 0x10, 8, 1),
	PIN_FIELD_BASE(118, 118, 7, 0x010, 0x10, 9, 1),
	PIN_FIELD_BASE(119, 119, 7, 0x010, 0x10, 10, 1),
	PIN_FIELD_BASE(120, 120, 7, 0x010, 0x10, 11, 1),
	PIN_FIELD_BASE(121, 121, 7, 0x010, 0x10, 12, 1),
	PIN_FIELD_BASE(122, 122, 8, 0x010, 0x10, 0, 1),
	PIN_FIELD_BASE(123, 123, 8, 0x010, 0x10, 1, 1),
	PIN_FIELD_BASE(124, 124, 8, 0x010, 0x10, 2, 1),
	PINS_FIELD_BASE(125, 130, 8, 0x010, 0x10, 1, 1),
	PIN_FIELD_BASE(131, 131, 8, 0x010, 0x10, 3, 1),
	PIN_FIELD_BASE(132, 132, 8, 0x010, 0x10, 1, 1),
	PIN_FIELD_BASE(133, 133, 8, 0x010, 0x10, 4, 1),
	PIN_FIELD_BASE(134, 134, 1, 0x010, 0x10, 0, 1),
	PIN_FIELD_BASE(135, 135, 1, 0x010, 0x10, 1, 1),
	PINS_FIELD_BASE(136, 143, 1, 0x010, 0x10, 2, 1),
	PINS_FIELD_BASE(144, 147, 1, 0x010, 0x10, 4, 1),
	PIN_FIELD_BASE(148, 148, 1, 0x010, 0x10, 5, 1),
	PIN_FIELD_BASE(149, 149, 1, 0x010, 0x10, 6, 1),
	PINS_FIELD_BASE(150, 153, 1, 0x010, 0x10, 8, 1),
	PIN_FIELD_BASE(154, 154, 1, 0x010, 0x10, 9, 1),
	PINS_FIELD_BASE(155, 157, 1, 0x010, 0x10, 10, 1),
	PINS_FIELD_BASE(158, 160, 1, 0x010, 0x10, 8, 1),
	PINS_FIELD_BASE(161, 164, 2, 0x010, 0x10, 0, 1),
	PINS_FIELD_BASE(165, 166, 2, 0x010, 0x10, 1, 1),
	PINS_FIELD_BASE(167, 168, 4, 0x010, 0x10, 2, 1),
	PIN_FIELD_BASE(169, 169, 4, 0x010, 0x10, 3, 1),
	PINS_FIELD_BASE(170, 174, 4, 0x010, 0x10, 4, 1),
	PINS_FIELD_BASE(175, 176, 4, 0x010, 0x10, 3, 1),
	PINS_FIELD_BASE(177, 179, 6, 0x010, 0x10, 4, 1),
};

static const struct mtk_pin_field_calc mt6771_pin_pullen_range[] = {
	PIN_FIELD_BASE(0, 3, 6, 0x060, 0x10, 6, 1),
	PIN_FIELD_BASE(4, 7, 6, 0x060, 0x10, 11, 1),
	PIN_FIELD_BASE(8, 8, 6, 0x060, 0x10, 0, 1),
	PIN_FIELD_BASE(9, 10, 6, 0x060, 0x10, 26, 1),
	PIN_FIELD_BASE(11, 11, 1, 0x060, 0x10, 10, 1),
	PIN_FIELD_BASE(12, 12, 1, 0x060, 0x10, 17, 1),
	PIN_FIELD_BASE(13, 28, 2, 0x060, 0x10, 6, 1),
	PIN_FIELD_BASE(43, 49, 3, 0x060, 0x10, 8, 1),
	PIN_FIELD_BASE(50, 60, 4, 0x060, 0x10, 0, 1),
	PIN_FIELD_BASE(61, 88, 5, 0x060, 0x10, 0, 1),
	PIN_FIELD_BASE(89, 89, 6, 0x060, 0x10, 24, 1),
	PIN_FIELD_BASE(90, 90, 6, 0x060, 0x10, 1, 1),
	PIN_FIELD_BASE(95, 95, 6, 0x060, 0x10, 15, 1),
	PIN_FIELD_BASE(96, 102, 6, 0x060, 0x10, 17, 1),
	PIN_FIELD_BASE(103, 106, 6, 0x060, 0x10, 28, 1),
	PIN_FIELD_BASE(107, 121, 7, 0x060, 0x10, 0, 1),
	PIN_FIELD_BASE(134, 143, 1, 0x060, 0x10, 0, 1),
	PIN_FIELD_BASE(144, 149, 1, 0x060, 0x10, 11, 1),
	PIN_FIELD_BASE(150, 160, 1, 0x060, 0x10, 18, 1),
	PIN_FIELD_BASE(161, 166, 2, 0x060, 0x10, 0, 1),
	PIN_FIELD_BASE(167, 176, 4, 0x060, 0x10, 11, 1),
	PIN_FIELD_BASE(177, 177, 6, 0x060, 0x10, 10, 1),
	PIN_FIELD_BASE(178, 178, 6, 0x060, 0x10, 16, 1),
	PIN_FIELD_BASE(179, 179, 6, 0x060, 0x10, 25, 1),
};

static const struct mtk_pin_field_calc mt6771_pin_pullsel_range[] = {
	PIN_FIELD_BASE(0, 3, 6, 0x080, 0x10, 6, 1),
	PIN_FIELD_BASE(4, 7, 6, 0x080, 0x10, 11, 1),
	PIN_FIELD_BASE(8, 8, 6, 0x080, 0x10, 0, 1),
	PIN_FIELD_BASE(9, 10, 6, 0x080, 0x10, 26, 1),
	PIN_FIELD_BASE(11, 11, 1, 0x080, 0x10, 10, 1),
	PIN_FIELD_BASE(12, 12, 1, 0x080, 0x10, 17, 1),
	PIN_FIELD_BASE(13, 28, 2, 0x080, 0x10, 6, 1),
	PIN_FIELD_BASE(43, 49, 3, 0x080, 0x10, 8, 1),
	PIN_FIELD_BASE(50, 60, 4, 0x080, 0x10, 0, 1),
	PIN_FIELD_BASE(61, 88, 5, 0x080, 0x10, 0, 1),
	PIN_FIELD_BASE(89, 89, 6, 0x080, 0x10, 24, 1),
	PIN_FIELD_BASE(90, 90, 6, 0x080, 0x10, 1, 1),
	PIN_FIELD_BASE(95, 95, 6, 0x080, 0x10, 15, 1),
	PIN_FIELD_BASE(96, 102, 6, 0x080, 0x10, 17, 1),
	PIN_FIELD_BASE(103, 106, 6, 0x080, 0x10, 28, 1),
	PIN_FIELD_BASE(107, 121, 7, 0x080, 0x10, 0, 1),
	PIN_FIELD_BASE(134, 143, 1, 0x080, 0x10, 0, 1),
	PIN_FIELD_BASE(144, 149, 1, 0x080, 0x10, 11, 1),
	PIN_FIELD_BASE(150, 160, 1, 0x080, 0x10, 18, 1),
	PIN_FIELD_BASE(161, 166, 2, 0x080, 0x10, 0, 1),
	PIN_FIELD_BASE(167, 176, 4, 0x080, 0x10, 11, 1),
	PIN_FIELD_BASE(177, 177, 6, 0x080, 0x10, 10, 1),
	PIN_FIELD_BASE(178, 178, 6, 0x080, 0x10, 16, 1),
	PIN_FIELD_BASE(179, 179, 6, 0x080, 0x10, 25, 1),
};

static const struct mtk_pin_field_calc mt6771_pin_drv_range[] = {
	PINS_FIELD_BASE(0, 3, 6, 0x0A0, 0x10, 12, 3),
	PINS_FIELD_BASE(4, 7, 6, 0x0A0, 0x10, 20, 3),
	PIN_FIELD_BASE(8, 8, 6, 0x0A0, 0x10, 0, 3),
	PINS_FIELD_BASE(9, 10, 6, 0x0B0, 0x10, 16, 3),
	PIN_FIELD_BASE(11, 11, 1, 0x0A0, 0x10, 12, 3),
	PIN_FIELD_BASE(12, 12, 1, 0x0A0, 0x10, 28, 3),
	PINS_FIELD_BASE(13, 16, 2, 0x0A0, 0x10, 8, 3),
	PINS_FIELD_BASE(17, 20, 2, 0x0A0, 0x10, 12, 3),
	PINS_FIELD_BASE(21, 24, 2, 0x0A0, 0x10, 16, 3),
	PINS_FIELD_BASE(25, 28, 2, 0x0A0, 0x10, 20, 3),
	PIN_FIELD_BASE(29, 29, 2, 0x0A0, 0x10, 24, 3),
	PIN_FIELD_BASE(30, 30, 2, 0x0A0, 0x10, 28, 3),
	PINS_FIELD_BASE(31, 31, 2, 0x0B0, 0x10, 0, 3),
	PINS_FIELD_BASE(32, 34, 2, 0x0A0, 0x10, 28, 3),
	PINS_FIELD_BASE(35, 37, 3, 0x0A0, 0x10, 0, 3),
	PINS_FIELD_BASE(38, 40, 3, 0x0A0, 0x10, 4, 3),
	PINS_FIELD_BASE(41, 42, 3, 0x0A0, 0x10, 8, 3),
	PINS_FIELD_BASE(43, 45, 3, 0x0A0, 0x10, 12, 3),
	PINS_FIELD_BASE(46, 47, 3, 0x0A0, 0x10, 16, 3),
	PINS_FIELD_BASE(48, 49, 3, 0x0A0, 0x10, 20, 3),
	PINS_FIELD_BASE(50, 51, 4, 0x0A0, 0x10, 0, 3),
	PINS_FIELD_BASE(52, 57, 4, 0x0A0, 0x10, 4, 3),
	PINS_FIELD_BASE(58, 60, 4, 0x0A0, 0x10, 8, 3),
	PINS_FIELD_BASE(61, 64, 5, 0x0A0, 0x10, 0, 3),
	PINS_FIELD_BASE(65, 66, 5, 0x0A0, 0x10, 4, 3),
	PINS_FIELD_BASE(67, 68, 5, 0x0A0, 0x10, 8, 3),
	PINS_FIELD_BASE(69, 71, 5, 0x0A0, 0x10, 12, 3),
	PINS_FIELD_BASE(72, 76, 5, 0x0A0, 0x10, 16, 3),
	PINS_FIELD_BASE(77, 80, 5, 0x0A0, 0x10, 20, 3),
	PIN_FIELD_BASE(81, 81, 5, 0x0A0, 0x10, 24, 3),
	PINS_FIELD_BASE(82, 83, 5, 0x0A0, 0x10, 28, 3),
	PIN_FIELD_BASE(84, 84, 5, 0x0A0, 0x10, 24, 3),
	PINS_FIELD_BASE(85, 88, 5, 0x0B0, 0x10, 0, 3),
	PIN_FIELD_BASE(89, 89, 6, 0x0B0, 0x10, 12, 3),
	PIN_FIELD_BASE(90, 90, 6, 0x0A0, 0x10, 4, 3),
	PINS_FIELD_BASE(91, 94, 6, 0x0A0, 0x10, 8, 3),
	PINS_FIELD_BASE(95, 96, 6, 0x0A0, 0x10, 24, 3),
	PINS_FIELD_BASE(97, 98, 6, 0x0A0, 0x10, 28, 3),
	PIN_FIELD_BASE(99, 99, 6, 0x0B0, 0x10, 0, 3),
	PIN_FIELD_BASE(100, 100, 6, 0x0B0, 0x10, 4, 3),
	PINS_FIELD_BASE(101, 102, 6, 0x0B0, 0x10, 8, 3),
	PINS_FIELD_BASE(103, 104, 6, 0x0B0, 0x10, 20, 3),
	PINS_FIELD_BASE(105, 106, 6, 0x0B0, 0x10, 24, 3),
	PIN_FIELD_BASE(107, 107, 7, 0x0A0, 0x10, 0, 3),
	PIN_FIELD_BASE(108, 108, 7, 0x0A0, 0x10, 4, 3),
	PIN_FIELD_BASE(109, 109, 7, 0x0A0, 0x10, 8, 3),
	PIN_FIELD_BASE(110, 110, 7, 0x0A0, 0x10, 0, 3),
	PIN_FIELD_BASE(111, 111, 7, 0x0A0, 0x10, 4, 3),
	PIN_FIELD_BASE(112, 112, 7, 0x0A0, 0x10, 8, 3),
	PIN_FIELD_BASE(113, 113, 7, 0x0A0, 0x10, 16, 3),
	PIN_FIELD_BASE(114, 114, 7, 0x0A0, 0x10, 20, 3),
	PIN_FIELD_BASE(115, 115, 7, 0x0A0, 0x10, 24, 3),
	PIN_FIELD_BASE(116, 116, 7, 0x0A0, 0x10, 28, 3),
	PIN_FIELD_BASE(117, 117, 7, 0x0B0, 0x10, 0, 3),
	PIN_FIELD_BASE(118, 118, 7, 0x0B0, 0x10, 4, 3),
	PIN_FIELD_BASE(119, 119, 7, 0x0B0, 0x10, 8, 3),
	PIN_FIELD_BASE(120, 120, 7, 0x0B0, 0x10, 12, 3),
	PIN_FIELD_BASE(121, 121, 7, 0x0B0, 0x10, 16, 3),
	PIN_FIELD_BASE(122, 122, 8, 0x0A0, 0x10, 0, 3),
	PIN_FIELD_BASE(123, 123, 8, 0x0A0, 0x10, 4, 3),
	PIN_FIELD_BASE(124, 124, 8, 0x0A0, 0x10, 8, 3),
	PINS_FIELD_BASE(125, 130, 8, 0x0A0, 0x10, 4, 3),
	PIN_FIELD_BASE(131, 131, 8, 0x0A0, 0x10, 12, 3),
	PIN_FIELD_BASE(132, 132, 8, 0x0A0, 0x10, 4, 3),
	PIN_FIELD_BASE(133, 133, 8, 0x0A0, 0x10, 16, 3),
	PIN_FIELD_BASE(134, 134, 1, 0x0A0, 0x10, 0, 3),
	PIN_FIELD_BASE(135, 135, 1, 0x0A0, 0x10, 4, 3),
	PINS_FIELD_BASE(136, 143, 1, 0x0A0, 0x10, 8, 3),
	PINS_FIELD_BASE(144, 147, 1, 0x0A0, 0x10, 16, 3),
	PIN_FIELD_BASE(148, 148, 1, 0x0A0, 0x10, 20, 3),
	PIN_FIELD_BASE(149, 149, 1, 0x0A0, 0x10, 24, 3),
	PINS_FIELD_BASE(150, 153, 1, 0x0B0, 0x10, 0, 3),
	PIN_FIELD_BASE(154, 154, 1, 0x0B0, 0x10, 4, 3),
	PINS_FIELD_BASE(155, 157, 1, 0x0B0, 0x10, 8, 3),
	PINS_FIELD_BASE(158, 160, 1, 0x0B0, 0x10, 0, 3),
	PINS_FIELD_BASE(161, 164, 2, 0x0A0, 0x10, 0, 3),
	PINS_FIELD_BASE(165, 166, 2, 0x0A0, 0x10, 4, 3),
	PINS_FIELD_BASE(167, 168, 4, 0x0A0, 0x10, 8, 3),
	PIN_FIELD_BASE(169, 169, 4, 0x0A0, 0x10, 12, 3),
	PINS_FIELD_BASE(170, 174, 4, 0x0A0, 0x10, 16, 3),
	PINS_FIELD_BASE(175, 176, 4, 0x0A0, 0x10, 12, 3),
	PINS_FIELD_BASE(177, 179, 6, 0x0A0, 0x10, 16, 3),
};

static const struct mtk_pin_field_calc mt6771_pin_pupd_range[] = {
	PIN_FIELD_BASE(29, 29, 2, 0x0C0, 0x10, 2, 1),
	PIN_FIELD_BASE(30, 30, 2, 0x0C0, 0x10, 6, 1),
	PIN_FIELD_BASE(31, 31, 2, 0x0C0, 0x10, 10, 1),
	PIN_FIELD_BASE(32, 32, 2, 0x0C0, 0x10, 14, 1),
	PIN_FIELD_BASE(33, 33, 2, 0x0C0, 0x10, 18, 1),
	PIN_FIELD_BASE(34, 34, 2, 0x0C0, 0x10, 22, 1),
	PIN_FIELD_BASE(35, 35, 3, 0x0C0, 0x10, 2, 1),
	PIN_FIELD_BASE(36, 36, 3, 0x0C0, 0x10, 6, 1),
	PIN_FIELD_BASE(37, 37, 3, 0x0C0, 0x10, 10, 1),
	PIN_FIELD_BASE(38, 38, 3, 0x0C0, 0x10, 14, 1),
	PIN_FIELD_BASE(39, 39, 3, 0x0C0, 0x10, 18, 1),
	PIN_FIELD_BASE(40, 40, 3, 0x0C0, 0x10, 22, 1),
	PIN_FIELD_BASE(41, 41, 3, 0x0C0, 0x10, 26, 1),
	PIN_FIELD_BASE(42, 42, 3, 0x0C0, 0x10, 30, 1),
	PIN_FIELD_BASE(91, 91, 6, 0x0C0, 0x10, 2, 1),
	PIN_FIELD_BASE(92, 92, 6, 0x0C0, 0x10, 6, 1),
	PIN_FIELD_BASE(93, 93, 6, 0x0C0, 0x10, 10, 1),
	PIN_FIELD_BASE(94, 94, 6, 0x0C0, 0x10, 14, 1),
	PIN_FIELD_BASE(122, 122, 8, 0x0C0, 0x10, 2, 1),
	PIN_FIELD_BASE(123, 123, 8, 0x0C0, 0x10, 6, 1),
	PIN_FIELD_BASE(124, 124, 8, 0x0C0, 0x10, 10, 1),
	PIN_FIELD_BASE(125, 125, 8, 0x0C0, 0x10, 14, 1),
	PIN_FIELD_BASE(126, 126, 8, 0x0C0, 0x10, 18, 1),
	PIN_FIELD_BASE(127, 127, 8, 0x0C0, 0x10, 22, 1),
	PIN_FIELD_BASE(128, 128, 8, 0x0C0, 0x10, 26, 1),
	PIN_FIELD_BASE(129, 129, 8, 0x0C0, 0x10, 30, 1),
	PIN_FIELD_BASE(130, 130, 8, 0x0D0, 0x10, 2, 1),
	PIN_FIELD_BASE(131, 131, 8, 0x0D0, 0x10, 6, 1),
	PIN_FIELD_BASE(132, 132, 8, 0x0D0, 0x10, 10, 1),
	PIN_FIELD_BASE(133, 133, 8, 0x0D0, 0x10, 14, 1),
};

static const struct mtk_pin_field_calc mt6771_pin_r0_range[] = {
	PIN_FIELD_BASE(29, 29, 2, 0x0C0, 0x10, 0, 1),
	PIN_FIELD_BASE(30, 30, 2, 0x0C0, 0x10, 4, 1),
	PIN_FIELD_BASE(31, 31, 2, 0x0C0, 0x10, 8, 1),
	PIN_FIELD_BASE(32, 32, 2, 0x0C0, 0x10, 12, 1),
	PIN_FIELD_BASE(33, 33, 2, 0x0C0, 0x10, 16, 1),
	PIN_FIELD_BASE(34, 34, 2, 0x0C0, 0x10, 20, 1),
	PIN_FIELD_BASE(35, 35, 3, 0x0C0, 0x10, 0, 1),
	PIN_FIELD_BASE(36, 36, 3, 0x0C0, 0x10, 4, 1),
	PIN_FIELD_BASE(37, 37, 3, 0x0C0, 0x10, 8, 1),
	PIN_FIELD_BASE(38, 38, 3, 0x0C0, 0x10, 12, 1),
	PIN_FIELD_BASE(39, 39, 3, 0x0C0, 0x10, 16, 1),
	PIN_FIELD_BASE(40, 40, 3, 0x0C0, 0x10, 20, 1),
	PIN_FIELD_BASE(41, 41, 3, 0x0C0, 0x10, 24, 1),
	PIN_FIELD_BASE(42, 42, 3, 0x0C0, 0x10, 28, 1),
	PIN_FIELD_BASE(48, 48, 3, 0x0F0, 0x10, 18, 1),
	PIN_FIELD_BASE(49, 49, 3, 0x0F0, 0x10, 13, 1),
	PIN_FIELD_BASE(50, 50, 4, 0x0F0, 0x10, 10, 1),
	PIN_FIELD_BASE(51, 51, 4, 0x0F0, 0x10, 5, 1),
	PIN_FIELD_BASE(81, 81, 5, 0x0F0, 0x10, 7, 1),
	PIN_FIELD_BASE(82, 82, 5, 0x0F0, 0x10, 5, 1),
	PIN_FIELD_BASE(83, 83, 5, 0x0F0, 0x10, 15, 1),
	PIN_FIELD_BASE(84, 84, 5, 0x0F0, 0x10, 17, 1),
	PIN_FIELD_BASE(91, 91, 6, 0x0C0, 0x10, 0, 1),
	PIN_FIELD_BASE(92, 92, 6, 0x0C0, 0x10, 4, 1),
	PIN_FIELD_BASE(93, 93, 6, 0x0C0, 0x10, 8, 1),
	PIN_FIELD_BASE(94, 94, 6, 0x0C0, 0x10, 12, 1),
	PIN_FIELD_BASE(103, 103, 6, 0x0F0, 0x10, 20, 1),
	PIN_FIELD_BASE(104, 104, 6, 0x0F0, 0x10, 10, 1),
	PIN_FIELD_BASE(105, 105, 6, 0x0F0, 0x10, 22, 1),
	PIN_FIELD_BASE(106, 106, 6, 0x0F0, 0x10, 12, 1),
	PIN_FIELD_BASE(122, 122, 8, 0x0C0, 0x10, 0, 1),
	PIN_FIELD_BASE(123, 123, 8, 0x0C0, 0x10, 4, 1),
	PIN_FIELD_BASE(124, 124, 8, 0x0C0, 0x10, 8, 1),
	PIN_FIELD_BASE(125, 125, 8, 0x0C0, 0x10, 12, 1),
	PIN_FIELD_BASE(126, 126, 8, 0x0C0, 0x10, 16, 1),
	PIN_FIELD_BASE(127, 127, 8, 0x0C0, 0x10, 20, 1),
	PIN_FIELD_BASE(128, 128, 8, 0x0C0, 0x10, 24, 1),
	PIN_FIELD_BASE(129, 129, 8, 0x0C0, 0x10, 28, 1),
	PIN_FIELD_BASE(130, 130, 8, 0x0D0, 0x10, 0, 1),
	PIN_FIELD_BASE(131, 131, 8, 0x0D0, 0x10, 4, 1),
	PIN_FIELD_BASE(132, 132, 8, 0x0D0, 0x10, 8, 1),
	PIN_FIELD_BASE(133, 133, 8, 0x0D0, 0x10, 12, 1),
};

static const struct mtk_pin_field_calc mt6771_pin_r1_range[] = {
	PIN_FIELD_BASE(29, 29, 2, 0x0C0, 0x10, 1, 1),
	PIN_FIELD_BASE(30, 30, 2, 0x0C0, 0x10, 5, 1),
	PIN_FIELD_BASE(31, 31, 2, 0x0C0, 0x10, 9, 1),
	PIN_FIELD_BASE(32, 32, 2, 0x0C0, 0x10, 13, 1),
	PIN_FIELD_BASE(33, 33, 2, 0x0C0, 0x10, 17, 1),
	PIN_FIELD_BASE(34, 34, 2, 0x0C0, 0x10, 21, 1),
	PIN_FIELD_BASE(35, 35, 3, 0x0C0, 0x10, 1, 1),
	PIN_FIELD_BASE(36, 36, 3, 0x0C0, 0x10, 5, 1),
	PIN_FIELD_BASE(37, 37, 3, 0x0C0, 0x10, 9, 1),
	PIN_FIELD_BASE(38, 38, 3, 0x0C0, 0x10, 13, 1),
	PIN_FIELD_BASE(39, 39, 3, 0x0C0, 0x10, 17, 1),
	PIN_FIELD_BASE(40, 40, 3, 0x0C0, 0x10, 21, 1),
	PIN_FIELD_BASE(41, 41, 3, 0x0C0, 0x10, 25, 1),
	PIN_FIELD_BASE(42, 42, 3, 0x0C0, 0x10, 29, 1),
	PIN_FIELD_BASE(48, 48, 3, 0x0F0, 0x10, 19, 1),
	PIN_FIELD_BASE(49, 49, 3, 0x0F0, 0x10, 14, 1),
	PIN_FIELD_BASE(50, 50, 4, 0x0F0, 0x10, 11, 1),
	PIN_FIELD_BASE(51, 51, 4, 0x0F0, 0x10, 6, 1),
	PIN_FIELD_BASE(81, 81, 5, 0x0F0, 0x10, 8, 1),
	PIN_FIELD_BASE(82, 82, 5, 0x0F0, 0x10, 6, 1),
	PIN_FIELD_BASE(83, 83, 5, 0x0F0, 0x10, 16, 1),
	PIN_FIELD_BASE(84, 84, 5, 0x0F0, 0x10, 18, 1),
	PIN_FIELD_BASE(91, 91, 6, 0x0C0, 0x10, 1, 1),
	PIN_FIELD_BASE(92, 92, 6, 0x0C0, 0x10, 5, 1),
	PIN_FIELD_BASE(93, 93, 6, 0x0C0, 0x10, 9, 1),
	PIN_FIELD_BASE(94, 94, 6, 0x0C0, 0x10, 13, 1),
	PIN_FIELD_BASE(103, 103, 6, 0x0F0, 0x10, 21, 1),
	PIN_FIELD_BASE(104, 104, 6, 0x0F0, 0x10, 11, 1),
	PIN_FIELD_BASE(105, 105, 6, 0x0F0, 0x10, 23, 1),
	PIN_FIELD_BASE(106, 106, 6, 0x0F0, 0x10, 13, 1),
	PIN_FIELD_BASE(122, 122, 8, 0x0C0, 0x10, 1, 1),
	PIN_FIELD_BASE(123, 123, 8, 0x0C0, 0x10, 5, 1),
	PIN_FIELD_BASE(124, 124, 8, 0x0C0, 0x10, 9, 1),
	PIN_FIELD_BASE(125, 125, 8, 0x0C0, 0x10, 13, 1),
	PIN_FIELD_BASE(126, 126, 8, 0x0C0, 0x10, 17, 1),
	PIN_FIELD_BASE(127, 127, 8, 0x0C0, 0x10, 21, 1),
	PIN_FIELD_BASE(128, 128, 8, 0x0C0, 0x10, 25, 1),
	PIN_FIELD_BASE(129, 129, 8, 0x0C0, 0x10, 29, 1),
	PIN_FIELD_BASE(130, 130, 8, 0x0D0, 0x10, 1, 1),
	PIN_FIELD_BASE(131, 131, 8, 0x0D0, 0x10, 5, 1),
	PIN_FIELD_BASE(132, 132, 8, 0x0D0, 0x10, 9, 1),
	PIN_FIELD_BASE(133, 133, 8, 0x0D0, 0x10, 13, 1),
};

static const struct mtk_pin_reg_calc mt6771_reg_cals[PINCTRL_PIN_REG_MAX] = {
	[PINCTRL_PIN_REG_MODE] = MTK_RANGE(mt6771_pin_mode_range),
	[PINCTRL_PIN_REG_DIR] = MTK_RANGE(mt6771_pin_dir_range),
	[PINCTRL_PIN_REG_DI] = MTK_RANGE(mt6771_pin_di_range),
	[PINCTRL_PIN_REG_DO] = MTK_RANGE(mt6771_pin_do_range),
	[PINCTRL_PIN_REG_SR] = MTK_RANGE(mt6771_pin_dir_range),
	[PINCTRL_PIN_REG_SMT] = MTK_RANGE(mt6771_pin_smt_range),
	[PINCTRL_PIN_REG_IES] = MTK_RANGE(mt6771_pin_ies_range),
	[PINCTRL_PIN_REG_PULLEN] = MTK_RANGE(mt6771_pin_pullen_range),
	[PINCTRL_PIN_REG_PULLSEL] = MTK_RANGE(mt6771_pin_pullsel_range),
	[PINCTRL_PIN_REG_DRV] = MTK_RANGE(mt6771_pin_drv_range),
	[PINCTRL_PIN_REG_PUPD] = MTK_RANGE(mt6771_pin_pupd_range),
	[PINCTRL_PIN_REG_R0] = MTK_RANGE(mt6771_pin_r0_range),
	[PINCTRL_PIN_REG_R1] = MTK_RANGE(mt6771_pin_r1_range),
};

static const struct mtk_eint_hw mt6771_eint_hw = {
	.port_mask = 7,
	.ports     = 6,
	.ap_num    = 212,
	.db_cnt    = 13,
};

static const struct mtk_pin_soc mt6771_data = {
	.reg_cal = mt6771_reg_cals,
	.pins = mtk_pins_mt6771,
	.npins = ARRAY_SIZE(mtk_pins_mt6771),
	.ngrps = ARRAY_SIZE(mtk_pins_mt6771),
	.nfuncs = 8,
	.eint_hw = &mt6771_eint_hw,
	.gpio_m = 0,
	.race_free_access = true,
	.bias_set_combo = mtk_pinconf_bias_set_combo,
	.bias_get_combo = mtk_pinconf_bias_get_combo,
	.drive_set = mtk_pinconf_drive_set_raw,
	.drive_get = mtk_pinconf_drive_get_raw,
};

static const struct of_device_id mt6771_pinctrl_of_match[] = {
	{ .compatible = "mediatek,mt6771-pinctrl", },
	{ }
};

static int mt6771_pinctrl_probe(struct platform_device *pdev)
{
	return mtk_paris_pinctrl_probe(pdev, &mt6771_data);
}

static struct platform_driver mt6771_pinctrl_driver = {
	.driver = {
		.name = "mt6771-pinctrl",
		.of_match_table = mt6771_pinctrl_of_match,
		.pm = &mtk_eint_pm_ops_v2,
	},
	.probe = mt6771_pinctrl_probe,
};

static int __init mt6771_pinctrl_init(void)
{
	return platform_driver_register(&mt6771_pinctrl_driver);
}
arch_initcall(mt6771_pinctrl_init);
