/******************************************************************************
*    Copyright (c) 2009-2010 by  Hisilicon Tech. Co., Ltd.
*    All rights reserved.
* ***
*
******************************************************************************/

#include <linux/mtd/nand.h>

/*****************************************************************************/

#define _2K             (2048)
#define _4K             (4096)
#define _8K             (8192)

#define _128K           (0x20000)
#define _256K           (0x40000)
#define _512K           (0x80000)
#define _768K           (_256K + _512K)
#define _1M             (0x100000)
#define _2M             (_1M << 1)

#define _512M           (0x20000000UL)
#define _1G             (0x40000000ULL)
#define _2G             (0x80000000ULL)
#define _4G             (0x100000000ULL)
/*****************************************************************************/

extern struct nand_flash_dev *(*nand_base_get_special_flash_type)(
    struct mtd_info *mtd, struct nand_chip *chip, int byte0, int byte1);

/*****************************************************************************
 * this is nand id probe function.    
******************************************************************************/
/*
 *  Hynix H27UAG8T2A  0xAD 0xD5 0x94 0x25 0x44 0x41
 *  Hynix H27UAG8T2B  0xAD 0xD5 0x94 0x9A 0x74 0x42
 */
static struct nand_flash_dev * H27UAG8T2x(unsigned char id[8])
{
    static struct nand_flash_dev type[2];

    int pagesizes[]   = {_2K, _4K, _8K, 0};
    int oobsizes[]    = {128, 224, 448, 0, 0, 0, 0, 0};
    int blocksizes[]  = {_128K, _256K, _512K, _768K, _1M, _2M, 0, 0};

    int blocktype = (((id[3] >> 5) & 0x04) | ((id[3] >> 4) & 0x03));
    int oobtype   = (((id[3] >> 2) & 0x03) | ((id[3] >> 4) & 0x04));

    type->chipsize  = (_2G >> 20);
    type->name      = "H27UAG8T2x";
    type->options   = 0;
    type->pagesize  = pagesizes[(id[3] & 0x03)];
    type->erasesize = blocksizes[blocktype];
    *(unsigned long *)&type[1] = oobsizes[oobtype];

    return type;
}
/*****************************************************************************/
/*
 *  Samsung K9LBG08U0D  0xEC 0xD7 0xD5 0x29 0x38 0x41
 *  Samsung K9HCG08U1D  same as top
 *  Samsung K9XDG08U5D  same as top
 */
static struct nand_flash_dev * K9xxG08UxD(unsigned char id[8])
{
    static struct nand_flash_dev type[2];

    int pagesizes[]   = {_2K, _4K, _8K, 0};
    int oobsizes[]    = {0, 128, 218, 0, 0, 0, 0, 0};
    int blocksizes[]  = {_128K, _256K, _512K, _1M, 0, 0, 0, 0};

    int blocktype = (((id[3] >> 5) & 0x04) | ((id[3] >> 4) & 0x03));
    int oobtype   = (((id[3] >> 4) & 0x04) | ((id[3] >> 2) & 0x03));

    type->chipsize  = (_4G >> 20);
    type->name      = "K9xxG08UxD";
    type->options   = 0;
    type->pagesize  = pagesizes[(id[3] & 0x03)];
    type->erasesize = blocksizes[blocktype];
    *(unsigned long *)&type[1] = oobsizes[oobtype];

    return type;
}
/*****************************************************************************/
/*
 *  Micron MT29F16G08CBABA  0x2C 0x48 0x04 0x46 0x85 0x00 0x00 0x00
 *  Micron MT29F16G08CBABB  same as top
 */
static struct nand_flash_dev * MT29F16G08CBABx(unsigned char id[8])
{
    static struct nand_flash_dev type[2];

    type->chipsize  = (_2G >> 20);
    type->name      = "MT29F16G08CBABx";
    type->options   = 0;
    type->pagesize  = _4K;
    type->erasesize = _1M;
    *(unsigned long *)&type[1] = 224;

    return type;
}
/*****************************************************************************/

static struct nand_flash_dev * TC58NVG4D2ETA00(unsigned char id[8])
{
    static struct nand_flash_dev type[2];

    type->chipsize  = (_2G >> 20);
    type->name      = "TC58NVG4D2ETA00";
    type->options   = 0;
    type->pagesize  = _8K;
    type->erasesize = _1M;
    *(unsigned long *)&type[1] = 360; // XXX

    return type;
}
/*****************************************************************************/

struct nand_flash_special_dev
{
    unsigned char id[8];
    int length;             /* length of id. */
    struct nand_flash_dev *(*probe)(unsigned char id[8]);
};
/*****************************************************************************/

static struct nand_flash_special_dev nand_flash_special_dev[] = 
{
    /* 0     1      2      3     4     5     6     7   len                    */ 
    { {0x2c, 0x48, 0x04, 0x46, 0x85, 0x00, 0x00, 0x00}, 8,  MT29F16G08CBABx,},
    { {0x98, 0xD5, 0x94, 0x32, 0x76, 0x00, 0x00, 0x00}, 5,  TC58NVG4D2ETA00,},
    { {0xEC, 0xD7, 0xD5, 0x29, 0x38, 0x41, 0x00, 0x00}, 6,  K9xxG08UxD, },
    { {0xAD, 0xD5, 0x94, 0x00, 0x00, 0x00, 0x00, 0x00}, 3,  H27UAG8T2x, },

    {{0},0,0},
};
/*****************************************************************************/

struct nand_flash_dev *nand_get_special_flash_type(struct mtd_info *mtd,
    struct nand_chip *chip, int byte0, int byte1)
{
    int ix;
    unsigned char byte[8];
    struct nand_flash_dev *flash_type;
    struct nand_flash_special_dev *spl_dev;

    byte[0] = (unsigned char)byte0;
    byte[1] = (unsigned char)byte1;
    for (ix = 2; ix < 8; ix++)
    {
        byte[ix] = chip->read_byte(mtd);
    }

    printk("Nand ID: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",
        byte[0], byte[1], byte[2], byte[3], byte[4], byte[5], byte[6], byte[7]);

    for (spl_dev = nand_flash_special_dev; spl_dev->length; spl_dev++)
    {
        if (memcmp(byte, spl_dev->id, spl_dev->length))
            continue;
        flash_type = spl_dev->probe(byte);
        flash_type->id = byte[1];
        return flash_type;
    }

    chip->cmdfunc(mtd, NAND_CMD_READID, 0x00, -1);
    chip->read_byte(mtd);
	chip->read_byte(mtd);

    return NULL;
}
/*****************************************************************************/

static int __init nand_spl_ids_init(void)
{
    printk("Special Nand id table Version 1.00\n");
    //nand_base_get_special_flash_type = nand_get_special_flash_type;
    return 0;
}
/*****************************************************************************/

static void __exit nand_spl_ids_exit(void)
{
}
/*****************************************************************************/

module_init(nand_spl_ids_init);
module_exit(nand_spl_ids_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Special NAND id table");

