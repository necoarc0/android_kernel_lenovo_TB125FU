/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#ifndef _DISP_ARR_H_
#define _DISP_ARR_H_

/* used by ARR2.0 */
int primary_display_get_cur_refresh_rate(void);
int primary_display_get_max_refresh_rate(void);
int primary_display_get_min_refresh_rate(void);
int primary_display_set_refresh_rate(unsigned int refresh_rate);
int primary_display_force_vdo_mode(unsigned int force_on);

#endif /* _DISP_ARR_H_ */
