// SPDX-License-Identifier: GPL-2.0-or-later

/*
 * Copyright (C) 2018 MediaTek Inc.
 */

#include <linux/kthread.h>

#include "mtk_spm_vcore_dvfs_ipi.h"
#include "v1/sspm_ipi.h"
#include <mtk_vcorefs_governor.h>

#ifdef CONFIG_MTK_QOS_SUPPORT
#include <helio-dvfsrc.h>
#endif

#if defined(CONFIG_MTK_TINYSYS_SSPM_SUPPORT)
static int qos_recv_thread(void *arg)
{
	struct ipi_action qos_isr;
	struct qos_data qos_d;
	unsigned int rdata, ret;

	qos_isr.data = &qos_d;

	ret = sspm_ipi_recv_registration(IPI_ID_QOS, &qos_isr);

	if (ret) {
		pr_info("[name:spm&]@%s: sspm_ipi_recv_registration failed (%d)\n",
			__func__, ret);
		return 0;
	}

	while (1) {
		rdata = 0;
		sspm_ipi_recv_wait(IPI_ID_QOS);

		switch (qos_d.cmd) {
		case QOS_IPI_ERROR_HANDLER:
			sspm_ipi_send_ack(IPI_ID_QOS, &rdata);
			break;
		}
	}
	return 0;
}

void helio_dvfsrc_sspm_ipi_init(int dvfs_en, int dram_type)
{
	struct qos_data qos_d;
	struct task_struct *qos_task;

	qos_d.cmd = QOS_IPI_QOS_ENABLE;
	qos_d.u.qos_init.enable = 1;
	qos_d.u.qos_init.dvfs_en = dvfs_en;
	qos_d.u.qos_init.spm_dram_type = dram_type;
	qos_ipi_to_sspm_command(&qos_d, 4);

	qos_task = kthread_run(qos_recv_thread, NULL, "qos_recv");
}
#endif

