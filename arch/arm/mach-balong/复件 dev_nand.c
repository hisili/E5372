/*
 * S3C series device definition for nand device
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/gfp.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <mach/balong_v100r001.h>

#include <mach/map.h>
#include <mach/devs.h>
#include <mach/nand.h>

static struct resource pv500v1_nand_resource[] = {
	[0] = {
		.start = PV500V1_NANDC_BASE,
		.end   = PV500V1_NANDC_BASE + PV500V1_NANDC_REGS_SIZE -1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = NANDF_BUFFER_ADDR,
		.end   = NANDF_BUFFER_ADDR + NANDF_BUFFER_SIZE -1,
		.flags = IORESOURCE_MEM,
	}
};

struct platform_device pv500v1_device_nand = {
	.name		  = "pv500v1_nand",
	.id		  = -1,
	.num_resources	  = ARRAY_SIZE(pv500v1_nand_resource),
	.resource	  = pv500v1_nand_resource,
};

EXPORT_SYMBOL(pv500v1_device_nand);


/* NAND parititon from 2.4.18-swl5 */

static struct mtd_partition balong_default_nand_part[] = {
	[0] = {
		.name	= "bootsec",
		.size	= SZ_2K,
		.offset	= 0,
	},
	[1] = {
		.name	= "uboot",
		.size	= SZ_1M,
		.offset	= SZ_2K,
	},
	[2] = {
		.name	= "kernel",
		.size	= SZ_2M,
		.offset	= SZ_2K + SZ_1M,
	},
	[3] = {
		.name	= "rootfs",
		.size	= MTDPART_SIZ_FULL,
		.offset	= SZ_2K + SZ_1M + SZ_2M,
	},
};

static struct balongv7r1_nand_set balong_nand_sets[] = {
	[0] = {
		.name		= "NAND",
		.nr_chips	= 1,
		.nr_partitions	= ARRAY_SIZE(balong_default_nand_part),
		.partitions	= balong_default_nand_part,
	},
};

/* choose a set of timings which should suit most 512Mbit
 * chips and beyond.
*/

struct balongv7r1_platform_nand smdk_nand_info = {
	.tacls		= 20,
	.twrph0		= 60,
	.twrph1		= 20,
	.nr_sets	= ARRAY_SIZE(balong_nand_sets),
	.sets		= balong_nand_sets,
};

/**
 * s3c_nand_copy_set() - copy nand set data
 * @set: The new structure, directly copied from the old.
 *
 * Copy all the fields from the NAND set field from what is probably __initdata
 * to new kernel memory. The code returns 0 if the copy happened correctly or
 * an error code for the calling function to display.
 *
 * Note, we currently do not try and look to see if we've already copied the
 * data in a previous set.
 */
static int __init balong_nand_copy_set(struct balongv7r1_nand_set *set)
{
	void *ptr;
	int size;

	size = sizeof(struct mtd_partition) * set->nr_partitions;
	if (size) {
		ptr = kmemdup(set->partitions, size, GFP_KERNEL);
		set->partitions = ptr;

		if (!ptr)
			return -ENOMEM;
	}
	
	if (set->nr_map && set->nr_chips) {
		size = sizeof(int) * set->nr_chips;
		ptr = kmemdup(set->nr_map, size, GFP_KERNEL);
		set->nr_map = ptr;

		if (!ptr)
			return -ENOMEM;
	}

	if (set->ecc_layout) {
		ptr = kmemdup(set->ecc_layout,
			      sizeof(struct nand_ecclayout), GFP_KERNEL);
		set->ecc_layout = ptr;

		if (!ptr)
			return -ENOMEM;
	}
	
	return 0;
}

void __init balong_nand_set_platdata(struct balongv7r1_platform_nand *nand)
{
	struct balongv7r1_platform_nand *npd;
	int size;
	int ret;

	/* note, if we get a failure in allocation, we simply drop out of the
	 * function. If there is so little memory available at initialisation
	 * time then there is little chance the system is going to run.
	 */ 

	npd = kmemdup(nand, sizeof(struct balongv7r1_platform_nand), GFP_KERNEL);
	if (!npd) {
		printk(KERN_ERR "%s: failed copying platform data\n", __func__);
		return;
	}

	/* now see if we need to copy any of the nand set data */

	size = sizeof(struct balongv7r1_nand_set) * npd->nr_sets;
	if (size) {
		struct balongv7r1_nand_set *from = npd->sets;
		struct balongv7r1_nand_set *to;
		int i;

		to = kmemdup(from, size, GFP_KERNEL);
		npd->sets = to;	/* set, even if we failed */

		if (!to) {
			printk(KERN_ERR "%s: no memory for sets\n", __func__);
			return;
		}
		
		for (i = 0; i < npd->nr_sets; i++) {
			ret = balong_nand_copy_set(to);
			if (ret) {
				printk(KERN_ERR "%s: failed to copy set %d\n",
				__func__, i);
				return;
			}
			to++;
		}
	}

	pv500v1_device_nand.dev.platform_data = npd;
}
