/*
 * aliotgateway.h
 *
 *  Created on: 04-Feb-2014
 *      Author: ranjit
 */

#ifndef ALIOTGATEWAY_H_
#define ALIOTGATEWAY_H_
#define double int

struct gateway_access {
	char access_key[64];
	char secret_key[64];
	char url[256];
    char http_ack_topic[100];
};


typedef void (*instruction_handler)(char *thing_key, char *alert_key, char *instruction);

struct thing {
	char key[64];
	char name[64];
	char description[256];

    /* Reserved. Do not Use */
    char instruction_topic[300];
    instruction_handler handler;
};

#define ERR_INVALID_PARAMS -1
#define ERR_OK              0
#define ERR_UNAUTHORIZED    1
#define ERR_INVALID_REQUEST 2
#define ERR_EXCESSIVE_RATE  3
#define ERR_FAILED          4
#define ERR_NOT_ACCEPTABLE  5
#define ERR_NO_CONNECTION   6
#define ERR_TIMED_OUT       7
#define ERR_BAD_REQUEST     8
#define ERR_INTERNAL_SERVER_ERROR 9


extern struct gateway_access configuration;

/*
 * Initializes this agent gateway
 *
 */
int initialize(char* access_key, char* secret_key);

/*
 * Creates a thing bean/object
 * Instruction handler is optional - Can be passed null
 */
void create_thing(struct thing *thing, char* key, char* name, char* description, instruction_handler handler);

/*
 * Creates a connection with Datonis
 */
int connect_datonis();

/*
 * Creates a connection to specified Datonis instance
 */
int connect_datonis_instance(char *server);

/*
 * Terminates existing datonis connection
 */
int disconnect_datonis();

/*
 * Registers the thing with Datonis
 * All other calls to Datonis should be done only after this is succesful
 */
int register_thing(struct thing *thing);

/*
 * Transmits a heartbeat (ping message) to Datonis
 */
int transmit_thing_heartbeat(struct thing *thing);

/*
 * Transmits a data event to datonis
 */
int transmit_thing_data(struct thing *thing, char* value, char *waypoint);

/*
 * Transmits a data event to datonis in compressed way
 */
int transmit_compressed_thing_data(struct thing *thing, char* value, char *waypoint);

/*
 * Transmits an alert notification to Datonis.
 * Last argument (data) is optional. If you do specify it, it should be in JSON format
 */
int transmit_thing_alert(struct thing *thing, int level, char *message, char *data);

/**
 * Send instruction feedback
 */
int transmit_instruction_feedback(char * alert_key, int level, char *message, char *data);

/**
 * Create a thing data packet
 */
char* create_thing_data_packet(char *buf, struct thing *thing, char* value, char *waypoint);

/**
 * Create a thing data packet with specified timestamp
 */
char* create_thing_data_packet_ts(char *buf, struct thing *thing, char* value, char *waypoint, double timestamp);

/**
 * Creates a bulk thing data packet from individual packets
 */
char* create_thing_bulk_data_packet(char *buf, char *packets, int packet_size, int count);

/**
 * Transmits a pre-created data packet.
 */
int transmit_thing_data_packet(char *packet);

/**
 * Transmits a pre-created data packet in compressed way.
 */
int transmit_compressed_thing_data_packet(char *packet);
/**
 * Yield for any incoming communication
 */
void yield(int milliseconds);

#endif
