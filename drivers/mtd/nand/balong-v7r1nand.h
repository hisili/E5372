#ifndef __DRIVER_BALONG_V7R1NAND_H__
#define __DRIVER_BALONG_V7R1NAND_H__

/*****************************************************************************/
/* this move to Kconfig file. */
//#define HINFC400_MAX_CHIP                   (1)

//#define HINFC400_BUFFER_SIZE                 (2048+128)
#define HINFC400_BUFFER_SIZE                 (2048+64)
/* this move to Kconfig file. */
//#define CONFIG_HINFC400_REG_BASE_ADDRESS     0x90010000
#define HINFC400_REG_BASE_ADDRESS_LEN        0x00010000

/* this move to Kconfig file. */
//#define CONFIG_HINFC400_BUFFER_BASE_ADDRESS  0x24000000
#define HINFC400_BUFFER_BASE_ADDRESS_LEN     0x00100000

/*****************************************************************************/
#define HINFC400_CON                                 0x00
#define HINFC400_CON_EXT_DATA_ECC_EN     (1U << 9)
#define HINFC400_CON_READY_BUSY_SEL      (1U << 7)
#define HINFC400_CON_ECC_EN              (1U << 5)
#define HINFC400_CON_OP_MODE             (1U << 0)
#ifdef CONFIG_MTD_NAND_BALONG_V3R2
#define HINFC400_CON_BUS_WIDTH_16             (1U << 3)   //qyf add for V3R2
#endif

#define HINFC400_CMD                                 0x0C
#define HINFC400_ADDRL                               0x10
#define HINFC400_ADDRH                               0x14
#define HINFC400_DATA_NUM                            0x18 

#define HINFC400_OP                                  0x1C
#define HINFC400_OP_ADDR_CYCLE_SHIFT     (0x9)
#define HINFC400_OP_NF_CS_SHIFT          (0x7)
#define HINFC400_OP_CMD1_EN              (1U << 6)
#define HINFC400_OP_ADDR_EN              (1U << 5)
#define HINFC400_OP_WRITE_DATA_EN        (1U << 4)
#define HINFC400_OP_CMD2_EN              (1U << 3)
#define HINFC400_OP_WAIT_READY_EN        (1U << 2)
#define HINFC400_OP_READ_DATA_EN         (1U << 1)
#define HINFC400_OP_READ_STATUS_EN       (1U << 0)

#define HINFC400_STATUS                               0x20

#define HINFC400_ECC_TEST                             0x5C
#define HINFC400_ECC_TEST_DEC_ONLY       (1U << 1)
#define HINFC400_ECC_TEST_ENC_ONLY       (1U << 0)

//add by qyf for V3R2
#ifdef CONFIG_MTD_NAND_BALONG_V3R2
#define HINFC400_INT_EN                                          0x24
#define HINFC400_INT_CTRL                                      0x2C
#define HINFC400_INT_MASK_NONE                           0
#define HINFC400_INT_MASK_ALL                              0x7FF
/*
ECC  纠错使能，只在读数据时有效 
EDC  校验使能，编程模式下，生成ECC码使能，读数据模式下进行校验使能
*/
#define HINFC400_OP_PARA                                  0x70
#define HINFC400_OP_PARA_OOB_ECC_EN          (1U << 5)  
#define HINFC400_OP_PARA_DATA_ECC_EN        (1U << 4)
#define HINFC400_OP_PARA_OOB_EDC_EN          (1U << 3) 
#define HINFC400_OP_PARA_DATA_EDC_EN        (1U << 2)
#define HINFC400_OP_PARA_OOB_RW_EN           (1U << 1)  // spare area read/write enable
#define HINFC400_OP_PARA_DATA_RW_EN         (1U << 0)  // data area read/write enable
#define NFC4_ECC_PARA_MASK_ALL                   0x3F       /*0011  1111*/
#define NFC4_ECC_PARA_DATA_ONLY                 0x15     /*0001  0101*/
#define NFC4_ECC_PARA_EN_ALL                        NFC4_ECC_PARA_MASK_ALL
#define NFC4_ECC_PARA_DISABLE                      0x03         /*0000  0010*/
#endif
//end  by qyf
/*****************************************************************************/

#define HINFC400_ADDR_CYCLE_MASK        0x4

#define HINFC400_CHIP_DELAY             25

/*****************************************************************************/
enum ecc_type
{
    et_ecc_none    = 0x00,
    et_ecc_1bit    = 0x01,
    et_ecc_4bytes  = 0x02,
    et_ecc_8bytes  = 0x03,
};

enum page_type
{
    pt_pagesize_512   = 0x00,
    pt_pagesize_2K    = 0x01,
    pt_pagesize_4K    = 0x02,
    pt_pagesize_8K    = 0x03,
};

struct hinfc_host 
{
    struct nand_chip *chip;
	struct mtd_info  *mtd;
    void __iomem *iobase;

    unsigned int offset;
    unsigned int command;

    int chipselect;

    enum ecc_type ecctype;

    void (*program)(struct hinfc_host *host, struct mtd_info *mtd);
    void (*read)(struct hinfc_host *host, struct mtd_info *mtd);

    unsigned long NFC_CON_ECC_ENABLE;
    unsigned long NFC_CON_ECC_DISABLE;

//add by qyf for V3R2
#ifdef CONFIG_MTD_NAND_BALONG_V3R2
    unsigned long NFC_OP_PARA_ECC_ENABLE;
    unsigned long NFC_OP_PARA_ECC_DISABLE;
#endif
//end by qyf
    int IsUseBuffer;

    unsigned int addr_cycle;
    unsigned int addr_value[2];
    unsigned int column;
    char * buffer;
};
/*****************************************************************************/
#define GET_PAGE_INDEX(host) \
    ((host->addr_value[0] >> 16) | (host->addr_value[1] << 16))

#ifndef DBG_NAND_PROGRAM
#  define dbg_nand_program_init(_p0)
#  define dbg_nand_erase(_p0)
#  define dbg_nand_program(_p0)
#endif /* DBG_NAND_PROGRAM */

#if !(defined(CONFIG_HINFC400_DBG_NAND_PROC_FILE) || defined(CONFIG_HINFC400_DBG_NAND_EC_NOTICE))
#  define dbg_nand_ec_get_status(_p0, _p1)
#  define dbg_nand_ec_notice(_p0, _p1)
#  define dbg_nand_ec_init()
#endif /* !(defined(CONFIG_HINFC400_DBG_NAND_PROC_FILE) || defined(CONFIG_HINFC400_DBG_NAND_EC_NOTICE)) */

#if !defined(CONFIG_HINFC400_DBG_NAND_PROC_FILE)
#  define dbg_nand_proc_save_logs(_p0, _p1, _p2, _p3)
#  define dbg_nand_proc_init()
#endif /* !defined(CONFIG_HINFC400_DBG_NAND_PROC_FILE) */

/*****************************************************************************/
#if 0
 extern void nand_check_dcore(void);

static inline u32 hinfc_read(struct hinfc_host *host,int  _reg) 
{
	nand_check_dcore();
 	return readl((char *)host->iobase + (_reg));
}

static inline void hinfc_write(struct hinfc_host *_host, int _value, int _reg) 
{
	nand_check_dcore();
	return writel((_value), (char *)_host->iobase + (_reg));
}
#else
#define hinfc_read(_host, _reg) \
    readl((char *)_host->iobase + (_reg))

#define hinfc_write(_host, _value, _reg) \
    writel((_value), (char *)_host->iobase + (_reg))
 #endif
/*****************************************************************************/

#define DBG_BUG(fmt, args...) do{\
    printk("%s(%d): !!! BUG " fmt, __FILE__, __LINE__, ##args); \
    BUG(); \
} while (0)

/*****************************************************************************/
#endif

