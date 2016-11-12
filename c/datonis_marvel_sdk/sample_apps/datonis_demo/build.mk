# Copyright (C) 2008-2016 Marvell International Ltd.
# All Rights Reserved.
#

ifeq ($(communication_mode),mqtt)
  mode_flag := -D_COMMUNICATION_MODE_MQTT_=1
else
  mode_flag := -D_COMMUNICATION_MODE_HTTP_=1
endif

exec-y += datonis_demo
datonis_demo-objs-y := src/main.c
datonis_demo-cflags-y := -I$(d)/src -DAPPCONFIG_DEBUG_ENABLE=1 $(mode_flag)

exec-y += datonis_demo_http
datonis_demo-objs-y := src/main.c
datonis_demo-cflags-y := -I$(d)/src -DAPPCONFIG_DEBUG_ENABLE=1 


# Applications could also define custom linker files if required using following:
#aws_starter_demo-linkerscript-y := /path/to/linkerscript
# Applications could also define custom board files if required using following:
#aws_starter_demo-board-y := /path/to/boardfile
