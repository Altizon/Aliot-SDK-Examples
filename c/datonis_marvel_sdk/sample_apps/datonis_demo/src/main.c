/**
 * Datonis Demo
 *
 */

#include <wm_os.h>
#include <wmstdio.h>
#include <wmtime.h>
#include <wmsdk.h>
#include <led_indicator.h>
#include <board.h>
#include <push_button.h>
#include "aliotgateway.h"
#include "aliotutil.h"

enum state {
	DATONIS_CONNECTED = 1,
	DATONIS_RECONNECTED,
	DATONIS_DISCONNECTED
};


/* These hold each pushbutton's count, updated in the callback ISR */
static volatile uint32_t pushbutton_a_count = 0;
static volatile uint32_t pushbutton_b_count = 0;
static volatile uint32_t led_1_state;
static volatile uint32_t led_1_state_prev = -1;

static output_gpio_cfg_t led_1;
static enum state device_state;

static os_thread_t datonis_demo_thread;
 /* Buffer to be used as stack */
static os_thread_stack_define(datonis_demo_stack, 50 * 1024);

#define RESET_TO_FACTORY_TIMEOUT 5000

/* callback function invoked on reset to factory */
static void device_reset_to_factory_cb()
{
	/* Clears device configuration settings from persistent memory
	 * and reboots the device.
	 */
	invoke_reset_to_factory();
}

/* board_button_2() is configured to perform reset to factory,
 * when pressed for more than 5 seconds.
 */
static void configure_reset_to_factory()
{
	input_gpio_cfg_t pushbutton_reset_to_factory = {
		.gpio = board_button_2(),
		.type = GPIO_ACTIVE_LOW
	};
	push_button_set_cb(pushbutton_reset_to_factory,
			   device_reset_to_factory_cb,
			   RESET_TO_FACTORY_TIMEOUT, 0, NULL);
}


/* callback function invoked when pushbutton_a is pressed */
static void pushbutton_a_cb()
{
	wmprintf("Button A pressed!\n\r");
	pushbutton_a_count++;
}

/* callback function invoked when pushbutton_b is pressed */
static void pushbutton_b_cb()
{
	wmprintf("Button B pressed!\n\r");
	pushbutton_b_count++;
}

/* Configure led and pushbuttons with callback functions */
static void configure_led_and_button()
{
	/* respective GPIO pins for pushbuttons and leds are defined in
	 * board file.
	 */
	input_gpio_cfg_t pushbutton_a = {
		.gpio = board_button_1(),
		.type = GPIO_ACTIVE_LOW
	};
	input_gpio_cfg_t pushbutton_b = {
		.gpio = board_button_2(),
		.type = GPIO_ACTIVE_LOW
	};

	led_1 = board_led_1();

	push_button_set_cb(pushbutton_a,
			   pushbutton_a_cb,
			   100, 0, NULL);
	push_button_set_cb(pushbutton_b,
			   pushbutton_b_cb,
			   100, 0, NULL);
}

void execute_instruction(char * thing_key, char * alert_key, char *instruction) {
    wmprintf("\nExecute Instruction: %s\n\r", instruction);

    wmprintf("\nDone executing instruction\n\r");

    transmit_instruction_feedback(alert_key, 1, "A demo warning to show feedback", "{\"some_key\":\"some_value\"}");
}

/* application thread */
static void datonis_demo(os_thread_arg_t data)
{

	struct thing t;
    char buf[500];
    char waypoint[50];

    initialize("93a768t53fd3e21tf7cc357c9b3f2f446297d872", "cefdf5e449c93e32567155f6ddct9d47e38tfd57");
    create_thing(&t, "d171366c3e", "M1", "MW300", execute_instruction);


    int response = 0;
    response = connect_datonis();
    if (response != ERR_OK) {
        wmprintf("Failed to connect to Datonis!\n\r");
        goto out;
    }
    response = register_thing(&t);
    if (response != ERR_OK) {
        wmprintf("Failed to Register Thing. Response Code: %d, Error: %s\n\r", response, get_error_code_message(response));
        goto out;
    } else {
        wmprintf("Successfully Registered thing with Datonis!\n\r");
    }

	/* indication that device is connected and cloud is started */
	led_on(board_led_2());
	wmprintf("Cloud Started\r\n");

    /*
    if (1) {
        send_example_alerts(&t);
    } */

    int counter = 5;
    while(1) {

        if (counter == 5) {
            transmit_thing_heartbeat(&t);
            counter = 0;
        }
        counter++;

        yield(2000);
        yield(2000);

        sprintf(buf, "{\"button_1\":%ld,\"button_2\":%ld}", pushbutton_a_count, pushbutton_b_count);
		//sprintf(waypoint, "[19.%ld,73.%ld]", random() % 100000, random() % 100000);

        /* You can send data to Datonis in a compressed form.
         * This not only reduces the network bandwidth usage for your agent
         * But also improves network latency to the Datonis server
         * This happens at the cost of some extra processing power needed on the device
         * You can still chose to send data in a uncompressed form
         */
	//You can send both meta-data as well as waypoint in single request.
	//Pass NULL in place of buf or waypoint if you don't want to send that data.
	//Atleast one from buf or waypoint should be passed. Both cannot be NULL.
		response = transmit_thing_data(&t, buf, NULL);
	//response = transmit_compressed_thing_data(&t, NULL, waypoint);
	//response = transmit_compressed_thing_data(&t, buf, waypoint);
	
        /* Uncomment to send data in an uncompressed way */
        // response = transmit_thing_data(&t, buf, waypoint);

        if (response != ERR_OK) {
            wmprintf("Failed to send thing data. Response Code: %d, Error: %s\n\r", response, get_error_code_message(response));
        } else {
            wmprintf("Transmitt Data Response: %d\n\r", response);
        }
        os_thread_sleep(os_msec_to_ticks(5000));
    }


out:
	os_thread_self_complete(NULL);
	return;
}


/* This function gets invoked when station interface connects to home AP.
 * Network dependent services can be started here.
 */
void wlan_event_normal_connected(void *data)
{
	int ret;
	/* Changed time to today */
	time_t time = 1468002977;

	wmprintf("Connected successfully to the configured network\r\n");

	if (!device_state) {
		/* set system time */
		wmtime_time_set_posix(time);

		/* create cloud thread */
		ret = os_thread_create(
			/* thread handle */
			&datonis_demo_thread,
			/* thread name */
			"Datonis-Demo",
			/* entry function */
			datonis_demo,
			/* argument */
			0,
			/* stack */
			&datonis_demo_stack,
			/* priority */
			OS_PRIO_3);
		if (ret != WM_SUCCESS) {
			wmprintf("Failed to start cloud_thread: %d\r\n", ret);
			return;
		}
	}

	if (!device_state)
		device_state = DATONIS_CONNECTED;
	else if (device_state == DATONIS_DISCONNECTED)
		device_state = DATONIS_RECONNECTED;
}


//#define WLAN_SSID "ALTIZON2.4"
//#define WLAN_PASSWORD "AltizonWirelesS"


//#define WLAN_SSID "TWEVENT"
//#define WLAN_PASSWORD "TWpune@1234"

#define WLAN_SSID "Rajesh Jangam"
#define WLAN_PASSWORD "12345678"

int main()
{
	char ip[16];
	wmprintf("Starting main");
	/* initialize the standard input output facility over uart */
	if (wmstdio_init(UART0_ID, 0) != WM_SUCCESS) {
		return -WM_FAIL;
	}

	/* initialize gpio driver */
	if (gpio_drv_init() != WM_SUCCESS) {
		wmprintf("gpio_drv_init failed\r\n");
		return -WM_FAIL;
	}

	wmprintf("Build Time: " __DATE__ " " __TIME__ "\r\n");
	wmprintf("\r\n#### DATONIS DEMO ####\r\n\r\n");

	/* configure pushbutton on device to perform reset to factory */
	configure_reset_to_factory();

	/* configure led and pushbutton to communicate with cloud */
	configure_led_and_button();

	//wm_wlan_start(WLAN_SSID, WLAN_PASSWORD);
	wm_wlan_connect(WLAN_SSID, WLAN_PASSWORD);
	os_thread_sleep(os_msec_to_ticks(30000));

	app_network_ip_get(ip);
	wmprintf("Connected to provisioned network with ip address =%s \n", ip);
	return 0;
}