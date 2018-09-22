obj-y                   := lm.o
obj-$(CONFIG_BALONG_EASY_SHELL)                 += easy_shell.o
#obj-$(CONFIG_BALONG_OM)                         += BSP_OM.o
obj-y                         += bsp_hw_adapt.o
obj-y                         += BSP_OM.o
obj-$(CONFIG_BALONG_MEMORY_SPINLOCK)                   += spinLockAmp.o
  
#obj-$(CONFIG_MACH_BOARD_FPGA)         += balong_core_p500bbit.o balong_p500bbit_v100r001.o BSP_VIC.o
#obj-$(CONFIG_MACH_BOARD_SFT)     += balong_core_v3r2sft.o balong_v3r2sft_v100r001.o 

ifeq ($(BOARD_TYPE), BOARD_SFT)
ifeq ($(VERSION_TYPE), VERSION_V7R1)
obj-y     += balong_core_v7r1sft.o balong_v7r1sft.o BSP_I2C.o
obj-y     +=pwrctrl/
endif

ifeq ($(VERSION_TYPE), VERSION_V3R2)
obj-y     += balong_core_v3r2sft.o balong_v3r2sft_v100r001.o  
obj-y     +=pwrctrl/
endif
endif

ifeq ($(BOARD_TYPE), BOARD_FPGA)
ifeq ($(VERSION_TYPE), VERSION_V3R2)
obj-y     += balong_core_p500bbit.o balong_p500bbit_v100r001.o BSP_VIC.o
obj-y     +=pwrctrl/
endif
endif

ifeq ($(BOARD_TYPE), BOARD_FPGA_P500)
ifeq ($(VERSION_TYPE), CHIP_BB_6920ES)
obj-y     += balong_core_p500fpga.o balong_p500fpga.o BSP_VIC.o bsp_guipc.o bsp_guvic.o
#obj-y     +=pwrctrl/
obj-y     +=product_info/
endif
endif

ifeq ($(BOARD_TYPE), BOARD_FPGA_P500)
ifeq ($(VERSION_TYPE), CHIP_BB_6920CS)
obj-y     += balong_core_hi6920csp500.o balong_hi6920csp500.o BSP_VIC.o bsp_guipc.o bsp_guvic.o
#obj-y     +=pwrctrl/
obj-y     +=product_info/
endif
endif

ifeq ($(BOARD_TYPE), BOARD_ASIC_BIGPACK)
ifeq ($(VERSION_TYPE), VERSION_V3R2)
#pandong cs
ifeq ($(CHIP_TYPE), CHIP_BB_6756CS)
obj-y     += balong_core_v3r2cs.o balong_v3r2cs.o 
obj-y     += pwrctrl/
else
obj-y     += balong_core_v3r2asic.o balong_v3r2asic.o BSP_I2C.o
obj-y     +=pwrctrl/
endif
endif
endif

ifeq ($(BOARD_TYPE), BOARD_ASIC)
ifeq ($(VERSION_TYPE), CHIP_BB_6920ES)
obj-y     += balong_core_v7r1asic.o balong_v7r1asic.o BSP_I2C.o
obj-y     +=pwrctrl/
obj-y     +=product_info/
obj-y 	  += mmi.o
endif
endif

ifeq ($(BOARD_TYPE), BOARD_SFT)
ifeq ($(VERSION_TYPE), CHIP_BB_6920CS)
obj-y     += balong_core_hi6920cs_sft.o balong_hi6920cs_sft.o BSP_I2C.o
obj-y     +=pwrctrl/
obj-y     +=product_info/
obj-y 	  += mmi.o
endif
endif

ifeq ($(BOARD_TYPE), BOARD_ASIC)
ifeq ($(VERSION_TYPE), CHIP_BB_6920CS)
obj-y     += balong_core_v7r1asic.o balong_v7r1asic.o BSP_I2C.o
obj-y     +=pwrctrl/
obj-y     +=product_info/
obj-y 	  += mmi.o
endif
endif

obj-y += power_exchange.o
ifeq ($(VERSION_TYPE), VERSION_V3R2)
obj-y += mmi.o
endif
