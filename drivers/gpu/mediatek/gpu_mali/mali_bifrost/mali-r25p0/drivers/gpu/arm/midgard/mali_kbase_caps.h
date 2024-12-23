/*
 *
 * (C) COPYRIGHT 2020 ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you can access it online at
 * http://www.gnu.org/licenses/gpl-2.0.html.
 *
 * SPDX-License-Identifier: GPL-2.0
 *
 */


/**
 * @file mali_kbase_caps.h
 *
 * Driver Capability Queries.
 */

#ifndef _KBASE_CAPS_H_
#define _KBASE_CAPS_H_

#include <linux/types.h>

typedef enum mali_kbase_cap {
	MALI_KBASE_CAP_SYSTEM_MONITOR = 0,
	MALI_KBASE_CAP_JIT_PRESSURE_LIMIT,
	MALI_KBASE_CAP_MEM_GROW_ON_GPF,
	MALI_KBASE_CAP_MEM_PROTECTED,
	MALI_KBASE_NUM_CAPS
} mali_kbase_cap;

extern bool mali_kbase_supports_cap(unsigned long api_version, mali_kbase_cap cap);

static inline bool mali_kbase_supports_system_monitor(unsigned long api_version)
{
	return mali_kbase_supports_cap(api_version, MALI_KBASE_CAP_SYSTEM_MONITOR);
}

static inline bool mali_kbase_supports_jit_pressure_limit(unsigned long api_version)
{
	return mali_kbase_supports_cap(api_version, MALI_KBASE_CAP_JIT_PRESSURE_LIMIT);
}

static inline bool mali_kbase_supports_mem_grow_on_gpf(unsigned long api_version)
{
	return mali_kbase_supports_cap(api_version, MALI_KBASE_CAP_MEM_GROW_ON_GPF);
}

static inline bool mali_kbase_supports_mem_protected(unsigned long api_version)
{
	return mali_kbase_supports_cap(api_version, MALI_KBASE_CAP_MEM_PROTECTED);
}

#endif	/* __KBASE_CAPS_H_ */
