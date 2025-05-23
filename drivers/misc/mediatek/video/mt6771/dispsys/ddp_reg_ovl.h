/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2019 MediaTek Inc.
 */

#ifndef _DDP_REG_OVL_H_
#define _DDP_REG_OVL_H_

/* field definition */
/* ------------------------------------------------------------- */
/* OVL */
#define DISP_REG_OVL_STA				(0x000UL)
	#define STA_FLD_RUN				REG_FLD_MSB_LSB(0, 0)
	#define STA_FLD_RDMA0_IDLE			REG_FLD_MSB_LSB(1, 1)
	#define STA_FLD_RDMA1_IDLE			REG_FLD_MSB_LSB(2, 2)
	#define STA_FLD_RDMA2_IDLE			REG_FLD_MSB_LSB(3, 3)
	#define STA_FLD_RDMA3_IDLE			REG_FLD_MSB_LSB(4, 4)

#define DISP_REG_OVL_INTEN				(0x004UL)
	#define INTEN_FLD_REG_CMT_INTEN			REG_FLD_MSB_LSB(0, 0)
	#define INTEN_FLD_FME_CPL_INTEN			REG_FLD_MSB_LSB(1, 1)
	#define INTEN_FLD_FME_UND_INTEN			REG_FLD_MSB_LSB(2, 2)
	#define INTEN_FLD_FME_SWRST_DONE_INTEN		REG_FLD_MSB_LSB(3, 3)
	#define INTEN_FLD_FME_HWRST_DONE_INTEN		REG_FLD_MSB_LSB(4, 4)
	#define INTEN_FLD_RDMA0_EOF_ABNORMAL_INTEN	REG_FLD_MSB_LSB(5, 5)
	#define INTEN_FLD_RDMA1_EOF_ABNORMAL_INTEN	REG_FLD_MSB_LSB(6, 6)
	#define INTEN_FLD_RDMA2_EOF_ABNORMAL_INTEN	REG_FLD_MSB_LSB(7, 7)
	#define INTEN_FLD_RDMA3_EOF_ABNORMAL_INTEN	REG_FLD_MSB_LSB(8, 8)
	#define INTEN_FLD_RDMA0_SMI_UNDERFLOW_INTEN	REG_FLD_MSB_LSB(9, 9)
	#define INTEN_FLD_RDMA1_SMI_UNDERFLOW_INTEN	REG_FLD_MSB_LSB(10, 10)
	#define INTEN_FLD_RDMA2_SMI_UNDERFLOW_INTEN	REG_FLD_MSB_LSB(11, 11)
	#define INTEN_FLD_RDMA3_SMI_UNDERFLOW_INTEN	REG_FLD_MSB_LSB(12, 12)
	#define INTEN_FLD_ABNORMAL_SOF			REG_FLD_MSB_LSB(13, 13)
	#define INTEN_FLD_START_INTEN			REG_FLD_MSB_LSB(14, 14)

#define DISP_REG_OVL_INTSTA				(0x008UL)
	#define INTSTA_FLD_REG_CMT_INTSTA		REG_FLD_MSB_LSB(0, 0)
	#define INTSTA_FLD_FME_CPL_INTSTA		REG_FLD_MSB_LSB(1, 1)
	#define INTSTA_FLD_FME_UND_INTSTA		REG_FLD_MSB_LSB(2, 2)
	#define INTSTA_FLD_FME_SWRST_DONE_INTSTA	REG_FLD_MSB_LSB(3, 3)
	#define INTSTA_FLD_FME_HWRST_DONE_INTSTA	REG_FLD_MSB_LSB(4, 4)
	#define INTSTA_FLD_RDMA0_EOF_ABNORMAL_INTSTA	REG_FLD_MSB_LSB(5, 5)
	#define INTSTA_FLD_RDMA1_EOF_ABNORMAL_INTSTA	REG_FLD_MSB_LSB(6, 6)
	#define INTSTA_FLD_RDMA2_EOF_ABNORMAL_INTSTA	REG_FLD_MSB_LSB(7, 7)
	#define INTSTA_FLD_RDMA3_EOF_ABNORMAL_INTSTA	REG_FLD_MSB_LSB(8, 8)
	#define INTSTA_FLD_RDMA0_SMI_UNDERFLOW_INTSTA	REG_FLD_MSB_LSB(9, 9)
	#define INTSTA_FLD_RDMA1_SMI_UNDERFLOW_INTSTA	REG_FLD_MSB_LSB(10, 10)
	#define INTSTA_FLD_RDMA2_SMI_UNDERFLOW_INTSTA	REG_FLD_MSB_LSB(11, 11)
	#define INTSTA_FLD_RDMA3_SMI_UNDERFLOW_INTSTA	REG_FLD_MSB_LSB(12, 12)
	#define INTSTA_FLD_ABNORMAL_SOF			REG_FLD_MSB_LSB(13, 13)
	#define INTSTA_FLD_START_INTEN			REG_FLD_MSB_LSB(14, 14)

#define DISP_REG_OVL_EN					(0x00CUL)
	#define EN_FLD_OVL_EN				REG_FLD_MSB_LSB(0, 0)
	#define EN_FLD_OVL_CKON				REG_FLD_MSB_LSB(8, 8)
	#define EN_FLD_SMI_CKON				REG_FLD_MSB_LSB(9, 9)
	#define EN_FLD_IGNORE_ABN_SOF			REG_FLD_MSB_LSB(16, 16)
	#define EN_FLD_BLOCK_EXT_ULTRA			REG_FLD_MSB_LSB(18, 18)
	#define EN_FLD_BLOCK_EXT_PREULTRA		REG_FLD_MSB_LSB(19, 19)
	#define EN_FLD_RD_WRK_REG			REG_FLD_MSB_LSB(20, 20)
	#define EN_FLD_FORCE_COMMIT			REG_FLD_MSB_LSB(21, 21)
	#define EN_FLD_BYPASS_SHADOW			REG_FLD_MSB_LSB(22, 22)

#define DISP_REG_OVL_TRIG				(0x010UL)
	#define TRIG_FLD_SW_TRIG			REG_FLD_MSB_LSB(0, 0)

#define DISP_REG_OVL_RST				(0x014UL)
#define DISP_REG_OVL_ROI_SIZE				(0x020UL)
	#define ROI_SIZE_FLD_ROI_W			REG_FLD_MSB_LSB(12, 0)
	#define ROI_SIZE_FLD_ROI_H			REG_FLD_MSB_LSB(28, 16)

#define DISP_REG_OVL_DATAPATH_CON			(0x024UL)
	#define DATAPATH_CON_FLD_LAYER_SMI_ID_EN	REG_FLD_MSB_LSB(0, 0)
	#define DATAPATH_CON_FLD_RANDOM_BGCLR_EN	REG_FLD_MSB_LSB(1, 1)
	#define DATAPATH_CON_FLD_BGCLR_IN_SEL		REG_FLD_MSB_LSB(2, 2)
	#define DATAPATH_CON_FLD_OUTPUT_NO_RND		REG_FLD_MSB_LSB(3, 3)
	#define DATAPATH_CON_FLD_L0_GPU_MODE		REG_FLD_MSB_LSB(8, 8)
	#define DATAPATH_CON_FLD_L1_GPU_MODE		REG_FLD_MSB_LSB(9, 9)
	#define DATAPATH_CON_FLD_L2_GPU_MODE		REG_FLD_MSB_LSB(10, 10)
	#define DATAPATH_CON_FLD_L3_GPU_MODE		REG_FLD_MSB_LSB(11, 11)
	#define DATAPATH_CON_FLD_ADOBE_MODE		REG_FLD_MSB_LSB(12, 12)
	#define DATAPATH_CON_FLD_ADOBE_LAYER		REG_FLD_MSB_LSB(14, 13)
	#define DATAPATH_CON_FLD_OVL_GAMMA_OUT		REG_FLD_MSB_LSB(15, 15)
	#define DATAPATH_CON_FLD_PQ_OUT_SEL		REG_FLD_MSB_LSB(17, 16)
	#define DATAPATH_CON_FLD_RDMA0_OUT_SEL		REG_FLD_MSB_LSB(20, 20)
	#define DATAPATH_CON_FLD_RDMA1_OUT_SEL		REG_FLD_MSB_LSB(21, 21)
	#define DATAPATH_CON_FLD_RDMA2_OUT_SEL		REG_FLD_MSB_LSB(22, 22)
	#define DATAPATH_CON_FLD_RDMA3_OUT_SEL		REG_FLD_MSB_LSB(23, 23)
	#define DATAPATH_CON_FLD_GCLAST_EN		REG_FLD_MSB_LSB(24, 24)
	#define DATAPATH_CON_FLD_OUTPUT_CLAMP		REG_FLD_MSB_LSB(26, 26)
	#define DATAPATH_CON_FLD_OUTPUT_INTERLACE	REG_FLD_MSB_LSB(27, 27)

#define DISP_REG_OVL_ROI_BGCLR				(0x028UL)
	#define ROI_BGCLR_FLD_BLUE			REG_FLD_MSB_LSB(7, 0)
	#define ROI_BGCLR_FLD_GREEN			REG_FLD_MSB_LSB(18, 8)
	#define ROI_BGCLR_FLD_RED			REG_FLD_MSB_LSB(23, 16)
	#define ROI_BGCLR_FLD_ALPHA			REG_FLD_MSB_LSB(31, 24)

#define DISP_REG_OVL_SRC_CON				(0x02CUL)
	#define SRC_CON_FLD_L0_EN			REG_FLD_MSB_LSB(0, 0)
	#define SRC_CON_FLD_L1_EN			REG_FLD_MSB_LSB(1, 1)
	#define SRC_CON_FLD_L2_EN			REG_FLD_MSB_LSB(2, 2)
	#define SRC_CON_FLD_L3_EN			REG_FLD_MSB_LSB(3, 3)
	#define SRC_CON_FLD_LC_EN			REG_FLD_MSB_LSB(4, 4)
	#define SRC_CON_FLD_L_EN			REG_FLD_MSB_LSB(3, 0)

#define DISP_REG_OVL_L0_CON				(0x030UL)
	#define L_CON_FLD_APHA				REG_FLD_MSB_LSB(7, 0)
	#define L_CON_FLD_AEN				REG_FLD_MSB_LSB(8, 8)
	#define L_CON_FLD_VIRTICAL_FLIP			REG_FLD_MSB_LSB(9, 9)
	#define L_CON_FLD_HORI_FLIP			REG_FLD_MSB_LSB(10, 10)
	#define L_CON_FLD_EXT_MTX_EN			REG_FLD_MSB_LSB(11, 11)
	#define L_CON_FLD_CFMT				REG_FLD_MSB_LSB(15, 12)
	#define L_CON_FLD_MTX				REG_FLD_MSB_LSB(19, 16)
	#define L_CON_FLD_EN_3D				REG_FLD_MSB_LSB(20, 20)
	#define L_CON_FLD_EN_LANDSCAPE			REG_FLD_MSB_LSB(21, 21)
	#define L_CON_FLD_EN_R_FIRST			REG_FLD_MSB_LSB(22, 22)
	#define L_CON_FLD_CLRFMT_MAN			REG_FLD_MSB_LSB(23, 23)
	#define L_CON_FLD_BTSW				REG_FLD_MSB_LSB(24, 24)
	#define L_CON_FLD_RGB_SWAP			REG_FLD_MSB_LSB(25, 25)
	#define L_CON_FLD_LSRC				REG_FLD_MSB_LSB(29, 28)
	#define L_CON_FLD_SKEN				REG_FLD_MSB_LSB(30, 30)
	#define L_CON_FLD_DKEN				REG_FLD_MSB_LSB(31, 31)

#define DISP_REG_OVL_L0_SRCKEY				(0x034UL)
#define DISP_REG_OVL_L0_SRC_SIZE			(0x038UL)
	#define SRC_SIZE_FLD_SRC_W			REG_FLD_MSB_LSB(12, 0)
	#define SRC_SIZE_FLD_SRC_H			REG_FLD_MSB_LSB(28, 16)

#define DISP_REG_OVL_L0_OFFSET				(0x03CUL)
#define DISP_REG_OVL_L0_ADDR				(0xf40UL)
#define DISP_REG_OVL_L0_PITCH				(0x044UL)
	#define L_PITCH_FLD_SRC_PITCH			REG_FLD_MSB_LSB(15, 0)
	#define L_PITCH_FLD_SA_SEL			REG_FLD_MSB_LSB(17, 16)
	#define L_PITCH_FLD_SRGB_SEL			REG_FLD_MSB_LSB(19, 18)
	#define L_PITCH_FLD_DA_SEL			REG_FLD_MSB_LSB(21, 20)
	#define L_PITCH_FLD_DRGB_SEL			REG_FLD_MSB_LSB(23, 22)
	#define L_PITCH_FLD_CONST_BLD			REG_FLD_MSB_LSB(28, 28)
	#define L_PITCH_FLD_BLEND_RND_SHT		REG_FLD_MSB_LSB(30, 30)
	#define L_PITCH_FLD_SURFL_EN			REG_FLD_MSB_LSB(31, 31)

#define DISP_REG_OVL_L0_TILE				(0x048UL)
#define DISP_REG_OVL_L0_CLIP				(0x04CUL)
	#define OVL_L_CLIP_FLD_LEFT			REG_FLD_MSB_LSB(7, 0)
	#define OVL_L_CLIP_FLD_RIGHT			REG_FLD_MSB_LSB(15, 8)
	#define OVL_L_CLIP_FLD_TOP			REG_FLD_MSB_LSB(23, 16)
	#define OVL_L_CLIP_FLD_BOTTOM			REG_FLD_MSB_LSB(31, 24)

#define DISP_REG_OVL_L1_CON				(0x050UL)
#define DISP_REG_OVL_L1_SRCKEY				(0x054UL)
#define DISP_REG_OVL_L1_SRC_SIZE			(0x058UL)
#define DISP_REG_OVL_L1_OFFSET				(0x05CUL)
#define DISP_REG_OVL_L1_ADDR				(0xf60UL)
#define DISP_REG_OVL_L1_PITCH				(0x064UL)
#define DISP_REG_OVL_L1_TILE				(0x068UL)
#define DISP_REG_OVL_L1_CLIP				(0x06CUL)
#define DISP_REG_OVL_L2_CON				(0x070UL)
#define DISP_REG_OVL_L2_SRCKEY				(0x074UL)
#define DISP_REG_OVL_L2_SRC_SIZE			(0x078UL)
#define DISP_REG_OVL_L2_OFFSET				(0x07CUL)
#define DISP_REG_OVL_L2_ADDR				(0xf80UL)
#define DISP_REG_OVL_L2_PITCH				(0x084UL)
#define DISP_REG_OVL_L2_TILE				(0x088UL)
#define DISP_REG_OVL_L2_CLIP				(0x08CUL)
#define DISP_REG_OVL_L3_CON				(0x090UL)
#define DISP_REG_OVL_L3_SRCKEY				(0x094UL)
#define DISP_REG_OVL_L3_SRC_SIZE			(0x098UL)
#define DISP_REG_OVL_L3_OFFSET				(0x09CUL)
#define DISP_REG_OVL_L3_ADDR				(0xfA0UL)
#define DISP_REG_OVL_L3_PITCH				(0x0A4UL)
#define DISP_REG_OVL_L3_TILE				(0x0A8UL)
#define DISP_REG_OVL_L3_CLIP				(0x0ACUL)
#define DISP_REG_OVL_RDMA0_CTRL				(0x0C0UL)
	#define RDMA0_CTRL_FLD_RDMA_EN			REG_FLD_MSB_LSB(0, 0)
	#define RDMA0_CTRL_FLD_RDMA_INTERLACE		REG_FLD_MSB_LSB(0, 0)
	#define RDMA0_CTRL_FLD_RMDA_FIFO_USED_SZ	REG_FLD_MSB_LSB(27, 16)

#define DISP_REG_OVL_RDMA0_MEM_GMC_SETTING		(0x0C8UL)
	#define FLD_OVL_RDMA_MEM_GMC_ULTRA_THRESHOLD	REG_FLD_MSB_LSB(9, 0)
	#define FLD_OVL_RDMA_MEM_GMC_PRE_ULTRA_THRESHOLD REG_FLD_MSB_LSB(25, 16)
	#define FLD_OVL_RDMA_MEM_GMC_ULTRA_THRESHOLD_HIGH_OFS \
							REG_FLD_MSB_LSB(28, 28)
	#define FLD_OVL_RDMA_MEM_GMC_PRE_ULTRA_THRESHOLD_HIGH_OFS \
							REG_FLD_MSB_LSB(31, 31)

#define DISP_REG_OVL_RDMA0_MEM_SLOW_CON			(0x0CCUL)
#define DISP_REG_OVL_RDMA0_FIFO_CTRL			(0x0D0UL)
	#define FLD_OVL_RDMA_FIFO_THRD			REG_FLD_MSB_LSB(9, 0)
	#define FLD_OVL_RDMA_FIFO_SIZE			REG_FLD_MSB_LSB(27, 16)
	#define FLD_OVL_RDMA_FIFO_UND_EN		REG_FLD_MSB_LSB(31, 31)
#define DISP_REG_OVL_RDMA1_CTRL				(0x0E0UL)
#define DISP_REG_OVL_RDMA1_MEM_GMC_SETTING		(0x0E8UL)
#define DISP_REG_OVL_RDMA1_MEM_SLOW_CON			(0x0ECUL)
#define DISP_REG_OVL_RDMA1_FIFO_CTRL			(0x0F0UL)
#define DISP_REG_OVL_RDMA2_CTRL				(0x100UL)
#define DISP_REG_OVL_RDMA2_MEM_GMC_SETTING		(0x108UL)
#define DISP_REG_OVL_RDMA2_MEM_SLOW_CON			(0x10CUL)
#define DISP_REG_OVL_RDMA2_FIFO_CTRL			(0x110UL)
#define DISP_REG_OVL_RDMA3_CTRL				(0x120UL)
#define DISP_REG_OVL_RDMA3_MEM_GMC_SETTING		(0x128UL)
#define DISP_REG_OVL_RDMA3_MEM_SLOW_CON			(0x12CUL)
#define DISP_REG_OVL_RDMA3_FIFO_CTRL			(0x130UL)

#define DISP_REG_OVL_L0_Y2R_PARA_R0			(0x134UL)
#define DISP_REG_OVL_L0_Y2R_PARA_R1			(0x138UL)
#define DISP_REG_OVL_L0_Y2R_PARA_G0			(0x13CUL)
#define DISP_REG_OVL_L0_Y2R_PARA_G1			(0x140UL)
#define DISP_REG_OVL_L0_Y2R_PARA_B0			(0x144UL)
#define DISP_REG_OVL_L0_Y2R_PARA_B1			(0x148UL)
#define DISP_REG_OVL_L0_Y2R_PARA_YUV_A_0		(0x14CUL)
#define DISP_REG_OVL_L0_Y2R_PARA_YUV_A_1		(0x150UL)
#define DISP_REG_OVL_L0_Y2R_PARA_RGB_A_0		(0x154UL)
#define DISP_REG_OVL_L0_Y2R_PARA_RGB_A_1		(0x158UL)
#define DISP_REG_OVL_L1_Y2R_PARA_R0			(0x15CUL)
#define DISP_REG_OVL_L1_Y2R_PARA_R1			(0x160UL)
#define DISP_REG_OVL_L1_Y2R_PARA_G0			(0x164UL)
#define DISP_REG_OVL_L1_Y2R_PARA_G1			(0x168UL)
#define DISP_REG_OVL_L1_Y2R_PARA_B0			(0x16CUL)
#define DISP_REG_OVL_L1_Y2R_PARA_B1			(0x170UL)
#define DISP_REG_OVL_L1_Y2R_PARA_YUV_A_0		(0x174UL)
#define DISP_REG_OVL_L1_Y2R_PARA_YUV_A_1		(0x178UL)
#define DISP_REG_OVL_L1_Y2R_PARA_RGB_A_0		(0x17CUL)
#define DISP_REG_OVL_L1_Y2R_PARA_RGB_A_1		(0x180UL)
#define DISP_REG_OVL_L2_Y2R_PARA_R0			(0x184UL)
#define DISP_REG_OVL_L2_Y2R_PARA_R1			(0x188UL)
#define DISP_REG_OVL_L2_Y2R_PARA_G0			(0x18CUL)
#define DISP_REG_OVL_L2_Y2R_PARA_G1			(0x190UL)
#define DISP_REG_OVL_L2_Y2R_PARA_B0			(0x194UL)
#define DISP_REG_OVL_L2_Y2R_PARA_B1			(0x198UL)
#define DISP_REG_OVL_L2_Y2R_PARA_YUV_A_0		(0x19CUL)
#define DISP_REG_OVL_L2_Y2R_PARA_YUV_A_1		(0x1A0UL)
#define DISP_REG_OVL_L2_Y2R_PARA_RGB_A_0		(0x1A4UL)
#define DISP_REG_OVL_L2_Y2R_PARA_RGB_A_1		(0x1A8UL)
#define DISP_REG_OVL_L3_Y2R_PARA_R0			(0x1ACUL)
#define DISP_REG_OVL_L3_Y2R_PARA_R1			(0x1B0UL)
#define DISP_REG_OVL_L3_Y2R_PARA_G0			(0x1B4UL)
#define DISP_REG_OVL_L3_Y2R_PARA_G1			(0x1B8UL)
#define DISP_REG_OVL_L3_Y2R_PARA_B0			(0x1BCUL)
#define DISP_REG_OVL_L3_Y2R_PARA_B1			(0x1C0UL)
#define DISP_REG_OVL_L3_Y2R_PARA_YUV_A_0		(0x1C4UL)
#define DISP_REG_OVL_L3_Y2R_PARA_YUV_A_1		(0x1C8UL)
#define DISP_REG_OVL_L3_Y2R_PARA_RGB_A_0		(0x1CCUL)
#define DISP_REG_OVL_L3_Y2R_PARA_RGB_A_1		(0x1D0UL)
#define DISP_REG_OVL_DEBUG_MON_SEL			(0x1D4UL)
#define DISP_REG_OVL_RDMA0_MEM_GMC_S2			(0x1E0UL)
	#define FLD_OVL_RDMA_MEM_GMC2_ISSUE_REQ_THRES	REG_FLD_MSB_LSB(11, 0)
	#define FLD_OVL_RDMA_MEM_GMC2_ISSUE_REQ_THRES_URG \
							REG_FLD_MSB_LSB(27, 16)
	#define FLD_OVL_RDMA_MEM_GMC2_REQ_THRES_PREULTRA REG_FLD_MSB_LSB(28, 28)
	#define FLD_OVL_RDMA_MEM_GMC2_REQ_THRES_ULTRA	REG_FLD_MSB_LSB(29, 29)
	#define FLD_OVL_RDMA_MEM_GMC2_FORCE_REQ_THRES	REG_FLD_MSB_LSB(30, 30)

#define DISP_REG_OVL_RDMA1_MEM_GMC_S2			(0x1E4UL)
#define DISP_REG_OVL_RDMA2_MEM_GMC_S2			(0x1E8UL)
#define DISP_REG_OVL_RDMA3_MEM_GMC_S2			(0x1ECUL)
#define DISP_REG_OVL_RDMA_BURST_CON0			(0x1F0UL)
#define DISP_REG_OVL_RDMA_BURST_CON1			(0x1F4UL)
#define DISP_REG_OVL_RDMA_GREQ_NUM			(0x1F8UL)
	#define FLD_OVL_RDMA_GREQ_LAYER0_GREQ_NUM	REG_FLD_MSB_LSB(3, 0)
	#define FLD_OVL_RDMA_GREQ_LAYER1_GREQ_NUM	REG_FLD_MSB_LSB(7, 4)
	#define FLD_OVL_RDMA_GREQ_LAYER2_GREQ_NUM	REG_FLD_MSB_LSB(11, 8)
	#define FLD_OVL_RDMA_GREQ_LAYER3_GREQ_NUM	REG_FLD_MSB_LSB(15, 12)
	#define FLD_OVL_RDMA_GREQ_OSTD_GREQ_NUM		REG_FLD_MSB_LSB(23, 16)
	#define FLD_OVL_RDMA_GREQ_GREQ_DIS_CNT		REG_FLD_MSB_LSB(26, 24)
	#define FLD_OVL_RDMA_GREQ_STOP_EN		REG_FLD_MSB_LSB(27, 27)
	#define FLD_OVL_RDMA_GREQ_GRP_END_STOP		REG_FLD_MSB_LSB(28, 28)
	#define FLD_OVL_RDMA_GREQ_GRP_BRK_STOP		REG_FLD_MSB_LSB(29, 29)
	#define FLD_OVL_RDMA_GREQ_IOBUF_FLUSH_PREULTRA	REG_FLD_MSB_LSB(30, 30)
	#define FLD_OVL_RDMA_GREQ_IOBUF_FLUSH_ULTRA	REG_FLD_MSB_LSB(31, 31)

#define DISP_REG_OVL_RDMA_GREQ_URG_NUM			(0x1FCUL)
	#define FLD_OVL_RDMA_GREQ_LAYER0_GREQ_URG_NUM	REG_FLD_MSB_LSB(3, 0)
	#define FLD_OVL_RDMA_GREQ_LAYER1_GREQ_URG_NUM	REG_FLD_MSB_LSB(7, 4)
	#define FLD_OVL_RDMA_GREQ_LAYER2_GREQ_URG_NUM	REG_FLD_MSB_LSB(11, 8)
	#define FLD_OVL_RDMA_GREQ_LAYER3_GREQ_URG_NUM	REG_FLD_MSB_LSB(15, 12)
	#define FLD_OVL_RDMA_GREQ_ARG_GREQ_URG_TH	REG_FLD_MSB_LSB(25, 16)
	#define FLD_OVL_RDMA_GREQ_ARG_URG_BIAS		REG_FLD_MSB_LSB(28, 28)
	#define FLD_OVL_RDMA_GREQ_NUM_SHT_VAL		REG_FLD_MSB_LSB(29, 29)
	#define FLD_OVL_RDMA_GREQ_NUM_SHT		REG_FLD_MSB_LSB(31, 30)

#define DISP_REG_OVL_DUMMY_REG				(0x200UL)
#define DISP_REG_OVL_GDRDY_PRD				(0x208UL)
#define DISP_REG_OVL_RDMA_ULTRA_SRC			(0x20CUL)
	#define FLD_OVL_RDMA_PREULTRA_BUF_SRC		REG_FLD_MSB_LSB(1, 0)
	#define FLD_OVL_RDMA_PREULTRA_SMI_SRC		REG_FLD_MSB_LSB(3, 2)
	#define FLD_OVL_RDMA_PREULTRA_ROI_END_SRC	REG_FLD_MSB_LSB(5, 4)
	#define FLD_OVL_RDMA_PREULTRA_RDMA_SRC		REG_FLD_MSB_LSB(7, 6)
	#define FLD_OVL_RDMA_ULTRA_BUF_SRC		REG_FLD_MSB_LSB(9, 8)
	#define FLD_OVL_RDMA_ULTRA_SMI_SRC		REG_FLD_MSB_LSB(11, 10)
	#define FLD_OVL_RDMA_ULTRA_ROI_END_SRC		REG_FLD_MSB_LSB(13, 12)
	#define FLD_OVL_RDMA_ULTRA_RDMA_SRC		REG_FLD_MSB_LSB(15, 14)

#define DISP_REG_OVL_RDMAn_BUF_LOW(layer)		(0x210UL + ((layer)<<2))
	#define FLD_OVL_RDMA_BUF_LOW_ULTRA_TH		REG_FLD_MSB_LSB(11, 0)
	#define FLD_OVL_RDMA_BUF_LOW_PREULTRA_TH	REG_FLD_MSB_LSB(23, 12)

#define DISP_REG_OVL_RDMAn_BUF_HIGH(layer)		(0x220UL + ((layer)<<2))
	#define FLD_OVL_RDMA_BUF_HIGH_PREULTRA_TH	REG_FLD_MSB_LSB(23, 12)
	#define FLD_OVL_RDMA_BUF_HIGH_PREULTRA_DIS	REG_FLD_MSB_LSB(31, 31)
#define DISP_REG_OVL_RDMA0_BUF_LOW			(0x210UL)
#define DISP_REG_OVL_RDMA1_BUF_LOW			(0x214UL)
#define DISP_REG_OVL_RDMA2_BUF_LOW			(0x218UL)
#define DISP_REG_OVL_RDMA3_BUF_LOW			(0x21cUL)

#define DISP_REG_OVL_SMI_DBG				(0x230UL)
#define DISP_REG_OVL_GREQ_LAYER_CNT			(0x234UL)
#define DISP_REG_OVL_GDRDY_PRD_NUM			(0x238UL)
#define DISP_REG_OVL_FLOW_CTRL_DBG			(0x240UL)
#define DISP_REG_OVL_ADDCON_DBG				(0x244UL)
	#define ADDCON_DBG_FLD_ROI_X			REG_FLD_MSB_LSB(12, 0)
	#define ADDCON_DBG_FLD_L0_WIN_HIT		REG_FLD_MSB_LSB(14, 14)
	#define ADDCON_DBG_FLD_L1_WIN_HIT		REG_FLD_MSB_LSB(15, 15)
	#define ADDCON_DBG_FLD_ROI_Y			REG_FLD_MSB_LSB(28, 16)
	#define ADDCON_DBG_FLD_L3_WIN_HIT		REG_FLD_MSB_LSB(31, 31)
	#define ADDCON_DBG_FLD_L2_WIN_HIT		REG_FLD_MSB_LSB(30, 30)
#define DISP_REG_OVL_RDMA0_DBG				(0x24CUL)
	#define RDMA0_DBG_FLD_RDMA0_WRAM_RST_CS		REG_FLD_MSB_LSB(2, 0)
	#define RDMA0_DBG_FLD_RDMA0_LAYER_GREQ          REG_FLD_MSB_LSB(3, 3)
	#define RDMA0_DBG_FLD_RDMA0_OUT_DATA		REG_FLD_MSB_LSB(27 4)
	#define RDMA0_DBG_FLD_RDMA0_OUT_READY		REG_FLD_MSB_LSB(28, 28)
	#define RDMA0_DBG_FLD_RDMA0_OUT_VALID		REG_FLD_MSB_LSB(29, 29)
	#define RDMA0_DBG_FLD_RDMA0_SMI_BUSY		REG_FLD_MSB_LSB(30, 30)
	#define RDMA0_DBG_FLD_RDMA0_SMI_GREQ		REG_FLD_MSB_LSB(31, 31)

#define DISP_REG_OVL_RDMA1_DBG				(0x250UL)
#define DISP_REG_OVL_RDMA2_DBG				(0x254UL)
#define DISP_REG_OVL_RDMA3_DBG				(0x258UL)
#define DISP_REG_OVL_L0_CLR				(0x25cUL)
#define DISP_REG_OVL_L1_CLR				(0x260UL)
#define DISP_REG_OVL_L2_CLR				(0x264UL)
#define DISP_REG_OVL_L3_CLR				(0x268UL)
#define DISP_REG_OVL_LC_CLR				(0x26cUL)
#define DISP_REG_OVL_CRC				(0x270UL)
#define DISP_REG_OVL_LC_CON				(0x280UL)
#define DISP_REG_OVL_LC_SRCKEY				(0x284UL)
#define DISP_REG_OVL_LC_SRC_SIZE			(0x288UL)
	#define FLD_OVL_LC_SRC_W			REG_FLD_MSB_LSB(12, 0)
	#define FLD_OVL_LC_SRC_H			REG_FLD_MSB_LSB(28, 16)
#define DISP_REG_OVL_LC_OFFSET				(0x28cUL)
	#define FLD_OVL_LC_XOFF				REG_FLD_MSB_LSB(11, 0)
	#define FLD_OVL_LC_YOFF				REG_FLD_MSB_LSB(27, 16)
#define DISP_REG_OVL_LC_SRC_SEL				(0x290UL)
	#define LC_SRC_SEL_FLD_L_SEL			REG_FLD_MSB_LSB(2, 0)
#define DISP_REG_OVL_BANK_CON				(0x29cUL)
#define DISP_REG_OVL_FUNC_DCM0				(0x2a0UL)
#define DISP_REG_OVL_FUNC_DCM1				(0x2a4UL)
#define DISP_REG_OVL_DVFS_L0_ROI			(0x2B0UL)
#define DISP_REG_OVL_DVFS_L1_ROI			(0x2B4UL)
#define DISP_REG_OVL_DVFS_L2_ROI			(0x2B8UL)
#define DISP_REG_OVL_DVFS_L3_ROI			(0x2BCUL)
#define DISP_REG_OVL_DVFS_EL0_ROI			(0x2C0UL)
#define DISP_REG_OVL_DVFS_EL1_ROI			(0x2C4UL)
#define DISP_REG_OVL_DVFS_EL2_ROI			(0x2C8UL)
#define DISP_REG_OVL_DATAPATH_EXT_CON			(0x324UL)
#define DISP_REG_OVL_EL0_CON				(0x330UL)
#define DISP_REG_OVL_EL0_SRCKEY				(0x334UL)
#define DISP_REG_OVL_EL0_SRC_SIZE			(0x338UL)
#define DISP_REG_OVL_EL0_OFFSET				(0x33CUL)
#define DISP_REG_OVL_EL0_ADDR				(0xFB0UL)
#define DISP_REG_OVL_EL0_PITCH				(0x344UL)
#define DISP_REG_OVL_EL0_TILE				(0x348UL)
#define DISP_REG_OVL_EL0_CLIP				(0x34CUL)
#define DISP_REG_OVL_EL1_CON				(0x350UL)
#define DISP_REG_OVL_EL1_SRCKEY				(0x354UL)
#define DISP_REG_OVL_EL1_SRC_SIZE			(0x358UL)
#define DISP_REG_OVL_EL1_OFFSET				(0x35CUL)
#define DISP_REG_OVL_EL1_ADDR				(0xFB4UL)
#define DISP_REG_OVL_EL1_PITCH				(0x364UL)
#define DISP_REG_OVL_EL1_TILE				(0x368UL)
#define DISP_REG_OVL_EL1_CLIP				(0x36CUL)
#define DISP_REG_OVL_EL2_CON				(0x370UL)
#define DISP_REG_OVL_EL2_SRCKEY				(0x374UL)
#define DISP_REG_OVL_EL2_SRC_SIZE			(0x378UL)
#define DISP_REG_OVL_EL2_OFFSET				(0x37CUL)
#define DISP_REG_OVL_EL2_ADDR				(0xFB8UL)
#define DISP_REG_OVL_EL2_PITCH				(0x384UL)
#define DISP_REG_OVL_EL2_TILE				(0x388UL)
#define DISP_REG_OVL_EL2_CLIP				(0x38CUL)
#define DISP_REG_OVL_EL0_CLEAR				(0x390UL)
#define DISP_REG_OVL_EL1_CLEAR				(0x394UL)
#define DISP_REG_OVL_EL2_CLEAR				(0x398UL)
#define DISP_REG_OVL_SECURE				(0xFC0UL)

#endif
