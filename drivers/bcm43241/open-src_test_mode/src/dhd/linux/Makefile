
# GNU Makefile for Broadcom Dongle Host Driver
#
# Copyright (C) 2012, Broadcom Corporation. All Rights Reserved.
# 
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
# 
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
# SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
# OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
# CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
# $Id: Makefile 343301 2012-07-06 13:07:32Z $
#

# Standard preamble work.
WLAN_ComponentsInUse += bcmwifi
include $(dir $(lastword $(MAKEFILE_LIST)))/../../makefiles/WLAN_Common.mk

# Root name for object dir - could be just a prefix, could be a directory.
ObjPfx := dhd-

# This should be the standard but it seems to break something
# in the android/panda build so turned off for now.
#SHELL := /bin/bash

# Although 32ON64 flag is supposed to be passed from caller, set it
# if not set by caller on certain kernels and platforms
ifeq ($(shell uname -m),x86_64)
     ifneq ($(findstring x86_64,$(LINUXVER)),x86_64)
          export 32ON64=1
          # $(warning Passing 32ON64 flag to force 32bit driver for $(LINUXVER))
     endif # LINUXVER
endif # CURPROC

# Try a couple of places for LINUXDIR if not specified
ifeq ($(LINUXDIR),)
ifeq ($(LINUXVER),)
# Neither one is specified, use uname for version
LINUXVER := $(shell uname -r)
endif
ifneq ($(wildcard /lib/modules/$(LINUXVER)/build/include/linux/version.h),)
LINUXDIR := /lib/modules/$(LINUXVER)/build
else
ifneq ($(wildcard /tools/linux/src/linux-$(LINUXVER)/include/linux/version.h),)
LINUXDIR := /tools/linux/src/linux-$(LINUXVER)
else
LINUXDIR := /usr/src/linux
endif
endif
endif

# Derive LINUXVER from LINUXDIR
MYKERNEL_RELEASE_KEYWORD:="KERNELRELEASE[[:space:]]*=.*kernel.release"
MYKERNEL_DEFINITION:=$(if \
  $(shell grep $(MYKERNEL_RELEASE_KEYWORD) $(LINUXDIR)/Makefile 2> /dev/null),\
  grep $(MYKERNEL_RELEASE_KEYWORD) $(LINUXDIR)/Makefile,\
  cat $(LINUXDIR)/Makefile)

# No need to require a kernel version when cleaning.
ifeq (,$(findstring clean,$(MAKECMDGOALS)))
LINUXVER:=$(shell ($(MYKERNEL_DEFINITION); echo "show_kernel_version_number$$$$:;@echo \$$(KERNELRELEASE)") 2> /dev/null | $(MAKE) --no-print-directory -k -C $(LINUXDIR) MYUNAME="" -f - show_kernel_version_number$$$$ 2> /dev/null)
endif

BCM_KVERSIONSTRING := $(word 1,$(subst -,$(space),$(LINUXVER)))

# Require LINUXVER to have at least 3 fields (x.y.z).
ifeq (,$(findstring clean,$(MAKECMDGOALS)))
ifeq (,$(word 3,$(subst .,$(space),$(BCM_KVERSIONSTRING))))
     $(error LINUXVER bad format: "$(LINUXVER)")
endif
endif

# Allow CROSS_COMPILE to specify compiler base
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
NM := $(CROSS_COMPILE)nm
OBJCOPY := $(CROSS_COMPILE)objcopy

vpath %.c $(addprefix $(WLAN_SrcBaseR)/,dhd/sys shared bcmsdio/sys wl/sys wl/phy) $(WLAN_ComponentSrcDirsR)

## Initialize DFLAGS
DFLAGS :=
CFILES :=


# basic options (defines in DFLAGS, includes in IFLAGS)
DFLAGS += -DLINUX -DSRCBASE=\"$(WLAN_SrcBaseA)\" -DBCMDRIVER -DBCMDONGLEHOST
DFLAGS += -DUNRELEASEDCHIP -DBCMDMA32
DFLAGS += -DBCMFILEIMAGE
ifeq ($(BCMQT),1)
	DFLAGS += -DBCMSLTGT -DBCMQT
endif
ifeq ($(WLTEST),1)
	DFLAGS += -DWLTEST -DIOCTL_RESP_TIMEOUT=20000
	DFLAGS += -DDHD_SPROM
endif

# Past 2.6.29 kernels, arch specific bits are re-organized in linux kernel. So
# append new include paths to existing ones to get 2.6.29+ kernels compile

# Default DHDARCH is x86
ifdef ARCH
   DHDARCH ?= $(ARCH)
else
   DHDARCH ?= x86
endif
ifneq ($(findstring native,$(TARGET)),)
   DHDARCH = x86
endif
ifneq ($(findstring mips,$(TARGET)),)
   DHDARCH = mips
endif
ifneq ($(findstring arm,$(TARGET)),)
   DHDARCH = arm
endif

# First include from linux kernel dirs
IFLAGS := -I$(LINUXDIR)/include
IFLAGS += -I$(LINUXDIR)/include/asm/mach-default
# Followed by 2.6.29+ specific paths
IFLAGS += -I$(LINUXDIR)/arch/$(DHDARCH)/include
IFLAGS += -I$(LINUXDIR)/arch/$(DHDARCH)/include/asm/mach-default

# From current workspace
IFLAGS += -I.
IFLAGS += -I$(WLAN_SrcBaseA)/include
IFLAGS += -I$(WLAN_SrcBaseA)/shared
IFLAGS += -I$(WLAN_SrcBaseA)/dhd/sys
IFLAGS += -I$(WLAN_SrcBaseA)/dongle
IFLAGS += -I$(WLAN_SrcBaseA)/wl/sys
IFLAGS += -I$(WLAN_SrcBaseA)/wl/phy
IFLAGS += $(WLAN_ComponentIncPath)

ifneq ($(wildcard $(LINUXDIR)/.config),)
include $(LINUXDIR)/.config
else
# This is dangerous, since we don't know if they are really configured.
CONFIG_WIRELESS_EXT=y
DFLAGS += -DCONFIG_WIRELESS_EXT
endif

ifeq ($(CONFIG_MMC_MSM7X00A),y)
DFLAGS += -Dlinux
DFLAGS += -DDHD_SDALIGN=64 -DMAX_HDR_READ=64 -DDHD_FIRSTREAD=64
endif

WFLAGS := -Wall -Wstrict-prototypes
ifeq (,$(findstring 2.4.18,$(LINUXVER)))
#  WFLAGS += -Werror
endif

#disabled# # Kernel starting from fc15 need a gcc 4.6.+ compiler that is more stricter
#disabled# # and doesn't allow warnings pass compilation step.
#disabled# # Development group requested that fixing FC15+ warnings takes few weeks
#disabled# # hence for now suppress those warnings, but do NOT let them linger for too
#disabled# # long as this may mask real sloppy compilation mistakes.

#disabled# ifeq ($(filter arm% mips%,$(TARGETARCH))$(filter arm% mips%,$(ARCH)),)
#disabled#   ifneq ($(filter 2.6.38% 2.6.39% 2.6.40%,$(LINUXVER)),)
#disabled#     WFLAGS += -Wno-unused-but-set-variable
#disabled#     WFLAGS += -Wno-unused-but-set-parameter
#disabled#     WFLAGS += -Wframe-larger-than=1024
#disabled#     WFLAGS += -maccumulate-outgoing-args
#disabled#   endif # LINUXVER
#disabled# endif # TARGETARCH

CFILES += dhd_linux.c dhd_linux_sched.c linux_osl.c bcmutils.c bcmevent.c dhd_common.c
CFILES += dhd_custom_gpio.c

ifeq ($(findstring -usb-,-$(TARGET)-),)
CFILES += siutils.c sbutils.c aiutils.c hndpmu.c
ifeq ($(WLTEST),1)
	CFILES += bcmsrom.c bcmotp.c
endif
endif

# threading options
ifeq ($(findstring -nothread-,-$(TARGET)-),)
DFLAGS += -DDHDTHREAD
endif

# Building hsic for 43239, 43236, 4330 and 4334
ifneq ($(findstring -hsic-,-$(TARGET)-),)
DFLAGS += -DEXTENDED_VID_PID=' { USB_DEVICE(0x0a5c, 0xbd1b) }, \
                               { USB_DEVICE(0x0a5c, 0xbd17) }, \
                               { USB_DEVICE(0x0a5c, 0xbd19) }, \
                               { USB_DEVICE(0x0a5c, 0xbd1c) }, \
                                { USB_DEVICE(0x0a5c, 0xbd1a) }'
#Enable interrupt ep support for HSIC driver
#DFLAGS += -DINTR_EP_ENABLE
endif

ifeq ($(CONFIG_NET_RADIO),y)
CFILES += wl_iw.c bcmwifi_channels.c
DFLAGS += -DWLLXIW
else
	ifeq ($(CONFIG_WIRELESS_EXT),y)
	CFILES += wl_iw.c bcmwifi_channels.c
	DFLAGS += -DWLLXIW -DUSE_IW
        DFLAGS += -DSOFTAP
	endif
endif

# FullDongle Usb aggregation
ifneq ($(findstring -fdaggr-,-$(TARGET)-),)
CFILES += bcm_rpc_tp_dbus.c linux_rpc_osl.c
DFLAGS += -DBCM_FD_AGGR
endif

OFILES=$(CFILES:.c=.o)

# Make debug a separate option
ifneq ($(findstring -debug-,-$(TARGET)-),)
DFLAGS += -DDHD_DEBUG -DSDTEST
endif

# enable queuing in host
ifneq ($(findstring -sdstd-,-$(TARGET)-),)
ifeq ($(findstring -noproptxstatus-,-$(TARGET)-),)
DFLAGS += -DPROP_TXSTATUS
endif
ifneq ($(findstring -dbgwlfc-,-$(TARGET)-),)
DFLAGS += -DPROP_TXSTATUS_DEBUG
endif
endif

# Make big-endian a separate option
ifneq ($(findstring -be-,-$(TARGET)-),)
DFLAGS += -DIL_BIGENDIAN
endif

ifneq ($(findstring -dnglimage-,-$(TARGET)-),)
## Embeddable dongle image name
DNGL_IMAGE_NAME?=
DFLAGS += -DBCM_DNGL_EMBEDIMAGE -DEMBED_IMAGE_GENERIC
IFLAGS += -I$(WLAN_SrcBaseA)/dongle/rte/wl/builds/$(DNGL_IMAGE_NAME)
IFLAGS += -I$(WLAN_SrcBaseA)/shared/zlib
endif

ifneq ($(findstring -cdc-,-$(TARGET)-),)
#DFLAGS += -DBDC -DTOE
DFLAGS += -DBDC
DFLAGS += -DDHD_BCMEVENTS -DSHOW_EVENTS
CFILES += dhd_cdc.c
endif
ifneq ($(findstring -rndis-,-$(TARGET)-),)
DFLAGS += -DRNDIS
CFILES += dhd_rndis.c
endif

ifneq ($(findstring -dbususb-,-$(TARGET)-),)
DFLAGS += -DBCMDHDUSB -DBCMDBUS
CFILES += dbus.c dbus_usb.c dbus_usb_linux.c
endif
ifneq ($(findstring -dbususb-dnglimage-,-$(TARGET)-),)
DFLAGS += -DBCMDHDUSB -DBCMDBUS
DFLAGS += -DBCM_DNGL_EMBEDIMAGE
CFILES += dbus.c dbus_usb.c dbus_usb_linux.c
endif
ifneq ($(findstring -usb-,-$(TARGET)-),)
DFLAGS += -DBCMDHDUSB -DBCMDBUS
CFILES += dbus.c dbus_usb.c dbus_usb_linux.c
endif
ifneq ($(findstring -dbus-,$(TARGET)-),)
DFLAGS += -DBCMDBUS -DBCMSDIO
CFILES += dbus.c dbus_sdh.c dbus_sdh_linux.c
endif
ifneq ($(findstring -dbussdstd-,$(TARGET)-),)
DFLAGS += -DBCMSDIO -DBCMSDIOH_STD
CFILES += bcmsdh.c bcmsdstd.c bcmsdstd_linux.c bcmsdh_linux.c
DFLAGS += -DBCMDBUS
CFILES += dbus.c dbus_sdh.c dbus_sdh_linux.c
endif
ifneq ($(findstring -sdio-,-$(TARGET)-),)
DFLAGS += -DBCMSDIO
CFILES += dhd_sdio.c
endif
ifneq ($(findstring -sdbcm-,$(TARGET)-),)
DFLAGS += -DBCMSDIO -DBCMSDIOH_BCM -DTESTDONGLE
CFILES += dhd_sdio.c bcmsdh.c bcmsdbrcm.c bcmsdbrcm_linux.c bcmsdh_linux.c hnddma.c
endif
ifneq ($(findstring -sdstd-,$(TARGET)-),)
DFLAGS += -DBCMSDIO -DBCMSDIOH_STD -DBCMSDIOH_TXGLOM 
CFILES += dhd_sdio.c bcmsdh.c bcmsdstd.c bcmsdstd_linux.c bcmsdh_linux.c
ifneq ($(findstring -hc3-,$(TARGET)-),)
DFLAGS += -DSDHOST3=1
endif
endif

ifneq ($(findstring -sdmmc-,-$(TARGET)-),)
ifeq ($(findstring -noproptxstatus-,-$(TARGET)-),)
  DFLAGS += -DPROP_TXSTATUS
endif
  DFLAGS += -DBCMSDIO -DBCMLXSDMMC -DBCMPLATFORM_BUS
  CFILES += dhd_sdio.c bcmsdh_sdmmc.c bcmsdh.c bcmsdh_linux.c bcmsdh_sdmmc_linux.c
endif

ifeq ($(call wlan_version_ge,$(BCM_KVERSIONSTRING),2.6.35),TRUE)
DFLAGS += -DARP_OFFLOAD_SUPPORT
endif
ifneq ($(findstring -android-,-$(TARGET)-),)
  DFLAGS += -DOEM_ANDROID -Dlinux -DBCMDBG
  DFLAGS += -DCUSTOMER_HW2 -DEMBEDDED_PLATFORM -DKEEP_ALIVE
  CFILES += wl_android.c wldev_common.c bcmwifi_channels.c
  ifeq ($(call wlan_version_ge,$(BCM_KVERSIONSTRING),2.6.35),TRUE)
    DFLAGS += -DCSCAN
  endif
endif

ifneq ($(findstring -cfg80211-,-$(TARGET)-),)
  ifndef COMPAT_WIRELESS
    ifneq ($(call wlan_version_ge,$(BCM_KVERSIONSTRING),2.6.38),TRUE)
      $(warning : Kernel < 2.6.38 needs compat-wireless****)
      COMPAT_WIRELESS=/projects/hnd/tools/linuxwireless/compat-wireless
      export COMPAT_WIRELESS
    endif
  endif
  ifdef COMPAT_WIRELESS
    DFLAGS += -DWL_COMPAT_WIRELESS
  endif
  DFLAGS += -DWL_CFG80211 -DWLP2P #-DPNO_SUPPORT
  CFILES += wl_cfg80211.c wl_cfgp2p.c dhd_cfg80211.c
  CFILES += wl_linux_mon.c
endif

ifneq ($(findstring -usb,-$(TARGET)-),)
ifeq ($(findstring -noproptxstatus-,-$(TARGET)-),)
DFLAGS += -DPROP_TXSTATUS
endif
ifneq ($(findstring -dbgwlfc-,-$(TARGET)-),)
DFLAGS += -DPROP_TXSTATUS_DEBUG
endif
endif


ifneq ($(findstring -oob-,-$(TARGET)-),)
  DFLAGS += -DOOB_INTR_ONLY
  DFLAGS += -DMMC_SDIO_ABORT
  ifeq ($(call wlan_version_ge,$(BCM_KVERSIONSTRING),2.6.35),TRUE)
    DFLAGS += -DHW_OOB
  endif
else
  ifneq ($(findstring -sdmmc-,-$(TARGET)-),)
    DFLAGS += -DSDIO_ISR_THREAD
  endif
endif

# *** PLEASE try not to overwirte the common -sdmmc- settings in your build.
# if all builds (nexus, dk8 etc) have its special settings, we will end up having may
# duplicate flags/files in DFLAGS, CFILES

ifneq ($(findstring -nexus-,-$(TARGET)-),)
  DFLAGS += -DDHD_USE_STATIC_BUF -DCUSTOM_OOB_GPIO_NUM=299
endif

ifneq ($(findstring -panda-,-$(TARGET)-),)
$(warning : =====> Panda build <======)
DFLAGS += -DBCMWAPI_WPI -DBCMWAPI_WAI -DBCM_REQUEST_FW
DFLAGS += -DCUSTOM_OOB_GPIO_NUM=138 -DDHD_USE_EARLYSUSPEND -DBOARD_PANDA -DWL_ENABLE_P2P_IF	\
    -DNEW_COMPAT_WIRELESS -DWL_CFG80211_STA_EVENT
  ifeq ($(CONFIG_WIRELESS_EXT),y)
    DFLAGS :=$(filter-out -DCSCAN,$(DFLAGS))
  endif
  ifneq ($(findstring -icsmr1-,-$(TARGET)-),)
    $(warning : ***** IceCream MR1 build *****)
    DFLAGS :=$(filter-out -DWL_ENABLE_P2P_IF,$(DFLAGS))
    DFLAGS :=$(filter-out -DWL_CFG80211_STA_EVENT,$(DFLAGS))
  endif
  ifneq ($(findstring -hsic-android-,-$(TARGET)-),)
    DFLAGS :=$(filter-out -DWL_ENABLE_P2P_IF,$(DFLAGS))
  endif
endif

ifneq ($(findstring -dk8-,-$(TARGET)-),)
  DFLAGS += -DMMC_SDIO_ABORT
endif

ifneq ($(findstring -prime-,-$(TARGET)-),)
$(info : =====> prime build <=====)
DFLAGS :=$(filter-out -DSOFTAP,$(DFLAGS))
DFLAGS :=$(filter-out -DSDIO_ISR_THREAD,$(DFLAGS))
CFILES += dhd_bta.c
DFLAGS += -DHD_GPL -DDHD_SCHED -DCUSTOMER_HW2 -DNEW_COMPAT_WIRELESS -DWIFI_ACT_FRAME 	\
    -DGET_CUSTOM_MAC_ENABLE -DPKT_FILTER_SUPPORT -DENABLE_INSMOD_NO_FW_LOAD				\
    -DPNO_SUPPORT -DWLBTAMP -DOOB_INTR_ONLY -DMMC_SDIO_ABORT -DHW_OOB -DWIFI_ACT_FRAME	\
    -DSDTEST -DDHD_USE_EARLYSUSPEND -DCUSTOM_OOB_GPIO_NUM=2 -DWL_ENABLE_P2P_IF			\
    -DWL_CFG80211_STA_EVENT
	ifneq ($(findstring -lungo-,-$(TARGET)-),)
     $(info : =====> lungo build <=====)
			DFLAGS += -fno-pic
			DFLAGS :=$(filter-out -DENABLE_INSMOD_NO_FW_LOAD,$(DFLAGS))
	endif
	ifneq ($(findstring -jellybean-,-$(TARGET)-),)
     $(warning : ***** Jellybean build *****)
			DFLAGS += -DSET_RANDOM_MAC_SOFTAP
			DFLAGS += -DWL_SCHED_SCAN		
	endif
  ifneq ($(findstring -icsmr1-,-$(TARGET)-),)
     $(warning : ***** IceCream MR1 build *****)
     DFLAGS :=$(filter-out -DWL_ENABLE_P2P_IF,$(DFLAGS))
     DFLAGS :=$(filter-out -DWL_CFG80211_STA_EVENT,$(DFLAGS))
  endif
endif


ifneq ($(findstring -sdspi-,$(TARGET)-),)
  DFLAGS += -DBCMSDIO -DBCMSDIOH_SPI -DTESTDONGLE # -DBCMSDYIELD
  CFILES += dhd_sdio.c bcmsdh.c bcmsdspi.c bcmsdspi_linux.c bcmsdh_linux.c
endif
ifneq ($(findstring -gspi-,$(TARGET)-),)
  DFLAGS += -DBCMSDIO -DBCMSDIOH_SPI -DTESTDONGLE -DBCMSPI
  CFILES += dhd_sdio.c bcmsdh.c bcmspibrcm.c bcmsdspi_linux.c bcmsdh_linux.c
endif
ifneq ($(findstring -gspibcm,$(TARGET)-),)
  DFLAGS += -DBCMSDIO -DBCMSDIOH_SPI -DTESTDONGLE -DBCMSPI
  CFILES += dhd_sdio.c bcmsdh.c bcmgspi.c bcmsdbrcm_linux.c bcmsdh_linux.c hnddma.c
endif
ifneq ($(findstring -pci,$(TARGET)-),)
  CFILES += bcmpcispi.c
endif
ifneq ($(findstring -sdext-,$(TARGET)-),)
  DFLAGS += -DBCMSDIO -DTESTDONGLE
  CFILES += dhd_sdio.c
endif

ifneq ($(findstring -huawei-,$(TARGET)-),)
  DFLAGS += -DDHD_DEBUG -DSDTEST
#  DFLAGS += -DBCM_REQUEST_FW
  DFLAGS += -DBCMINTERNAL -DBCMDBG -DDHD_DEBUG -DBCMDBG_MEM -DSDTEST -DBCMPERFSTATS
endif


#ifdef WLBTAMP
 ifneq ($(findstring -btamp-,$(TARGET)-),)
  DFLAGS += -DWLBTAMP
  CFILES += dhd_bta.c
endif

#ifdef WIFI_ACT_FRAME
DFLAGS += -DWIFI_ACT_FRAME
#endif
ifneq ($(findstring -comp,-$(TARGET)),)
DFLAGS += -DBCMUSBDEV_COMPOSITE
endif

#
# SAVERESTORE
#
# DFLAGS += -DUSE_CMD14
# Without CMD14, defaults to CMD52 toggle to wakeup device
# Or define this for OOB wakeup
# DFLAGS += -DUSE_OOB_GPIO1

ifneq ($(findstring -reqfw-,$(TARGET)-),)
DFLAGS += -DBCM_REQUEST_FW
endif

CFLAGS += -fshort-wchar $(DFLAGS) $(WFLAGS) $(IFLAGS) $(MY_C_DEFINES)

LDFLAGS := -r
MODULES := dhd.o
ifeq ($(call wlan_version_ge,$(BCM_KVERSIONSTRING),2.5.0),TRUE)
  ##Kernel module names in 2.6 kernel have .ko suffix
  KMODULES:=dhd.ko

  ifneq ($(findstring -android-,-$(TARGET)-),)
      KMODULES:=bcmdhd.ko
  endif

else
  KMODULES:=$(MODULES)
endif

# host options
HOSTCC := $(CC)
ifeq ($(call wlan_version_ge,$(BCM_KVERSIONSTRING),2.5.0),TRUE)
  HOSTCFLAGS := $(CFLAGS) -D__KERNEL__
  export DHDCFLAGS := $(HOSTCFLAGS) -I$(shell pwd)
  export DHDOFILES := $(OFILES)
else
  HOSTCFLAGS := $(CFLAGS) $(shell $(MAKE) --no-print-directory -s -C $(LINUXDIR) script 'SCRIPT=@echo $$(CFLAGS) $$(MODFLAGS)')
endif

# Please do NOT explicitly specify any -debug build here, the Makefile can handles that postfix.
# That is, -debug can be added to any of the following builds#

TARGETS := \
	cdc-usb \
	cdc-dbususb \
	cdc-dbususb-dnglimage \
	cdc-sdio \
	cdc-dbus \
	cdc-sdbcm \
	cdc-sdstd \
	cdc-dbussdstd \
	cdc-sdspi-pci \
	cdc-sdstd-noproptxstatus \
	cdc-sdmmc-android-nexus\
	cdc-sdmmc-android-nexus-oob\
	cdc-sdmmc-android-nexus-cfg80211\
	cdc-sdmmc-android-nexus-cfg80211-oob\
	cdc-sdmmc-android-panda-oob \
	cdc-sdmmc-android-panda-cfg80211 \
	cdc-sdmmc-android-panda-cfg80211-oob \
	cdc-sdmmc-android-panda-icsmr1-cfg80211 \
	cdc-sdmmc-android-panda-icsmr1-cfg80211-oob \
	cdc-sdmmc-android-prime-cfg80211 \
	cdc-sdmmc-android-prime-icsmr1-cfg80211 \
	cdc-sdmmc-android-prime-cfg80211-gpl \
	cdc-sdmmc-android-prime-cfg80211-oob-gpl \
	cdc-sdmmc-android-prime-jellybean-cfg80211-oob-gpl \
	cdc-sdmmc-android-prime-lungo-cfg80211 \
	cdc-sdmmc-android-prime-lungo-icsmr1-cfg80211 \
	cdc-sdmmc-android-prime-lungo-cfg80211-gpl \
	cdc-sdmmc-android-prime-lungo-cfg80211-oob-gpl \
	cdc-sdmmc-android-prime-lungo-jellybean-cfg80211-oob-gpl \
	cdc-sdmmc-android-dk8 \
	cdc-sdmmc-android-dk8-cfg80211-oob \
	cdc-sdmmc-android-dk8-apsta-btamp \
	cdc-sdstd-nothread \
	cdc-sdmmc-gpl \
	cdc-usb-gpl \
	cdc-usb-reqfw-gpl \
	cdc-usb-reqfw-android-cfg80211 \
	cdc-usb-hsic-android-panda-cfg80211 \
	cdc-usb-dnglimage-gpl \
	cdc-usb-fdaggr-gpl \
	cdc-usb-comp \
	cdc-usb-comp-gpl \
	cdc-sdstd-apsta \
	cdc-sdio-dnglimage \
	cdc-sdbcm-dnglimage \
	cdc-sdstd-dnglimage \
	cdc-dbussdstd-dnglimage \
	cdc-dbus-dnglimage \
	cdc-sdspi-pci-dnglimage \
	cdc-gspi-pci \
	cdc-gspibcm \
	cdc-usb-hsic-huawei-gpl\
	cdc-usb-hsic-huawei-gpl-dbgwlfc\
	cdc-usb-hsic-huawei-gpl-noproptxstatus\
	cdc-usb-hsic-gpl\
	cdc-usb-hsic-debug-gpl-noproptxstatus \
	cdc-sdstd-hc3

#ifdef RNDIS
TARGETS += rndis-usb
#endif
TARGETS += cdc-sdext-be
TARGETS += cdc-sdext-be-dnglimage
#ifdef WLBTAMP
TARGETS += cdc-sdstd-btamp
#endif

TARGETS := $(addprefix $(ObjPfx),$(TARGETS))
TARGETS += $(foreach tgt, $(TARGETS), $(tgt)-debug)

ifeq ($(BUILD_TAG),TRUNK)
OBJDIR=host/$(TARGET_ARCH)/$(TARGET_OS)/$(TARGET)-$(LINUXVER)$(if $(BCMQT),-bcmqt)
else
OBJDIR=$(TARGET)-$(LINUXVER)$(if $(BCMQT),-bcmqt)
endif

all: $(filter %-sdio %-sdbcm %-sdstd %-usb %sdspi-pci %-sdmmc, $(TARGETS))
sdio: $(filter %-sdio %-sdbcm %-sdstd, $(TARGETS))
usb: $(filter %-usb, $(TARGETS))
sdspi: $(filter %-sdspi-pci, $(TARGETS))

# Allow making target with the LINUXVER suffix already on it.
# (Typical of command line tab completion; trailing slash still not allowed)
%-$(LINUXVER): force
	$(MAKE) $(@:%-$(LINUXVER)=%)

$(TARGETS):
	@echo "MAKING $@"
	$(MAKE) TARGET=$@ objdir

# Show compiler version, for the current target build
showenv:
	@echo "CC = $(CC) (ver=`$(CC) -dumpversion`; host=`hostname`; processor=`uname -m`)"

objdir: showenv
	@echo "Making objdir $(OBJDIR)"
	@echo "TARGET is $(TARGET)"
	mkdir -p $(OBJDIR)
ifneq ($(call wlan_version_ge,$(BCM_KVERSIONSTRING),2.5.0),TRUE)
	$(MAKE) -C $(OBJDIR) -f $(WLAN_Makefile) dep
endif
	$(MAKE) -C $(OBJDIR) -f $(WLAN_Makefile) modules
ifeq ($(call wlan_version_ge,$(BCM_KVERSIONSTRING),2.5.0),TRUE)
	$(OBJCOPY) --strip-unneeded $(OBJDIR)/$(KMODULES) $(OBJDIR)/$(KMODULES).stripped
else
	$(OBJCOPY) --strip-unneeded $(OBJDIR)/dhd.o $(OBJDIR)/dhd.o.stripped
endif

dep: $(foreach file,$(CFILES),.$(file).depend)
.%.c.depend: %.c
	$(HOSTCC) $(HOSTCFLAGS) -M $< > $@ || (rm -f $@; exit 1)
.%.c.depend::
	touch $@

ifeq ($(call wlan_version_ge,$(BCM_KVERSIONSTRING),2.5.0),TRUE)
modules: $(OFILES)
	test -r ./Makefile || ln -s $(WLAN_SrcBaseR)/dhd/linux/makefile.26 ./Makefile
	$(strip $(MAKE) -C $(LINUXDIR) M=$(shell pwd) $(if $(VERBOSE),V=1) $(WLAN_ShellDebugSHELL) modules)
else
modules: $(MODULES)
endif

$(MODULES): $(OFILES)
	$(LD) $(LDFLAGS) -o $@ $^

ifeq ($(call wlan_version_ge,$(BCM_KVERSIONSTRING),2.5.0),TRUE)
%.o: %.c
  # when make is called from 2.6, vpath doesn't work so we need to link the files.
	test -r ./$(<F) || ln -s $< .
else
%.o: %.c
	$(HOSTCC) $(HOSTCFLAGS) -c -o $@ $<
	@( \
	echo 'ifneq ($$(HOSTCFLAGS),$(HOSTCFLAGS))' ; \
	echo '$@: force' ; \
	echo 'endif' ; \
	) > .$*.c.flags
endif

force:

clean:
ifeq (,$(filter %/,$(ObjPfx)))
	$(RM) -r $(ObjPfx)*
else
	$(RM) -r $(ObjPfx)
endif

-include $(wildcard .*.depend)
-include $(wildcard .*.flags)
