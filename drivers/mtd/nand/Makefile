#
# linux/drivers/nand/Makefile
#

obj-$(CONFIG_MTD_NAND)			+= nand.o
obj-$(CONFIG_MTD_NAND_ECC)		+= nand_ecc.o
#obj-$(CONFIG_MTD_NAND_IDS)		+= nand_ids.o
obj-$(CONFIG_MTD_SM_COMMON) 		+= sm_common.o

obj-$(CONFIG_MTD_NAND_CAFE)		+= cafe_nand.o
obj-$(CONFIG_MTD_NAND_SPIA)		+= spia.o
obj-$(CONFIG_MTD_NAND_AMS_DELTA)	+= ams-delta.o
obj-$(CONFIG_MTD_NAND_AUTCPU12)		+= autcpu12.o
obj-$(CONFIG_MTD_NAND_DENALI)		+= denali.o
obj-$(CONFIG_MTD_NAND_EDB7312)		+= edb7312.o
obj-$(CONFIG_MTD_NAND_AU1550)		+= au1550nd.o
obj-$(CONFIG_MTD_NAND_BF5XX)		+= bf5xx_nand.o
obj-$(CONFIG_MTD_NAND_PPCHAMELEONEVB)	+= ppchameleonevb.o
obj-$(CONFIG_MTD_NAND_S3C2410)		+= s3c2410.o
obj-$(CONFIG_MTD_NAND_DAVINCI)		+= davinci_nand.o
obj-$(CONFIG_MTD_NAND_DISKONCHIP)	+= diskonchip.o
obj-$(CONFIG_MTD_NAND_H1900)		+= h1910.o
obj-$(CONFIG_MTD_NAND_RTC_FROM4)	+= rtc_from4.o
obj-$(CONFIG_MTD_NAND_SHARPSL)		+= sharpsl.o
obj-$(CONFIG_MTD_NAND_NANDSIM)		+= nandsim.o
obj-$(CONFIG_MTD_NAND_CS553X)		+= cs553x_nand.o
obj-$(CONFIG_MTD_NAND_NDFC)		+= ndfc.o
obj-$(CONFIG_MTD_NAND_ATMEL)		+= atmel_nand.o
obj-$(CONFIG_MTD_NAND_GPIO)		+= gpio.o
obj-$(CONFIG_MTD_NAND_OMAP2) 		+= omap2.o
obj-$(CONFIG_MTD_NAND_CM_X270)		+= cmx270_nand.o
obj-$(CONFIG_MTD_NAND_PXA3xx)		+= pxa3xx_nand.o
obj-$(CONFIG_MTD_NAND_TMIO)		+= tmio_nand.o
obj-$(CONFIG_MTD_NAND_PLATFORM)		+= plat_nand.o
obj-$(CONFIG_MTD_ALAUDA)		+= alauda.o
obj-$(CONFIG_MTD_NAND_PASEMI)		+= pasemi_nand.o
obj-$(CONFIG_MTD_NAND_ORION)		+= orion_nand.o
obj-$(CONFIG_MTD_NAND_FSL_ELBC)		+= fsl_elbc_nand.o
obj-$(CONFIG_MTD_NAND_FSL_UPM)		+= fsl_upm.o
obj-$(CONFIG_MTD_NAND_SH_FLCTL)		+= sh_flctl.o
obj-$(CONFIG_MTD_NAND_MXC)		+= mxc_nand.o
obj-$(CONFIG_MTD_NAND_SOCRATES)		+= socrates_nand.o
obj-$(CONFIG_MTD_NAND_TXX9NDFMC)	+= txx9ndfmc.o
obj-$(CONFIG_MTD_NAND_NUC900)		+= nuc900_nand.o
obj-$(CONFIG_MTD_NAND_NOMADIK)		+= nomadik_nand.o
obj-$(CONFIG_MTD_NAND_BCM_UMI)		+= bcm_umi_nand.o nand_bcm_umi.o
obj-$(CONFIG_MTD_NAND_MPC5121_NFC)	+= mpc5121_nfc.o
obj-$(CONFIG_MTD_NAND_RICOH)		+= r852.o
ifeq ($(VERSION_TYPE),VERSION_V3R2)
obj-$(CONFIG_MTD_NAND_BALONG_NANDC)     += nandc/nandc_mtd.o nandc/nandc_port.o nandc/nandc_v400.o nandc/nandc_nand.o \
                                           nandc/nandc_native.o nandc/nandc_cfg.o nandc/nandc_ctrl.o nandc/nandc_data.o \
					   nandc/nandc_host.o  nandc/nand_ids.o  nandc/nandc_v200.o  nandc/nandc_v320.o \
					   nandc/nandc_balong.o
obj-$(CONFIG_MTD_NAND_BALONG_NANDC)     += ptable/ptable_common.o ptable/ptable_product.o

nand-objs := nand_base.o nand_bbt.o

KBUILD_CPPFLAGS +=	-Idrivers/mtd/nand/ptable \
			-Idrivers/mtd/nand/nandc  \
			-Iarch/arm/mach-balong/include/mach/common \
			-Iarch/arm/mach-balong/include/mach
endif

ifeq ($(VERSION_TYPE),CHIP_BB_6920ES)
obj-$(CONFIG_MTD_NAND_BALONG_NANDC)     += nandc/nandc_mtd.o nandc/nandc_port.o nandc/nandc_v400.o nandc/nandc_nand.o \
                                           nandc/nandc_native.o nandc/nandc_cfg.o nandc/nandc_ctrl.o nandc/nandc_data.o \
					   nandc/nandc_host.o  nandc/nand_ids.o  nandc/nandc_v200.o  nandc/nandc_v320.o \
					   nandc/nandc_balong.o

obj-$(CONFIG_MTD_NAND_BALONG_NANDC)     += ptable/ptable_common.o ptable/ptable_product.o

nand-objs := nand_base.o nand_bbt.o

KBUILD_CPPFLAGS +=	-Idrivers/mtd/nand/ptable \
			-Idrivers/mtd/nand/nandc  \
			-Iarch/arm/mach-balong/include/mach/common \
			-Iarch/arm/mach-balong/include/mach
endif

ifeq ($(VERSION_TYPE),CHIP_BB_6920CS)
obj-$(CONFIG_MTD_NAND_BALONG_NANDC)     += nandc/nandc_mtd.o nandc/nandc_port.o nandc/nandc_v400.o nandc/nandc_nand.o \
                                           nandc/nandc_native.o nandc/nandc_cfg.o nandc/nandc_ctrl.o nandc/nandc_data.o \
					   nandc/nandc_host.o  nandc/nand_ids.o  nandc/nandc_v200.o  nandc/nandc_v320.o \
					   nandc/nandc_balong.o

obj-$(CONFIG_MTD_NAND_BALONG_NANDC)     += ptable/ptable_common.o ptable/ptable_product.o

nand-objs := nand_base.o nand_bbt.o

KBUILD_CPPFLAGS +=	-Idrivers/mtd/nand/ptable \
			-Idrivers/mtd/nand/nandc  \
			-Iarch/arm/mach-balong/include/mach/common \
			-Iarch/arm/mach-balong/include/mach
endif

			