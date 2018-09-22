/******************************************************************
 * Copyright (C), 2005-2011, HUAWEI Tech. Co., Ltd.
 *
 * File name:
 *
 * Description:
 *     power ctrl  head file.
 *
 * Date:       2012.02.01
 * Version:    v1.0
 *
 *
 * History:
 * Date:       2012.02.01
 * Discribe:   Initial
 *******************************************************************/
#ifdef PRODUCT_CFG_CORE_TYPE_MODEM
#ifndef PWRCTRL_DRX_LTE_H
#define PWRCTRL_DRX_LTE_H

#ifdef  __cplusplus
extern "C"
{
#endif

/*****************************************************************************/
/*include headfile */
/*****************************************************************************/
#include "product_config.h"
#include "BSP_PWC_COMMON.h"

/*****************************************************************************/
/*macro define*/
/*****************************************************************************/
#define PWC_PARA_INVALID				0x1
#define PWC_SUCCESS						0
#define PWR_BBP_MTCMOS_MASK 			(0xD << 8)
#define PWR_BBP_ISO_MASK				(0xD << 8)
#define PWR_BBP_SRST_MASK				(0xD << 3)
#define PWR_BBP_CLOCK_MASK				(0xD << 2)
#define PWR_BBP_PLL_EN					(0x1 << 0)
#define PWR_BBP_PLL_LOCK				(0x1 << 31)


#define PWR_DSP_PLL_EN					(0x1 << 0)
#define PWR_DSP_MTCMOS_MASK				(0x1 << 18)
#define PWR_DSP_PLL_MASK				(0x1 << 31)
#define PWR_DSP_SRST_MASK_REGION		(0x1 << 1)
#define PWR_DSP_SRST_MASK				(0x1 << 0)
#define PWR_DSP_ISO_MASK				(0x1 << 18)
#define PWR_DSP_CLOCK_MASK				(0x1 << 0)
#define PWR_DSP_PLL_LOCK                 (0x1 << 31)

#define PWR_PERI_PLL_LOCK				(0x1 << 31)


/*****************************************************************************/
/*type define*/
/*****************************************************************************/

/*****************************************************************************/
/*variable prototype*/
/*****************************************************************************/

/*****************************************************************************/
/*function prototype*/
/*****************************************************************************/

#ifdef  __cplusplus
}
#endif

#endif
#endif
