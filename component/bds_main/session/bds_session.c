#include "bds_session.h"

#include "bds_client_log.h"
#include "bds_client_memory.h"
#include "bds_queue.h"
#include "bds_session_type.h"
#include "bds_tts_frame.h"
#include "bds_tts_parser.h"
#include "tts_parser/bds_tts_frame.h"

#define TAG              "session"
#define TTS_Q_ENTRY_NUM  10
#define TTS_Q_ENTRY_SIZE sizeof(bds_tts_frame_t*)

typedef struct {
    bds_session_param_t        param;
    bds_session_id_t           id;
    bds_tts_parser_t*          tts_parser;
    bds_tts_parser_listener_t* tts_listener;
    bds_queue_handle           tts_q;
} bds_session_t;

static void on_tts_frame(bds_tts_frame_t* frame, bds_session_t* h) {
    bdsc_logw(TAG, "frame_audio=%p", frame->audio);
    int header_len = tts_frame_get_header_length(frame);
    char* temp = bdsc_malloc(header_len + 1);
    memcpy(temp, frame->header, header_len);
    bdsc_logw(TAG, "header=%s", temp);
    bdsc_free(temp);
    BaseType_t ret = bds_queue_push(h->tts_q, &frame);
    if (ret != pdPASS) {
        bdsc_loge(TAG, "tts q push failed!");
        bds_tts_frame_destroy(frame);
    }
}

bds_session_h bds_session_create(bds_session_param_t* param, bds_session_id_t* id) {
    bds_session_t* h = bdsc_malloc(sizeof(bds_session_t));
    memcpy(&h->param, param, sizeof(bds_session_param_t));
    memcpy(&h->id, id, sizeof(bds_session_id_t));
    h->tts_listener           = bdsc_malloc(sizeof(bds_tts_parser_listener_t));
    h->tts_listener->custom   = h;
    h->tts_listener->on_tts_f = on_tts_frame;
    h->tts_parser             = bds_tts_parser_create();
    bds_set_tts_parser_listener(h->tts_parser, h->tts_listener);
    h->tts_q = bds_queue_create(TTS_Q_ENTRY_NUM, TTS_Q_ENTRY_SIZE);
    return h;
}

void bds_session_destroy(bds_session_h handle) {
    if (!handle) {
        return;
    }
    bds_session_t* h = handle;
    if (h->tts_parser) {
        bds_tts_parser_destroy(h->tts_parser);
        h->tts_parser = NULL;
    }
    if (h->tts_listener) {
        bdsc_free(h->tts_listener);
        h->tts_listener = NULL;
    }
    if (h->tts_q) {
        bds_queue_delete(h->tts_q);
        h->tts_q = NULL;
    }
    bdsc_free(handle);
}

bds_session_id_t* bds_session_get_id(bds_session_h handle) {
    if (!handle) {
        bdsc_loge(TAG, "invalid params! h=%p", handle);
        return NULL;
    }
    bds_session_t* h = handle;
    return &h->id;
}

void bds_session_put_online_audio(bds_session_h handle, bdsc_event_data_t* data) {
    bds_session_t* h = handle;
    bds_tts_parser_decode(h->tts_parser, data, 0);
}

int bds_session_take_online_audio(bds_session_h handle, bds_tts_frame_t** data) {
    bds_session_t* h   = handle;
    BaseType_t     ret = bds_queue_pop(h->tts_q, data);
    if (ret != pdPASS) {
        bdsc_loge(TAG, "tts q pop failed!");
        *data = NULL;
        return -20;
    }
    return 0;
}
