#include "aliotgateway.h"
#include "aliotutil.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void execute_instruction(char * thing_key, char * alert_key, char *instruction) {
    printf("\nExecute Instruction: %s\n", instruction);
    transmit_instruction_feedback(alert_key, 1, "A demo warning to show feedback", "{\"some_key\":\"some_value\"}");
}

static void transmit_heartbeat_to_datonis(struct thing *s) {
    int response = transmit_thing_heartbeat(s);
    if (response != ERR_OK) {
        fprintf(stderr, "Failed to send thing heartbeat. Response Code: %d, Error: %s\n", response, get_error_code_message(response));
    } else {
        printf("Heartbeat Response: %d\n", response);
    }
}

int main(int argc, char *argv[]) {
    struct thing s;
    char buf[500];
    char waypoint[50];
    int response = 0;

    if (argc < 11) {
        fprintf(stderr, "\nUsage: %s <server-name> <access-key> <secret-key> <thing-key> <thing-name> <thing-type> <thing-description> <thing-metadata> <mode> <mode-data> <timestamp> [waypoint]", argv[0]);
        exit(1);
    }

    initialize(argv[2], argv[3]);
    create_thing(&s, argv[4], argv[5], argv[6], argv[7], argv[8], execute_instruction);

    response = connect_datonis_instance(argv[1]);
    if (response != ERR_OK) {
        fprintf(stderr, "Failed to connect to Datonis!\n");
        exit(1);
    }

    if (!strcasecmp(argv[9], "register") || !strcasecmp(argv[9], "instruction")) {
        response = register_thing(&s);
        if (response != ERR_OK) {
            fprintf(stderr, "Failed to Register Thing. Response Code: %d, Error: %s\n", response, get_error_code_message(response));
            exit(1);
        } else {
            printf("Successfully Registered thing with Datonis!\n");
        }
    }

    if (!strcasecmp(argv[9], "events")) {
        char packet[1024];
	if (argc == 12) {
            create_thing_data_packet_ts(packet, &s, argv[10], argv[12], atof(argv[11]));
	else {
	    create_thing_data_packet_ts(packet, &s, argv[10], atof(argv[11]));
	}
        response = transmit_thing_data_packet(packet);
        if (response != ERR_OK) {
            fprintf(stderr, "Failed to send thing data. Response Code: %d, Error: %s\n", response, get_error_code_message(response));
        } else {
            printf("Transmitt Data Response: %d\n", response);
        }
    } else if (!strcasecmp(argv[9], "instruction")) {
        yield(10000);
    } else if (!strcasecmp(argv[9], "alert")) {
        int response = transmit_thing_alert(&s, 0, argv[10], "{\"foo\":\"bar\"}");
        if (response != ERR_OK) {
            fprintf(stderr, "Failed to send alert to datonis. Response code: %d, Error: %s\n", response, get_error_code_message(response));
        } else {
            printf("Successfully sent an alert to datonis with level: %d, message: %s\n", 0, argv[10]);
        }
    }
    disconnect_datonis();

    return 0;
}

