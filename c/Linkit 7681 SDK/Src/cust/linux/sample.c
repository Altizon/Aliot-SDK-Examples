#include "Aliot/aliotgateway.h"
#include "Aliot/aliotutil.h"
#include <stdio.h>
#include <stdlib.h>
#define printf printf_high
#define fprintf printf_high
// #define ERR_OK -1

void execute_instruction(char * thing_key, char * alert_key, char *instruction) {
	printf("\nExecute Instruction: %s\n", instruction);
	transmit_instruction_feedback(alert_key, 1,
			"A demo warning to show feedback", "{\"some_key\":\"some_value\"}");
}

static void transmit_heartbeat_to_datonis(struct thing *t) {
	int response = transmit_thing_heartbeat(t);
	if (response != ERR_OK) {
		fprintf(stderr,
				"Failed to send thing heartbeat. Response Code: %d, Error: %s\n",
				response, get_error_code_message(response));
	} else {
		printf("Heartbeat Response: %d\n", response);
	}
}

static void send_example_alert(struct thing *t, int level, char * msg) {
	int response = transmit_thing_alert(t, level, msg, "{\"foo\":\"bar\"}");
	if (response != ERR_OK) {
		fprintf(stderr,
				"Failed to send alert to datonis. Response code: %d, Error: %s\n",
				response, get_error_code_message(response));
	} else {
		printf(
				"Successfully sent an alert to datonis with level: %d, message: %s\n",
				level, msg);
	}
}

static void send_example_alerts(struct thing *t) {
	send_example_alert(t, 0, "Example INFO alert from C Agent");
	send_example_alert(t, 1, "Example WARNING alert from C Agent");
	send_example_alert(t, 2, "Example ERROR alert from C Agent");
	send_example_alert(t, 3, "Example CRITICAL alert from C Agent");
}

int ThisIsNotmain() {
	char buf[100] = "{\"max\":45,\"min\":75}";
	char waypoint[50];
	static uint32 Counter = 0;
	struct thing t;
	//initialize(access_key, secret_key)
	initialize("15df71a657654b32116e71948e1b2ab8fe9e7771",
			"597ab884e9385tdf2a1c55eeaa8e16a47aeab3tc");
	//create_thing(struct thing *thing, char* key, char* name, char* description, instruction_handler handler)
	create_thing(&t, "77869dfd84", "APU099", "C Program Input",
			execute_instruction);
	if (Counter == 0) {
		register_thing(&t);
	}
	if (Counter % 30 == 29) {
		send_example_alert(&t, 3, "Example CRITICAL alert from C Agent");
	}
	else if (Counter % 15 == 14) {
	 strcpy(buf, "{\"max\":45,\"min\":75}");
	 sprintf(buf, "{\"max\":%ld,\"min\":%ld}", 45, 75);
	 sprintf(waypoint, "[19.%ld,73.%ld]", 7584 % 100000, 8476 % 100000);
	 printf_high("%s ",buf);
	 transmit_thing_data(&t, buf, waypoint);
	}
	else if (Counter % 2 == 1) {
		transmit_heartbeat_to_datonis(&t);
	}
	Counter++;
}
