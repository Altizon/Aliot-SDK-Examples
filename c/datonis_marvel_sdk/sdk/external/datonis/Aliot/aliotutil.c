/*
 * AliotUtil.c
 *
 *  Created on: 03-Feb-2014
 *      Author: Ranjit Nair
 */

#include "aliotutil.h"
#include "aliotgateway.h"
#include "jsonutils.h"
#include "timeutil.h"
#include "hmac.h"
#include "jsmn.h"
#include <string.h>
#include <stdio.h>

char* get_hmac(const char* message, char *hmacdigest) {

    hmac_sha256 hmac;
    int i;

    hmac_sha256_initialize(&hmac, configuration.secret_key, strlen(configuration.secret_key));
    hmac_sha256_update(&hmac, message, strlen(message));
    hmac_sha256_finalize(&hmac, NULL, 0);

    char *ptr = hmacdigest;
    for (i = 0; i < 32; ++i) {
        sprintf(ptr, "%02lx", (unsigned long ) hmac.digest[i]);
        ptr += 2;
    }
    hmacdigest[64] = '\0';

    return hmacdigest;

}

/*
 * Gets the registration packet for a thing->
 *
 */
char* get_thing_register_json(char *json, struct thing *thing) {
	startJSON(json);
    double timestamp = get_time_ms();
    putJSONDoubleAndComma(json, "timestamp", timestamp);
    putJSONStringAndComma(json, "name", thing->name);
    if (thing->description)
	putJSONStringAndComma(json, "description", thing->description);
    putJSONStringAndComma(json, "thing_key", thing->key);
    strcat(json, "\"bi_directional\":");
    (thing->handler == NULL) ? strcat(json, "0") : strcat(json, "1");
    endJSON(json);
    return json;
}

char* get_thing_heartbeat_json(char *json, struct thing *thing) {

	double timestamp = get_time_ms();

	startJSON(json);

	//First add the device JSON
	putJSONStringAndComma(json, "thing_key", thing->key);
	putJSONDouble(json, "timestamp", timestamp);

	endJSON(json);
	return json;

}

char* get_thing_bulk_data_json(char *json, char *packets, int packet_size, int count) {
    int i = 0;
    char *sep = "";
    startJSON(json);
    strcat(json, "\"events\":[");
    for (i = 0; i < count; i++) {
        strcat(json, sep);
        strcat(json, packets);
        packets += packet_size;
        sep = ",";
    }
    strcat(json, "]");
    endJSON(json);
    return json;
}

char* get_thing_data_json_ts(char *json, struct thing *thing, char *value, char *waypoint, double timestamp) {
    startJSON(json);

    putJSONDoubleAndComma(json, "timestamp", timestamp);
    putJSONStringAndComma(json, "thing_key", thing->key);
    if (value != NULL) {
        strcat(json,"\"data\":");
        strcat(json, value);
    }
    
    if (waypoint != NULL) {
	if (value != NULL)
	    strcat(json,",");
        strcat(json,"\"waypoint\":");
        strcat(json, waypoint);
    }
    endJSON(json);
    return json;
}

char* get_thing_data_json(char * json, struct thing *thing, char* value, char *waypoint) {
	double timestamp = get_time_ms();
    return get_thing_data_json_ts(json, thing, value, waypoint, timestamp);
}

static char *get_alert_json(char *json, struct thing *thing, char *alert_key, int level, char *message, char *data) {
    double timestamp = get_time_ms();
    startJSON(json);

    strcat(json, "\"alert\":{");

	putJSONDoubleAndComma(json, "timestamp", timestamp);
    if (thing != NULL) {
    	putJSONStringAndComma(json, "thing_key", thing->key);
    }
    if (alert_key != NULL) {
        putJSONStringAndComma(json, "alert_key", alert_key);
    }
    	putJSONIntAndComma(json, "alert_type", level);
	putJSONString(json, "message", message);

    if (data != NULL) {
        strcat(json, ",\"data\":");
        strcat(json, data);
    }

    endJSON(json);
    endJSON(json);
    return json;
}

char* get_thing_alert_json(char *json, struct thing *thing, int level, char * message, char * data) {
    return get_alert_json(json, thing, NULL, level, message, data);
}

char* get_instruction_alert_json(char *json, char *alert_key, int level, char *message, char *data) {
    return get_alert_json(json, NULL, alert_key, level, message, data);
}

static int json_handler(const char *js, jsmntok_t *t, size_t count, char *current, char *hash, char *thing_key, char *alert_key, char *instruction, int *state) {
    int i, j, k;
    char buf[200];

    if (count == 0) {
        return 0;
    }

    if (t->type == JSMN_PRIMITIVE) {
        sprintf(buf, "%.*s", t->end - t->start, js+t->start);
        strcat(current, buf);
        return 1;
    } else if (t->type == JSMN_STRING) {
        sprintf(buf, "%.*s", t->end - t->start, js+t->start);
        if (*state == 1) {
            strcpy(hash, buf);
            *state = 0;
            return 1;
        } else if (*state == 2) {
            *state = 0;
            return 1;
        } else if (*state == 3) {
            strcpy(alert_key, buf);
        } else if (*state == 4) {
            strcpy(thing_key, buf);
        }

        if (!strcmp(buf, "instruction")) {
            *state = 5;
        } else if (!strcmp(buf, "hash")) {
            *state = 1;
            return 1;
        } else if (!strcmp(buf, "access_key")) {
            *state = 2;
            return 1;
        } else if (!strcmp(buf, "alert_key")) {
            *state = 3;
        } else if (!strcmp(buf, "thing_key")) {
            *state = 4;
        } else  {
            *state = 0;
        }
        strcat(current, "\"");
        strcat(current, buf);
        strcat(current, "\"");
        return 1;
    } else if (t->type == JSMN_OBJECT) {
        char new_object[2048];
        int org_state = *state;
        strcpy(new_object, "{");
        j = 0;
        for (i = 0; i < t->size; i++) {
            int key_skipped = 0;
            j += json_handler(js, t+1+j, count-j, new_object, hash, thing_key, alert_key, instruction, state);
            if (*state != 1 && *state != 2) {
                strcat(new_object, ":");
            } else {
                key_skipped = 1;
            }
            j += json_handler(js, t+1+j, count-j, new_object, hash, thing_key, alert_key, instruction, state);
            if (!key_skipped) {
                strcat(new_object, ",");
            }
        }
        new_object[strlen(new_object) - 1] = '\0';
        strcat(new_object, "}");
        strcat(current, new_object);
        if (org_state == 5) {
           strcpy(instruction, new_object);
        } 
        return j+1;
    } else if (t->type == JSMN_ARRAY) {
        char new_object[1024];
        int org_state = *state;
        j = 0;
        strcpy(new_object, "[");
        for (i = 0; i < t->size; i++) {
            j += json_handler(js, t+1+j, count-j, new_object, hash, thing_key, alert_key, instruction, state);
            strcat(new_object, ",");
        }
        new_object[strlen(new_object) - 1] = '\0';
        strcat(new_object, "]");
        strcat(current, new_object);
        if (org_state == 5) {
            strcpy(instruction, new_object);
        }
        return j+1;
    } 
    return 0;
}

int parse_instruction_json(char *json, char *original_json, char *hash, char *thing_key, char *alert_key, char *instruction) {
    jsmntok_t tokens[100];
    jsmn_parser p;
    int r, status;

    //printf("----Input json-----\n-%s-\n", json);
    jsmn_init(&p);
    r = jsmn_parse(&p, json, strlen(json), tokens, 100);
    if (r < 0) {
        fprintf(stderr, "Could not parse json: %s. Return code: %d", json, r);
        return r;
    } 
    strcpy(original_json, "");
    status = 0;
    json_handler(json, tokens, p.toknext, original_json, hash, thing_key, alert_key, instruction, &status);
    return 0;
}

static int ack_handler(const char *js, jsmntok_t *t, size_t count, int *state, char *context, int *http_code) {
    char buf[1000];
    int i, j;
    if (count == 0) {
        return 0;
    }

    if (t->type == JSMN_PRIMITIVE) {
        sprintf(buf, "%.*s", t->end - t->start, js+t->start);
        if (*state == 2) {
            *http_code = atoi(buf);
        }
        *state = 0;
        return 1;
    } else if (t->type == JSMN_STRING) {
        sprintf(buf, "%.*s", t->end - t->start, js+t->start);
        if (*state == 1) {
            strcpy(context, buf);
        } else if (*state == 2) {
            *http_code = atoi(buf);
        } else if (!strcmp(buf, "context")) {
            *state = 1;
            return 1;
        } else if (!strcmp(buf, "http_code")) {
            *state = 2;
            return 1;
        }
        *state = 0;
        return 1;
    } else if (t->type == JSMN_OBJECT) {
        j = 0;
        for (i = 0; i < t->size; i++) {
            j += ack_handler(js, t+1+j, count-j, state, context, http_code);
            j += ack_handler(js, t+1+j, count-j, state, context, http_code);
        }
        return j+1;
    } else if (t->type == JSMN_ARRAY) {
        j = 0;
        for (i = 0; i < t->size; i++) {
            j += ack_handler(js, t+1+j, count-j, state, context, http_code);
        }
        return j+1;
    }
    return 0;
}

void  parse_http_ack(char *json, char *context, int *response_code) {
    jsmntok_t tokens[100];
    jsmn_parser p;
    int r, status;

    //printf("----Input json-----\n-%s-\n", json);
    jsmn_init(&p);
    r = jsmn_parse(&p, json, strlen(json), tokens, 100);
    if (r < 0) {
        fprintf(stderr, "Could not parse json: %s. Return code: %d", json, r);
    }
    status = 0;
    ack_handler(json, tokens, p.toknext, &status, context, response_code);
}

int translate_http_code(int http_code) {
   switch (http_code) {
    case 200:
    case 202:
        return ERR_OK;
    case 401:
        return ERR_UNAUTHORIZED;
    case 406:
        return ERR_NOT_ACCEPTABLE;
    case 422:
        return ERR_INVALID_REQUEST;
    case 429:
        return ERR_EXCESSIVE_RATE;
    case 408:
        return ERR_TIMED_OUT;
    case 400:
        return ERR_BAD_REQUEST;
    case 500:
        return ERR_INTERNAL_SERVER_ERROR;
    default:
        return ERR_FAILED;
    }
    return ERR_FAILED;
}

const char* get_error_code_message(int error_code) {
    switch (error_code) {
        case ERR_UNAUTHORIZED:
            return "Unauthorized access. Please check your access and secret key";

        case ERR_EXCESSIVE_RATE:
            return "You are pushing data at a rate that is greater than what your license allows";

        case ERR_INVALID_REQUEST:
            return "Request is invalid";

        case ERR_FAILED:
            return "Failed to send data. Reasons are unknown";

        case ERR_NOT_ACCEPTABLE:
            return "Failed to send data. Request is unacceptable";

        case ERR_NO_CONNECTION:
            return "Not connected";

        case ERR_TIMED_OUT:
            return "Timed out while receiving a response from the server. Please try again";

        case ERR_BAD_REQUEST:
            return "Request could not be processed. Please check the parameters supplied";

        case ERR_INTERNAL_SERVER_ERROR:
            return "Server failed to process the request. Please check the JSON format of the request sent. If the problem persists, contact Datonis Support.";

        default:
            return "Failed to send data.";
    }
} 

