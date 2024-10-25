/*************************************************************************
	> File Name: net_ctl.c
	> Author: ligaofeng
	> Mail: ligaofeng01@baidu.com 
	> Created Time: 2020年02月13日 星期四 11时53分41秒
 ************************************************************************/

#include<stdio.h>

#include "bds_client_params.h"
#include "bds_client_command.h"

void test_start_link(void* handle)
{
    bds_client_command_t link_start = {
            .key = CMD_LINK_START,
            .content = NULL,
            .content_length = 0
    };
    bds_client_send(handle, &link_start);
}

void test_stop_link(void* handle)
{
    bds_client_command_t link_stop = {
            .key = CMD_LINK_STOP,
            .content = NULL,
            .content_length = 0
    };
    bds_client_send(handle, &link_stop);
}
