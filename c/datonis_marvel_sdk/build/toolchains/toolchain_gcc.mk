# Copyright (C) 2008-2016, Marvell International Ltd.
# All Rights Reserved.

CROSS_COMPILE := arm-none-eabi-

AS    := $(CROSS_COMPILE)gcc
CC    := $(CROSS_COMPILE)gcc
CPP   := $(CROSS_COMPILE)g++
LD    := $(CROSS_COMPILE)gcc
AR    := $(CROSS_COMPILE)ar
OBJCOPY := $(CROSS_COMPILE)objcopy
STRIP := $(CROSS_COMPILE)strip
HOST_CC := gcc


######### Common Linker File Handling
# This can be overriden from the apps
global-linkerscript-y := build/toolchains/GNU/$(arch_name-y).ld

# Toolchain specific global cflags-y
global-cflags-y :=

######### XIP Handling
ifeq ($(XIP), 1)
  global-linkerscript-y := build/toolchains/GNU/$(arch_name-y)-xip.ld
  global-linkerscript-$(CONFIG_ENABLE_MCU_PM3) := build/toolchains/GNU/$(arch_name-y)-xip-pm3.ld
  global-cflags-y += -DCONFIG_XIP_ENABLE
endif

compiler-version := $(shell $(CC) -dumpversion)
ifneq ($(compiler-version),4.9.3)
  $(error " Please use: $(CC) 2015 q3 version")
endif

# Compiler environment variable
tc-env := GCC
tc-gcc-env-y := y

tc-install-dir-y := for_gcc
tc-install-dir-$(use_extd_libc) := for_extd

# define disable-lto-for empty
disable-lto-for :=

# file include option
tc-include-opt := -include

# library strip options
tc-strip-opt := --strip-debug

# gcc specific extra linker flags
tc-lflags-y := \
		-Xlinker --undefined \
		-Xlinker uxTopUsedPriority \
		-nostartfiles \
		-Xlinker --cref \
		-Xlinker --gc-sections

# Linker flags
tc-lflags-$(tc-lto-y) += -Xlinker -flto
tc-lflags-$(tc-cortex-m4-y) += \
		-Xlinker --defsym=_rom_data=64 \
		-mthumb -g -Os \
		-fdata-sections \
		-ffunction-sections \
		-ffreestanding \
		-MMD -Wall \
		-fno-strict-aliasing \
		-fno-common \
		-mfloat-abi=softfp \
		-mfpu=fpv4-sp-d16 \
		-D__FPU_PRESENT

tc-lflags-$(tc-cortex-m3-y) += -mcpu=cortex-m3
tc-lflags-$(tc-cortex-m4-y) += -mcpu=cortex-m4


global-cflags-y += \
		-mthumb -g -Os \
		-fdata-sections \
		-ffunction-sections \
		-ffreestanding \
		-MMD -Wall \
		-fno-strict-aliasing \
		-fno-common

global-cflags-$(tc-cortex-m4-y) += \
		-mfloat-abi=softfp \
		-mfpu=fpv4-sp-d16 \
		-D__FPU_PRESENT

global-cflags-$(tc-cortex-m3-y) += -mcpu=cortex-m3
global-cflags-$(tc-cortex-m4-y) += -mcpu=cortex-m4

global-cpp-cflags-y := \
		-D_Bool=bool \
		-std=c++1y \
		--specs=nosys.specs

global-c-cflags-y := -fgnu89-inline

##############################################
## GCC Tololchain specific rules

# The command for converting a .c/.cc/.cpp/.S/.s to .o
# arg1 the .c/.cc/.cpp/.S/.s filename
# arg2 the object filename
#
# This file has the default rule that maps an object file from the standard
# build output directory to the corresponding .c/.cc/.cpp/.S/.s file in the src directory
#
define b-cmd-c-to-o
  @echo " [cc] $(1)"
  $(AT)$(CC) $(b-trgt-cflags-y) $(global-cflags-y) $(global-c-cflags-y) -o $(2) -c $(1) -MMD
endef

ifneq ($(CONFIG_ENABLE_CPP_SUPPORT),)
define b-cmd-cpp-to-o
  @echo " [cpp] $@"
  $(AT)$(CPP) $(b-trgt-cflags-y) $(global-cflags-y) $(global-cpp-cflags-y) -o $(2) -c $(1) -MMD
endef
endif

define b-cmd-axf
  @echo " [axf] $(call b-abspath,$(2))"
  $(AT)$($(1)-LD) -o $(2) $($(1)-objs-y) $($(1)-lflags-y) $($(1)-cflags-y) -Xlinker --start-group $($(1)-prebuilt-libs-y) $($(1)-libs-paths-y) $(global-prebuilt-libs-y) -Xlinker --end-group -T $($(1)-linkerscript-y) -Xlinker -M -Xlinker -Map -Xlinker $(2:%.axf=%.map) $(tc-lflags-y) $(global-cflags-y)
endef

define b-cmd-archive
  @echo " [ar] $(1)"
  $(AT)$(AR) cru $(1) $(2)
endef
##############################################
