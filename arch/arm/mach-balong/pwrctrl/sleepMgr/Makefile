obj-y		:= BSP_PWC_SLEEP.o BSP_PWC_SLEEPMGR.o

ifeq ($(VERSION_TYPE), VERSION_V3R2)
obj-y     +=v3r2/
endif

ifeq ($(VERSION_TYPE), CHIP_BB_6920ES)
obj-y     +=v7r1/
endif
ifeq ($(VERSION_TYPE), CHIP_BB_6920CS)
obj-y     +=v7r1/
endif

obj-y		+=  BSP_PWC_SLEEPMNTN.o BSP_PWC_SLEEPTST.o
