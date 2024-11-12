#include "bds_speech.h"

#include "bds_client.h"
#include "bds_client_command.h"
#include "bds_client_context.h"
#include "bds_client_event.h"
#include "bds_client_log.h"
#include "bds_client_memory.h"
#include "bds_client_params.h"
#include "bds_common_utility.h"

#define TAG "speech"

extern char* get_wifi_mac();

typedef struct {
    bds_main_ctx_h      ctx;
    bds_client_handle_t client;
} bds_speech_t;

static int32_t event_callback(bds_client_event_t* event, bds_speech_t* h) {
    if (event != NULL) {
        bdsc_logw(TAG, "callback key:%d", event->key);
        switch (event->key) {
            case EVENT_DSP_FATAL_ERROR: {
                bdsc_event_error_t* error = (bdsc_event_error_t*)event->content;
                break;
            }
            case EVENT_ASR_ERROR: {
                bdsc_event_error_t* error = (bdsc_event_error_t*)event->content;
                break;
            }
            case EVENT_ASR_CANCEL: {
                bdsc_event_process_t* process = (bdsc_event_process_t*)event->content;
                break;
            }
            case EVENT_ASR_BEGIN: {
                bdsc_event_process_t* process = (bdsc_event_process_t*)event->content;
                break;
            }
            case EVENT_ASR_RESULT: {
                bdsc_event_data_t* asr_result = (bdsc_event_data_t*)event->content;
                break;
            }
            case EVENT_ASR_EXTERN_DATA: {
                bdsc_event_data_t* extern_result = (bdsc_event_data_t*)event->content;
                break;
            }
            case EVENT_ASR_TTS_DATA: {
                bdsc_event_data_t* tts_data = (bdsc_event_data_t*)event->content;
                break;
            }
            case EVENT_ASR_GENERAL_INFO: {
                char* data = (char*)event->content;
                break;
            }
            case EVENT_ASR_END: {
                bdsc_event_process_t* process = (bdsc_event_process_t*)event->content;
                break;
            }
            case EVENT_EVENTUPLOAD_BEGIN: {
                bdsc_event_process_t* process = (bdsc_event_process_t*)event->content;
                break;
            }
            case EVENT_EVENTUPLOAD_END: {
                bdsc_event_process_t* process = (bdsc_event_process_t*)event->content;
                break;
            }
            case EVENT_EVENTUPLOAD_DATA: {
                bdsc_event_data_t* upload_data = (bdsc_event_data_t*)event->content;
                break;
            }
            case EVENT_EVENTUPLOAD_CANCEL: {
                bdsc_event_process_t* process = (bdsc_event_process_t*)event->content;
                break;
            }
            case EVENT_EVENTUPLOAD_ERROR: {
                bdsc_event_error_t* error = (bdsc_event_error_t*)event->content;
                break;
            }
            case EVENT_PUSH_DATA: {
                bdsc_event_data_t* push_data = (bdsc_event_data_t*)event->content;
                break;
            }
            case EVENT_PUSH_ERROR: {
                bdsc_event_error_t* error = (bdsc_event_error_t*)event->content;
                break;
            }
            case EVENT_WAKEUP_TRIGGER: {
                bdsc_event_wakeup_t* wakeup = (bdsc_event_wakeup_t*)event->content;
                if (!wakeup) {
                    bdsc_loge(TAG, "wp null!");
                    break;
                }
                bds_mc_submit_wp(h->ctx, wakeup);
                break;
            }
            case EVENT_WAKEUP_ERROR: {
                bdsc_event_error_t* error = (bdsc_event_error_t*)event->content;
                break;
            }
            case EVENT_DIRECT: {
                bdsc_event_direct_t* direct = (bdsc_event_direct_t*)event->content;
                break;
            }
            case EVENT_LINK_CONNECTED: {
                bdsc_event_data_t* push_data = (bdsc_event_data_t*)event->content;
                break;
            }
            case EVENT_LINK_DISCONNECTED: {
                bdsc_event_data_t* dis_data = (bdsc_event_data_t*)event->content;
                break;
            }
            case EVENT_LINK_ERROR: {
                bdsc_event_error_t* error = (bdsc_event_error_t*)event->content;
                break;
            }
            default:
                bdsc_loge(TAG, "!!! unknow event !!!");
                break;
        }
    }
    return 0;
}

static void config_sdk(bds_client_handle_t client) {
    char sn[37];
    bds_generate_uuid(sn);
    char*                 pam_data      = get_dcs_pam();
    char*                 mac           = get_wifi_mac();
    bdsc_engine_params_t* engine_params = bdsc_engine_params_create(
        sn, 1775, "leetest.baidu.com", 443, PROTOCOL_TLS, mac, "com.baidu.iot", strlen(pam_data) + 1, pam_data);
    /* bdsc_engine_params_t* engine_params = bdsc_engine_params_create( */
    /*      sn, 1775, "leetest.baidu.com", 80, PROTOCOL_DEFAULT, mac, "com.baidu.iot", strlen(pam_data) + 1, pam_data);
     */

    /* bdsc_engine_params_t* engine_params = bdsc_engine_params_create( */
    /*     sn, 1775, "10.174.33.38", 8114, PROTOCOL_DEFAULT, mac, "com.baidu.iot", strlen(pam_data) + 1, pam_data); */

    bds_client_params_t params = {0};
    params.engine_params       = engine_params;
    bds_client_config(client, &params);
    bdsc_engine_params_destroy(engine_params);
    bdsc_free(pam_data);
}

bds_speech_h bds_speech_create(bds_main_ctx_h ctx) {
    bds_speech_t* h = bdsc_malloc(sizeof(bds_speech_t));
    h->ctx          = ctx;
    h->client       = bds_client_create(NULL);
    bds_client_set_event_listener(h->client, event_callback, h);
    bds_set_log_level(3);
    config_sdk(h->client);
    bds_client_start(h->client);
    bds_speech_start_wp(h);
    return h;
}

void bds_speech_destroy(bds_speech_h handle) {
    if (!handle) {
        return;
    }
    bds_speech_t* h = handle;
    if (h->client) {
        bds_client_stop(h->client);
        bds_client_destroy(h->client);
        h->client = NULL;
    }
    bdsc_free(handle);
}

void bds_speech_start_link(bds_speech_h handle) {
    bds_speech_t*        h          = handle;
    bds_client_command_t link_start = {
        .key            = CMD_LINK_START,
        .content        = NULL,
        .content_length = 0,
    };
    bds_client_send(h->client, &link_start);
}

void bds_speech_stop_link(bds_speech_h handle) {
    bds_speech_t*        h         = handle;
    bds_client_command_t link_stop = {
        .key            = CMD_LINK_STOP,
        .content        = NULL,
        .content_length = 0,
    };
    bds_client_send(h->client, &link_stop);
}

void bds_speech_start_wp(bds_speech_h handle) {
    bds_speech_t*        h            = handle;
    bdsc_wp_params_t     params       = {0};
    bds_client_command_t wakeup_start = {
        .key            = CMD_WAKEUP_START,
        .content        = &params,
        .content_length = sizeof(bdsc_wp_params_t),
    };
    bds_client_send(h->client, &wakeup_start);
}

void bds_speech_stop_wp(bds_speech_h handle) {
    bds_speech_t*        h           = handle;
    bds_client_command_t wakeup_stop = {
        .key = CMD_WAKEUP_STOP,
    };
    bds_client_send(h->client, &wakeup_stop);
}

void bds_speech_start_asr(bds_speech_h handle, int back_time) {
    bds_speech_t* h = handle;
    char          sn[37];
    bds_generate_uuid(sn);
    bdsc_asr_params_t* asr_params =
        bdsc_asr_params_create(sn, 1775, "com.baidu.iot", get_wifi_mac(), back_time, 0, BDS_ASR_CONTACT_OFF, 0, NULL);
    bds_client_command_t asr_start = {
        .key            = CMD_ASR_START,
        .content        = asr_params,
        .content_length = sizeof(bdsc_asr_params_t),
    };
    bds_client_send(h->client, &asr_start);
    bdsc_asr_params_destroy(asr_params);
}

void bds_speech_cancel_asr(bds_speech_h handle) {
    bds_speech_t*        h          = handle;
    bds_client_command_t asr_cancel = {
        .key = CMD_ASR_CANCEL,
    };
    bds_client_send(h->client, &asr_cancel);
}
