/*
 * Copyright (c) 2024 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "FreeRTOS.h"
#include "task.h"
#include "shell.h"
#include "lwip/init.h"
#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "netconf.h"


#define LOG_E printf
#define LOG_I printf
#define LOG_W printf

typedef uint32_t tick_t;
typedef TaskHandle_t thread_t;

#ifndef NAME_MAX
#define NAME_MAX 8
#endif

#define IPERF_PRIORITY  (configMAX_PRIORITIES - 10)
#define IPERF_THREAD_STACK_SIZE 4096
#define IPERF_PORT          5001
#define IPERF_BUFSZ         (64 * 1024)

#define IPERF_MODE_STOP     0
#define IPERF_MODE_SERVER   1
#define IPERF_MODE_CLIENT   2


typedef struct {
    int mode;
    char *host;
    int port;
} IPERF_PARAM;

static IPERF_PARAM param = { IPERF_MODE_STOP, NULL, IPERF_PORT };

static void iperf_udp_client(void *thread_param)
{
    (void) thread_param;
    struct sockaddr_in server;
    uint32_t packet_count = 0;
    uint64_t sentlen = 0;

    int send_size = IPERF_BUFSZ > 1470 ? 1470 : IPERF_BUFSZ;
    uint32_t *buffer = malloc(IPERF_BUFSZ);
    if (buffer == NULL) {
        return;
    }
    memset(buffer, 0x00, IPERF_BUFSZ);
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        LOG_E("can't create socket!\r\n");
        free(buffer);
        return;
    }
    server.sin_family = PF_INET;
    server.sin_port = htons(param.port);
    server.sin_addr.s_addr = inet_addr(param.host);
    LOG_I("iperf udp mode run...\r\n");
    uint32_t tick1 = xTaskGetTickCount();
    thread_t tid = xTaskGetCurrentTaskHandle();
    while (param.mode != IPERF_MODE_STOP) {
        packet_count++;
        uint32_t tick = xTaskGetTickCount();
        buffer[0] = htonl(packet_count);
        buffer[1] = htonl(tick / configTICK_RATE_HZ);
        buffer[2] = htonl((tick % configTICK_RATE_HZ) * 1000);
        int ret = sendto(sock, buffer, send_size, 0, (struct sockaddr *)&server, sizeof(struct sockaddr_in));
        if (ret > 0) {
            sentlen += ret;
        }
        if (ret < 0) {
            LOG_E("%s: failed to send data out\r\n", pcTaskGetName(tid));
            break;
        }

        uint32_t tick2 = xTaskGetTickCount();
        if (tick2 - tick1 >= configTICK_RATE_HZ * 5) {
            long data;
            int integer, decimal;
            data = sentlen * configTICK_RATE_HZ / 125 / (tick2 - tick1);
            integer = data / 1000;
            decimal = data % 1000;
            LOG_I("%s: %d.%03d0 Mbps!\r\n", pcTaskGetName(tid), integer, decimal);
            tick1 = tick2;
            sentlen = 0;
        }
    }
    LOG_I("Shutdown %s client!\r\n", pcTaskGetName(tid));
    param.mode = IPERF_MODE_STOP;
    closesocket(sock);
    free(buffer);

    vTaskDelete(NULL);
}

static void iperf_udp_server(void *thread_param)
{
    (void) thread_param;
    struct sockaddr_in server;
    struct sockaddr_in sender;
    int sender_len;
    uint32_t pcount = 0, last_pcount = 0;
    struct timeval timeout;

    uint32_t *buffer = malloc(IPERF_BUFSZ);
    if (buffer == NULL) {
        return;
    }
    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        LOG_E("can't create socket! exit!\r\n");
        goto __udp_server_close;
    }
    server.sin_family = PF_INET;
    server.sin_port = htons(param.port);
    server.sin_addr.s_addr = inet_addr("0.0.0.0");

    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        LOG_E("setsockopt failed!\r\n");
        goto __udp_server_close;
    }
    if (bind(sock, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) < 0) {
        LOG_E("iperf server bind failed! exit!\r\n");
        goto __udp_server_close;
    }
    while (param.mode != IPERF_MODE_STOP) {
        tick_t tick1 = xTaskGetTickCount();
        tick_t tick2 = tick1;
        uint32_t lost = 0;
        uint32_t total = 0;
        uint64_t sentlen = 0;
        while ((tick2 - tick1) < (configTICK_RATE_HZ * 5)) {
            int r_size = recvfrom(sock, buffer, IPERF_BUFSZ, 0, (struct sockaddr *)&sender, (socklen_t *)&sender_len);
            if (r_size > 12) {
                pcount = ntohl(buffer[0]);
                if (last_pcount < pcount) {
                    lost += pcount - last_pcount - 1;
                    total += pcount - last_pcount;
                }
                last_pcount = pcount;
                sentlen += r_size;
            }
            tick2 = xTaskGetTickCount();
        }
        if (sentlen > 0) {
            thread_t tid = xTaskGetCurrentTaskHandle();
            long data = sentlen * configTICK_RATE_HZ / 125 / (tick2 - tick1);
            int integer = data / 1000;
            int decimal = data % 1000;
            LOG_I("%s: %d.%03d0 Mbps! lost:%d total:%d\r\n", pcTaskGetName(tid), integer, decimal, lost, total);
        }
    }
__udp_server_close:
    param.mode = IPERF_MODE_STOP;
    free(buffer);
    closesocket(sock);

    vTaskDelete(NULL);
}

static void iperf_client(void *thread_param)
{
    (void) thread_param;
    int tips = 1;
    struct sockaddr_in addr;

    uint8_t *send_buf = (uint8_t *) malloc(IPERF_BUFSZ);
    if (!send_buf) {
        return;
    }

    for (int i = 0; i < IPERF_BUFSZ; i++) {
        send_buf[i] = i & 0xff;
    }

    while (param.mode != IPERF_MODE_STOP) {
        int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock < 0) {
            LOG_E("create socket failed!\r\n");
            vTaskDelay(configTICK_RATE_HZ);
            continue;
        }

        addr.sin_family = PF_INET;
        addr.sin_port = htons(param.port);
        addr.sin_addr.s_addr = inet_addr((char *)param.host);

        int ret = connect(sock, (const struct sockaddr *)&addr, sizeof(addr));
        if (ret == -1) {
            if (tips) {
                LOG_E("Connect to iperf server failed, Waiting for the server to open!\r\n");
                tips = 0;
            }
            closesocket(sock);
            vTaskDelay(configTICK_RATE_HZ);
            continue;
        }

        LOG_I("Connect to iperf server successful!\r\n"); {
            int flag = 1;

            setsockopt(sock,
                       IPPROTO_TCP,
                       /* set option at TCP level */
                       TCP_NODELAY,
                       /* name of option */
                       (void *) &flag,
                       /* the cast is historical cruft */
                       sizeof(int)); /* length of option value */
        }

        uint64_t sentlen = 0;

        tick_t tick1 = xTaskGetTickCount();
        while (param.mode != IPERF_MODE_STOP) {
            tick_t tick2 = xTaskGetTickCount();
            if (tick2 - tick1 >= configTICK_RATE_HZ * 5) {
                thread_t tid = xTaskGetCurrentTaskHandle();

                long data = sentlen * configTICK_RATE_HZ / 125 / (tick2 - tick1);
                int integer = data / 1000;
                int decimal = data % 1000;
                LOG_I("%s: %d.%03d0 Mbps!\r\n", pcTaskGetName(tid), integer, decimal);
                tick1 = tick2;
                sentlen = 0;
            }

            ret = send(sock, send_buf, IPERF_BUFSZ, 0);
            if (ret > 0) {
                sentlen += ret;
            }

            if (ret < 0) {
                break;
            }
        }

        closesocket(sock);

        vTaskDelay(configTICK_RATE_HZ * 2);
        LOG_W("Disconnected, iperf server shut down!\r\n");
        tips = 1;
    }
    free(send_buf);
    param.mode = IPERF_MODE_STOP;
    vTaskDelete(NULL);
}

void iperf_server(void *thread_param)
{
    (void) thread_param;
    socklen_t sin_size;
    int sock = -1;
    struct sockaddr_in server_addr, client_addr;
    fd_set readset;
    struct timeval timeout;

    uint8_t *recv_data = (uint8_t *) malloc(IPERF_BUFSZ);
    if (recv_data == NULL) {
        LOG_E("No memory!\r\n");
        goto __exit;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        LOG_E("Socket error!\r\n");
        goto __exit;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(param.port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), 0x0, sizeof(server_addr.sin_zero));

    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        LOG_E("Unable to bind!\r\n");
        goto __exit;
    }

    if (listen(sock, 5) == -1) {
        LOG_E("Listen error!\r\n");
        goto __exit;
    }

    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    while (param.mode != IPERF_MODE_STOP) {
        FD_ZERO(&readset);
        FD_SET(sock, &readset);

        if (select(sock + 1, &readset, NULL, NULL, &timeout) == 0) {
            continue;
        }
        sin_size = sizeof(struct sockaddr_in);

        int connected = accept(sock, (struct sockaddr *)&client_addr, &sin_size);

        LOG_I("new client connected from (%s, %d)\r\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        int flag = 1;

        setsockopt(connected,
                   IPPROTO_TCP,
                   /* set option at TCP level */
                   TCP_NODELAY,
                   /* name of option */
                   (void *) &flag,
                   /* the cast is historical cruft */
                   sizeof(int)); /* length of option value */

        uint64_t recvlen = 0;
        tick_t tick1 = xTaskGetTickCount();
        while (param.mode != IPERF_MODE_STOP) {
            int bytes_received = recv(connected, recv_data, IPERF_BUFSZ, 0);
            if (bytes_received <= 0) {
                break;
            }
            recvlen += bytes_received;

            tick_t tick2 = xTaskGetTickCount();
            if (tick2 - tick1 >= configTICK_RATE_HZ * 5) {
                thread_t tid = xTaskGetCurrentTaskHandle();
                long data = recvlen * configTICK_RATE_HZ / 125 / (tick2 - tick1);
                int integer = data / 1000;
                int decimal = data % 1000;
                LOG_I("%s: %d.%03d0 Mbps!\r\n", pcTaskGetName(tid), integer, decimal);
                tick1 = tick2;
                recvlen = 0;
            }
        }
        LOG_W("client disconnected (%s, %d)\r\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        if (connected >= 0) {
            closesocket(connected);
        }
    }

__exit:
    if (sock >= 0) {
        closesocket(sock);
    }
    if (recv_data) {
        free(recv_data);
    }
    param.mode = IPERF_MODE_STOP;
    vTaskDelete(NULL);
}

void iperf_usage(void)
{
    printf("Usage: iperf [-s|-c host] [options] [multi-threaded]\n");
    printf("       iperf [-h|--stop]\n");
    printf("\n");
    printf("Client/Server:\n");
    printf("  -p #         server port to listen on/connect to\n");
    printf("\n");
    printf("Server specific:\n");
    printf("  -s           run in server mode\n");
    printf("\n");
    printf("Client specific:\n");
    printf("  -c <host>    run in client mode, connecting to <host>\n");
    printf("\n");
    printf("Miscellaneous:\n");
    printf("  -h           print this message and quit\n");
    printf("  --stop       stop iperf program\n");
    printf("  -u           testing UDP protocol\n");
}

int iperf(int argc, char **argv)
{
    int mode = 0; /* server mode */
    char *host = NULL;
    int port = IPERF_PORT;
    int numtid = 1;
    int use_udp = 0;
    int index = 1;

    if (argc == 1) {
        goto __usage;
    }
    if (strcmp(argv[1], "-u") == 0) {
        index = 2;
        use_udp = 1;
    }
    if (strcmp(argv[index], "-h") == 0) {
        goto __usage;
    } else if (strcmp(argv[index], "--stop") == 0) {
        /* stop iperf */
        param.mode = IPERF_MODE_STOP;
        return 0;
    } else if (strcmp(argv[index], "-s") == 0) {
        mode = IPERF_MODE_SERVER; /* server mode */

        /* iperf -s -p 5000 */
        if (argc >= 4) {
            if (strcmp(argv[index + 1], "-p") == 0) {
                port = atoi(argv[index + 2]);
            } else {
                goto __usage;
            }
        }
    } else if (strcmp(argv[index], "-c") == 0) {
        mode = IPERF_MODE_CLIENT; /* client mode */
        if (argc < 3) {
            goto __usage;
        }

        host = argv[index + 1];
        if (argc >= 5) {
            /* iperf -c host -p port */
            if (strcmp(argv[index + 2], "-p") == 0) {
                port = atoi(argv[index + 3]);
            } else {
                goto __usage;
            }
        }
    } else {
        goto __usage;
    }

    if (argc >= 7) {
        if (strcmp(argv[argc - 2], "-m") == 0) {
            numtid = atoi(argv[argc - 1]);
        } else {
            goto __usage;
        }
    }

    /* start iperf */
    if (param.mode == IPERF_MODE_STOP) {
        int i = 0;
        char tid_name[NAME_MAX + 1] = { 0 };

        param.mode = mode;
        param.port = port;
        if (param.host) {
            free(param.host);
            param.host = NULL;
        }
        if (host) {
            param.host = strdup(host);
        }

        for (i = 0; i < numtid; i++) {
            void (*function)(void *parameter) = NULL;

            if (use_udp) {
                if (mode == IPERF_MODE_CLIENT) {
                    snprintf(tid_name, sizeof(tid_name), "iperfc%02d", i + 1);
                    function = iperf_udp_client;
                } else if (mode == IPERF_MODE_SERVER) {
                    snprintf(tid_name, sizeof(tid_name), "iperfd%02d", i + 1);
                    function = iperf_udp_server;
                }
            } else {
                if (mode == IPERF_MODE_CLIENT) {
                    snprintf(tid_name, sizeof(tid_name), "iperfc%02d", i + 1);
                    function = iperf_client;
                } else if (mode == IPERF_MODE_SERVER) {
                    snprintf(tid_name, sizeof(tid_name), "iperfd%02d", i + 1);
                    function = iperf_server;
                }
            }

            if (function != NULL) {
                int error = xTaskCreate(function,
                                        tid_name,
                                        IPERF_THREAD_STACK_SIZE / sizeof(uint32_t),
                                        NULL,
                                        IPERF_PRIORITY,
                                        NULL);
                if (error != pdTRUE) {
                    LOG_E("Failed to create task:%s!\r\n", tid_name);
                }
            }
        }
    } else {
        printf("Please stop iperf firstly, by:\n");
        printf("iperf --stop\n");
    }

    return 0;

__usage:
    iperf_usage();
    return 0;
}

CSH_CMD_EXPORT_ALIAS(iperf, iperf, "iperf")
