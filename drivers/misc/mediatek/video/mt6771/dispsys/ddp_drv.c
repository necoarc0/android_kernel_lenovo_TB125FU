/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#define LOG_TAG "ddp_drv"

#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <generated/autoconf.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/param.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/timer.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
/* ION */
/* #include <linux/ion.h> */
/* #include <linux/ion_drv.h> */
/* #include "m4u.h" */
#include <linux/vmalloc.h>
#include <linux/dma-mapping.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/io.h>
/* #include "mt-plat/mtk_smi.h" */
/* #include <mach/mt_reg_base.h> */
/* #include <mach/mt_irq.h> */
#include "ddp_clkmgr.h"
/* #include "mach/mt_irq.h" */
#include "mt-plat/sync_write.h"
/* #include "mt-plat/mtk_smi.h" */

#include "ddp_drv.h"
#include "ddp_reg.h"
#include "ddp_hal.h"
#include "ddp_log.h"
#include "ddp_irq.h"
#include "ddp_info.h"
#include "ddp_m4u.h"
#include "display_recorder.h"

/* #define DISP_NO_DPI */
#ifndef DISP_NO_DPI
#include "ddp_dpi_reg.h"
#endif
#include "disp_helper.h"


#define DISP_DEVNAME "DISPSYS"
/* device and driver */
static dev_t disp_devno;
static struct cdev *disp_cdev;
static struct class *disp_class;

struct disp_node_struct {
	pid_t open_pid;
	pid_t open_tgid;
	struct list_head testList;
	spinlock_t node_lock;
};

static struct platform_device mydev;
cmdqBackupSlotHandle dispsys_slot;

#if 0 /* defined but not used */
static unsigned int ddp_ms2jiffies(unsigned long ms)
{
	return (ms * HZ + 512) >> 10;
}
#endif

static int _disp_init_cmdq_slots(cmdqBackupSlotHandle *pSlot,
		int count, int init_val)
{
	int i;

	cmdqBackupAllocateSlot(pSlot, count);

	for (i = 0; i < count; i++)
		cmdqBackupWriteSlot(*pSlot, i, init_val);

	return 0;
}

static int _disp_get_cmdq_slots(cmdqBackupSlotHandle Slot,
		unsigned int slot_index, unsigned int *value)
{
	int ret;

	ret = cmdqBackupReadSlot(Slot, slot_index, value);

	/* cmdq get slot fail */
	if (ret)
		DDPERR("DISP CMDQ get slot failed:%d\n", ret);

	return ret;
}

int disp_get_ovl_bandwidth(unsigned int in_fps, unsigned int out_fps,
		unsigned int *bandwidth)
{
	int ret = 0;
	unsigned int is_dc;
	unsigned int ovl0_bw, ovl0_2l_bw, rdma0_bw, wdma0_bw;

	ret |= _disp_get_cmdq_slots(DISPSYS_SLOT_BASE,
		DISP_SLOT_IS_DC, &is_dc);
	ret |= _disp_get_cmdq_slots(DISPSYS_SLOT_BASE,
		DISP_SLOT_OVL0_BW, &ovl0_bw);
	ret |= _disp_get_cmdq_slots(DISPSYS_SLOT_BASE,
		DISP_SLOT_OVL0_2L_BW, &ovl0_2l_bw);
	ret |= _disp_get_cmdq_slots(DISPSYS_SLOT_BASE,
		DISP_SLOT_RDMA0_BW, &rdma0_bw);
	ret |= _disp_get_cmdq_slots(DISPSYS_SLOT_BASE,
		DISP_SLOT_WDMA0_BW, &wdma0_bw);

	/* cmdq get slot fail */
	if (ret) {
		DDPERR("DISP CMDQ get slot failed:%d\n", ret);
		*bandwidth = 0;

		return ret;
	}

	if (is_dc) {
		*bandwidth = ((ovl0_bw + ovl0_2l_bw + wdma0_bw) * in_fps) +
			 (rdma0_bw * out_fps);
	} else
		*bandwidth = (ovl0_bw + ovl0_2l_bw) * out_fps;
	*bandwidth /= 1000;

	return ret;
}

int disp_get_rdma_bandwidth(unsigned int out_fps, unsigned int *bandwidth)
{
	int ret = 0;
	unsigned int rdma0_bw;

	ret = _disp_get_cmdq_slots(DISPSYS_SLOT_BASE, DISP_SLOT_RDMA0_BW,
			&rdma0_bw);

	/* cmdq get slot fail */
	if (ret) {
		DDPERR("DISP CMDQ get slot failed:%d\n", ret);
		*bandwidth = 0;
	} else {
		*bandwidth = rdma0_bw * out_fps;
		*bandwidth /= 1000;
	}

	return ret;
}

static long disp_unlocked_ioctl(struct file *file, unsigned int cmd,
		unsigned long arg)
{
	return 0;
}

static long disp_compat_ioctl(struct file *file, unsigned int cmd,
			      unsigned long arg)
{
#if defined(CONFIG_TRUSTONIC_TEE_SUPPORT) && \
	defined(CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT)
	if (cmd == DISP_IOCTL_SET_TPLAY_HANDLE)
		return disp_unlocked_ioctl(file, cmd, arg);
#endif

	return 0;
}

static int disp_open(struct inode *inode, struct file *file)
{
	struct disp_node_struct *pNode = NULL;

	DDPDBG("enter %s process:%s\n", __func__, current->comm);

	/* allocate and initialize private data */
	file->private_data = kmalloc(sizeof(struct disp_node_struct),
				     GFP_ATOMIC);
	if (!file->private_data) {
		DDPMSG("Not enough entry for DDP open operation\n");
		return -ENOMEM;
	}

	pNode = (struct disp_node_struct *)file->private_data;
	pNode->open_pid = current->pid;
	pNode->open_tgid = current->tgid;
	INIT_LIST_HEAD(&(pNode->testList));
	spin_lock_init(&pNode->node_lock);

	return 0;
}

static ssize_t disp_read(struct file *file, char __user *data, size_t len,
			 loff_t *ppos)
{
	return 0;
}

static int disp_release(struct inode *inode, struct file *file)
{
	struct disp_node_struct *pNode = NULL;

	DDPDBG("enter %s() process:%s\n", __func__, current->comm);

	pNode = (struct disp_node_struct *)file->private_data;

	spin_lock(&pNode->node_lock);
	spin_unlock(&pNode->node_lock);

	kfree(file->private_data);
	file->private_data = NULL;

	return 0;
}

static int disp_flush(struct file *file, fl_owner_t a_id)
{
	return 0;
}

/* remap register to user space */
#if defined(CONFIG_MTK_ENG_BUILD)
static int disp_mmap(struct file *file, struct vm_area_struct *a_pstVMArea)
{
#if (defined(CONFIG_MTK_TEE_GP_SUPPORT) || \
	defined(CONFIG_TRUSTONIC_TEE_SUPPORT)) && \
	defined(CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT)
	a_pstVMArea->vm_page_prot = pgprot_noncached(a_pstVMArea->vm_page_prot);
	if (remap_pfn_range(a_pstVMArea, a_pstVMArea->vm_start,
			    a_pstVMArea->vm_pgoff,
			    (a_pstVMArea->vm_end - a_pstVMArea->vm_start),
			    a_pstVMArea->vm_page_prot)) {
		DDPPR_ERR("MMAP failed!!\n");
		return -1;
	}
#endif

	return 0;
}
#endif /* CONFIG_MTK_ENG_BUILD */

struct dispsys_device {
	struct device *dev;
};

struct device *disp_get_device(void)
{
	return &(mydev.dev);
}

#if (defined(CONFIG_MTK_TEE_GP_SUPPORT) || \
	defined(CONFIG_TRUSTONIC_TEE_SUPPORT)) && \
	defined(CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT)
static struct miscdevice disp_misc_dev;
#endif

/* Kernel interface */
static const struct file_operations disp_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = disp_unlocked_ioctl,
	.compat_ioctl = disp_compat_ioctl,
	.open = disp_open,
	.release = disp_release,
	.flush = disp_flush,
	.read = disp_read,
#if defined(CONFIG_MTK_ENG_BUILD)
	.mmap = disp_mmap
#endif
};

/* disp_clk_init
 * 1. parsing dtsi
 * 2. clk force on
 */
static void disp_clk_init(struct platform_device *pdev)
{
#ifdef ENABLE_CLK_MGR
	int i;
	struct clk *pclk = NULL;

	DDPMSG("DT disp clk parse beign\n");

	for (i = 0; i < MAX_DISP_CLK_CNT; i++) {
		DDPMSG("DISPSYS get clock %s\n", ddp_get_clk_name(i));
		pclk = devm_clk_get(&pdev->dev, ddp_get_clk_name(i));
		if (IS_ERR(pclk)) {
			DDPPR_ERR("%s:%d, DISPSYS get %d,%s clock error!!!\n",
			       __FILE__, __LINE__, i, ddp_get_clk_name(i));
			continue;
		}
		ddp_set_clk_handle(pclk, i);
	}

	DDPMSG("DT disp clk parse end\n");

	/* disp-clk force on */
	ddp_clk_force_on(1);
#endif
}

static int disp_probe(struct platform_device *pdev)
{
	static unsigned int disp_probe_cnt;

	if (disp_probe_cnt)
		return 0;

	pr_info("disp driver(1) %s begin\n", __func__);

	/* save pdev for disp_probe_1 */
	memcpy(&mydev, pdev, sizeof(mydev));

	disp_helper_option_init();

	if (disp_helper_get_stage() == DISP_HELPER_STAGE_NORMAL)
		disp_clk_init(pdev);

	disp_probe_cnt++;

	pr_info("disp driver(1) %s end\n", __func__);

	return 0;
}

/* begin for irq check */
static inline unsigned int gic_irq(struct irq_data *d)
{
	return d ? d->hwirq : 0;
}

static inline unsigned int virq_to_hwirq(unsigned int virq)
{
	struct irq_desc *desc;
	unsigned int hwirq;

	desc = irq_to_desc(virq);

	WARN_ON(!desc);
	if (!desc)
		return 0;

	hwirq = gic_irq(&desc->irq_data);

	return hwirq;
}
/* end for irq check */

static int __init disp_probe_1(void)
{
	int ret = 0;
	int i;
	unsigned long va;
	unsigned int irq;

	pr_info("disp driver(1) %s begin\n", __func__);

#if (defined(CONFIG_MTK_TEE_GP_SUPPORT) || \
	defined(CONFIG_TRUSTONIC_TEE_SUPPORT)) && \
	defined(CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT)
	disp_misc_dev.minor = MISC_DYNAMIC_MINOR;
	disp_misc_dev.name = "mtk_disp";
	disp_misc_dev.fops = &disp_fops;
	disp_misc_dev.parent = NULL;
	ret = misc_register(&disp_misc_dev);
	if (ret) {
		pr_err("disp: fail to create mtk_disp node\n");
		return (unsigned long)(ERR_PTR(ret));
	}
#endif
	/* do disp_init_irq before register irq */
	disp_init_irq();

	/* iomap registers and irq */
	for (i = 0; i < DISP_MODULE_NUM; i++) {
		int status;
		struct device_node *node = NULL;
		struct resource res;

		if (!is_ddp_module_has_reg_info(i))
			continue;

		node = of_find_compatible_node(NULL, NULL,
					       ddp_get_module_dtname(i));
		if (node == NULL) {
			DDPERR("[ERR]DT, i=%d, module=%s, unable to find node, dt_name=%s\n",
			       i, ddp_get_module_name(i),
			       ddp_get_module_dtname(i));
			continue;
		}

		va = (unsigned long)of_iomap(node, 0);
		if (!va) {
			DDPERR("[ERR]DT, i=%d, module=%s, unable to ge VA, of_iomap fail\n",
			       i, ddp_get_module_name(i));
			continue;
		} else {
			ddp_set_module_va(i, va);
		}

		status = of_address_to_resource(node, 0, &res);
		if (status < 0) {
			DDPERR("[ERR]DT, i=%d, module=%s, unable to get PA\n",
			       i, ddp_get_module_name(i));
			continue;
		}

		if (ddp_get_module_pa(i) != res.start)
			DDPERR("[ERR]DT, i=%d, module=%s, map_addr=%p, reg_pa=0x%lx!=0x%pa\n",
			       i, ddp_get_module_name(i),
			       (void *)ddp_get_module_va(i),
			       ddp_get_module_pa(i),
			       (void *)(uintptr_t)res.start);

		/* get IRQ ID and request IRQ */
		irq = irq_of_parse_and_map(node, 0);
		ddp_set_module_irq(i, irq);

		DDPMSG("DT, i=%d, module=%s, map_addr=%p, map_irq=%d, reg_pa=0x%lx\n",
		       i, ddp_get_module_name(i), (void *)ddp_get_module_va(i),
		       ddp_get_module_irq(i), ddp_get_module_pa(i));
	}

	/* initialize display slot */
	_disp_init_cmdq_slots(&(dispsys_slot), DISP_SLOT_NUM, 0);

	/* register irq */
	for (i = 0; i < DISP_MODULE_NUM; i++) {
		if (ddp_is_irq_enable(i) != 1)
			continue;

		if (ddp_get_module_irq(i) == 0) {
			DDPERR("[ERR]DT, i=%d, module=%s, map_irq=%d\n",
			       i, ddp_get_module_name(i),
			       ddp_get_module_irq(i));
			ddp_module_irq_disable(i);
			continue;
		}

#ifdef CONFIG_MTK_SYSIRQ
		/* In MTK SYSIRQ, the irq offset has been removed. */
		if (ddp_get_module_checkirq(i) - 32 !=
		    virq_to_hwirq(ddp_get_module_irq(i))) {
#else
		if (ddp_get_module_checkirq(i) !=
		    virq_to_hwirq(ddp_get_module_irq(i))) {
#endif
			DDPERR("[ERR]DT, i=%d, module=%s, map_irq=%d, virtohw_irq=%d, check_irq=%d\n",
			       i, ddp_get_module_name(i), ddp_get_module_irq(i),
			       virq_to_hwirq(ddp_get_module_irq(i)),
			       ddp_get_module_checkirq(i));

			ddp_module_irq_disable(i);
			continue;
		}

		/*
		 * IRQF_TRIGGER_NONE dose not take effect here,
		 * real trigger mode set in dts file
		 */
		ret = request_irq(ddp_get_module_irq(i),
				  (irq_handler_t)disp_irq_handler,
				  IRQF_TRIGGER_NONE, ddp_get_module_name(i),
				  NULL);
		if (ret) {
			DDPERR("[ERR]DT, i=%d, module=%s, req_irq(%d) fail\n",
			       i, ddp_get_module_name(i),
			       ddp_get_module_irq(i));
			continue;
		}
		DDPMSG("irq enabled, module=%s, irq=%d\n",
		       ddp_get_module_name(i), ddp_get_module_irq(i));
	}

	DPI_REG = (struct DPI_REGS *)ddp_get_module_va(DISP_MODULE_DPI);

	/* power on MMSYS for early porting */
#ifdef CONFIG_FPGA_EARLY_PORTING
	DDPMSG("[FPGA Only] before power on MMSYS:0x%x,0x%x\n",
	       DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0),
	       DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1));

	DISP_REG_SET(NULL, DISP_REG_CONFIG_MMSYS_CG_CLR0, 0xFFFFFFFF);
	DISP_REG_SET(NULL, DISP_REG_CONFIG_MMSYS_CG_CLR1, 0xFFFFFFFF);
	DDPMSG("[FPGA Only] before power on MMSYS:0x%x,0x%x\n",
	       DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON0),
	       DISP_REG_GET(DISP_REG_CONFIG_MMSYS_CG_CON1));
#endif

	ddp_path_init();
	disp_m4u_init();

	pr_info("disp driver(1) %s end\n", __func__);
	/* NOT_REFERENCED(class_dev); */
	return ret;
}

static int disp_remove(struct platform_device *pdev)
{
#if (defined(CONFIG_MTK_TEE_GP_SUPPORT) || \
	defined(CONFIG_TRUSTONIC_TEE_SUPPORT)) && \
	defined(CONFIG_MTK_SEC_VIDEO_PATH_SUPPORT)
	misc_deregister(&disp_misc_dev);
#endif
	return 0;
}

static void disp_shutdown(struct platform_device *pdev)
{
	/* Nothing yet */
}

/* PM suspend */
static int disp_suspend(struct platform_device *pdev, pm_message_t mesg)
{
	return 0;
}

/* PM resume */
static int disp_resume(struct platform_device *pdev)
{
	return 0;
}

static const struct of_device_id dispsys_of_ids[] = {
	{.compatible = "mediatek,dispsys",},
	{}
};

static struct platform_driver dispsys_of_driver = {
	.driver = {
		   .name = DISP_DEVNAME,
		   .owner = THIS_MODULE,
		   .of_match_table = dispsys_of_ids,
		   },
	.probe = disp_probe,
	.remove = disp_remove,
	.shutdown = disp_shutdown,
	.suspend = disp_suspend,
	.resume = disp_resume,
};

static int __init disp_init(void)
{
	int ret = 0;

	init_log_buffer();
	DDPMSG("Register the disp driver\n");
	if (platform_driver_register(&dispsys_of_driver)) {
		DDPPR_ERR("failed to register disp driver\n");
		/* platform_device_unregister(&disp_device); */
		ret = -ENODEV;
		return ret;
	}
	DDPMSG("disp driver init done\n");
	return 0;
}

static void __exit disp_exit(void)
{
	ASSERT(0);
	/* disp-clk force on disable ??? */

	cdev_del(disp_cdev);
	unregister_chrdev_region(disp_devno, 1);

	platform_driver_unregister(&dispsys_of_driver);

	device_destroy(disp_class, disp_devno);
	class_destroy(disp_class);
}

static int __init disp_late(void)
{
	int ret = 0;

	DDPMSG("disp driver(1) %s begin\n", __func__);
	/* for rt5081 */
	ret = display_bias_regulator_init();
	if (ret < 0)
		pr_err("get dsv_pos fail, ret = %d\n", ret);

	display_bias_enable();

	DDPMSG("disp driver(1) %s end\n", __func__);
	return 0;
}

#ifndef MTK_FB_DO_NOTHING
arch_initcall(disp_init);
module_init(disp_probe_1);
module_exit(disp_exit);
late_initcall(disp_late);
#endif
MODULE_AUTHOR("Tzu-Meng, Chung <Tzu-Meng.Chung@mediatek.com>");
MODULE_DESCRIPTION("Display subsystem Driver");
MODULE_LICENSE("GPL");
