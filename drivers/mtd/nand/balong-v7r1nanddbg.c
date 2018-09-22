/******************************************************************************
*    Copyright (c) 2009-2010 by Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/
/* move the define to config file */
//#define CONFIG_HINFC400_DBG_NAND_PROC_FILE
//#define CONFIG_HINFC400_DBG_NAND_EC_NOTICE

#if defined(CONFIG_HINFC400_DBG_NAND_PROC_FILE) || defined(CONFIG_HINFC400_DBG_NAND_EC_NOTICE)
#  define DBG_NAND_CORRECT_ERROR
#endif /* defined(CONFIG_HINFC400_DBG_NAND_PROC_FILE) || defined(CONFIG_HINFC400_DBG_NAND_EC_NOTICE) */

/*****************************************************************************/
/* check wether there is a uncorrectable error                               */
/*****************************************************************************/
#ifdef DBG_NAND_CORRECT_ERROR

static int dbg_nand_num_of_unc_ecc = 0;
/*****************************************************************************/
static void dbg_nand_ec_init(void)
{
#  if defined(CONFIG_HINFC400_DBG_NAND_PROC_FILE)
    printk("nand debug: DBG_NAND_CORRECT_ERROR\n");
#  endif /* defined(CONFIG_HINFC400_DBG_NAND_PROC_FILE) */

#  ifdef CONFIG_HINFC400_DBG_NAND_EC_NOTICE 
    printk("nand debug: CONFIG_HINFC400_DBG_NAND_EC_NOTICE\n");
#  endif /* CONFIG_HINFC400_DBG_NAND_EC_NOTICE */
}
/*****************************************************************************/
/*
 *  first offset = 0;
 *  next  offset = 1;
 *  ...
 */
static void dbg_nand_ec_get_status(struct hinfc_host *host, int offset)
{
    int ix;
    unsigned int ecc_num;
    unsigned int reg_val;

    if (host->ecctype == et_ecc_none)
        return;

    if (offset == 0)
        dbg_nand_num_of_unc_ecc = 0;

    reg_val = hinfc_read(host, 0x20);
    ecc_num = (reg_val >> 16);

    offset <<= 2;

    if (host->ecctype == et_ecc_1bit)
    {
        for (ix = 0; ix < 8; ix++)
        {
            if ((ecc_num & 0x03) == 0x03)
            {
                dbg_nand_num_of_unc_ecc++;
            }
            ecc_num >>= 2;
        }
    }
    else if (host->ecctype == et_ecc_4bytes
        || host->ecctype == et_ecc_8bytes)
    {
        for (ix = 0; ix < 4; ix++)
        {
            if ((ecc_num & 0x0F) == 0x0F)
            {
                dbg_nand_num_of_unc_ecc++;
            }
            ecc_num >>= 4;
        }
    }
}
/*****************************************************************************/

static void dbg_nand_ec_notice(struct hinfc_host *host, int oobstart)
{
    if (host->ecctype == et_ecc_none)
        return;

    if (dbg_nand_num_of_unc_ecc)
    {
        unsigned long *oob = (unsigned long *)(host->buffer + oobstart);

        if (oob[0] != 0xFFFFFFFF || oob[1] != 0xFFFFFFFF
            || oob[2] != 0xFFFFFFFF || oob[3] != 0xFFFFFFFF
            || oob[4] != 0xFFFFFFFF || oob[5] != 0xFFFFFFFF
            || oob[6] != 0xFFFFFFFF || oob[7] != 0xFFFFFFFF)
        {
#  ifdef CONFIG_HINFC400_DBG_NAND_EC_NOTICE
            /* if exists file "/proc/nand", this also show in "/proc/nand" */
            printk("!!! nand uncorrectable error, page:0x%08X\n",
                GET_PAGE_INDEX(host));
#  endif
        }
        else
        {
            dbg_nand_num_of_unc_ecc = 0;
        }
    }
}
#endif /* !defined(DBG_NAND_CORRECT_ERROR) */

/*****************************************************************************/
/* check wether pragrom on page more than once.                              */
/*****************************************************************************/
#ifdef DBG_NAND_PROGRAM 

struct dbg_nand_program_param_t
{
    unsigned char page[0x1FFFFF];
    unsigned int  nPagePerBlock;

};
static struct dbg_nand_program_param_t dbg_nand_program_param;
/*****************************************************************************/

static void dbg_nand_program_init(int nPagePerBlock)
{
    printk("nand debug: DBG_NAND_PROGRAM\n");
    memset(&dbg_nand_program_param, 0, sizeof(dbg_nand_program_param));
    dbg_nand_program_param.nPagePerBlock = nPagePerBlock;
}
/*****************************************************************************/

static void dbg_nand_erase(unsigned int page)
{
    int ix;
    int byte, bit;

    if (page + dbg_nand_program_param.nPagePerBlock >= 0x1FFFFF)
    {
        printk("erase page: 0x%08X out of range.\n", page);
        return;
    }

    for (ix = 0; ix < dbg_nand_program_param.nPagePerBlock; ix++, page++)
    {
        byte = (page >> 3);
        bit  = (page & 0x07);
        dbg_nand_program_param.page[byte] &= ~(1 << bit);
    }
}
/*****************************************************************************/

static void dbg_nand_program(unsigned int page)
{
    int byte, bit;

    if (page >= 0x1FFFFF)
    {
        printk("write page: 0x%08X out of range.\n", page);
        return;
    }

    byte = (page >> 3);
    bit  = (page & 0x7);

    if (dbg_nand_program_param.page[byte] & (1 << bit))
    {
        printk("!!! page 0x%08X program more than once.\n", page);
    }

    dbg_nand_program_param.page[byte] |= (1 << bit);
}
#endif /* defined(DBG_NAND_PROGRAM) */

/*****************************************************************************
*  create file "/proc/nand" show the last 100 operation      
******************************************************************************/
#if defined(CONFIG_HINFC400_DBG_NAND_PROC_FILE)

#  include <linux/proc_fs.h>
#  include <linux/module.h>
#  include <linux/moduleparam.h>

struct dbg_nand_proc_t
{
    struct proc_dir_entry *entry;
    int IsInit;
    int index;
    int NumOfLogs;

#  define NAND_MAX_OF_LOGS      (100)
    char logs[NAND_MAX_OF_LOGS][100];
};

static struct dbg_nand_proc_t dbg_nand_proc =
{
    .entry = NULL,
    .IsInit = 0,
    .index = 0,
    .NumOfLogs = 0,
};
    
static int dbg_read_nand_proc(char *page, char **start, off_t off, int count,
                              int *eof, void *data_unused);
/*****************************************************************************/

static int dbg_nand_proc_init(void)
{
    int res = -ENOMEM;

    if (dbg_nand_proc.IsInit++)
        return 0;

    dbg_nand_proc.entry = create_proc_entry("nand",
        S_IFREG | S_IRUSR | S_IWUSR, NULL);
    if (!dbg_nand_proc.entry)
        goto out;

    dbg_nand_proc.entry->read_proc = dbg_read_nand_proc;
#ifdef LUOCHUANZAO
    dbg_nand_proc.entry->owner = THIS_MODULE;
#endif
    printk("nand debug: CONFIG_HINFC400_DBG_NAND_PROC_FILE\n");

    res = 0;
out:
    return res;
}
/*****************************************************************************/

static int dbg_read_nand_proc(char *page, char **start, off_t off, int count,
                           int *eof, void *data_unused)
{
#  define NUM_OF_WRITE      (30)

    int pos;
    int offset = off * NUM_OF_WRITE;
    int NumOfLogs;
    int NumOfWrite;

    char *ppage = page;
    
    *(int *)start = 1;
    *eof = 1;

    if ((NumOfLogs = NAND_MAX_OF_LOGS) > dbg_nand_proc.NumOfLogs)
        NumOfLogs = dbg_nand_proc.NumOfLogs;

    if (NumOfLogs <= 0 || offset >= NumOfLogs)
        return 0;

    if ((pos = dbg_nand_proc.index - 1) < 0)
        pos = NumOfLogs - 1;

    NumOfWrite = offset;
    while (NumOfWrite-- > 0)
    {
        if (--pos < 0)
            pos = NumOfLogs - 1;
    }

    if (!offset)
    {
        ppage += sprintf(ppage, 
            "UTC Clock      op    cylce   page-offset           data\n");
    }

    for (NumOfWrite = NUM_OF_WRITE;
        NumOfWrite > 0 && ++offset < NumOfLogs; NumOfWrite--)
    {
        ppage += sprintf(ppage, "%s", dbg_nand_proc.logs[pos]);
        if (--pos < 0)
            pos = NumOfLogs - 1;
    }

    return ppage - page;
#  undef NUM_OF_WRITE
}
/*****************************************************************************/

static void do_gettime(int *hour, int *min, int *sec, int *msec)
{
    long val;
    struct timeval tv;

    do_gettimeofday(&tv);
    val = tv.tv_sec % 86400; /* the second form 0 hour */

    if (hour)
        *hour = val / 3600;
    val %= 3600;
    if (min)   
        *min  = val / 60;
    if (sec)   
        *sec  = val % 60;
    if (msec)  
        *msec = tv.tv_usec / 1000;
}
/*****************************************************************************/

static int dbg_nand_proc_save_logs(char *op, unsigned int addr[2], 
                                   unsigned int cycle, char *buf)
{
    char *correct_status = "";
    int hour, min, sec, msec;
    dbg_nand_proc.NumOfLogs++;

    do_gettime(&hour, &min, &sec, &msec);

#  ifdef DBG_NAND_CORRECT_ERROR
    if (dbg_nand_num_of_unc_ecc)
    {
        correct_status = "UC";
        dbg_nand_num_of_unc_ecc = 0;
    }
#  endif /* DBG_NAND_CORRECT_ERROR */

    if (buf)
    {
        unsigned int page   = ((addr[0] >> 16) | (addr[1] << 16));
        unsigned int offset = (addr[0] & 0xFFFF);
        unsigned char *p = buf;

        sprintf(dbg_nand_proc.logs[dbg_nand_proc.index],
            "%02d:%02d:%02d.%04d  %-8s %-4u 0x%08X-%04X %02X %02X %02X %02X %02X %02X %02X %02X   %s\n", 
            hour, min, sec, msec, op, cycle, page, offset,
            p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
            correct_status);
    }
    else
    {
        unsigned int page   = addr[0];
        unsigned int offset = 0;

        sprintf(dbg_nand_proc.logs[dbg_nand_proc.index],
            "%02d:%02d:%02d.%04d  %-8s %-4u 0x%08X-%04X  --\n", 
            hour, min, sec, msec, op, cycle, page, offset);
    }

    if (++dbg_nand_proc.index >= NAND_MAX_OF_LOGS)
    {
        dbg_nand_proc.index = 0;
    }
    return 0;
}
/*****************************************************************************/
#  undef NAND_MAX_OF_LOGS

#endif /* defined(CONFIG_HINFC400_DBG_NAND_PROC_FILE) */
