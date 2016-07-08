/*
 * aliotgateway.c
 *
 * Implements non-hardware specific API of the aliotgateway interface
 * 
 * Rajesh Jangam (Altizon Systems Pvt. Ltd) 2015
 */
#include "aliotgateway.h"
#include "aliotutil.h"
#include <stdio.h>
#include <string.h>

struct gateway_access configuration;

int initialize(char* access_key, char* secret_key) {

    if(access_key == NULL || secret_key == NULL)
        return -1;
    strcpy(configuration.access_key, access_key);
    strcpy(configuration.secret_key, secret_key);
    strcpy(configuration.url, "api.datonis.io");
    return 0;
}


void create_thing(struct thing *thing, char* key, char* name, char* description, instruction_handler h) {

    if(key == NULL || name == NULL)
        return;

    strcpy(thing->key, key);
    strcpy(thing->name, name);
    if(description != NULL)
        strcpy(thing->description, description);
    thing->handler = h;
}

char* create_thing_data_packet(char *json, struct thing *thing, char* value, char *waypoint) {
    return get_thing_data_json(json, thing, value, waypoint);
}

char* create_thing_data_packet_ts(char *json, struct thing *thing, char* value, char *waypoint, double timestamp) {
    return get_thing_data_json_ts(json, thing, value, waypoint, timestamp);
}

char* create_thing_bulk_data_packet(char *json, char *packets, int packet_size, int count) {
    return get_thing_bulk_data_json(json, packets, packet_size, count);
}
