# Copyright (C) 2008-2016, Marvell International Ltd.
# All Rights Reserved.

libs-y += libdatonis

ifeq ($(communication_mode),mqtt)
  libdatonis-objs-y := MQTTPacket/src/MQTTSerializePublish.c \
	MQTTPacket/src/MQTTUnsubscribeClient.c \
	MQTTPacket/src/MQTTConnectClient.c \
	MQTTPacket/src/MQTTPacket.c \
	MQTTPacket/src/MQTTSubscribeClient.c \
	MQTTPacket/src/MQTTDeserializePublish.c \
	Aliot/MQTTClient.c \
	Aliot/aliotgateway.c \
	Aliot/aliotgateway_mqtt.c \
	Aliot/aliotutil.c \
	Aliot/hmac.c \
	Aliot/jsmn.c \
	Aliot/jsonutils.c \
	Implementation/MQTTMbed.c \
	Implementation/timeutil_mbed.c
	

  libdatonis-cflags-y := -I $(d)/MQTTPacket/src -I $(d)/Aliot -I $(d)/Implementation -D_COMMUNICATION_MODE_MQTT_=1

else
  libdatonis-objs-y := Aliot/aliotgateway.c \
	Aliot/aliotgateway_http.c \
	Aliot/aliotutil.c \
	Aliot/hmac.c \
	Aliot/jsmn.c \
	Aliot/jsonutils.c \
	Implementation/MQTTMbed.c \
	Implementation/timeutil_mbed.c

  libdatonis-cflags-y :=  -I $(d)/Aliot -I $(d)/Implementation -D_COMMUNICATION_MODE_HTTP_=1
endif