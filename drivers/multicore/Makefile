ifneq ($(PRODUCT_CFG_BUILD_TYPE),SEPARATE)
ifeq ($(VERSION_TYPE), CHIP_BB_6920ES)
obj-$(CONFIG_BALONG_MULTICORE)		+= nv_descend/LMSPNV.o 
endif

ifeq ($(VERSION_TYPE), CHIP_BB_6920CS)
obj-$(CONFIG_BALONG_MULTICORE)		+= nv_descend/LMSPNV.o 
endif


endif

ifeq ($(VERSION_TYPE), VERSION_V3R2)
obj-$(CONFIG_BALONG_MULTICORE)		+= nv_descend/LMSPNV.o 
endif

obj-$(CONFIG_BALONG_MULTICORE)	+= multi_core.o

obj-y		+= icc/
obj-y		+= ifc/
obj-y		+= udi/
obj-y		+= ipcm/
#obj-y		+= sync/
obj-y		+= common/
obj-$(CONFIG_BALONG_MULTICORE_TEST)           += test/
