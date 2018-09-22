#
# USB Remote Download Makefile - Host linux user mode driver
#
#
# Copyright (C) 2011, Broadcom Corporation
# All Rights Reserved.
# 
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior
# written permission of Broadcom Corporation.
#
# $Id: Makefile,v 1.18.2.3 2010-12-16 23:50:16 $


SRCBASE = ../..
include $(SRCBASE)/Makerules

vpath %.c $(SRCBASE)/shared

CFLAGS= -Wall -Wstrict-prototypes -g -O2 -I$(SRCBASE)/include -I$(SRCBASE)/shared/zlib
LIBS = -lusb
TARGET = bcmdl
OBJECTS := bcmdl.o adler32.o inffast.o inflate.o infcodes.o infblock.o inftrees.o infutil.o zutil.o crc32.o bcmutils.o

TESTS = tsthwa tstjtag timehwa timejtag

VPATH := $(SRCBASE)/shared/zlib

ifeq ($(TARGETENV), linux)
	OBJECTS += usb_linux.o
	COMPILE = $(CC)
	CLEAN = clean_linux
	CFLAGS += -I../libusb
endif

ifeq ($(TARGETENV), linuxmips_be) 
	OBJECTS += usb_linux.o
	COMPILE = mips-linux-gcc 
	LIBS := -L../libusb/.libs ../libusb/.libs/libusb.a
	CFLAGS += -I../libusb -DIL_BIGENDIAN
endif

ifeq ($(TARGETENV), linuxmips)
	TARGET_PREFIX := mipsel-uclibc-
	OBJECTS += usb_linux.o
	CFLAGS += -I ../libusb
	COMPILE = mipsel-uclibc-gcc 
	CLEAN = clean_linux
	LDFLAGS += -L ../libusb/.libs
	LIBS= ../libusb/.libs/libusb.a
install: 
	cp bcmdl $(SRCBASE)/router/mipsel-uclibc/target/bin
endif

ifeq ($(TARGETENV), linux26mips)
	TARGET_PREFIX := mipsel-uclibc-linux26-
	TARGET := bcmdlmips26
	OBJECTS += usb_linux.o
	CFLAGS += -I ../libusb
	COMPILE = mipsel-uclibc-linux26-gcc 
	CLEAN = clean_linux_mips26
	LDFLAGS += -L /projects/hnd/tools/linux/lib/mips26
install: 
	cp bcmdl $(SRCBASE)/router/mipsel-uclibc/target/bin
endif

ifeq ($(TARGETENV), macos)
	TARGET = bcmdl_macos
	CLEAN = clean_macos
	COMPILE = xcodebuild
	PROJECT = bcmdl.xcodeproj
endif

ifeq ($(BCMQT),1)
	CFLAGS += -DBCMQT
	TARGET = bcmdl_qt
endif

all: $(TARGET)

tests: $(TESTS)

bcmdl: $(OBJECTS)
	$(COMPILE)  $(OBJECTS)  $(CFLAGS) $(CROSS_LD_PATH) $(LIBS)  -o $@

bcmdl_qt: $(OBJECTS)
	$(COMPILE)  $(OBJECTS)  $(CFLAGS) $(LIBS)  -o $@

bcmdlmips26: $(OBJECTS:%=$(TARGETARCH)/%)
	$(COMPILE) $^ $(CFLAGS) $(LDFLAGS) $(LIBS)  -o $@

# Create targetarch specific .o files
$(TARGETARCH)/%.o: %.c
	@install -dv $(@D)
	$(COMPILE) -c $< $(CFLAGS) -o $@

$(TESTS): %: %.o
	$(COMPILE) $< $(CFLAGS) $(LIBS) -o $@

# Executable is located in build/Debug/bcmdl
#
bcmdl_macos:
	$(COMPILE) -project $(PROJECT) -target $(TARGET) -configuration Debug build

clean_macos:
	$(COMPILE) -project $(PROJECT) -target $(TARGET) -configuration Debug clean

clean_linux:
	rm -rf bcmdl_qt $(TARGET) $(OBJECTS) $(TESTS) $(TESTS:=.o)

clean_linux_mips26:
	rm -rf bcmdlmips26 $(TARGET) $(OBJECTS:%=$(TARGETARCH)/%)

clean: $(CLEAN)
