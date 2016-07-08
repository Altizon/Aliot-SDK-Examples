#include "aliotgateway.h"
#include "aliotutil.h"
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 10

void execute_instruction(char * thing_key, char * alert_key, char *instruction) {
    printf("\nExecute Instruction: %s\n", instruction);
    transmit_instruction_feedback(alert_key, 1, "A demo warning to show feedback", "{\"some_key\":\"some_value\"}");
}

static void transmit_heartbeat_to_datonis(struct thing *t) {
    int response = transmit_thing_heartbeat(t);
    if (response != ERR_OK) {
        fprintf(stderr, "Failed to send thing heartbeat. Response Code: %d, Error: %s\n", response, get_error_code_message(response));
    } else {
        printf("Heartbeat Response: %d\n", response);
    }
}

static void send_example_alert(struct thing *t, int level, char * msg) {
    int response = transmit_thing_alert(t, level, msg, "{\"foo\":\"bar\"}");
    if (response != ERR_OK) {
        fprintf(stderr, "Failed to send alert to datonis. Response code: %d, Error: %s\n", response, get_error_code_message(response));
    } else {
        printf("Successfully sent an alert to datonis with level: %d, message: %s\n", level, msg);
    }
}


static void send_example_alerts(struct thing *t) {
    send_example_alert(t, 0, "Example INFO alert from C Agent");
    send_example_alert(t, 1, "Example WARNING alert from C Agent");
    send_example_alert(t, 2, "Example ERROR alert from C Agent");
    send_example_alert(t, 3, "Example CRITICAL alert from C Agent");
}


int main() {
    struct thing t;
    char buf[500];
    char waypoint[50];
    char packet_buffer[BUFFER_SIZE][1024];

    //initialize(access_key, secret_key)
    initialize("1d2fb5c369863fd54afafde654c26dtd51122t8e", "f4e31122629etaeaa48d9c8c72b8cctfc9d63acc");
    //create_thing(struct thing *thing, char* key, char* name, char* description, instruction_handler handler)
    create_thing(&t, "7t38b9dbt9", "Compressor", "Thing for compressors", execute_instruction);

    int response = 0;
    response = connect_datonis();
    if (response != ERR_OK) {
        fprintf(stderr, "Failed to connect to Datonis!\n");
        exit(1);
    }
    response = register_thing(&t);
    if (response != ERR_OK) {
        fprintf(stderr, "Failed to Register Thing. Response Code: %d, Error: %s\n", response, get_error_code_message(response));
        exit(1);
    } else {
        printf("Successfully Registered thing with Datonis!\n");
    }

    if (1) {
        send_example_alerts(&t);
    }

    /* Send the first hearbeat. Next ones will be sent regulartly */
    transmit_heartbeat_to_datonis(&t);
    int counter = 0;
    while(1) {

        sprintf(buf, "{\"pressure\":%ld,\"temperature\":%ld}", random(), random());
	//waypoint format: [latitude, longitude], where latitude and longitude must be double values.	
	sprintf(waypoint, "[19.%ld,73.%ld]", random() % 100000, random() % 100000);
	//You can send both meta-data as well as waypoint in single request.
	//Pass NULL in place of buf or waypoint if you don't want to send that data.
	//Atleast one from buf or waypoint should be passed. Both cannot be NULL.
        create_thing_data_packet(packet_buffer[counter], &t, buf, waypoint);
	//create_thing_data_packet(packet_buffer[counter], &t, buf, NULL);
	//create_thing_data_packet(packet_buffer[counter], &t, NULL, waypoint);
        counter++;

        if (counter == BUFFER_SIZE) {
            char bulk_packet[1024 * BUFFER_SIZE];
            transmit_heartbeat_to_datonis(&t);
            create_thing_bulk_data_packet(bulk_packet, packet_buffer[0], 1024, BUFFER_SIZE);
            /*
             * Send data to Datonis in a compressed way
             * This is now the new default. This not only saves the amount of data sent over the wire.
             * but also optimizes the time it takes to send the packet.
             * If you still wish to send the data packet in an uncompressed way, comment the line below and uncomment the one after it
             */
            response = transmit_compressed_thing_data_packet(bulk_packet);
            // response = transmit_thing_data_packet(bulk_packet);
            if (response != ERR_OK) {
                fprintf(stderr, "Failed to send thing data. Response Code: %d, Error: %s\n", response, get_error_code_message(response));
            } else {
                printf("Transmitt Data Response: %d\n", response);
            }
            counter = 0;
        } else {
            printf("Buffered Data packet for later transmission\n");
        }

        yield(2000);
        sleep(3);
    }

    return 0;
}

