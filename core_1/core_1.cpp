/**
 * Copyright (c) 2022 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <string.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/dns.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "hardware/rtc.h"
#include "pico/util/datetime.h"
#include "pico/util/queue.h"
#include "pico/multicore.h"

extern "C"{
 // #include "lwipopts.h"  
}
#include "wifi_data.h"
#include "core_1.h"

queue_t q_date_time;
bool date_received = false;


// Called with results of operation
static void ntp_result(NTP_T* state, int status, time_t *result) {
    datetime_t dt;
    if (status == 0 && result) {
        struct tm *utc = gmtime(result);
        printf("c1: got ntp response: %02d/%02d/%04d %02d:%02d:%02d\n", utc->tm_mday, utc->tm_mon + 1, utc->tm_year + 1900,
               (utc->tm_hour)+2, utc->tm_min, utc->tm_sec);

            dt.year = utc->tm_year+1900;
            dt.month = utc->tm_mon+1;
            dt.day = utc->tm_mday;
            dt.dotw = 6;
            dt.hour = ((utc->tm_hour)+2)%24;
            dt.min = utc->tm_min;
            dt.sec = utc->tm_sec;
            //printf("c1: put val in q\n");
            //queue_add_blocking(&q_date_time,&dt);
            //printf("c1: val added\n");
            rtc_init();
            if(rtc_set_datetime(&dt)){
                date_received = true;
            }
            printf("c1: uhr läuft");
            //busy_wait_ms(1);
            //rtc_get_datetime(&dt);
            //printf("%d:%d:%d\n",dt.hour,dt.min,dt.sec);

    }

    if (state->ntp_resend_alarm > 0) {
        cancel_alarm(state->ntp_resend_alarm);
        state->ntp_resend_alarm = 0;
    }
    state->ntp_test_time = make_timeout_time_ms(NTP_TEST_TIME);
    state->dns_request_sent = false;
}



// Make an NTP request
static void ntp_request(NTP_T *state) {
    // cyw43_arch_lwip_begin/end should be used around calls into lwIP to ensure correct locking.
    // You can omit them if you are in a callback from lwIP. Note that when using pico_cyw_arch_poll
    // these calls are a no-op and can be omitted, but it is a good practice to use them in
    // case you switch the cyw43_arch type later.
    cyw43_arch_lwip_begin();
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, NTP_MSG_LEN, PBUF_RAM);
    uint8_t *req = (uint8_t *) p->payload;
    memset(req, 0, NTP_MSG_LEN);
    req[0] = 0x1b;
    udp_sendto(state->ntp_pcb, p, &state->ntp_server_address, NTP_PORT);
    pbuf_free(p);
    cyw43_arch_lwip_end();
}

static int64_t ntp_failed_handler(alarm_id_t id, void *user_data)
{
    NTP_T* state = (NTP_T*)user_data;
    printf("c1: ntp request failed\n");
    ntp_result(state, -1, NULL);
    return 0;
}

// Call back with a DNS result
static void ntp_dns_found(const char *hostname, const ip_addr_t *ipaddr, void *arg) {
    NTP_T *state = (NTP_T*)arg;
    if (ipaddr) {
        state->ntp_server_address = *ipaddr;
        printf("c1: ntp address %s\n", ipaddr_ntoa(ipaddr));
        ntp_request(state);
    } else {
        printf("c1: ntp dns request failed\n");
        ntp_result(state, -1, NULL);
    }
}

// NTP data received
static void ntp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {
    NTP_T *state = (NTP_T*)arg;
    uint8_t mode = pbuf_get_at(p, 0) & 0x7;
    uint8_t stratum = pbuf_get_at(p, 1);

    // Check the result
    if (ip_addr_cmp(addr, &state->ntp_server_address) && port == NTP_PORT && p->tot_len == NTP_MSG_LEN &&
        mode == 0x4 && stratum != 0) {
        uint8_t seconds_buf[4] = {0};
        pbuf_copy_partial(p, seconds_buf, sizeof(seconds_buf), 40);
        uint32_t seconds_since_1900 = seconds_buf[0] << 24 | seconds_buf[1] << 16 | seconds_buf[2] << 8 | seconds_buf[3];
        uint32_t seconds_since_1970 = seconds_since_1900 - NTP_DELTA;
        time_t epoch = seconds_since_1970;
        ntp_result(state, 0, &epoch);
    } else {
        printf("c1: invalid ntp response\n");
        ntp_result(state, -1, NULL);
    }
    pbuf_free(p);
}

// Perform initialisation
static NTP_T* ntp_init(void) {
    NTP_T *state = (NTP_T*)calloc(1, sizeof(NTP_T));
    if (!state) {
        printf("c1: failed to allocate state\n");
        return NULL;
    }
    state->ntp_pcb = udp_new_ip_type(IPADDR_TYPE_ANY);
    if (!state->ntp_pcb) {
        printf("c1: failed to create pcb\n");
        free(state);
        return NULL;
    }
    udp_recv(state->ntp_pcb, ntp_recv, state);
    return state;
}

// Runs ntp test forever
void run_ntp_test(void) {
    NTP_T *state = ntp_init();
    if (!state)
        return;
    while(!date_received) {
        if (absolute_time_diff_us(get_absolute_time(), state->ntp_test_time) < 0 && !state->dns_request_sent) {
            // Set alarm in case udp requests are lost
            state->ntp_resend_alarm = add_alarm_in_ms(NTP_RESEND_TIME, ntp_failed_handler, state, true);

            // cyw43_arch_lwip_begin/end should be used around calls into lwIP to ensure correct locking.
            // You can omit them if you are in a callback from lwIP. Note that when using pico_cyw_arch_poll
            // these calls are a no-op and can be omitted, but it is a good practice to use them in
            // case you switch the cyw43_arch type later.
            cyw43_arch_lwip_begin();
            int err = dns_gethostbyname(NTP_SERVER, &state->ntp_server_address, ntp_dns_found, state);
            cyw43_arch_lwip_end();

            state->dns_request_sent = true;
            if (err == ERR_OK) {
                ntp_request(state); // Cached result
            } else if (err != ERR_INPROGRESS) { // ERR_INPROGRESS means expect a callback
                printf("c1: dns request failed\n");
                ntp_result(state, -1, NULL);
            }
        }
#if PICO_CYW43_ARCH_POLL
        // if you are using pico_cyw43_arch_poll, then you must poll periodically from your
        // main loop (not from a timer interrupt) to check for Wi-Fi driver or lwIP work that needs to be done.
        cyw43_arch_poll();
        // you can poll as often as you like, however if you have nothing else to do you can
        // choose to sleep until either a specified time, or cyw43_arch_poll() has work to do:
        cyw43_arch_wait_for_work_until(state->dns_request_sent ? at_the_end_of_time : state->ntp_test_time);
#else
        // if you are not using pico_cyw43_arch_poll, then WiFI driver and lwIP work
        // is done via interrupt in the background. This sleep is just an example of some (blocking)
        // work you might be doing.
        sleep_ms(1000);
#endif
    }
    free(state);
    printf("c1: date received, nothing more to do for me\n");
}

int core_1_main(void){
    printf("c1: cor1main accessed\n");
    if (cyw43_arch_init()) {
        printf("c1: failed to initialise\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();
    bool not_connected = true;
    while(not_connected){
        not_connected = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000);
        if(not_connected == false) break;
        printf("c1: faled to connect ...retry\n");
        sleep_ms(30000);
    }
    printf("c1: connected!\n");
    run_ntp_test();
    cyw43_arch_deinit();
    return 0;
}