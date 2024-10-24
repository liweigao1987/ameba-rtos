/*************************************************************************
        > File Name: sdk_callback.c
        > Author: ligaofeng
        > Mail: ligaofeng01@baidu.com
        > Created Time: 2020年02月13日 星期四 20时08分56秒
 ************************************************************************/
#include <inttypes.h>
#include <stdio.h>

#include "bds_client_event.h"
#include "cJSON.h"

#define EVENT_TAG "event_sdk"

#define WAKEUP_BACKTIME 200

#define ESP_LOGW(tag, fmt, ...) printf(fmt, ##__VA_ARGS__)
#define ESP_LOGE(tag, fmt, ...) printf(fmt, ##__VA_ARGS__)

int32_t bdsc_event_callback(bds_client_event_t* event, void* custom) {
    if (event != NULL) {
        ESP_LOGW(EVENT_TAG, "callback key:%d", event->key);
        switch (event->key) {
            case EVENT_DSP_FATAL_ERROR: {
                bdsc_event_error_t* error = (bdsc_event_error_t*)event->content;
                if (error) {
                    ESP_LOGW(EVENT_TAG,
                             "---> EVENT_DSP_FATAL_ERROR, code=%" PRId32 "--info_length=%" PRIu16 "--info=%s",
                             error->code, error->info_length, error->info);
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_DSP_FATAL_ERROR error null");
                }
                break;
            }
            case EVENT_ASR_ERROR: {
                bdsc_event_error_t* error = (bdsc_event_error_t*)event->content;
                if (error) {
                    ESP_LOGW(EVENT_TAG,
                             "---> EVENT_ASR_ERROR sn=%s, code=%" PRId32 "--info_length=%" PRIu16 "--info=%s",
                             error->sn, error->code, error->info_length, error->info);
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_ASR_ERROR error null");
                }
                break;
            }
            case EVENT_ASR_CANCEL: {
                bdsc_event_process_t* process = (bdsc_event_process_t*)event->content;
                if (process) {
                    ESP_LOGW(EVENT_TAG, "---> EVENT_ASR_CANCEL sn=%s", process->sn);
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_ASR_CANCEL process null");
                }
                break;
            }
            case EVENT_ASR_BEGIN: {
                bdsc_event_process_t* process = (bdsc_event_process_t*)event->content;
                if (process) {
                    ESP_LOGW(EVENT_TAG, "---> EVENT_ASR_BEGIN sn=%s", process->sn);
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_ASR_BEGIN process null");
                }
                break;
            }
            case EVENT_ASR_RESULT: {
                bdsc_event_data_t* asr_result = (bdsc_event_data_t*)event->content;
                if (asr_result) {
                    ESP_LOGW(EVENT_TAG, "---> EVENT_ASR_RESULT sn=%s, idx=%d, buffer_length=%d, buffer=%s",
                             asr_result->sn, asr_result->idx, asr_result->buffer_length, asr_result->buffer);
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_ASR_RESULT result null");
                }
                break;
            }
            case EVENT_ASR_EXTERN_DATA: {
                bdsc_event_data_t* extern_result = (bdsc_event_data_t*)event->content;
                if (extern_result) {
                    ESP_LOGW(EVENT_TAG, "---> EVENT_ASR_EXTERN_DATA sn=%s, idx=%d, buffer_length=%d, buffer=%p",
                             extern_result->sn, extern_result->idx, extern_result->buffer_length,
                             extern_result->buffer);
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_ASR_EXTERN_DATA extern null");
                }
                break;
            }
            case EVENT_ASR_TTS_DATA: {
                bdsc_event_data_t* tts_data = (bdsc_event_data_t*)event->content;
                if (tts_data) {
                    ESP_LOGW(EVENT_TAG, "---> EVENT_ASR_TTS_DATA sn=%s, idx=%d, buffer_length=%d, buffer=%p",
                             tts_data->sn, tts_data->idx, tts_data->buffer_length, tts_data->buffer);
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_ASR_TTS_DATA extern null");
                }
                break;
            }
            case EVENT_ASR_GENERAL_INFO: {
                char* data = (char*)event->content;
                if (data) {
                    ESP_LOGW(EVENT_TAG, "---> EVENT_ASR_GENERAL_INFO %s", data);
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_ASR_GENERAL_INFO data null");
                }
                break;
            }
            case EVENT_ASR_END: {
                bdsc_event_process_t* process = (bdsc_event_process_t*)event->content;
                if (process) {
                    ESP_LOGW(EVENT_TAG, "---> EVENT_ASR_END sn=%s", process->sn);
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_ASR_END process null");
                }
                break;
            }
            case EVENT_EVENTUPLOAD_BEGIN: {
                bdsc_event_process_t* process = (bdsc_event_process_t*)event->content;
                if (process) {
                    ESP_LOGW(EVENT_TAG, "---> EVENT_EVENTUPLOAD_BEGIN sn=%s", process->sn);
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_EVENTUPLOAD_BEGIN process null");
                }
                break;
            }
            case EVENT_EVENTUPLOAD_END: {
                bdsc_event_process_t* process = (bdsc_event_process_t*)event->content;
                if (process) {
                    ESP_LOGW(EVENT_TAG, "---> EVENT_EVENTUPLOAD_END sn=%s", process->sn);
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_EVENTUPLOAD_END process null");
                }
                break;
            }
            case EVENT_EVENTUPLOAD_DATA: {
                bdsc_event_data_t* upload_data = (bdsc_event_data_t*)event->content;
                if (upload_data) {
                    ESP_LOGW(EVENT_TAG, "---> EVENT_EVENTUPLOAD_DATA sn=%s, idx=%d, buffer_length=%d, buffer=%p",
                             upload_data->sn, upload_data->idx, upload_data->buffer_length, upload_data->buffer);
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_EVENTUPLOAD_DATA data null");
                }
                break;
            }
            case EVENT_EVENTUPLOAD_CANCEL: {
                bdsc_event_process_t* process = (bdsc_event_process_t*)event->content;
                if (process) {
                    ESP_LOGW(EVENT_TAG, "---> EVENT_EVENTUPLOAD_CANCEL sn=%s", process->sn);
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_EVENTUPLOAD_CANCEL process null");
                }
                break;
            }
            case EVENT_EVENTUPLOAD_ERROR: {
                bdsc_event_error_t* error = (bdsc_event_error_t*)event->content;
                if (error) {
                    ESP_LOGW(EVENT_TAG, "---> EVENT_EVENTUPLOAD_ERROR sn=%s, code=%" PRId32 "--info_length=%d, info=%s",
                             error->sn, error->code, error->info_length, error->info);
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_EVENTUPLOAD_ERROR error null");
                }
                break;
            }
            case EVENT_PUSH_DATA: {
                bdsc_event_data_t* push_data = (bdsc_event_data_t*)event->content;
                if (push_data) {
                    ESP_LOGW(EVENT_TAG, "---> EVENT_PUSH_DATA idx=%d, buffer_length=%d, buffer=%p", push_data->idx,
                             push_data->buffer_length, push_data->buffer);
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_PUSH_DATA data null");
                }
                break;
            }
            case EVENT_PUSH_ERROR: {
                bdsc_event_error_t* error = (bdsc_event_error_t*)event->content;
                if (error) {
                    ESP_LOGW(EVENT_TAG, "---> EVENT_PUSH_ERROR code=%" PRId32 "--info=%s", error->code, error->info);
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_PUSH_ERROR error null");
                }
                break;
            }
            case EVENT_WAKEUP_TRIGGER: {
                bdsc_event_wakeup_t* wakeup = (bdsc_event_wakeup_t*)event->content;
                if (wakeup) {
                    ESP_LOGW(EVENT_TAG, "---> EVENT_WAKEUP_TRIGGER dci=%f", wakeup->dci_result);

                    test_cancel_asr(custom);
                    test_start_asr(custom, WAKEUP_BACKTIME);
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_WAKEUP_TRIGGER wakeup null");
                }
                break;
            }
            case EVENT_WAKEUP_ERROR: {
                bdsc_event_error_t* error = (bdsc_event_error_t*)event->content;
                if (error) {
                    ESP_LOGW(EVENT_TAG, "---> EVENT_WAKEUP_ERROR code=%" PRId32 "--info=%s", error->code, error->info);
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_WAKEUP_ERROR error null");
                }
                break;
            }
            case EVENT_DIRECT: {
                bdsc_event_direct_t* direct = (bdsc_event_direct_t*)event->content;
                if (direct) {
                    ESP_LOGW(EVENT_TAG, "---> EVENT_DIRECT words=%s", direct->keywords);
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_WAKEUP_TRIGGER wakeup null");
                }
                break;
            }
            case EVENT_LINK_CONNECTED: {
                bdsc_event_data_t* push_data = (bdsc_event_data_t*)event->content;
                if (push_data) {
                    ESP_LOGW(EVENT_TAG, "---> ---> EVENT_LINK_CONNECTED buffer_length=%d, buffer=%s",
                             push_data->buffer_length, push_data->buffer);
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_LINK_CONNECTED data null");
                }
                break;
            }
            case EVENT_LINK_DISCONNECTED: {
                bdsc_event_data_t* dis_data = (bdsc_event_data_t*)event->content;
                if (dis_data) {
                    ESP_LOGW(EVENT_TAG, "---> ---> EVENT_LINK_DISCONNECTED buffer_length=%d, buffer=%s",
                             dis_data->buffer_length, dis_data->buffer);
                }
                break;
            }
            case EVENT_LINK_ERROR: {
                bdsc_event_error_t* error = (bdsc_event_error_t*)event->content;
                if (error) {
                    ESP_LOGW(EVENT_TAG, "---> EVENT_LINK_ERROR code=%" PRId32 "--info=%s", error->code, error->info);
                    if (error->code == ERROR_BDSC_LINK_CONNECT_FAILED) {
                        //                    test_reconnect(g_client_handle);
                    }
                } else {
                    ESP_LOGE(EVENT_TAG, "---> EVENT_LINK_ERROR error null");
                }
                break;
            }
            default:
                ESP_LOGE(EVENT_TAG, "!!! unknow event !!!");
                break;
        }
    }
    return 0;
}
