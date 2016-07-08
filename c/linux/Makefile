SHELL = /bin/sh
.PHONY: clean, mkdir, install

# assume this is normally run in the main Paho directory
ifndef srcdir
  srcdir = MQTTPacket/src
endif

ifndef blddir
  blddir = build/output
endif

SOURCE_FILES_C = $(srcdir)/*.c

HEADERS = $(srcdir)/*.h


SAMPLE_C = sample.c
SAMPLE_MQTT = ${blddir}/samples/sample_mqtt
SAMPLE_HTTP = ${blddir}/samples/sample_http
SAMPLE_BULK_C = sample_bulk_send.c
SAMPLE_MQTT_BULK = ${blddir}/samples/sample_bulk_send_mqtt
SAMPLE_HTTP_BULK = ${blddir}/samples/sample_bulk_send_http

# The names of libraries to be built
MQTT_EMBED_LIB_C = paho-embed-mqtt3c


# determine current platform
ifeq ($(OS),Windows_NT)
	OSTYPE = $(OS)
else
	OSTYPE = $(shell uname -s)
	MACHINETYPE = $(shell uname -m)
endif

ifeq ($(OSTYPE),Linux)

CC ?= gcc

ifndef INSTALL
INSTALL = install
endif
INSTALL_PROGRAM = $(INSTALL)
INSTALL_DATA =  $(INSTALL) -m 644

MAJOR_VERSION = 1
MINOR_VERSION = 0
VERSION = ${MAJOR_VERSION}.${MINOR_VERSION}

EMBED_MQTTLIB_C_TARGET = ${blddir}/lib${MQTT_EMBED_LIB_C}.so.${VERSION}


CCFLAGS_SO = -g -fPIC -Os -Wall -fvisibility=hidden -DLINUX_SO
FLAGS_EXE = -I ${srcdir}  -L ${blddir}

LDFLAGS_C = -shared -Wl,-soname,lib$(MQTT_EMBED_LIB_C).so.${MAJOR_VERSION}

all: build
	
build: | mkdir ${EMBED_MQTTLIB_C_TARGET} ${SAMPLE_MQTT} ${SAMPLE_HTTP} ${SAMPLE_MQTT_BULK} ${SAMPLE_HTTP_BULK}

clean:
	rm -rf ${blddir}/*
	
mkdir:
	-mkdir -p ${blddir}/samples

${SAMPLE_MQTT}: ${SAMPLE_C}
	${CC} -g3 -D_COMMUNICATION_MODE_MQTT_ -o ${SAMPLE_MQTT}  MQTTPacket/src/*c Aliot/*c Implementation/*c LZFCompression/*c ${SAMPLE_C} -I Aliot -I MQTTPacket/src -I Implementation -I LZFCompression

${SAMPLE_HTTP}: ${SAMPLE_C}
	${CC} -g3 -D_COMMUNICATION_MODE_HTTP_ -o ${SAMPLE_HTTP}  Aliot/*c Implementation/*c LZFCompression/*c ${SAMPLE_C} -I Aliot -I LZFCompression -lcurl

${SAMPLE_MQTT_BULK}: ${SAMPLE_BULK_C}
	${CC} -g3 -D_COMMUNICATION_MODE_MQTT_ -o ${SAMPLE_MQTT_BULK}  MQTTPacket/src/*c Aliot/*c Implementation/*c LZFCompression/*c ${SAMPLE_BULK_C} -I Aliot -I MQTTPacket/src -I Implementation -I LZFCompression

${SAMPLE_HTTP_BULK}: ${SAMPLE_BULK_C}
	${CC} -g3 -D_COMMUNICATION_MODE_HTTP_ -o ${SAMPLE_HTTP_BULK}  Aliot/*c Implementation/*c LZFCompression/*c ${SAMPLE_BULK_C} -I Aliot -I LZFCompression -lcurl


${EMBED_MQTTLIB_C_TARGET}: ${SOURCE_FILES_C} ${HEADERS_C}
	${CC} ${CCFLAGS_SO} -o $@ ${SOURCE_FILES_C} ${LDFLAGS_C}
	-ln -s lib$(MQTT_EMBED_LIB_C).so.${VERSION}  ${blddir}/lib$(MQTT_EMBED_LIB_C).so.${MAJOR_VERSION}
	-ln -s lib$(MQTT_EMBED_LIB_C).so.${MAJOR_VERSION} ${blddir}/lib$(MQTT_EMBED_LIB_C).so

install: build 

endif

