/*
 * aliotgateway_http.c
 *
 * HTTP based Implementation for posting data to Datonis
*/
#if _COMMUNICATION_MODE_HTTP_
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include "aliotgateway.h"
#include "aliotutil.h"
#include "timeutil.h"
#include "LZFcompress.h"

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

int transmit_instruction_feedback(char * alert_key, int level, char *message, char *data) {
    /* HTTP does not have capabilities to execute instruction */
    return ERR_OK;
}

static int encode_and_send_data(const char *url, const char *json, int flag) {
    CURL *curl;
    CURLcode res;
    long http_code = 0;
    unsigned char compressedBuffer[MAX_BLOCKSIZE + MAX_HDR_SIZE + 16];
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if (curl) {
        char buf[67];
        char* hash = get_hmac(json, buf);
        struct curl_slist *header = NULL;

        char accessheader[1024];
        char signature[1024];
	double begin, end;

        memset(accessheader, sizeof(accessheader), '\0');
        memset(signature, sizeof(signature), '\0');


        sprintf(accessheader, "X-Access-Key:%s", configuration.access_key);
        sprintf(signature, "X-Dtn-Signature:%s", hash);

	printf("accessheader: %s\nsignature: %s", accessheader, hash);

	header = curl_slist_append(header, accessheader);
        header = curl_slist_append(header, signature);
	if(strlen(json)>1024 && flag == 1)
	{
		char compressed[25];
		char contentType[50];
		ssize_t len = 0;
		memset(compressed, sizeof(compressed), '\0');
		memset(contentType, sizeof(contentType), '\0');

		sprintf(compressed, "X-Compressed:%s", "LZF");
		sprintf(contentType, "Content-Type:%s", "application/octet-stream");
		header = curl_slist_append(header, compressed);
		header = curl_slist_append(header, contentType);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
		begin = get_time_ms();
		printf("Original Data Size: %zd\n", strlen(json));
		len = LZFcompress_data(json, compressedBuffer);
		end = get_time_ms();
		printf("Compressed Data Size: %zd\n", len);
		printf("Compression Time: %lf\n", (end-begin));
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, len);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, compressedBuffer);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_URL, url);
	        res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
	        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json);
	}
	begin = get_time_ms();
        curl_easy_perform(curl);
	end = get_time_ms();

	printf("Data Transmission Time: %lf\n", (end-begin));
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        curl_slist_free_all(header);// necessary otherwise memory leaks..
    }
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return http_code;
}

int register_thing(struct thing * thing) {
    char jsonbuf[2048];
    memset(jsonbuf, '\0', sizeof(jsonbuf));
    char *json = get_thing_register_json(jsonbuf, thing);
    return translate_http_code(encode_and_send_data("http://api.datonis.io/api/v3/things/register.json", json, 0));
}

int transmit_thing_heartbeat(struct thing * thing) {
    char jsonbuf[2048];
    memset(jsonbuf, '\0', sizeof(jsonbuf));
    char *json = get_thing_heartbeat_json(jsonbuf, thing);
    return translate_http_code(encode_and_send_data("http://api.datonis.io/api/v3/things/heartbeat.json", json, 0));
}

int transmit_thing_data(struct thing * thing, char* value, char* waypoint) {
    char jsonbuf[2048];
    memset(jsonbuf, '\0', sizeof(jsonbuf));
    char *json = get_thing_data_json(jsonbuf, thing, value, waypoint);
    return translate_http_code(encode_and_send_data("http://api.datonis.io/api/v3/things/event.json", json, 0));
}

int transmit_compressed_thing_data(struct thing *thing, char* value, char* waypoint) {
    char jsonbuf[2048];
    memset(jsonbuf, '\0', sizeof(jsonbuf));
    char *json = get_thing_data_json(jsonbuf, thing, value, waypoint);
    return translate_http_code(encode_and_send_data("http://api.datonis.io/api/v3/things/event.json", json, 1));
}

int transmit_thing_alert(struct thing * thing, int level, char *message, char *data) {
    char jsonbuf[4096];
    memset(jsonbuf, '\0', sizeof(jsonbuf));
    char *json = get_thing_alert_json(jsonbuf, thing, level, message, data);
    return translate_http_code(encode_and_send_data("http://api.datonis.io/api/v3/alerts.json", json, 0));
}

int transmit_thing_data_packet(char *packet) {
    return translate_http_code(encode_and_send_data("http://api.datonis.io/api/v3/things/event.json", packet,0));
}

int transmit_compressed_thing_data_packet(char *packet) {
    return translate_http_code(encode_and_send_data("http://api.datonis.io/api/v3/things/event.json", packet, 1));
}

#endif

