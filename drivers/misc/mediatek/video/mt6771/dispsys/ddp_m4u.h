/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#ifndef __DSI_M4U_H__
#define __DSI_M4U_H__

#ifdef CONFIG_MTK_M4U
#include "m4u.h"
#include "m4u_port.h"
#endif
#include "ddp_hal.h"
#include "mtk_ion.h"
#include "ion_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * display m4u port wrapper
 * -- by chip
 */
#define DISP_M4U_PORT_DISP_OVL0 M4U_PORT_DISP_OVL0
#define DISP_M4U_PORT_DISP_OVL0_2L M4U_PORT_DISP_2L_OVL0_LARB0
#define DISP_M4U_PORT_DISP_OVL1_2L M4U_PORT_DISP_2L_OVL1_LARB0
#define DISP_M4U_PORT_DISP_RDMA0 M4U_PORT_DISP_RDMA0
#define DISP_M4U_PORT_DISP_RDMA1 M4U_PORT_DISP_RDMA1
#define DISP_M4U_PORT_DISP_WDMA0 M4U_PORT_DISP_WDMA0

/* DUMMY M4U struct for early porting */
#ifndef CONFIG_MTK_M4U

struct m4u_port {
	/* hardware port ID, defined in M4U_PORT_ID */
	int ePortID;
	unsigned int Virtuality;
	unsigned int Security;
	/* domain : 0 1 2 3 */
	unsigned int domain;
	unsigned int Distance;
	/* 0:- 1:+ */
	unsigned int Direction;
};

enum {
	M4U_PORT_DISP_OVL0,
	M4U_PORT_DISP_2L_OVL0_LARB0,
	M4U_PORT_DISP_2L_OVL1_LARB0,
	M4U_PORT_DISP_RDMA0,
	M4U_PORT_DISP_RDMA1,
	M4U_PORT_DISP_WDMA0,
	M4U_PORT_MDP_RDMA0,
	M4U_PORT_MDP_WROT0,
	M4U_PORT_MDP_WDMA0,
	M4U_PORT_DISP_FAKE0,

	M4U_PORT_UNKNOWN
};
#define M4U_PORT_NR M4U_PORT_UNKNOWN

enum m4u_callback_ret {
	M4U_CALLBACK_HANDLED,
	M4U_CALLBACK_NOT_HANDLED,
};

struct m4u_client {
	/* mutex to protect mvaList */
	/*
	 * should get this mutex whenever
	 * add/delete/interate mvaList
	 */
	struct mutex dataMutex;
	pid_t open_pid;
	pid_t open_tgid;
	struct list_head mvaList;
};
#endif

struct module_to_m4u_port_t {
	enum DISP_MODULE_ENUM module;
	int larb;
	int port;
};

int module_to_m4u_port(enum DISP_MODULE_ENUM module);
enum DISP_MODULE_ENUM m4u_port_to_module(int port);
int disp_m4u_callback(int port, unsigned long mva, void *data);
void disp_m4u_init(void);
int config_display_m4u_port(void);

int disp_mva_map_kernel(enum DISP_MODULE_ENUM module, unsigned int mva,
			unsigned int size, unsigned long *map_va,
			unsigned int *map_size);
int disp_mva_unmap_kernel(unsigned int mva, unsigned int size,
			  unsigned long map_va);

struct ion_client *disp_ion_create(const char *name);
struct ion_handle *disp_ion_alloc(struct ion_client *client,
				  unsigned int heap_id_mask, size_t align,
				  unsigned int size);
int disp_ion_get_mva(struct ion_client *client, struct ion_handle *handle,
		     unsigned int *mva, int port);
struct ion_handle *disp_ion_import_handle(struct ion_client *client, int fd);
void disp_ion_free_handle(struct ion_client *client, struct ion_handle *handle);
void disp_ion_cache_flush(struct ion_client *client, struct ion_handle *handle,
			  enum ION_CACHE_SYNC_TYPE sync_type);
void disp_ion_destroy(struct ion_client *client);

#ifndef CONFIG_MTK_IOMMU
int disp_allocate_mva(struct m4u_client_t *client, enum DISP_MODULE_ENUM module,
		      unsigned long va, struct sg_table *sg_table,
		      unsigned int size, unsigned int prot, unsigned int flags,
		      unsigned int *pMva);
int disp_hal_allocate_framebuffer(phys_addr_t pa_start, phys_addr_t pa_end,
				  unsigned long *va, unsigned long *mva);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __DSI_M4U_H__ */
