/*
 * Utilities for use in gateway implementations
 */

#ifndef _ALIOTUTIL_H_
#define _ALIOTUTIL_H_

#include "aliotgateway.h"

char* get_hmac(const char* message, char *hmacdigest);
char* get_thing_register_json(char *json, struct thing *thing);
char* get_thing_heartbeat_json(char *json, struct thing *thing);
char* get_thing_data_json(char *json, struct thing *thing, char* value, char* waypoint);
int   parse_instruction_json(char *json, char *original_json, char *hash, char *thing_key, char *alert_key, char *instruction); 
const char* get_error_code_message(int error_code);
char* get_thing_alert_json(char *json, struct thing *thing, int level, char * message, char * data);
char* get_instruction_alert_json(char *json, char *alert_key, int level, char * message, char * data);
int translate_http_code(int http_code);
char* get_thing_bulk_data_json(char *json, char *packets, int packet_size, int count);
char* get_thing_data_json_ts(char *json, struct thing *thing, char *value, char *waypoint, double timestamp);
void  parse_http_ack(char *json, char *context, int *response_code);

#endif
