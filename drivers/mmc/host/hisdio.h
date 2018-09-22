#ifndef _HI_SDIO_H_
#define _HI_SDIO_H_


extern int sdio_trace_level;
#define HISDIO_TRACE_LEVEL  (10)
extern int balong_sdio_trace_level;


#ifdef BALONG_SDIO_PRINTK_TRACE
#define balong_sdio_trace(level, msg...) do { \
	if((level) <= balong_sdio_trace_level) { \
		printk("balong_sdio_trace:%s:%d: ", __FUNCTION__, __LINE__); \
		printk(msg); \
		printk("\n"); \
	} \
}while(0)
#else

#define balong_sdio_print(level, msg...) do { \
	if((level) <= balong_sdio_trace_level) { \
		printk("balongsdio:%s:%d: ", __FUNCTION__, __LINE__); \
		printk(msg); \
		printk("\n"); \
	} \
}while(0)

#include "wifi_printf.h"
#define balong_sdio_trace(level, msg...) do { \
        if(OM_PrintWithModuleIsReady)\
        {\
    		DEBUG_FUNC_WIFI_MODULE(SDIO_ID_DEBUG,level, "balongsdio:%s:%d: ", __FUNCTION__, __LINE__);\
    		DEBUG_FUNC_WIFI_MODULE(SDIO_ID_DEBUG,level,msg);\
        }\
        else{\
    		if((level) <= balong_sdio_trace_level) { \
    		printk("balong_sdio_trace:%s:%d: ", __FUNCTION__, __LINE__); \
    		printk(msg); \
    		printk("\n"); \
    		}\
        }\
  } while(0)
#endif
/*
   0 - all message
   1 - dump all register read/write
   2 - flow trace
   3 - timeouut err and protocol err
   */

#define hisdio_trace(level, msg...) do { \
	if((level) >= sdio_trace_level) { \
		printk("hisdio_trace:%s:%d: ", __FUNCTION__, __LINE__); \
		printk(msg); \
		printk("\n"); \
	} \
}while(0)


#define hisdio_error(s...) 
/*
do{ \
	printk(KERN_ERR "hisdio:%s:%d: ", __FUNCTION__, __LINE__); \
	printk(s); \
	printk("\n"); \
}while(0)
*/

#define DRIVER_RESUME_THREAD	"hi_sdio_resume_thread"

#define DRIVER_NAME	"hi_sdio_io"
#define DETECT_NAME "hi_sdio_detect"
#define DETECT_SD_BUSY_CHECK_NAME  "hi_sdio_busy_check"
#define SDIO_DRIVER_NAME	"balong_sdio"

#define SDIO_DATA_MAX_POLL  64
#define SDIO_DATA_TIMEOUT   500
#define SDIO_DATA_BUSY_CHECK_TIMEOUT 500

#define MAX_SCATTERNODE_NUM	1024

#define HI_SDIO_REQUEST_TIMEOUT  (2 * HZ)

#define MAX_RETRY_COUNT   0x100000

#define MMC_CTRL          0x000
#define MMC_PWREN		  0x004
#define MMC_CLKDIV        0x008
#define MMC_CLKSRC        0x00C
#define MMC_CLKENA        0x010
#define MMC_TMOUT         0x014
#define MMC_CTYPE         0x018
#define MMC_BLKSIZ        0x01C
#define MMC_BYTCNT        0x020
#define MMC_INTMASK       0x024
#define MMC_CMDARG        0x028
#define MMC_CMD           0x02C
#define MMC_RESP0         0x030
#define MMC_RESP1         0x034
#define MMC_RESP2         0x038
#define MMC_RESP3         0x03C
#define MMC_MINTSTS       0x040
#define MMC_RINTSTS       0x044
#define MMC_STATUS        0x048
#define MMC_FIFOTH        0x04C
#define MMC_CDETECT       0x050
#define MMC_WRTPRT        0x054
#define MMC_GPIO          0x058
#define MMC_TCBCNT        0x05C
#define MMC_TBBCNT        0x060
#define MMC_DEBNCE        0x064
#define MMC_USRID         0x068
#define MMC_VERID         0x06C
#define MMC_HCON          0x070
#define MMC_UHSREG        0x074
#define MMC_RSTn       	  0x078
#define MMC_BMOD       	  0x080
#define MMC_PLDMND        0x084
#define MMC_DBADDR        0x088
#define MMC_IDSTS		  0x08C
#define MMC_IDINTEN		  0x090
#define MMC_DSCADDR		  0x094
#define MMC_BUFADDR		  0x098
#define MMC_CardThrCtl	  0x100
#define MMC_DATA          0x200

/*bit 25:internel dma enable*/
#define IDMA_ENABLE	   (1<<25)
/* bit 5: dma enable */
#define DMA_ENABLE        (1<<5)

/* bit 4: MMC host controller all intterrupt enable */ 
#define INT_ENABLE        (1<<4)
#define HOST_RESET       (1<<0)

/*bit 2: internal DMA reset*/
#define IDMA_RESET	(1<<2)

/* bit 1: FIFO reset controler*/
#define FIFO_RESET        (1<<1)


/* bit 0: enable of card clk*/
#define CCLK_ENABLE       (1<<0)
/*bit 16 :clk low power ctrl*/    /*syb*/
#define CCLK_LOW_POWER    (1<<16)



/* bit 31-8: data read timeout param */
#define DATA_TIMEOUT      (0xffffff<<8)

/* bit 7-0: response timeout param */
#define RESPONSE_TIMEOUT  0xff



/* bit 0: card width */
#define CARD_WIDTH        (0x1<<0)



/*bit 5 receive data fifo request*/
#define INTMASK_RXMASK	(1<<5)

/*bit 4 transmit data fifo request*/
#define INTMASK_TXMASK	(1<<4)

/* bit 16-1: mask MMC host controller each interrupt */
#define ALL_INT_MASK      0x1fffe

/*bit 5-4: mask hle[12] rxdr[5] and txdr[4] interrupt for dma mode */
#define DMA_INT_MASK      0x1afce



/* bit 31: cmd execute or load start param of interface clk bit */
/*Warning 648: (Warning -- Overflow in computing constant for operation:
   'shift left')*/
#define START_CMD         (1UL<<31)

/* bit 21: switch of adjusting interface clk  */
#define UPDATE_CLK_REG_ONLY   (1<<21)



/* bit 16: sdio interrupt status */
#define SDIO_INT_STATUS    (0x1<<16)

/* bit 15: end-bit error (read)/write no CRC interrupt status */
#define EBE_INT_STATUS    (0x1<<15)

/* bit 14: auto command done interrupt status */
#define ACD_INT_STATUS    (0x1<<14)

/* bit 13: start bit error interrupt status */
#define SBE_INT_STATUS    (0x1<<13)

/* bit 12: hardware locked write error interrupt status */
#define HLE_INT_STATUS    (0x1<<12)

/* bit 11: FIFO underrun/overrun error interrupt status */
#define FRUN_INT_STATUS    (0x1<<11)

/* bit 10: data starvation-by-host timeout interrupt status/Volt_switch_int */
#define HTO_INT_STATUS    (0x1<<10)
#define HTO_INT_NUM        (10)

/* bit 9: data read timeout interrupt status */
#define DRTO_INT_STATUS    (0x1<<9)

/* bit 8: response timeout interrupt status */
#define RTO_INT_STATUS    (0x1<<8)

/* bit 7: data CRC error interrupt status */
#define DCRC_INT_STATUS    (0x1<<7)

/* bit 6: response CRC error interrupt status */
#define RCRC_INT_STATUS    (0x1<<6)

/* bit 5: receive FIFO data request interrupt status */
#define RXDR_INT_STATUS    (0x1<<5)

/* bit 4: transmit FIFO data request interrupt status */
#define TXDR_INT_STATUS    (0x1<<4)

/* bit 3: data transfer Over interrupt status */
#define DTO_INT_STATUS    (0x1<<3)
#define DTO_INT_NUM       (3)

/* bit 2: command done interrupt status */
#define CD_INT_STATUS    (0x1<<2)
#define CD_INT_NUM       (2)

/* bit 1: response error interrupt status */
#define RE_INT_STATUS    (0x1<<1)


/* bit 16-1: clear MMC host controller each interrupt 
   but hardware locked write error interrupt
   */
#define ALL_INT_CLR          0xfffe
#define ALL_INT_CLR1       0xffff
#define MMC_CMD_DONE    (0x01 << 2)


#define DMA_FIFO_EMPTY_MASK (1<<2)
#define DMA_FIFO_FULL_MASK  (1<<3)
#define DATA_3_STATUS   (0x1<<8)
#define DATA_BUSY           (0x1<<9)


/* bit 30-28:  DMA one burst data */
/*  
000: 1    burst_size 1
001: 4    burst_size 4
010: 8    burst_sixe 8
*/


/* bit 27-16: rx data FIFO threshold */
/* bit 11-0: tx data FIFO threshold */
#if (defined(CHIP_BB_6920CS) \
	&& (defined(BOARD_ASIC) || defined(BOARD_SFT)))
#define BURST_SIZE        (0x5<<28)
#define TX_WMARK          0x40	
#define RX_WMARK          (0x3f<<16)
#else
#define BURST_SIZE        (0x2<<28)
#define TX_WMARK          0x8
#define RX_WMARK          (0x7<<16)
#endif


/* offset of sdio dma fifo */
#define MMC_DATA1			0x200
#define FIFO_DEPTH			8


#define MAC_LEN           6


/*bit 0:  VOLT_REG :hige voltage mode, 1:1.8v 0:3v */
/*bit 16: DDR_REG  :DDR mode,1:DDR mode ,0:Non-DDR mode*/
#define VOLT_REG_18V      (0)
#define DDR_MODE          (16)
#define MMC1_DMA_REQ_NUM 21
#define MMC_SDIO_CPU_TRXFER_MAX_LEN 512
#define MMC_SDIO_READ 0
#define MMC_SDIO_WRITE 1

#define DDR_MODE_CHOICE   (1)
#define VOLT_18V_CHOICE    (1)

typedef struct hisdio_arg{	
	 unsigned char  wlanmac[MAC_LEN]; //WLAN MAC ADDR
}sdio_args;

struct hisdio_host{	
	struct mmc_host         *mmc;	
	spinlock_t              lock;	
	struct mmc_request      *mrq;	
	struct mmc_command      *cmd;	
	struct mmc_data         *data;
	void __iomem		    *base;	
	unsigned int            card_status;
	struct scatterlist      *dma_sg;
	unsigned int 	        dma_lli_head[2];			
	unsigned int            dma_channel;	
	unsigned int	        dma_len;	
	unsigned int            dma_count;	
	unsigned int    	    dma_dir;
	u8                      config;	
	int                     irq;
	struct work_struct		sd_detect_work;
	struct work_struct		sdup_detect_work; /*y00186965 for sd_update*/
	struct workqueue_struct *detect_queue;
	struct workqueue_struct *sdupdate_queue; /*y00186965 for sd_update*/
	unsigned int    	    fifo_depth;
};

typedef union{
	unsigned int cmd_arg;
	struct cmd_bits_arg{
		unsigned int cmd_index                :  6;
		unsigned int response_expect          :  1;
		unsigned int response_length          :  1;
		unsigned int check_response_crc       :  1;
		unsigned int data_transfer_expected   :  1;
		unsigned int read_write               :  1;
		unsigned int transfer_mode            :  1;
		unsigned int send_auto_stop           :  1;
		unsigned int wait_prvdata_complete    :  1;
		unsigned int stop_abort_cmd           :  1;
		unsigned int send_initialization      :  1;
		unsigned int card_number              :  5;
		unsigned int update_clk_reg_only      :  1; /* bit 21 */
		unsigned int reserved                 :  6;
       	unsigned int volt_switch              :  1;
        unsigned int use_hold_reg             :  1;
		unsigned int reserved_1               :  1;
		unsigned int start_cmd                :  1; /* HSB */
	}bits;
}cmd_arg_s;

typedef union{
	unsigned int ulReg;
	struct
	{
		unsigned int ulOCR				: 24;  /*bit0-23*/
		unsigned int ulStuffBits			: 3; /*bit24-26*/
		unsigned int ulMemPresent		: 1;	/*bit27*/
		unsigned int ulNoOfFunctions		: 3; /*bit28-30*/
		unsigned int ulReady		: 1; /*bit31*/
	}Bits;
}SDIO_R4_RESPONSE;

typedef union{
	unsigned int ulReg;
	struct
	{
		unsigned int ulRdWrData			: 8;  /*bit0-7*/
		unsigned int ulOutRange			: 1; /*bit8*/
		unsigned int ulFunctionNumber		: 1;	/*bit9*/
		unsigned int ulUnused1			: 1; /*10*/
		unsigned int ulError				: 1; /*bit11*/
		unsigned int ulIoCurrentState		: 2; /*bit12-13*/
		unsigned int ulIllegalCommand		: 1; /*bit14*/
		unsigned int ulComCrcError		: 1; /*bit15*/
		unsigned int ulUnused2			: 16; /*bit16-31*/
	}Bits;
}SDIO_R5_RESPONSE;

typedef union
{
	unsigned int ulReg;
	struct
	{
		unsigned int ulCardStatus	:16;/*bit0-15*/
		unsigned int ulCardRCA	:16;/*bit16-31*/
	}Bits;
}SDIO_R6_RESPONSE;

typedef union
{
	unsigned int ulReg;
	struct
	{
		unsigned int ulWrData		: 8; /*bits0-7*/
		unsigned int ulUnused2		: 1; /*bits8*/
		unsigned int ulRegAddress	: 17; /*bits9-25*/
		unsigned int ulUnused1		: 1; /*bits26*/
		unsigned int ulRawFlag		: 1; /*bits27*/
		unsigned int ulFunctionNo	: 3; /*bits28-30*/
		unsigned int ulRwFlag		: 1; /*bits31*/	
	}Bits;
}SDIO_CMD52_INFO;

typedef union
{
	unsigned int ulReg;
	struct
	{
		unsigned int ulByteCount                : 9; /*bits0-8*/
		unsigned int ulRegAddress               : 17; /*bits9-25*/
		unsigned int ulOpcode                   : 1; /*bits26*/
		unsigned int ulBlkMode                  : 1; /*bits27*/
		unsigned int ulFunctionNo               : 3; /*bits28-30*/
		unsigned int ulRwFlag           : 1; /*bits31*/
	}Bits;
}SDIO_CMD53_INFO;

typedef union
{
	unsigned int ulReg;
	struct
	{
		unsigned int fifo_rx_watermark          : 1; /*bits0*/
		unsigned int fifo_tx_watermark                  : 1; /*bits1*/
		unsigned int fifo_empty                         : 1; /*bits2*/
		unsigned int fifo_full                                  : 1; /*bits3*/
		unsigned int cmd_fsm_states                     : 4; /*bits4-7*/
		unsigned int data_3_status                              : 1; /*bits8*/
		unsigned int data_busy                          : 1; /*bits9*/
		unsigned int data_state_mc_busy         : 1; /*bits10*/
		unsigned int response_index                     : 6; /*bits11-16*/
		unsigned int fifo_count                                 : 13; /*bits17-29*/
		unsigned int dma_ack                                    : 1; /*bits30*/
		unsigned int dma_req                                    : 1; /*bits31*/
	}Bits;
}SDIO_STATUS_REG;

/*y00186965 for PMU双核通信*/
    /*++by pandong cs*/	
#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2)) \
    || (defined(BOARD_SFT) && defined(VERSION_V7R1))
/*LDO开关参数*/
#define LDO7                 7
#define LDO10                10
#define LDO10_3_3V          330
#define LDO10_3_0V          300
#define LDO10_1_8V          180

typedef enum _VOLT_SWITCH
{
    VOLT_SWTICH_NO = 0,
    VOLT_SWTICH_YES
}VOLT_SWTICH_STU;

typedef enum _LDO_STATUS
{
    LDO_OFF = 0,
    LDO_ON
}LDO_STATUS_STU;

typedef enum _SD_LDO_SWITCH
{
    CLOSE_LDO = 0,
    OPEN_LDO
}SD_LDO_SWITCH_STU;


typedef struct _SD_IDOCTRL_ARG
{
    unsigned int LDO_id;
	unsigned int is_voltage_switch;
    unsigned int open_close;
	unsigned int voltage_val;
}HISDIO_IDOCTRL_ARG;

/*y00186965*/
typedef struct
{
    unsigned int u32ModuleId;
    unsigned int u32FuncId;
    HISDIO_IDOCTRL_ARG  stSDLDOArg;    
}HISDIO_IFC_MSG_STRU;
#elif ((defined (BOARD_ASIC)||defined(BOARD_SFT)) \
	&& (defined(CHIP_BB_6920ES) || defined(CHIP_BB_6920CS)))
/*q00175519*/
#define LDO7                 7
#define LDO10                10

enum slot_state 
{
	SD_MMC_CARD_NO_REMOVE	= 0,
	SD_MMC_CARD_NO_INSERT 	= 1,
	SD_MMC_CARD_REMOVED		= 2,
	SD_MMC_CARD_INSERTED	= 3
};
int hi_keyboard_respond(void);
#endif
/*y00186965 for PMU双核通信end*/
	/*--by pandong cs*/
#endif


