/*
 * aliotgateway_http.c
 *
 * HTTP based Implementation for posting data to Datonis
*/
#if _COMMUNICATION_MODE_HTTP_

#include <string.h>
#include <stdlib.h>
#include "aliotgateway.h"
#include "aliotutil.h"
#include "timeutil.h"
#include "MQTTMbed.h"
#include <wmstdio.h>

static Network n;

int connect_datonis() {
    return connect_datonis_instance("api.datonis.io");
}

int connect_datonis_instance(char *server) {
    /* Not implemented for HTTP mode */
    NewNetwork(&n);
    return ConnectNetwork(&n, server, 80);
}

int disconnect_datonis() {
     n.disconnect(&n);
     return 0;
}

void yield(int milliseconds) {
    /* NOOP for http */
}

int transmit_instruction_feedback(char * alert_key, int level, char *message, char *data) {
    /* HTTP does not have capabilities to execute instruction */
    return ERR_OK;
}

static int encode_and_send_data(const char * host, const char *url, const char *json, int flag) {
    char data[10000];
    char *token;
    char *saveptr1 = NULL;
    char buf[67];
    int http_code = 400;
        
    unsigned long long begin, end;
    char* hash = get_hmac(json, buf);

    connect_datonis();
    sprintf(data, "POST %s HTTP/1.1\r\nHost: %s\r\nAccept: */*\r\nX-Access-Key: %s\r\nX-Dtn-Signature: %s\r\nContent-Type: application/json\r\nContent-Length: %d\r\nCache-Control: no-cache\r\n\r\n%s", url, host, configuration.access_key, hash, strlen(json), json);
	begin = get_time_ms();
    n.mqttwrite(&n, data, strlen(data), 100);
    n.mqttread(&n, data, sizeof(data), 1000);
    wmprintf("Server Response: %s\n\r", data);

    token = &data[9];
    if (token != NULL) {
        token = strtok_r(token, " ", &saveptr1);
        //wmprintf("Token: %s\n\r", token);
        http_code = atoi(token);
    }

    disconnect_datonis();

	end = get_time_ms();

	wmprintf("Data Transmission Time: %lld\n\r", (end-begin));
	/* TODO receive response code */
    return http_code;
}

int register_thing(struct thing * thing) {
    char jsonbuf[2048];
    memset(jsonbuf, '\0', sizeof(jsonbuf));
    char *json = get_thing_register_json(jsonbuf, thing);
    return translate_http_code(encode_and_send_data("api.datonis.io", "/api/v3/things/register.json", json, 0));
}

int transmit_thing_heartbeat(struct thing * thing) {
    char jsonbuf[2048];
    memset(jsonbuf, '\0', sizeof(jsonbuf));
    char *json = get_thing_heartbeat_json(jsonbuf, thing);
    return translate_http_code(encode_and_send_data("api.datonis.io", "/api/v3/things/heartbeat.json", json, 0));
}

int transmit_thing_data(struct thing * thing, char* value, char* waypoint) {
    char jsonbuf[2048];
    memset(jsonbuf, '\0', sizeof(jsonbuf));
    char *json = get_thing_data_json(jsonbuf, thing, value, waypoint);
    return translate_http_code(encode_and_send_data("api.datonis.io", "/api/v3/things/event.json", json, 0));
}

int transmit_compressed_thing_data(struct thing *thing, char* value, char* waypoint) {
    char jsonbuf[2048];
    memset(jsonbuf, '\0', sizeof(jsonbuf));
    char *json = get_thing_data_json(jsonbuf, thing, value, waypoint);
    return translate_http_code(encode_and_send_data("api.datonis.io", "/api/v3/things/event.json", json, 1));
}

int transmit_thing_alert(struct thing * thing, int level, char *message, char *data) {
    char jsonbuf[4096];
    memset(jsonbuf, '\0', sizeof(jsonbuf));
    char *json = get_thing_alert_json(jsonbuf, thing, level, message, data);
    return translate_http_code(encode_and_send_data("api.datonis.io", "/api/v3/alerts.json", json, 0));
}

int transmit_thing_data_packet(char *packet) {
    return translate_http_code(encode_and_send_data("api.datonis.io", "/api/v3/things/event.json", packet,0));
}

int transmit_compressed_thing_data_packet(char *packet) {
    return translate_http_code(encode_and_send_data("api.datonis.io", "/api/v3/things/event.json", packet, 1));
}

#endif

