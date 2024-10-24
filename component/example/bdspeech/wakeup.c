/*************************************************************************
        > File Name: wakeup.c
        > Author: ligaofeng
        > Mail: ligaofeng01@baidu.com
        > Created Time: 2020年02月13日 星期四 20时06分12秒
 ************************************************************************/

#include <stdio.h>

#include "cJSON.h"
#include "bds_client_params.h"
#include "bds_client_command.h"

#define TAG "case_wakeup"

void test_start_wakeup(void* handle) {
    bdsc_wp_params_t     params       = {0};
    bds_client_command_t wakeup_start = {
        .key            = CMD_WAKEUP_START,
        .content        = &params,
        .content_length = sizeof(bdsc_wp_params_t),
    };
    bds_client_send(handle, &wakeup_start);
}

void test_stop_wakeup(void* handle) {
    bds_client_command_t wakeup_stop = {
        .key = CMD_WAKEUP_STOP,
    };
    bds_client_send(handle, &wakeup_stop);
}
