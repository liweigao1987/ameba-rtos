#include "bds_client_command.h"
#include "bds_client_event.h"
#include "bds_client_params.h"
#include "bds_common_utility.h"

#define TAG "case_init"

extern int32_t bdsc_event_callback(bds_client_event_t* event, void* custom);

void* test_create_sdk() {
    /* ESP_LOGI(TAG, "%s", __func__); */
    bds_client_context_t context;
    void*                handle = bds_client_create(&context);
    bds_client_set_event_listener(handle, bdsc_event_callback, handle);
    bds_set_log_level(3);
    return handle;
}

void test_start_sdk(void* handle) {
    bds_client_start(handle);
    /* ESP_LOGW(TAG, "dsp_version=%s", bds_dsp_get_version()); */
    /* ESP_LOGW(TAG, "dsp_productor_version=%s", bds_dsp_get_productor_version()); */
}

void test_stop_sdk(void* handle) {
    bds_client_stop(handle);
}

void test_destory_sdk(void* handle) {
    bds_client_destroy(handle);
}

extern char* get_wifi_mac();
extern char* get_dcs_pam();

void test_config_sdk(void* handle) {
    // "10.99.197.131", 8110   lee.baidu.com  PROTOCOL_TLS PROTOCOL_DEFAULT
    char sn[37];
    bds_generate_uuid(sn);
    char* pam_data = get_dcs_pam();
    char* mac      = get_wifi_mac();
    /* ESP_LOGW(TAG, "mac=%s", mac); */
    bdsc_engine_params_t* engine_params = bdsc_engine_params_create(
        sn, 1775, "leetest.baidu.com", 443, PROTOCOL_TLS, mac, "com.baidu.iot", strlen(pam_data) + 1, pam_data);
   /* bdsc_engine_params_t* engine_params = bdsc_engine_params_create( */
   /*      sn, 1775, "leetest.baidu.com", 80, PROTOCOL_DEFAULT, mac, "com.baidu.iot", strlen(pam_data) + 1, pam_data); */
 
    /* bdsc_engine_params_t* engine_params = bdsc_engine_params_create( */
    /*     sn, 1775, "10.174.33.38", 8114, PROTOCOL_DEFAULT, mac, "com.baidu.iot", strlen(pam_data) + 1, pam_data); */

    bds_client_params_t params;
    memset(&params, 0, sizeof(bds_client_params_t));
    params.engine_params = engine_params;
    bds_client_config(handle, &params);
    bdsc_engine_params_destroy(engine_params);
    rtos_mem_free(pam_data);
}
