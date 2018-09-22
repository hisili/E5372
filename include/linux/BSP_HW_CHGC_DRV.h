/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  BSP_CHGC_DRV.h
*
*
*   描    述 :  CHGC模块用户头文件
*
*
*************************************************************************/

#ifndef _BSP_HW_CHGC_DRV_H                                                                  
#define _BSP_HW_CHGC_DRV_H

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************
  头文件包含                            
**************************************************************************/
#include <mach/common/bsp_version.h>
#include "BSP.h"

/**************************************************************************
  宏定义 
**************************************************************************/
#define CHG_DETECT_NAME     "hw_charger_detect"
#define USBID_DETECT_NAME   "usbid_detect"

/*System ctrl register, offset */
#define SC_CRG_REG_BASE_MEM         0x90000000
#define SC_USB_REG_BASE_MEM         0x900c0000

#define USB_PHY_BC_CTL   (0xc000)
#define USB_PHY_CAC		 (0xc004)
#define USB_PHY_IDENTIFY (0xc00c)
#define USB_CRG_CTRL03   (0x000c)
#define USB_CRG_CTRL04	 (0x0010)

#define  USB_CRG_CTRL14  (0x0038)
#define  USB_SC_CTRL36   (0x0490)
#define  USB_SC_CTRL37   (0x0494)


#define BALONG_BOARD_TYPE_DETECT    BALONG_GPIO_0(11)

/**************************************************************************
  错误码定义
**************************************************************************/

typedef struct device_event_st
{
    BSP_S32 device_id;   /*设备ID*/
    BSP_S32 value;         /*消息value*/ 
    BSP_S8 * desc;     /*描述*/
}device_event_t;

#define SYN_HS_EP0_MAX_PACKET 64

#define EP_INDEX_TO_NUMBER(i)  ((i) >> 1)
#define EP_NUMBER_TO_IN_INDEX(n) ((n) << 1)
#define EP_NUMBER_TO_OUT_INDEX(n) (((n) << 1) + 1)

/* Controller core related registers */
#define GAHBCFG_REG  0x8
# define CORE_GLOBAL_INT_MASK (1<<0)
# define DMA_ENABLE           (1<<5)
# define EMPTY_LEVEL_INT      (1<<7)
/* Internal DMA burst lenght */
# define BURST_LEN_SINGLE     (0<<1)
# define BURST_LEN_INCR       (1<<1)
# define BURST_LEN_INCR4      (3<<1)
# define BURST_LEN_INCR8      (5<<1)
# define BURST_LEN_INCR16     (7<<1)
#define GAHBCFG_MASK (DMA_ENABLE | CORE_GLOBAL_INT_MASK | EMPTY_LEVEL_INT | \
    BURST_LEN_INCR16)

#define GUSBCFG_REG          0xc
# define PHY_TIME_OUT       (1<<0)
# define PHY_IFACE_16_BIT   (1<<3)
# define ULPI               (1<<4)
# define FOUR_BIT_ULPI      (1<<7)
# define SRP_ENABLE         (1<<8)
# define HNP_ENABLE         (1<<9)
/* modified by mabinjie at 2010-11-13 */
/* # define TURNAROUND_TIME    (9<<10) */
# define TURNAROUND_TIME_8BIT   (9<<10)
# define TURNAROUND_TIME_16BIT  (5<<10)
/* modified end by mabinjie at 2010-11-13 */
# define OTG_I2C_INTERFACE  (1<<16)
# define FORCE_DEVICE_MODE  (1<<30)
/* modified by mabinjie at 2010-11-13 */
/* #define GUSBCFG_MASK (TURNAROUND_TIME) */
#define GUSBCFG_MASK_8BIT (TURNAROUND_TIME_8BIT)
#define GUSBCFG_MASK_16BIT (TURNAROUND_TIME_16BIT)
/* #define TURNAROUND_TIME_MASK 0x1400 */
#define TURNAROUND_TIME_MASK 0x3C00 /* bits 13-10 */
/* modified end by mabinjie at 2010-11-13 */

#define GRSTCTL_REG       0x10
# define FLUSH_RX             (1<<4)
# define FLUSH_TX             (1<<5)
# define FLUSH_TX_FIFO(n)     ((n)<<6 | FLUSH_TX)
#  define FLUSH_ALL_FIFO       0x10

#define GOTGCTL_REG          0x00
#define GNPTXFSIZ_REG        0x28
#define GLPMCFG_REG          0x54
#define GI2CCTL_REG          0x30
#define GPWRDN_REG           0x58
#define GINTSTS_REG          0x14
#define GINTMSK_REG          0x18
# define CORE_MODE_MISMATCH_INT (1<<1)
# define CORE_OTG_INT           (1<<2)
# define CORE_SOF_INT           (1<<3)
# define CORE_RX_NOT_EMPTY_INT  (1<<4)
# define CORE_GOUTNAKEFF        (1<<7)
# define CORE_EARLY_SUSPEND     (1<<10)
# define CORE_SUSPEND_INT       (1<<11)
# define CORE_RESET_INT         (1<<12)
# define CORE_ENUM_DONE_INT     (1<<13)
# define CORE_EOPF              (1<<15)
# define CORE_IN_INT            (1<<18)
# define CORE_OUT_INT           (1<<19)
# define CORE_ISOIN_INCOMP      (1<<20)
# define CORE_ISOOUT_INCOMP     (1<<21)
# define CORE_DISCONNECT_INT    (1<<29)
/* BEGIN: Modified by zhuyuanyuan, 2010-9-8*/
# define CORE_SESSION_DETECTED_INT (1<<30)
/* END: Modified by zhuyuanyuan, 2010-9-8*/
# define CORE_RESUME_INT        ((uint32_t)1<<31)
#define GINTMSK_MASK (CORE_RESET_INT | CORE_ENUM_DONE_INT | CORE_OUT_INT | \
    CORE_IN_INT | CORE_SUSPEND_INT | CORE_RESUME_INT)

#define GRXFSIZE_REG         0x24
# define RX_FIFO_SIZE        0x200

#define GTXFSIZE_REG         0x28
# define TX_FIFO_BASE_ADDR    RX_FIFO_SIZE
# define TX_FIFO_SIZE        0x100
/* modified by mabinjie at 2010-11-11 */
# define EP0_TX_FIFO_SIZE   0x10
/* modified end by mabinjie at 2010-11-11 */

#define DIEPTXF_REG(fifo_number) (0x104 + ((fifo_number) - 1) * 0x4 )

#define CORE_SYNOPSYS_ID     0x40

#define GHWCFG1_REG          0x44
# define GHWCFG1_BI_DIR      0x0
# define GHWCFG1_IN_DIR      0x1
# define GHWCFG1_OUT_DIR     0x2

#define GHWCFG2_REG          0x48
# define GHWCFG2_NUMDEVEP_MSK 0x3C00
# define GHWCFG2_NUMDEVEP_SHFT 10

#define GHWCFG3_REG          0x4C
# define GHWCFG3_DFIFO_SHIFT 16
#define GHWCFG4_REG          0x50
# define GHWCFG4_INEP_MSK    0x3C000000
# define GHWCFG4_INEP_SHFT   26
# define OTG_EN_DED_TX_FIFO  (1<<25)
#define DCFG_REG             0x800
# define DCFG_DEVSPD_HIGH       (0<<0)
# define DCFG_DEVSPD_FULL       (1<<0)
# define DCFG_ENABLE_SG         (1<<23)
# define DCFG_DEV_ADDRESS(addr) ((addr)<<4)

#define DCTL_REG             0x804
# define DCTL_REMOTE_WAKEUP     (1<<0)
# define DCTL_SOFT_DISCON       (1<<1)
# define DCTL_TEST_J            (1<<4)
# define DCTL_TEST_K            (2<<4)
# define DCTL_TEST_SE0_NAK      (3<<4)
# define DCTL_TEST_PACKET       (4<<4)
# define DCTL_TEST_FORCE_ENABLE (5<<4)
# define DCTL_SET_GLOBAL_OUT_NAK   (1<<9)
# define DCTL_CLEAR_GLOBAL_OUT_NAK (1<<10)
# define DCTL_PWR_ON_PRG_DONE   (1<<11)
# define DCTL_INNORE_FRM_NO     (1<<15)

#define DSTS_REG             0x808
# define DSTS_SOFFN(a)       (((a)>>8) & 0x3FFF)
# define DSTS_SUSPEND        (1<<0)
/* modified by mabinjie at 2010-11-14 */
/* # define DSTS_SPEED_HIGH     (0<<1) */
/* # define DSTS_SPEED_FULL     (1<<1) */
# define DSTS_SPEED_MASK    (3<<1)
# define DSTS_SPEED_HIGH    (0<<1)
# define DSTS_SPEED_FULL    (1<<1)
# define DSTS_SPEED_LOW     (2<<1)
/* modified end by mabinjie at 2010-11-14 */

#define DIEPMSK_REG          0x810
# define DIEP_TRANSFER_COMPLETE (1<<0)
# define DIEP_EP_DISABLED      (1<<1)
# define DIEP_INTKN_EMPTY      (1<<4)
# define DIEP_INEPNAKEFF_MSK   (1<<6)
# define DIEP_BNA              (1<<9)
/* modified by mabinjie at 2010-11-15 */
# define DIEP_AHBERR           (1<<2)
/* modified end by mabinjie at 2010-11-15 */

#define DOEPMSK_REG          0x814
# define DOEP_TRANSFER_COMPLETE (1<<0)
# define DOEP_SETUP_DONE        (1<<3)
# define DOEP_EP_DISABLED      (1<<1)
# define DOEP_BNA              (1<<9)
/* modified by mabinjie at 2010-11-15 */
# define DOEP_AHBERR           (1<<2)
/* modified end by mabinjie at 2010-1-15 */

#define DAINT_REG            0x818
#define DAINT_MSK_REG        0x81c
#define DIEPEMSK_REG         0x834
# define DAINT_INEP(n)          (1<<(n))
# define DAINT_OUTEP(n)         (1<<((n) + 16))
/* BEGIN: Modified by zhuyuanyuan, 2010-9-8*/
#define PCGCCTL_REG           (0xe00)
#define STOP_PCLK              (0x1)
#define PWR_CLMP               (1<<2)
#define RST_PDENMODULE         (1<<3)
#define STOP_HCLK             (1<<1)

#define DIEPCTL(ep)         (0x900 + ((ep) * 0x20))
# define DIEPCTL_MPS(mps)       ((mps)<<0)
# define DIEPCTL_ACTIVE         (1<<15)
# define DIEPCTL_ISOC_EVE_ODD   (1<<16)
# define DIEPCTL_NAKSTS         (1<<17)
# define DIEPCTL_TYPE_CTL       (0<<18)
# define DIEPCTL_TYPE_ISOCH     (1<<18)
# define DIEPCTL_TYPE_BULK      (2<<18)
# define DIEPCTL_TYPE_INTR      (3<<18)
# define DIEPCTL_STALL          (1<<21)
# define DIEPCTL_CLEAR_NAK      (1<<26)
# define DIEPCTL_SET_NAK        (1<<27)
# define DIEPCTL_DATA0          (1<<28)
# define DIEPCTL_DATA1          (1<<29)
# define DIEPCTL_DISABLE        (1<<30)
# define DIEPCTL_ENABLE         ((uint32_t)1<<31)


#define DIEPINT(ep)         (0x908 + ((ep) * 0x20))
#define DOEPINT(ep)         (0xb08 + ((ep) * 0x20))
# define DEPINT_TRANSFER_COMPLETED (1<<0)
# define DEPINT_SETUP_DONE         (1<<3)
# define DEPINT_TOKEN_RECEIVED     (1<<4)
# define DEPINT_STS_PHSE_RCVD      (1<<5)
/* modified by mabinjie at 2010-11-15 */
# define DEPINT_INTkn_EP_MIS        (1<<5)
/* modified end by mabinjie at 2010-11-15 */
# define DEPINT_TIME_OUT           (1<<3)
# define DEPINT_EP_DISABLED        (1<<1)
# define DEPINT_EP_INTKN_EMPTY     (1<<5)
# define DEPINT_EP_INEPNAKEF       (1<<6)
# define DEPINT_BNA                (1<<9)

#ifdef __cplusplus
}
#endif

#endif   
