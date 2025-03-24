
/*******************************************************************************
 * Copyright 2025 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file httpserver-netconn.c
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief File implements a webserver to track the receive and transmit statistics of
 * the CoreTSE.
 *
 * Please refer to the file README.md for further details about this example.
 *
 */

#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"

#include "httpserver-netconn.h"
#include "microchip_logo.h"
#include "http_index_html.h"

#include "lwip/debug.h"
#include "lwip/err.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

#include "drivers/fpga_ip/CoreTSE/core_tse.h"

#include "ethernet_status.h"

#include <assert.h>

#if LWIP_NETCONN

#ifndef HTTPD_DEBUG
#define HTTPD_DEBUG LWIP_DBG_OFF
#endif

extern tse_instance_t *g_tse;

ethernet_status_t g_ethernet_status;

/*------------------------------------------------------------------------------
 * External functions.
 */
extern uint32_t get_ip_address(void);
void get_mac_address(uint8_t *mac_addr);

/*------------------------------------------------------------------------------
 *
 */
const static char http_html_hdr[] = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\n\r\n";
const static char http_json_hdr[] =
    "HTTP/1.1 200 OK\r\nContent-type: application/jsonrequest\r\n\r\n";

const static char http_post_resp_hdr[] = "HTTP/1.1 204 No Content\r\n\r\n";
const static char http_html_ok_hdr[] = "HTTP/1.1 200 OK\r\n\r\n";

/*------------------------------------------------------------------------------
 *
 */
static char status_json[800];

/*------------------------------------------------------------------------------
 *
 */

const char *mac_speed_lut[] = {"10Mbps", "100Mbps", "1000Mbps", "invalid"};

const char *duplex_mode_lut[] = {"half duplex", "full duplex"};
/*------------------------------------------------------------------------------
 *
 */
/** Serve one HTTP connection accepted in the http thread */
static void
http_server_netconn_serve(struct netconn *conn)
{
    struct netbuf *inbuf;
    char *buf;
    u16_t buflen;
    err_t err;

    /* Read the data from the port, blocking if nothing yet there.
    We assume the request (the part we care about) is in one netbuf */
    err = netconn_recv(conn, &inbuf);

    if (err == ERR_OK)
    {
        netbuf_data(inbuf, (void **)&buf, &buflen);

        /* Is this an HTTP GET command? (only check the first 5 chars, since
        there are other formats for GET, and we're keeping it very simple )*/
        if (buflen >= 5 && buf[0] == 'G' && buf[1] == 'E' && buf[2] == 'T' && buf[3] == ' ' &&
            buf[4] == '/')
        {
            if (buf[5] == 's')
            {
                int json_resp_size;
                uint32_t ip_addr;
                uint8_t mac_addr[6];
                uint32_t statistic;
                uint8_t link_up = 0;

                ip_addr = get_ip_address();
                get_mac_address(mac_addr);

                link_up = TSE_get_link_status(g_tse,
                                              &g_ethernet_status.speed,
                                              &g_ethernet_status.duplex_mode);

                if (g_ethernet_status.speed > TSE_INVALID_SPEED)
                    g_ethernet_status.speed = TSE_INVALID_SPEED;

                if (g_ethernet_status.duplex_mode > TSE_FULL_DUPLEX)
                    g_ethernet_status.duplex_mode = TSE_FULL_DUPLEX;

                json_resp_size = snprintf(
                    status_json,
                    sizeof(status_json),
                    "{\r\n\"M_A\": \"%02x:%02x:%02x:%02x:%02x:%02x\",\"TCPIP_Addr\": \"%d.%d.%d.%d\",\r\n\"LinkSpeed\": \"%s\"\r,\
                                          \r\n",
                    mac_addr[0],
                    mac_addr[1],
                    mac_addr[2],
                    mac_addr[3],
                    mac_addr[4],
                    mac_addr[5],
                    (int)(ip_addr & 0x000000FFu),
                    (int)((ip_addr >> 8u) & 0x000000FFu),
                    (int)((ip_addr >> 16u) & 0x000000FFu),
                    (int)((ip_addr >> 24u) & 0x000000FFu),
                    mac_speed_lut[g_ethernet_status.speed]);

                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"DPM\": \"%s\", ",
                                           duplex_mode_lut[g_ethernet_status.duplex_mode]);

                statistic = TSE_read_stat(g_tse, TSE_RX_BYTE_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S0\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_RX_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S1\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_RX_FCS_ERR_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S2\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_RX_MULTICAST_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S3\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_RX_BROADCAST_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S4\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_RX_CTRL_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S5\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_RX_PAUSE_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S6\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_RX_UNKNOWN_OPCODE_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S7\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_RX_ALIGN_ERR_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S8\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_RX_FRAMELENGTH_ERR_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S9\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_RX_CODE_ERR_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S10\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_RX_CS_ERR_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S11\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_RX_UNDERSIZE_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S12\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_RX_OVERSIZE_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S13\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_RX_FRAGMENT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S14\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_RX_JABBER_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S15\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_RX_DROP_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S16\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_FRAME_CNT_64);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S17\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_FRAME_CNT_127);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S18\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_FRAME_CNT_255);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S19\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_FRAME_CNT_511);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S20\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_FRAME_CNT_1K);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S21\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_FRAME_CNT_MAX);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S22\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_FRAME_CNT_VLAN);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S23\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_TX_BYTE_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S24\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_TX_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S25\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_TX_MULTICAST_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S26\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_TX_BROADCAST_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S27\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_TX_PAUSE_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S28\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_TX_DEFFERAL_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S29\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_TX_EXCS_DEFFERAL_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S30\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_TX_SINGLE_COLL_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S31\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_TX_MULTI_COLL_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S32\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_TX_LATE_COLL_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S33\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_TX_EXCSS_COLL_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S34\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_TX_TOTAL_COLL_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S35\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_TX_PAUSE_HONORED_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S36\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_TX_DROP_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S37\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_TX_JABBER_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S38\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_TX_FCS_ERR_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S39\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_TX_CNTRL_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S40\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_TX_OVERSIZE_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S41\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_TX_UNDERSIZE_PKT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S42\": \"%d\", ",
                                           (int)statistic);

                statistic = TSE_read_stat(g_tse, TSE_TX_FRAGMENT_CNT);
                json_resp_size += snprintf(&status_json[json_resp_size],
                                           sizeof(status_json),
                                           "\"S43\": \"%d\"} ",
                                           (int)statistic);

                assert(json_resp_size < sizeof(status_json));
                if (json_resp_size > sizeof(status_json))
                {
                    json_resp_size = sizeof(status_json);
                }

                /* Send the HTML header
                 * subtract 1 from the size, since we dont send the \0 in the string
                 * NETCONN_NOCOPY: our data is const static, so no need to copy it
                 */
                netconn_write(conn, http_json_hdr, sizeof(http_json_hdr) - 1, NETCONN_NOCOPY);

                /* Send our HTML page */
                netconn_write(conn, status_json, json_resp_size - 1, NETCONN_NOCOPY);
            }
            else if (buf[5] == 'm')
            {
                netconn_write(conn, microchip_logo, sizeof(microchip_logo) - 1, NETCONN_NOCOPY);
            }
            else if (buf[5] == 'c')
            {
                TSE_clear_statistics(g_tse);
                netconn_write(conn, http_html_ok_hdr, sizeof(http_html_ok_hdr) - 1, NETCONN_NOCOPY);
            }
            else
            {
                /* Send the HTML header
                 * subtract 1 from the size, since we dont send the \0 in the string
                 * NETCONN_NOCOPY: our data is const static, so no need to copy it
                 */
                netconn_write(conn, http_html_hdr, sizeof(http_html_hdr) - 1, NETCONN_NOCOPY);

                /* Send our HTML page */
                netconn_write(conn, http_index_html, sizeof(http_index_html) - 1, NETCONN_NOCOPY);
            }
        }
        else if (buflen >= 6 && buf[0] == 'P' && buf[1] == 'O' && buf[2] == 'S' && buf[3] == 'T' &&
                 buf[4] == ' ' && buf[5] == '/')
        {
            err = netconn_recv(conn, &inbuf);

            ++err;

            /* Send the HTML header
             * subtract 1 from the size, since we dont send the \0 in the string
             * NETCONN_NOCOPY: our data is const static, so no need to copy it
             */
            netconn_write(conn, http_post_resp_hdr, sizeof(http_post_resp_hdr) - 1, NETCONN_NOCOPY);
        }
    }
    /* Close the connection (server closes in HTTP) */
    netconn_close(conn);

    /* Delete the buffer (netconn_recv gives us ownership,
      so we have to make sure to deallocate the buffer) */
    netbuf_delete(inbuf);
}

// extern tse_instance_t g_tse;
/** The main function, never returns! */
void
http_server_netconn_thread(void *arg)
{
    uint8_t link_up;

    struct netconn *conn, *newconn;
    err_t err;
    ethernet_status_t ethernet_status;
    portBASE_TYPE queue_rx;
    g_ethernet_status.speed = TSE_MAC10MBPS;
    g_ethernet_status.duplex_mode = TSE_FULL_DUPLEX;

    /* Create a new TCP connection handle */
    conn = netconn_new(NETCONN_TCP);
    LWIP_ERROR("http_server: invalid conn", (conn != NULL), return;);

    /* Bind to port 80 (HTTP) with default IP address */
    netconn_bind(conn, IP_ADDR_ANY, 80);

    /* Put the connection into LISTEN state */
    netconn_listen(conn);

    do
    {
        err = netconn_accept(conn, &newconn);
        if (err == ERR_OK)
        {
            http_server_netconn_serve(newconn);
            netconn_delete(newconn);
        }
    } while (err == ERR_OK);

    LWIP_DEBUGF(
        HTTPD_DEBUG,
        ("http_server_netconn_thread: netconn_accept received error %d, shutting down", err));
    netconn_close(conn);
    netconn_delete(conn);
}

/** Initialize the HTTP server (start its thread) */
void
http_server_netconn_init(void)
{
    sys_thread_new("http_server_netconn",
                   http_server_netconn_thread,
                   NULL,
                   DEFAULT_THREAD_STACKSIZE,
                   DEFAULT_THREAD_PRIO);
}

#endif /* LWIP_NETCONN*/
