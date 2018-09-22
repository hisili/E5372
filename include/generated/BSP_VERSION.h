/* Huawei Hisi, Copyright (c) 2010 hisi LTE Ltd. */ 

#ifndef __BSP_VERSION_H__
#define __BSP_VERSION_H__

#ifndef EXCLUDE_MATH_LIB 
/* VERSION DEFINE */ 

#define BSP_MMU_OPEN 
//#undef BSP_MMU_OPEN 

#define DMA_CHN0_INT_DISABLE 
//#undef DMA_CHN0_INT_DISABLE 

#ifndef BUILD_ONEBUILDER
/* version no */ 
#define PRODUCT_CFG_VERSION_STR "V700R001C10B040" 

/* asic type */ 
#ifndef PRODUCT_CFG_ASIC_TYPE_PV500 
#define PRODUCT_CFG_ASIC_TYPE_PV500 
#endif 

/* board type */ 
#ifndef PRODUCT_CFG_BOARD_TYPE_P500_FPGA 
#define PRODUCT_CFG_BOARD_TYPE_P500_FPGA  
#endif 


/* image type  */ 
#ifndef PRODUCT_CFG_IMAGE_TYPE_VXWORKS 
#define PRODUCT_CFG_IMAGE_TYPE_VXWORKS  
#endif 

/* product type  */ 
#ifndef PRODUCT_CFG_PRODUCT_TYPE_USBSTICK 
#define PRODUCT_CFG_PRODUCT_TYPE_USBSTICK  
#endif 

/* lib type  */ 
#ifndef PRODUCT_CFG_VERSION_DEBUG
#define PRODUCT_CFG_VERSION_DEBUG
#endif 

/* mode type  */ 
#ifndef PRODUCT_CFG_MULTIMODE_GUL 
#define PRODUCT_CFG_MULTIMODE_GUL  
#endif 

#ifdef PRODUCT_CFG_PRODUCT_TYPE_USBSTICK 
#ifdef PRODUCT_CFG_CORE_TYPE_APP 
#ifndef PRODUCT_CFG_DEV_USE_JUSB 
#define PRODUCT_CFG_DEV_USE_JUSB 
#endif 
#ifndef PRODUCT_CFG_DEV_USE_SDIO 
#define PRODUCT_CFG_DEV_USE_SDIO 
#endif 
#undef PRODUCT_CFG_DEV_USE_GMAC 
#undef PRODUCT_CFG_ETH_PHY_GMII 
#else 
#undef PRODUCT_CFG_DEV_USE_JUSB 
#undef PRODUCT_CFG_DEV_USE_SDIO 
#ifdef PRODUCT_CFG_VERSION_DEBUG 
#ifndef PRODUCT_CFG_DEV_USE_GMAC 
#define PRODUCT_CFG_DEV_USE_GMAC 
#endif 
#ifndef PRODUCT_CFG_ETH_PHY_GMII 
#define PRODUCT_CFG_ETH_PHY_GMII 
#endif 
#endif /* BSP_DEBUG */ 

#ifdef PRODUCT_CFG_IMAGE_TYPE_BOOTROM 
#ifndef PRODUCT_CFG_DEV_USE_JUSB 
#define PRODUCT_CFG_DEV_USE_JUSB 
#endif 
#undef PRODUCT_CFG_DEV_USE_SDIO 
#undef PRODUCT_CFG_DEV_USE_GMAC 
#undef PRODUCT_CFG_ETH_PHY_GMII 
#endif /* end of PRODUCT_CFG_IMAGE_TYPE_BOOTROM */ 
#endif /* end of PRODUCT_CFG_CORE_TYPE_APP */ 
#endif /* end of BSP_PRODUCT_USBSTICK */ 
#endif

#endif /*EXCLUDE_MATH_LIB*/ 

#endif /*__BSP_VERSION_H__*/ 

