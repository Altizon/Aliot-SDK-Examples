

// #include "linux/Aliot/aliotgateway.h"
// #include "linux/Aliot/aliotutil.h"
#include <stdio.h>
#include <stdlib.h>
#include "iot_tcpip_interface.h"
#include "uip.h"
#include "uiplib.h"
#include "iot_api.h"
#include "string.h"
#include "webclient.h"
#include "uip_timer.h"
/*



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

*/
int iot_connect(u8_t *dst, u16_t port)
{
    UIP_CONN *conn=NULL;
    uip_ipaddr_t ip;

    if (uiplib_ipaddrconv((char *)dst,(unsigned char *)&ip) == 0) {
        return -1;
    }

    conn = uip_connect(&ip, htons(port));
    if (conn != NULL) {
        return conn->fd;
    } else {
        return -1;
    }
}

void iot_disconnect(u8_t fd)
{
    if (fd < TCPUDP_FD_MAGIC) {
        uip_conns[fd].appstate.state = IOT_APP_S_CLOSED;
    } else {
        uip_udp_remove(&uip_udp_conns[fd-TCPUDP_FD_MAGIC]);
    }
    return;
}

int iot_send1(UIP_CONN* tcp_conn, u8_t *buf, u16_t len)
{
    struct iot_tcp_app_state *s = &(tcp_conn->appstate);
    u8_t state = s->state;
printf_high("App state is %d \n" ,state);
    if (state == IOT_APP_S_CLOSED) {
        return -1;
    } else if (state == IOT_APP_S_DATA_SEND) {
        return -2;
    } else if (state == IOT_APP_S_WAIT_SEND) {
        return -3;
    } else {
        printf_high("Sending data -- \n");
        s->buf = buf;
        s->len = len;
        s->state = IOT_APP_S_WAIT_SEND;
        uip_poll_conn(tcp_conn);
        if (uip_len > 0) {
            uip_arp_out();
            mt76xx_dev_send();
            printf_high("Data sent\n");
        }

        return len;
    }
}


int ThisIsmain() {


    static UIP_CONN *tcp_conn=NULL;
    int len =0;
    char* data = "This is data";
/*
    uip_ipaddr_t raddr;
    u8_t iot_srv_ip[MAC_IP_LEN] = {216,58,199,142};
    u16_t server_port = 80;
    printf_high("Connecting to server %d %d %d %d \n", iot_srv_ip[0],iot_srv_ip[1], iot_srv_ip[2],iot_srv_ip[3]);
    int fd=-1;
    fd = iot_connect(iot_srv_ip,server_port);
    printf_high("FD is %d -\n",fd);
    iot_send(fd,data,12);
    iot_disconnect(fd);
*/

u16_t ipaddr[2];
uip_ipaddr(ipaddr, 192,168,43,58);
tcp_conn = uip_connect(ipaddr, HTONS(9999));
printf_high("FD is %d \n\n\n ",tcp_conn->fd);

if( tcp_conn ->fd != -1)
{
    len=iot_send1(tcp_conn,data,12);
printf_high("data length = %d \n",len);
uip_udp_remove(tcp_conn);
}



    // uip_ipaddr(raddr, iot_srv_ip[0],iot_srv_ip[1], iot_srv_ip[2],iot_srv_ip[3]);

    // /* Specify remote address and port here. */
    // tcp_conn = uip_connect(&raddr, HTONS(server_port));
    // if (tcp_conn) {
    //     printf_high("Value of tcp_conn %s \n",tcp_conn);

    //     tcp_conn->lport = HTONS(8888);
    //     printf_high("connection Successfully \n");
    //     //printf("fd %d uip_aborted.%d\n", tcp_conn->fd, HTONS(tcp_conn->lport));
    //     uip_send(data,12);
    // } else {
    //     printf_high("connect fail\n");
    // }
    // if(uip_closed())
    // {
    //     printf_high("UIP was not open");
    // }
    // else
    // {

    //     uip_close();
    //     printf_high("UIP was closed");
    // }
 printf_high("This is Main\n");
 /*
    struct thing t;
    char buf[500];
    char waypoint[50];
    //initialize(access_key, secret_key)
    initialize("15df71a657654b32116e71948e1b2ab8fe9e7771", "597ab884e9385tdf2a1c55eeaa8e16a47aeab3tc");
    //create_thing(struct thing *thing, char* key, char* name, char* description, instruction_handler handler)
    create_thing(&t, "77869dfd84", "APU099", "C Program Input", execute_instruction);

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

    int counter = 5;
    while(1) {

        if (counter == 5) {
            transmit_heartbeat_to_datonis(&t);
            counter = 0;
        }
        counter++;

        yield(2000);
        yield(2000);

        sprintf(buf, "{\"max\":%ld,\"min\":%ld}", random(), random());
	//waypoint format: [latitude, longitude], where latitude and longitude must be double values.	
	sprintf(waypoint, "[19.%ld,73.%ld]", random() % 100000, random() % 100000);

        /* You can send data to Datonis in a compressed form.
         * This not only reduces the network bandwidth usage for your agent
         * But also improves network latency to the Datonis server
         * This happens at the cost of some extra processing power needed on the device
         * You can still chose to send data in a uncompressed form
         
	//You can send both meta-data as well as waypoint in single request.
	//Pass NULL in place of buf or waypoint if you don't want to send that data.
	//Atleast one from buf or waypoint should be passed. Both cannot be NULL.
	response = transmit_compressed_thing_data(&t, buf, NULL);
	//response = transmit_compressed_thing_data(&t, NULL, waypoint);
	//response = transmit_compressed_thing_data(&t, buf, waypoint);
	
        /* Uncomment to send data in an uncompressed way 
        // response = transmit_thing_data(&t, buf, waypoint);

        if (response != ERR_OK) {
            fprintf(stderr, "Failed to send thing data. Response Code: %d, Error: %s\n", response, get_error_code_message(response));
        } else {
            printf("Transmitt Data Response: %d\n", response);
        }
        sleep(5);
    }
*/
    return 0;
}

