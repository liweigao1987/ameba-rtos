/*
 * bds_tts_parser.c
 *
 *  Created on: Apr 8, 2020
 *      Author: liweigao
 */

#include "bds_tts_parser.h"

bds_tts_parser_t* bds_tts_parser_create() {
    bds_tts_parser_t *handle = bdsc_malloc(sizeof(bds_tts_parser_t));
    handle->header_length_state = bds_tp_header_length_state_create(handle);
    handle->header_state = bds_tp_header_state_create(handle);
    handle->audio_length_state = bds_tp_audio_length_state_create(handle);
    handle->audio_state = bds_tp_audio_state_create(handle);
    handle->state = &handle->header_length_state->base_state;
    handle->listener = NULL;
    handle->frame = NULL;
    handle->lock_call = xSemaphoreCreateRecursiveMutex();
    return handle;
}

void bds_set_tts_parser_listener(bds_tts_parser_t *handle, bds_tts_parser_listener_t *listener) {
    if (handle == NULL || listener == NULL) {
        return;
    }
    xSemaphoreTakeRecursive(handle->lock_call, portMAX_DELAY);
    handle->listener = listener;
    xSemaphoreGiveRecursive(handle->lock_call);
}

void bds_tts_parser_decode(bds_tts_parser_t *handle, bdsc_event_data_t *data, int offset) {
    if (handle == NULL || data == NULL) {
        return;
    }
    xSemaphoreTakeRecursive(handle->lock_call, portMAX_DELAY);
    if(handle->state == &handle->header_length_state->base_state) {
        bdsc_logi(TAG, "to header length state");
    } else if(handle->state == &handle->header_state->base_state) {
        bdsc_logi(TAG, "to header state header len=%d", tts_frame_get_header_length(handle->frame));
    } else if(handle->state == &handle->audio_length_state->base_state) {
        bdsc_logi(TAG, "to audio length state");
    } else if(handle->state == &handle->audio_state->base_state) {
        bdsc_logi(TAG, "to audio state audio len=%d", tts_frame_get_audio_length(handle->frame));
    }
    handle->state->decode(handle, data, offset);
    xSemaphoreGiveRecursive(handle->lock_call);
}

void bds_tts_parser_destroy(bds_tts_parser_t *handle) {
    if (handle == NULL) {
        return;
    }
    xSemaphoreTakeRecursive(handle->lock_call, portMAX_DELAY);
    if (handle->header_length_state) {
        bds_tp_header_length_state_destroy(handle->header_length_state);
        handle->header_length_state = NULL;
    }
    if (handle->header_state) {
        bds_tp_header_state_destroy(handle->header_state);
        handle->header_state = NULL;
    }
    if (handle->audio_length_state) {
        bds_tp_audio_length_state_destroy(handle->audio_length_state);
        handle->audio_length_state = NULL;
    }
    if (handle->audio_state) {
        bds_tp_audio_state_destroy(handle->audio_state);
        handle->audio_state = NULL;
    }
    if (handle->frame) {
        bds_tts_frame_destroy(handle->frame);
        handle->frame = NULL;
    }
    xSemaphoreGiveRecursive(handle->lock_call);
    vSemaphoreDelete(handle->lock_call);
    bdsc_free(handle);
}

void bds_set_tts_parser_state(bds_tts_parser_t *handle, bds_tp_state_t *state) {
    if (handle == NULL || state == NULL) {
        return;
    }
    handle->state = state;
}

void bds_tts_parser_on_frame(bds_tts_parser_t *handle) {
    if (handle == NULL) {
        return;
    }
    if (handle->listener && handle->frame) {
        handle->listener->on_tts_f(handle->frame, handle->listener->custom);
    }
    if (handle->frame) {
        /* bds_tts_frame_destroy(handle->frame); */
        handle->frame = NULL;
    }
}
