/*************************************************************************
        > File Name: event_upload.c
        > Author: ligaofeng
        > Mail: ligaofeng01@baidu.com
        > Created Time: 2020年02月13日 星期四 11时53分59秒
 ************************************************************************/

#include <stdio.h>

#include "bds_client_params.h"
#include "bds_client_command.h"

extern int generate_pam(char* pam_prama);
extern char*     get_wifi_mac();

void test_start_event(void* handle) {
    /*
     * CMD_EVENTUPLOAD_START,CMD_EVENTUPLOAD_CANCEL,CMD_EVENTUPLOAD_DATA
     */
    // start
    char sn[37];
    bds_generate_uuid(sn);
    char* pam_data = rtos_mem_malloc(4096);
    generate_pam(pam_data);
    bdsc_eventupload_params_t* event_params =
        bdsc_event_params_create(sn, 1775, "com.baidu.iot", get_wifi_mac(), strlen(pam_data) + 1, pam_data);
    bds_client_command_t event_start = {
        .key            = CMD_EVENTUPLOAD_START,
        .content        = event_params,
        .content_length = sizeof(bdsc_eventupload_params_t) + strlen(pam_data) + 1,
    };
    bds_client_send(handle, &event_start);
    bdsc_event_params_destroy(event_params);
    // data
    char*                dcs_data   = "{\"version\":100,\"id\":123,\"code\":2,\"path\":\"test_event\"}";
    bdsc_cmd_data_t*     data       = bdsc_cmd_data_create(FLAG_TAIL, strlen(dcs_data), (uint8_t*)dcs_data, sn);
    bds_client_command_t event_data = {
        .key            = CMD_EVENTUPLOAD_DATA,
        .content        = data,
        .content_length = sizeof(bdsc_cmd_data_t) + strlen(dcs_data),
    };
    bds_client_send(handle, &event_data);
    bdsc_cmd_data_destroy(data);
}
