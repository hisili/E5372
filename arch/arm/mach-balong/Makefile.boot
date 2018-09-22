-include $(srctree)/include/generated/balong_product_config.mk

# 新编译工具使用
ifdef CFG_BALONG_OBUILD_VERSION
zreladdr-y	:= $(ACORE_BOOTLOADER_REL)
params_phys-y	:= $(ACORE_BOOTLOADER_ARGS)
initrd_phys-y	:= $(ACORE_START_ADDR)

# 老编译工具使用，要删除
else

ifeq ($(BOARD_TYPE), BOARD_SFT)
ifeq ($(VERSION_TYPE), VERSION_V7R1)
zreladdr-y	:= 0x34408000
params_phys-y	:= 0x34400100
initrd_phys-y	:= 0x34c00000
endif
ifeq ($(VERSION_TYPE), VERSION_V3R2)
zreladdr-y	:= 0x34408000
params_phys-y	:= 0x34400100
initrd_phys-y	:= 0x34c00000
endif
endif

ifeq ($(BOARD_TYPE), BOARD_FPGA)
ifeq ($(VERSION_TYPE), VERSION_V3R2)
zreladdr-y	:= 0xc4408000
params_phys-y	:= 0xc4400100
initrd_phys-y	:= 0xc4c00000
endif
endif

ifeq ($(BOARD_TYPE), BOARD_ASIC_BIGPACK)
ifeq ($(VERSION_TYPE), VERSION_V3R2)
ifeq ($(MEMSIZE), 64M)
zreladdr-y	:= 0x31A08000
params_phys-y	:= 0x31A00100
initrd_phys-y	:= 0x32400000
else
zreladdr-y	:= 0x34408000
params_phys-y	:= 0x34400100
initrd_phys-y	:= 0x34c00000
endif
endif
endif

ifeq ($(BOARD_TYPE), BOARD_ASIC)
ifeq ($(VERSION_TYPE), CHIP_BB_6920ES)
zreladdr-y	:= 0x33908000
params_phys-y	:= 0x33900100
initrd_phys-y	:= 0x33900000
endif
endif

ifeq ($(BOARD_TYPE), BOARD_ASIC)
ifeq ($(VERSION_TYPE), CHIP_BB_6920CS)
zreladdr-y	:= $(ACORE_BOOTLOADER_REL)
params_phys-y	:= $(ACORE_BOOTLOADER_ARGS)
initrd_phys-y	:= $(ACORE_START_ADDR)
endif
endif

ifeq ($(BOARD_TYPE), BOARD_SFT)
ifeq ($(VERSION_TYPE), CHIP_BB_6920ES)
zreladdr-y	:= 0x33908000
params_phys-y	:= 0x33900100
initrd_phys-y	:= 0x33900000
endif
endif

ifeq ($(BOARD_TYPE), BOARD_SFT)
ifeq ($(VERSION_TYPE), CHIP_BB_6920CS)
zreladdr-y	:= 0x35208000
params_phys-y	:= 0x35200100
initrd_phys-y	:= 0x35200000
endif
endif

ifeq ($(BOARD_TYPE), BOARD_FPGA_P500)
ifeq ($(VERSION_TYPE), CHIP_BB_6920ES)
zreladdr-y	:= 0xc3908000
params_phys-y	:= 0xc3900100
initrd_phys-y	:= 0xc3900000
endif
endif

ifeq ($(BOARD_TYPE), BOARD_FPGA_P500)
ifeq ($(VERSION_TYPE), CHIP_BB_6920CS)
zreladdr-y	:= 0xc5208000
params_phys-y	:= 0xc5200100
initrd_phys-y	:= 0xc5200000
endif
endif

endif