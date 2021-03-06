/*
 * Simple MTD partitioning layer
 *
 * (C) 2000 Nicolas Pitre <nico@fluxnic.net>
 *
 * This code is GPL
 *
 * 	02-21-2002	Thomas Gleixner <gleixner@autronix.de>
 *			added support for read_oob, write_oob
 */

/*lint -e413*/
/*lint -e713*/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/kmod.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/compatmac.h>

/* Our partition linked list */
static LIST_HEAD(mtd_partitions);

/* Our partition node structure */
struct mtd_part {
	struct mtd_info mtd;
	struct mtd_info *master;
	uint64_t offset;    /*lint !e958 */
	struct list_head list;
};

/*
 * Given a pointer to the MTD object in the mtd_part structure, we can retrieve
 * the pointer to that structure with this macro.
 */
#define PART(x)  ((struct mtd_part *)(x))


/*
 * MTD methods which simply translate the effective address and pass through
 * to the _real_ device.
 */


unsigned int find_correct_position(struct mtd_part *part, unsigned int from )
{
    int position = 0;
    int skip = 0;

    if(NULL == part)
    {
        return from;
    }

    if((part->mtd.ecc_stats.badblocks > 0)
            && (!strcmp(part->mtd.name, MMC_PARTITION_NAME)))
     {
        printk("find_correct_position : %d.....!\n",part->mtd.ecc_stats.badblocks);
        position = ((int )from) / part->mtd.erasesize;

            /*如果当前方位位置的BLK 比坏块表中的某些BLK 位置大，则要跳过这些坏块= N(举例)，
                同时要保证后续的N 块中不能包含坏块! 因此运算的过程中当前位置要+ SKIP 的变化。
            */
            while( (skip < part->mtd.ecc_stats.badblocks)
                     && ( skip < MAX_TRY_BLKS)
                     && (part->mtd.ecc_stats.bbtarr[skip] <= position + skip))
            {
                skip++;
                printk(KERN_EMERG "A bad block is %d, skip it!\n",
                part->mtd.ecc_stats.bbtarr[skip]);
            }
          
            from += skip * part->mtd.erasesize;
        }

        return from;
}



static int part_read(struct mtd_info *mtd, loff_t from, size_t len,
		size_t *retlen, u_char *buf)
{
	struct mtd_part *part = PART(mtd);
	struct mtd_ecc_stats stats;
	int res;

	stats = part->master->ecc_stats;

	if ((uint64_t)from >= mtd->size)
		len = 0;
	else if ((uint64_t)from + len > mtd->size)
		len = (size_t)(mtd->size - (uint64_t)from);
		
    from = find_correct_position(part, from);

	res = part->master->read(part->master, (uint64_t)from + part->offset,
				   len, retlen, buf);
	/*lint -e730*/
	if (unlikely(res)) {
		if (res == -EUCLEAN)
			mtd->ecc_stats.corrected += part->master->ecc_stats.corrected - stats.corrected;
		if (res == -EBADMSG)
			mtd->ecc_stats.failed += part->master->ecc_stats.failed - stats.failed;
	}
	/*lint +e730*/
	return res;
}

static int part_point(struct mtd_info *mtd, loff_t from, size_t len,
		size_t *retlen, void **virt, resource_size_t *phys)
{
	struct mtd_part *part = PART(mtd);
	if ((uint64_t)from >= mtd->size)
		len = 0;
	else if ((uint64_t)from + len > mtd->size)
		len = (size_t)(mtd->size - (uint64_t)from);
	return part->master->point (part->master, (uint64_t)from + part->offset,
				    len, retlen, virt, phys);
}

static void part_unpoint(struct mtd_info *mtd, loff_t from, size_t len)
{
	struct mtd_part *part = PART(mtd);

	part->master->unpoint(part->master, (uint64_t)from + part->offset, len);
}

static unsigned long part_get_unmapped_area(struct mtd_info *mtd,
					    unsigned long len,
					    unsigned long offset,
					    unsigned long flags)
{
	struct mtd_part *part = PART(mtd);
	offset += (unsigned long)part->offset;
	return part->master->get_unmapped_area(part->master, len, offset,
					       flags);
}

static int part_read_oob(struct mtd_info *mtd, loff_t from,
		struct mtd_oob_ops *ops)
{
	struct mtd_part *part = PART(mtd);
	int res;

	if ((uint64_t)from >= mtd->size)
		return -EINVAL;
	if (ops->datbuf && ((uint64_t)from) + ops->len > mtd->size)
		return -EINVAL;

    from = find_correct_position(part, from);
	res = part->master->read_oob(part->master, (uint64_t)from + part->offset, ops);
	/*lint -e730*/
	if (unlikely(res)) {
		if (res == -EUCLEAN)
			mtd->ecc_stats.corrected++;
		if (res == -EBADMSG)
			mtd->ecc_stats.failed++;
	}   /*lint +e730*/
	return res;
}

static int part_read_user_prot_reg(struct mtd_info *mtd, loff_t from,
		size_t len, size_t *retlen, u_char *buf)
{
	struct mtd_part *part = PART(mtd);
	return part->master->read_user_prot_reg(part->master, from,
					len, retlen, buf);
}

static int part_get_user_prot_info(struct mtd_info *mtd,
		struct otp_info *buf, size_t len)
{
	struct mtd_part *part = PART(mtd);
	return part->master->get_user_prot_info(part->master, buf, len);
}

static int part_read_fact_prot_reg(struct mtd_info *mtd, loff_t from,
		size_t len, size_t *retlen, u_char *buf)
{
	struct mtd_part *part = PART(mtd);
	return part->master->read_fact_prot_reg(part->master, from,
					len, retlen, buf);
}

static int part_get_fact_prot_info(struct mtd_info *mtd, struct otp_info *buf,
		size_t len)
{
	struct mtd_part *part = PART(mtd);
	return part->master->get_fact_prot_info(part->master, buf, len);
}

static int part_write(struct mtd_info *mtd, loff_t to, size_t len,
		size_t *retlen, const u_char *buf)
{
	struct mtd_part *part = PART(mtd);
	if (!(mtd->flags & MTD_WRITEABLE))
		return -EROFS;
	if ((uint64_t)to >= mtd->size)
		len = 0;
	else if ((uint64_t)to + len > mtd->size)
		len = (size_t)(mtd->size - (uint64_t)to);
    to = find_correct_position(part, to);
	return part->master->write(part->master, (uint64_t)to + part->offset,
				    len, retlen, buf);
}

static int part_panic_write(struct mtd_info *mtd, loff_t to, size_t len,
		size_t *retlen, const u_char *buf)
{
	struct mtd_part *part = PART(mtd);
	if (!(mtd->flags & MTD_WRITEABLE))
		return -EROFS;
	if ((uint64_t)to >= mtd->size)
		len = 0;
	else if ((uint64_t)to + len > mtd->size)
		len = (size_t)(mtd->size - (uint64_t)to);
	return part->master->panic_write(part->master, (uint64_t)to + part->offset,
				    len, retlen, buf);
}

static int part_write_oob(struct mtd_info *mtd, loff_t to,
		struct mtd_oob_ops *ops)
{
	struct mtd_part *part = PART(mtd);

	if (!(mtd->flags & MTD_WRITEABLE))
		return -EROFS;

	if ((uint64_t)to >= mtd->size)
		return -EINVAL;
	if (ops->datbuf && (uint64_t)to + ops->len > mtd->size)
		return -EINVAL;
    to = find_correct_position(part, to);
	return part->master->write_oob(part->master, (uint64_t)to + part->offset, ops);
}

static int part_write_user_prot_reg(struct mtd_info *mtd, loff_t from,
		size_t len, size_t *retlen, u_char *buf)
{
	struct mtd_part *part = PART(mtd);
	return part->master->write_user_prot_reg(part->master, from,
					len, retlen, buf);
}

static int part_lock_user_prot_reg(struct mtd_info *mtd, loff_t from,
		size_t len)
{
	struct mtd_part *part = PART(mtd);
	return part->master->lock_user_prot_reg(part->master, from, len);
}

static int part_writev(struct mtd_info *mtd, const struct kvec *vecs,
		unsigned long count, loff_t to, size_t *retlen)
{
	struct mtd_part *part = PART(mtd);
	if (!(mtd->flags & MTD_WRITEABLE))
		return -EROFS;
	return part->master->writev(part->master, vecs, count,
					(uint64_t)to + part->offset, retlen);
}

static int part_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct mtd_part *part = PART(mtd);
	int ret;
	if (!(mtd->flags & MTD_WRITEABLE))
		return -EROFS;
	if (instr->addr >= mtd->size)
		return -EINVAL;
	instr->addr += part->offset;
	ret = part->master->erase(part->master, instr);
	if (ret) {
		if (instr->fail_addr != (uint64_t)MTD_FAIL_ADDR_UNKNOWN)
			instr->fail_addr -= part->offset;
		instr->addr -= part->offset;
	}
	return ret;
}

void mtd_erase_callback(struct erase_info *instr)
{
	if (instr->mtd->erase == part_erase) {
		struct mtd_part *part = PART(instr->mtd);

		if (instr->fail_addr != (uint64_t)MTD_FAIL_ADDR_UNKNOWN)
			instr->fail_addr -= part->offset;
		instr->addr -= part->offset;
	}
	if (instr->callback)
		instr->callback(instr);
}
EXPORT_SYMBOL_GPL(mtd_erase_callback);

static int part_lock(struct mtd_info *mtd, loff_t ofs, uint64_t len)
{
	struct mtd_part *part = PART(mtd);
	if ((len + (uint64_t)ofs) > mtd->size)
		return -EINVAL;
    ofs = find_correct_position(part, ofs);
	return part->master->lock(part->master, (uint64_t)ofs + part->offset, len);
}

static int part_unlock(struct mtd_info *mtd, loff_t ofs, uint64_t len)
{
	struct mtd_part *part = PART(mtd);
	if ((len + (uint64_t)ofs) > mtd->size)
		return -EINVAL;
    ofs = find_correct_position(part, ofs);

	return part->master->unlock(part->master, (uint64_t)ofs + part->offset, len);
}

static void part_sync(struct mtd_info *mtd)
{
	struct mtd_part *part = PART(mtd);
	part->master->sync(part->master);
}

static int part_suspend(struct mtd_info *mtd)
{
	struct mtd_part *part = PART(mtd);
	return part->master->suspend(part->master);
}

static void part_resume(struct mtd_info *mtd)
{
	struct mtd_part *part = PART(mtd);
	part->master->resume(part->master);
}

static int part_block_isbad(struct mtd_info *mtd, loff_t ofs)
{
	struct mtd_part *part = PART(mtd);
	if ((uint64_t)ofs >= mtd->size)
		return -EINVAL;
    ofs = (uint64_t)ofs + part->offset;
	return part->master->block_isbad(part->master, ofs);
}

static int part_block_markbad(struct mtd_info *mtd, loff_t ofs)
{
	struct mtd_part *part = PART(mtd);
	int res;

	if (!(mtd->flags & MTD_WRITEABLE))
		return -EROFS;
	if ((uint64_t)ofs >= mtd->size)
		return -EINVAL;
	ofs = (uint64_t)ofs + part->offset;
	res = part->master->block_markbad(part->master, ofs);
	if (!res)
		mtd->ecc_stats.badblocks++;
	return res;
}

/* <BB5D01328.FLASH: yangjianping 2009-6-2 ADD BEGIN */
int fixed_fs_partitions(struct mtd_info *master, struct mtd_part *part)
{
    unsigned int pos = 0;

    if( (NULL == part)  || (NULL == part->mtd.name))
    {
        printk(KERN_EMERG "fixed_mtd_partitions , mtd info is not exist!\n");
        return -1;
    }

    printk("Test --fixed_fs_partitions Partition \n");

    /*For raw and  fs partiton, find the first good block  */
    if( (!strcmp( part->mtd.name, MMC_PARTITION_NAME))
        ||(!strcmp( part->mtd.name, MCORE_PARTITION_NAME))
        ||(!strcmp( part->mtd.name, SYSTEM_PARTITION_NAME))
        ||(!strcmp( part->mtd.name, APP_PARTITION_NAME))
        ||(!strcmp( part->mtd.name, OEM_PARTITION_NAME))
        ||(!strcmp( part->mtd.name, DATA_PARTITION_NAME))
        ||(!strcmp( part->mtd.name, ONLINE_PARTITION_NAME))
        ||(!strcmp( part->mtd.name, WEBUI_PARTITION_NAME)))
    {
      printk("Test --fixed_fs_partitions Partition entry\n");
        
        while(pos < (MAX_TRY_BLKS * part->mtd.erasesize))
        {
            if(((pos % part->mtd.erasesize) == 0) 
            && (master->block_isbad(master, pos + part->offset)))
            { 
                pos += part->mtd.erasesize;
                continue;
            }

            part->offset =  part->offset + pos;
            part->mtd.size = part->mtd.size - pos;
            printk(KERN_INFO "Partition %s position fixed, Offset:%llx Size:0x%llx\n",
            part->mtd.name, part->offset, part->mtd.size);
            printk("liufang --Partition %s position fixed, Offset:%llx Size:0x%llx\n",
            part->mtd.name, part->offset, part->mtd.size);
            return 0;
        }
    }
    return -1;
}
/* BB5D01328.FLASH: yangjianping 2009-6-2 ADD END> */


void part_fill_badblockstats(struct mtd_info *mtd)
{
	struct mtd_part *part = PART(mtd);
	if (part->master->block_isbad) {
    uint32_t offs = 0;

    fixed_fs_partitions(part->master, part);
		mtd->ecc_stats.badblocks = 0;
		while (offs < mtd->size) {
			if (part->master->block_isbad(part->master,
						offs + part->offset))
        {
            if(part->mtd.ecc_stats.badblocks < MAX_TRY_BLKS)
            {
                part->mtd.ecc_stats.bbtarr[part->mtd.ecc_stats.badblocks] 
                = offs / part->mtd.erasesize;
            }
            else
            {
                printk(KERN_EMERG "badblock is over %d,fs can not recovery!\n!",
                MAX_TRY_BLKS);
            }
                printk(KERN_EMERG "Current badblocks %4d, offset=0x%4x summay count=%d\n",
                offs / part->mtd.erasesize + 1,
                part->offset,
                part->mtd.ecc_stats.badblocks);

                part->mtd.ecc_stats.badblocks++;
		  }

    offs += mtd->erasesize;
        }
    }
}
/*
 * This function unregisters and destroy all slave MTD objects which are
 * attached to the given master MTD object.
 */
/*lint -e613 */
int del_mtd_partitions(struct mtd_info *master)
{
    /*解决slave 空指针问题*/
	struct mtd_part *slave = NULL, *next = NULL;

	/*list_for_each_entry_safe是linux内核代码，空指针问题暂且不改*/
	list_for_each_entry_safe(slave, next, &mtd_partitions, list)
		if (NULL != slave) {
			if (slave->master == master) {
			   list_del(&slave->list);
			   del_mtd_device(&slave->mtd);
			   kfree(slave);
		    }
		}
	
	return 0;
}
/*lint +e613 */
EXPORT_SYMBOL(del_mtd_partitions);

static struct mtd_part *add_one_partition(struct mtd_info *master,
		const struct mtd_partition *part, int partno,
		uint64_t cur_offset)
{
	struct mtd_part *slave;

	/* allocate the partition structure */
	slave = kzalloc(sizeof(*slave), GFP_KERNEL);
	if (!slave) {
		printk(KERN_ERR"memory allocation error while creating partitions for \"%s\"\n",
			master->name);
		del_mtd_partitions(master);
		return NULL;
	}
	list_add(&slave->list, &mtd_partitions);

	/* set up the MTD object for this partition */
	slave->mtd.type = master->type;
	slave->mtd.flags = master->flags & ~part->mask_flags;
	slave->mtd.size = part->size;
	slave->mtd.writesize = master->writesize;
	slave->mtd.oobsize = master->oobsize;
	slave->mtd.oobavail = master->oobavail;
	slave->mtd.subpage_sft = master->subpage_sft;

	slave->mtd.name = part->name;
	slave->mtd.owner = master->owner;
	slave->mtd.backing_dev_info = master->backing_dev_info;

	/* NOTE:  we don't arrange MTDs as a tree; it'd be error-prone
	 * to have the same data be in two different partitions.
	 */
	slave->mtd.dev.parent = master->dev.parent;

	slave->mtd.read = part_read;
	slave->mtd.write = part_write;

	if (master->panic_write)
		slave->mtd.panic_write = part_panic_write;

	if (master->point && master->unpoint) {
		slave->mtd.point = part_point;
		slave->mtd.unpoint = part_unpoint;
	}

	if (master->get_unmapped_area)
		slave->mtd.get_unmapped_area = part_get_unmapped_area;
	if (master->read_oob)
		slave->mtd.read_oob = part_read_oob;
	if (master->write_oob)
		slave->mtd.write_oob = part_write_oob;
	if (master->read_user_prot_reg)
		slave->mtd.read_user_prot_reg = part_read_user_prot_reg;
	if (master->read_fact_prot_reg)
		slave->mtd.read_fact_prot_reg = part_read_fact_prot_reg;
	if (master->write_user_prot_reg)
		slave->mtd.write_user_prot_reg = part_write_user_prot_reg;
	if (master->lock_user_prot_reg)
		slave->mtd.lock_user_prot_reg = part_lock_user_prot_reg;
	if (master->get_user_prot_info)
		slave->mtd.get_user_prot_info = part_get_user_prot_info;
	if (master->get_fact_prot_info)
		slave->mtd.get_fact_prot_info = part_get_fact_prot_info;
	if (master->sync)
		slave->mtd.sync = part_sync;
	if (!partno && !master->dev.class && master->suspend && master->resume) {
			slave->mtd.suspend = part_suspend;
			slave->mtd.resume = part_resume;
	}
	if (master->writev)
		slave->mtd.writev = part_writev;
	if (master->lock)
		slave->mtd.lock = part_lock;
	if (master->unlock)
		slave->mtd.unlock = part_unlock;
	if (master->block_isbad)
		slave->mtd.block_isbad = part_block_isbad;
	if (master->block_markbad)
		slave->mtd.block_markbad = part_block_markbad;
	slave->mtd.erase = part_erase;
	slave->master = master;
	slave->offset = part->offset;

	if (slave->offset == (uint64_t)MTDPART_OFS_APPEND)
		slave->offset = cur_offset;
	if (slave->offset == (uint64_t)MTDPART_OFS_NXTBLK) {
		slave->offset = cur_offset;
		if (mtd_mod_by_eb(cur_offset, master) != 0) {
			/* Round up to next erasesize */
			slave->offset = (mtd_div_by_eb(cur_offset, master) + 1) * master->erasesize;/*lint !e647*/
			printk(KERN_NOTICE "Moving partition %d: "
			       "0x%012llx -> 0x%012llx\n", partno,
			       (unsigned long long)cur_offset, (unsigned long long)slave->offset);
		}
	}
	if (slave->mtd.size == MTDPART_SIZ_FULL)
		slave->mtd.size = master->size - slave->offset;

	printk(KERN_NOTICE "0x%012llx-0x%012llx : \"%s\"\n", (unsigned long long)slave->offset,
		(unsigned long long)(slave->offset + slave->mtd.size), slave->mtd.name);

	/* let's do some sanity checks */
	if (slave->offset >= master->size) {
		/* let's register it anyway to preserve ordering */
		slave->offset = 0;
		slave->mtd.size = 0;
		printk(KERN_ERR"mtd: partition \"%s\" is out of reach -- disabled\n",
			part->name);
		goto out_register;
	}
	if (slave->offset + slave->mtd.size > master->size) {
		slave->mtd.size = master->size - slave->offset;
		printk(KERN_WARNING"mtd: partition \"%s\" extends beyond the end of device \"%s\" -- size truncated to %#llx\n",
			part->name, master->name, (unsigned long long)slave->mtd.size);
	}
	if (master->numeraseregions > 1) {
		/* Deal with variable erase size stuff */
		int i, max = master->numeraseregions;   /*lint !e123 */
		u64 end = slave->offset + slave->mtd.size;
		struct mtd_erase_region_info *regions = master->eraseregions;

		/* Find the first erase regions which is part of this
		 * partition. */
		for (i = 0; i < max && regions[i].offset <= slave->offset; i++) /*lint !e123 */
			;
		/* The loop searched for the region _behind_ the first one */
		if (i > 0)
			i--;

		/* Pick biggest erasesize */
		for (; i < max && regions[i].offset < end; i++) {/*lint !e123 */
			if (slave->mtd.erasesize < regions[i].erasesize) {
				slave->mtd.erasesize = regions[i].erasesize;
			}
		}
		BUG_ON(slave->mtd.erasesize == 0);   /*lint !e730*/
	} else {
		/* Single erase size */
		slave->mtd.erasesize = master->erasesize;
	}

	if ((slave->mtd.flags & MTD_WRITEABLE) &&
	    mtd_mod_by_eb(slave->offset, &slave->mtd)) {
		/* Doesn't start on a boundary of major erase size */
		/* FIXME: Let it be writable if it is on a boundary of
		 * _minor_ erase size though */
		slave->mtd.flags &= ~MTD_WRITEABLE;
		printk(KERN_WARNING"mtd: partition \"%s\" doesn't start on an erase block boundary -- force read-only\n",
			part->name);
	}
	if ((slave->mtd.flags & MTD_WRITEABLE) &&
	    mtd_mod_by_eb(slave->mtd.size, &slave->mtd)) {
		slave->mtd.flags &= ~MTD_WRITEABLE;
		printk(KERN_WARNING"mtd: partition \"%s\" doesn't end on an erase block -- force read-only\n",
			part->name);
	}

	slave->mtd.ecclayout = master->ecclayout;
	if (master->block_isbad) {

#if (defined(BOARD_ASIC_BIGPACK) && defined(VERSION_V3R2))
	    /*don't do bad block check*/
#else
		uint64_t offs = 0;

		while (offs < slave->mtd.size) {
			if (master->block_isbad(master,
						offs + slave->offset))
				slave->mtd.ecc_stats.badblocks++;
			offs += slave->mtd.erasesize;
		}
#endif		
	}
    part_fill_badblockstats(&slave->mtd);


out_register:
	/* register our partition */
	add_mtd_device(&slave->mtd);

	return slave;
}

/*
 * This function, given a master MTD object and a partition table, creates
 * and registers slave MTD objects which are bound to the master according to
 * the partition definitions.
 *
 * We don't register the master, or expect the caller to have done so,
 * for reasons of data integrity.
 */

int add_mtd_partitions(struct mtd_info *master,
		       const struct mtd_partition *parts,
		       int nbparts)
{
	struct mtd_part *slave;
	uint64_t cur_offset = 0;
	int i;

	printk(KERN_NOTICE "Creating %d MTD partitions on \"%s\":\n", nbparts, master->name);

	for (i = 0; i < nbparts; i++) {
		slave = add_one_partition(master, parts + i, i, cur_offset);
		if (!slave)
			return -ENOMEM;
		cur_offset = slave->offset + slave->mtd.size;
	}

	return 0;
}
EXPORT_SYMBOL(add_mtd_partitions);

static DEFINE_SPINLOCK(part_parser_lock); /*lint !e43*/
static LIST_HEAD(part_parsers);/*lint !e129*/

static struct mtd_part_parser *get_partition_parser(const char *name)/*lint !e129*/
{   /*lint !e745 */
    /*解决p 空指针问题*/
	struct mtd_part_parser *p = NULL, *ret = NULL;

	spin_lock(&part_parser_lock);

	/*list_for_each_entry是linux内核代码，空指针问题暂且不改*/
	/*lint -e613 -e746*/
	list_for_each_entry(p, &part_parsers, list)
		if (NULL != p) {
			if (!strcmp(p->name, name) && try_module_get(p->owner)) {
			   ret = p;
			   break;
		    }
		}

	spin_unlock(&part_parser_lock);
	/*lint +e613 +e746*/
	
	return ret;/*lint !e64*/
}

int register_mtd_parser(struct mtd_part_parser *p)
{
	spin_lock(&part_parser_lock);
	list_add(&p->list, &part_parsers);
	spin_unlock(&part_parser_lock);

	return 0;
}
EXPORT_SYMBOL_GPL(register_mtd_parser);

int deregister_mtd_parser(struct mtd_part_parser *p)
{
	spin_lock(&part_parser_lock);
	list_del(&p->list);
	spin_unlock(&part_parser_lock);
	return 0;
}
EXPORT_SYMBOL_GPL(deregister_mtd_parser);

int parse_mtd_partitions(struct mtd_info *master, const char **types,
			 struct mtd_partition **pparts, unsigned long origin)
{
	struct mtd_part_parser *parser;
	int ret = 0;

	for ( ; ret <= 0 && *types; types++) {
		parser = get_partition_parser(*types);/*lint !e64*/
		if (!parser && !request_module("%s", *types))/*lint !e747*/
				parser = get_partition_parser(*types);/*lint !e64*/
		if (!parser) {
			printk(KERN_NOTICE "%s partition parsing not available\n",
			       *types);
			continue;
		}
		ret = (*parser->parse_fn)(master, pparts, origin);
		if (ret > 0) {
			printk(KERN_NOTICE "%d %s partitions found on MTD device %s\n",
			       ret, parser->name, master->name);
		}
		put_partition_parser(parser);
	}
	return ret;
}
EXPORT_SYMBOL_GPL(parse_mtd_partitions);
