/*
 * aliotgateway_http.c
 *
 * HTTP based Implementation for posting data to Datonis
 */
#ifndef _COMMUNICATION_MODE_HTTP_

#ifndef __TEMP
#define __TEMP

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "aliotgateway.h"
#include "aliotutil.h"
#include "webclient.h"
#include "iot_tcp_app.h"
#include "iot_api.h"
#include "uip_timer.h"
#define ERR_OK -1
#define get_time_ms iot_get_ms_time

#define strcat my_strcat

char local_my_web_data[500];
char jsonbuf[300];
int MyHttpPort = 0;
bool ReadyFlag = TRUE;

int connect_datonis() {
	/* Not implemented for HTTP mode */
	return ERR_OK;
}

int connect_datonis_instance(char *server) {
	/* Not implemented for HTTP mode */
}

int disconnect_datonis() {
	/* Not implemented for HTTP mode */
}

void yield(int milliseconds) {
	/* NOOP for http */
}

int transmit_instruction_feedback(char * alert_key, int level, char *message,
		char *data) {
	/* HTTP does not have capabilities to execute instruction */
	return ERR_OK;
}

void ConnectionClosed(void) {
	printf_high("Connection Closed\n");
	ReadyFlag = FALSE;
}

void ConnectionAcked(void) {
	printf_high("Connection Acknowledged\n");
}

void ResponseFromServer(void) {
	printf_high("TCP Client RX [%d] bytes\n", uip_datalen());
	iot_uart_output(uip_appdata, uip_datalen());
}

void SendData(void) {
	printf_high("Sending Data %s \n", local_my_web_data);
	if (strlen(local_my_web_data) != 0) {
		uip_send(local_my_web_data, strlen(local_my_web_data));
		memset(local_my_web_data, 0, sizeof(local_my_web_data));
	}
}

int encode_and_send_data(const char *host, const char *path, const char *json) {
	char buf[67];
	char* hash = get_hmac(json, buf);
	if (!ReadyFlag)
	{
		app_init_connection();
		ReadyFlag = TRUE;
	}

	if (strlen(local_my_web_data) == 0) { //&&  app_init_connection()) {
		sprintf(local_my_web_data,
				"POST %s HTTP/1.1\r\nHost: %s\r\nAccept: */*\r\nX-Access-Key: %s\r\nX-Dtn-Signature: %s\r\nContent-Type: application\/json\r\nContent-Length: %d\r\nCache-Control: no-cache\r\n\r\n%s",
				path, host, configuration.access_key, hash, strlen(json), json);
		printf_high("Packet is - %s\n", local_my_web_data);
	}
	return 1;
}

int register_thing(struct thing * thing) {
//	char jsonbuf[2048];
	memset(jsonbuf, '\0', sizeof(jsonbuf));
	char *json = get_thing_register_json(jsonbuf, thing);
	printf_high("Register thing only");
	return encode_and_send_data("http://api.datonis.io",
			"/api/v3/things/register.json", json);
}

int transmit_thing_heartbeat(struct thing * thing) {
//	char jsonbuf[2048];
	memset(jsonbuf, '\0', sizeof(jsonbuf));
	char *json = get_thing_heartbeat_json(jsonbuf, thing);
	return translate_http_code(
			encode_and_send_data("http://api.datonis.io",
					"/api/v3/things/heartbeat.json", json));
}

int transmit_thing_data(struct thing * thing, char* value, char* waypoint) {
//	char jsonbuf[2048];
	memset(jsonbuf, '\0', sizeof(jsonbuf));
	char *json = get_thing_data_json(jsonbuf, thing, value, waypoint);
	return translate_http_code(
			encode_and_send_data("http://api.datonis.io",
					"/api/v3/things/event.json", json));
}

int transmit_thing_alert(struct thing * thing, int level, char *message,
		char *data) {
//	char jsonbuf[4096];
	memset(jsonbuf, '\0', sizeof(jsonbuf));
	char *json = get_thing_alert_json(jsonbuf, thing, level, message, data);
	return translate_http_code(
			encode_and_send_data("http://api.datonis.io", "/api/v3/alerts.json",
					json));
}

int transmit_thing_data_packet(char *packet) {
	return translate_http_code(
			encode_and_send_data("http://api.datonis.io",
					"/api/v3/things/event.json", packet));
}

#endif
#endif

