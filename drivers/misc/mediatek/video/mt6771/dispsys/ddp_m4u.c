/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#include "ddp_m4u.h"
#include "ddp_dump.h"
#include "ddp_hal.h"
#include "ddp_reg.h"
#include "ddp_log.h"
#include "disp_helper.h"
#include "disp_drv_platform.h"
#include <ion_priv.h>
#include "primary_display.h"

#ifdef CONFIG_MTK_IOMMU
#include "mtk_iommu.h"
#include <dt-bindings/memory/mt6763-larb-port.h>
#endif

/**
 * display m4u port / display module mapping table
 * -- by chip
 */
#define DISP_PAGE_MASK 0xfffL
#ifdef MTKFB_M4U_SUPPORT
struct m4u_client_t *g_client;
#endif

static struct module_to_m4u_port_t module_to_m4u_port_mapping[] = {
	{DISP_MODULE_OVL0, 0, DISP_M4U_PORT_DISP_OVL0},
	{DISP_MODULE_OVL0_2L, 0, DISP_M4U_PORT_DISP_OVL0_2L},
	{DISP_MODULE_OVL1_2L, 0, DISP_M4U_PORT_DISP_OVL1_2L},
	{DISP_MODULE_RDMA0, 0, DISP_M4U_PORT_DISP_RDMA0},
	{DISP_MODULE_RDMA1, 0, DISP_M4U_PORT_DISP_RDMA1},
	{DISP_MODULE_WDMA0, 0, DISP_M4U_PORT_DISP_WDMA0},
};

int module_to_m4u_port(enum DISP_MODULE_ENUM module)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(module_to_m4u_port_mapping); i++)
		if (module_to_m4u_port_mapping[i].module == module)
			return module_to_m4u_port_mapping[i].port;

	DDPPR_ERR("%s, get m4u port fail(module=%s)\n",
	       __func__, ddp_get_module_name(module));
	return M4U_PORT_NR;
}

int module_to_m4u_larb(enum DISP_MODULE_ENUM module)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(module_to_m4u_port_mapping); i++)
		if (module_to_m4u_port_mapping[i].module == module)
			return module_to_m4u_port_mapping[i].larb;

	DDPPR_ERR("module_to_m4u_port, get m4u larb fail(module=%s)\n",
	       ddp_get_module_name(module));
	return M4U_PORT_NR;
}

enum DISP_MODULE_ENUM m4u_port_to_module(int port)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(module_to_m4u_port_mapping); i++)
		if (module_to_m4u_port_mapping[i].port == port)
			return module_to_m4u_port_mapping[i].module;

	DDPPR_ERR("%s: unknown port=%d\n", __func__, port);
	return DISP_MODULE_UNKNOWN;
}

void disp_m4u_init(void)
{
	unsigned int i;

	if (disp_helper_get_option(DISP_OPT_USE_M4U)) {
		/* init M4U callback */
		DDPMSG("register m4u callback\n");
		for (i = 0; i < ARRAY_SIZE(module_to_m4u_port_mapping); i++) {
#ifdef CONFIG_MTK_IOMMU
			mtk_iommu_register_fault_callback(
				module_to_m4u_port_mapping[i].port,
				(mtk_iommu_fault_callback_t *)disp_m4u_callback,
				0);
#elif defined(CONFIG_MTK_M4U)
			m4u_register_fault_callback(
				module_to_m4u_port_mapping[i].port,
				(m4u_fault_callback_t *)disp_m4u_callback, 0);
#endif
		}
	} else {
		/* disable m4u port, used for m4u not ready */
		DDPMSG("m4u not enable, disable m4u port\n");
		for (i = 0; i < 4; i++)
			DISP_REG_SET_FIELD(0, REG_FLD_MMU_EN,
				DISP_REG_SMI_LARB0_NON_SEC_CON + i * 4, 0);

		for (i = 0; i < 3; i++)
			DISP_REG_SET_FIELD(0, REG_FLD_MMU_EN,
				DISP_REG_SMI_LARB1_NON_SEC_CON + i * 4, 0);
	}
}

int config_display_m4u_port(void)
{
	int ret = 0;
#ifdef CONFIG_MTK_IOMMU
	/* if you config to pa mode, please contact iommu owner */
	struct device *disp_larbdev = NULL;

	ret = pm_runtime_get_sync(disp_larbdev);
#elif defined(CONFIG_MTK_M4U)
	struct M4U_PORT_STRUCT sPort;
	unsigned int i;
	char *m4u_usage = disp_helper_get_option(DISP_OPT_USE_M4U) ?
						"virtual" : "physical";

	sPort.ePortID = M4U_PORT_NR;
	sPort.Virtuality = disp_helper_get_option(DISP_OPT_USE_M4U);
	sPort.Security = 0;
	sPort.Distance = 1;
	sPort.Direction = 0;

	for (i = 0; i < ARRAY_SIZE(module_to_m4u_port_mapping); i++) {
		sPort.ePortID = module_to_m4u_port_mapping[i].port;
		ret = m4u_config_port(&sPort);
		if (ret) {
			enum DISP_MODULE_ENUM module;

			module = module_to_m4u_port_mapping[i].module;
			DDPPR_ERR("config M4U Port %s to %s FAIL(ret=%d)\n",
				ddp_get_module_name(module), m4u_usage, ret);
			return -1;
		}
	}
#endif
	return ret;
}

int disp_m4u_callback(int port, unsigned long mva, void *data)
{
	enum DISP_MODULE_ENUM module;

	DDPPR_ERR("fault call port=%d, mva=0x%lx, data=0x%p\n",
			port, mva, data);
	module = m4u_port_to_module(port);
	ddp_dump_analysis(module);
	ddp_dump_reg(module);
	primary_display_diagnose();

	return M4U_CALLBACK_HANDLED;
}

int disp_mva_map_kernel(enum DISP_MODULE_ENUM module, unsigned int mva,
			unsigned int size, unsigned long *map_va,
			unsigned int *map_size)
{
#ifdef CONFIG_MTK_IOMMU
	map_va = mtk_iommu_iova_to_va(NULL, mva, size);
#elif defined(CONFIG_MTK_M4U)
	m4u_mva_map_kernel(mva, size, map_va, map_size);
#endif

	return 0;
}

int disp_mva_unmap_kernel(unsigned int mva, unsigned int size,
			  unsigned long map_va)
{
#ifdef CONFIG_MTK_IOMMU
	vunmap((void *)(map_va & (~DISP_PAGE_MASK)));
#else
	vunmap((void *)(map_va & (~DISP_PAGE_MASK)));
#endif
	return 0;
}

struct ion_client *disp_ion_create(const char *name)
{
	struct ion_client *disp_ion_client = NULL;

#if defined(MTK_FB_ION_SUPPORT)
	if (g_ion_device)
		disp_ion_client = ion_client_create(g_ion_device, name);

	if (!disp_ion_client) {
		DDPPR_ERR("create ion client failed!\n");
		return NULL;
	}

	DDPDBG("create ion client 0x%p\n", disp_ion_client);
#endif
	return disp_ion_client;
}

struct ion_handle *disp_ion_alloc(struct ion_client *client,
				  unsigned int heap_id_mask, size_t align,
				  unsigned int size)
{
	struct ion_handle *disp_handle = NULL;

#if defined(MTK_FB_ION_SUPPORT)
	disp_handle = ion_alloc(client, size, align, heap_id_mask, 0);
	if (IS_ERR(disp_handle)) {
		DDPPR_ERR("%s 1error %p\n", __func__, disp_handle);
		DDPPR_ERR("2error %p\n", disp_handle);
		DDPPR_ERR("3error %p\n", disp_handle);
		DDPPR_ERR("4error %p\n", disp_handle);
		return NULL;
	}

	DDPDBG("%s 0x%p\n", __func__, disp_handle);
#endif
	return disp_handle;
}

int disp_ion_get_mva(struct ion_client *client, struct ion_handle *handle,
		     unsigned int *mva, int port)
{
#if defined(MTK_FB_ION_SUPPORT)
	struct ion_mm_data mm_data;
	size_t mva_size;
	ion_phys_addr_t phy_addr = 0;

	memset((void *)&mm_data, 0, sizeof(struct ion_mm_data));
	mm_data.config_buffer_param.module_id = port;
	mm_data.config_buffer_param.kernel_handle = handle;
	mm_data.mm_cmd = ION_MM_CONFIG_BUFFER;
	if (ion_kernel_ioctl(client, ION_CMD_MULTIMEDIA,
			     (unsigned long)&mm_data) < 0) {
		DDPPR_ERR("%s: config buffer failed.0x%p -0x%p\n",
			__func__, client, handle);
		ion_free(client, handle);
		return -1;
	}

	ion_phys(client, handle, &phy_addr, &mva_size);
	*mva = (unsigned int)phy_addr;
	DDPDBG("alloc mmu addr hnd=0x%p,mva=0x%08x\n",
	       handle, (unsigned int)*mva);
#endif
	return 0;
}

struct ion_handle *disp_ion_import_handle(struct ion_client *client, int fd)
{
	struct ion_handle *handle = NULL;
#if defined(MTK_FB_ION_SUPPORT)
	struct ion_mm_data mm_data;

	/* If no need ION support, do nothing! */
	if (fd <= 0) {
		DDPPR_ERR("NO NEED ion support, fd %d\n", fd);
		return handle;
	}

	if (!client) {
		DDPPR_ERR("invalid ion client!\n");
		return handle;
	}

	handle = ion_import_dma_buf_fd(client, fd);
	if (IS_ERR(handle)) {
		DDPPR_ERR("import ion handle failed!\n");
		return NULL;
	}
	mm_data.mm_cmd = ION_MM_CONFIG_BUFFER;
	mm_data.config_buffer_param.kernel_handle = handle;
	mm_data.config_buffer_param.module_id = 0;
	mm_data.config_buffer_param.security = 0;
	mm_data.config_buffer_param.coherent = 0;

	if (ion_kernel_ioctl(client, ION_CMD_MULTIMEDIA,
			     (unsigned long)&mm_data))
		DDPPR_ERR("configure ion buffer failed!\n");

	DDPDBG("import ion handle fd=%d,hnd=0x%p\n", fd, handle);
#endif
	return handle;
}

void disp_ion_free_handle(struct ion_client *client, struct ion_handle *handle)
{
#if defined(MTK_FB_ION_SUPPORT)
	if (!client) {
		DDPPR_ERR("invalid ion client!\n");
		return;
	}
	if (!handle)
		return;

	ion_free(client, handle);

	DDPDBG("free ion handle 0x%p\n", handle);
#endif
}

void disp_ion_destroy(struct ion_client *client)
{
#if defined(MTK_FB_ION_SUPPORT)
	if (client && g_ion_device)
		ion_client_destroy(client);
#endif
}

void disp_ion_cache_flush(struct ion_client *client, struct ion_handle *handle,
			  enum ION_CACHE_SYNC_TYPE sync_type)
{
#if defined(MTK_FB_ION_SUPPORT)
	struct ion_sys_data sys_data;
	void *buffer_va;

	if (!client || !handle)
		return;

	sys_data.sys_cmd = ION_SYS_CACHE_SYNC;
	sys_data.cache_sync_param.kernel_handle = handle;
	sys_data.cache_sync_param.sync_type = ION_CACHE_FLUSH_BY_RANGE;

	buffer_va = ion_map_kernel(client, handle);
	sys_data.cache_sync_param.va = buffer_va;
	sys_data.cache_sync_param.size = handle->buffer->size;

	if (ion_kernel_ioctl(client, ION_CMD_SYSTEM, (unsigned long)&sys_data))
		DDPPR_ERR("ion cache flush failed!\n");

	ion_unmap_kernel(client, handle);
#endif
}

#ifndef CONFIG_MTK_IOMMU
#ifdef MTKFB_M4U_SUPPORT
static struct sg_table table;
#endif

int disp_allocate_mva(struct m4u_client_t *client, enum DISP_MODULE_ENUM module,
		      unsigned long va, struct sg_table *sg_table,
		      unsigned int size, unsigned int prot,
		      unsigned int flags, unsigned int *pMva)
{
	int port = module_to_m4u_port(module);

	if (port == M4U_PORT_NR)
		return 1; /* err */

#ifdef MTKFB_M4U_SUPPORT
	return m4u_alloc_mva(client, port, va, sg_table, size, prot, flags,
			     pMva);
#else
	return 0;
#endif
}

int disp_hal_allocate_framebuffer(phys_addr_t pa_start, phys_addr_t pa_end,
				  unsigned long *va, unsigned long *mva)
{
#ifdef MTKFB_M4U_SUPPORT
	int ret = 0;
#endif

#ifndef FREE_FB_BUFFER
	*va = (unsigned long)ioremap_nocache(pa_start, pa_end - pa_start + 1);
#else
	*va = (unsigned long)__va(pa_start);
#endif
	pr_debug("%s: pa_start=0x%pa, pa_end=0x%pa, va=0x%lx\n",
		 __func__, &pa_start, &pa_end, *va);

	if (disp_helper_get_option(DISP_OPT_USE_M4U)) {
#ifdef MTKFB_M4U_SUPPORT
		struct sg_table *sg_table = &table;

		sg_alloc_table(sg_table, 1, GFP_KERNEL);

		sg_dma_address(sg_table->sgl) = pa_start;
		sg_dma_len(sg_table->sgl) = (pa_end - pa_start + 1);
		g_client = m4u_create_client();
		if (IS_ERR_OR_NULL(g_client))
			DDPPR_ERR("create client fail!\n");

		*mva = pa_start & 0xffffffffULL;
		ret = m4u_alloc_mva(g_client, DISP_M4U_PORT_DISP_OVL0, 0,
				    sg_table, (pa_end - pa_start + 1),
				    M4U_PROT_READ | M4U_PROT_WRITE, 0,
				    (unsigned int *)mva);
		if (ret)
			DDPPR_ERR("m4u_alloc_mva returns fail: %d\n", ret);

		pr_debug("[DISPHAL] FB MVA is 0x%lx PA is 0x%pa\n",
			 *mva, &pa_start);
#endif
	} else {
		*mva = pa_start & 0xffffffffULL;
	}

	pr_info("disphal_allocate_fb, pa=%pa, va=0x%lx mva=0x%lx\n",
		&pa_start, *va, *mva);
	return 0;
}

#endif /* !CONFIG_MTK_IOMMU */
