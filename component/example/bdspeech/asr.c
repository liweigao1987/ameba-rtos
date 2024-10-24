/*************************************************************************
        > File Name: asr.c
        > Author: ligaofeng
        > Mail: ligaofeng01@baidu.com
        > Created Time: 2020年02月13日 星期四 20时06分26秒
 ************************************************************************/

#include <stdio.h>

#include "cJSON.h"
#include "bds_client_params.h"
#include "bds_client_command.h"

extern int generate_pam(char* pam_prama);
extern char* get_wifi_mac();

void         test_start_asr(void* handle, int back_time) {
    char sn[37];
    bds_generate_uuid(sn);
    char* pam_data = malloc(4096);
    generate_pam(pam_data);
    bdsc_asr_params_t*   asr_params = bdsc_asr_params_create(sn, 1775, "com.baidu.iot", get_wifi_mac(), back_time, 0,
                                                           BDS_ASR_CONTACT_ON, strlen(pam_data) + 1, pam_data);
    bds_client_command_t asr_start = {
        .key            = CMD_ASR_START,
        .content        = asr_params,
        .content_length = sizeof(bdsc_asr_params_t) + strlen(pam_data) + 1,
    };
    bds_client_send(handle, &asr_start);
    bdsc_asr_params_destroy(asr_params);
    free(pam_data);
}

void test_cancel_asr(void* handle) {
    bds_client_command_t asr_cancel = {.key = CMD_ASR_CANCEL};
    bds_client_send(handle, &asr_cancel);
}
