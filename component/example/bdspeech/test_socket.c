#include <stdlib.h>

#include "basic_types.h"
#include "lwip_netconf.h"
#include "lwipconf.h"
#include "platform_stdlib.h"
#include "rtw_wifi_defs.h"
#include "wifi_conf.h"

static char      server_ip[32]      = {0};
static int       server_port        = 0;
static char      send_buffer[850]   = {1};

static void socket_run() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("sockfd=%d\n", sockfd);
    struct timeval timeout = {2 * 30000, 0};
    setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
    int flag = 1;
    setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(server_port);
    inet_pton(AF_INET, server_ip, &serv_addr.sin_addr.s_addr);
    /* serv_addr.sin_addr.s_addr = ((struct in_addr*)(hptr->h_addr))->s_addr; */

    int ret = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (ret != 0) {
        printf("connect failed! ret=%d\n", ret);
        return;
    }
    printf("connect success!\n");
    while (1) {
        printf("send +\n");
        ret = send(sockfd, send_buffer, 850, 0);
        printf("send ret=%d\n", ret);
        vTaskDelay(100);
    }
    rtos_task_delete(NULL);
}

void test_socket(char* ip, char* port) {
    memcpy(server_ip, ip, strlen(ip));
    server_port = atoi(port);
    printf("ip=%s, port=%d\n", server_ip, server_port);
    if (rtos_task_create(NULL, ((const char*)"test_socket"), socket_run, NULL, 1024 * 4, 1) != SUCCESS) {
        RTK_LOGS("test", "\n\r%s rtos_task_create(init_thread) failed", __FUNCTION__);
    }
}
