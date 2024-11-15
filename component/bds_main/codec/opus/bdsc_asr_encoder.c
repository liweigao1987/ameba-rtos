/*
 * bdsc_asr_encoder.c
 *
 *  Created on: 2020��8��4��
 *      Author: liweigao
 */

#include <stdbool.h>

#include "bdsc_asr_encoder.h"
#include "bdsc_opus.h"
#include "bds_opus_type.h"

#define TAG "encoder"

#define BDSC_PCM_COUNT 5
#define BDSC_OPUS_HEADER 4

typedef struct {
    bdsc_opus_h encoder;
    bool is_first;
    bdsc_asr_encoder_listener_t *listener;
    bdsc_audio_t *opus_cache;
} bdsc_asr_encoder_t;

bdsc_asr_encoder_h bdsc_asr_encoder_create() {
    bdsc_asr_encoder_t *handle = bdsc_malloc(sizeof(bdsc_asr_encoder_t));
    handle->encoder = bdsc_opus_create();
    handle->is_first = true;
    handle->opus_cache = NULL;
    return handle;
}

void bdsc_asr_encoder_set_listener(bdsc_asr_encoder_h handle, bdsc_asr_encoder_listener_t *listener) {
    bdsc_asr_encoder_t *h = (bdsc_asr_encoder_t *)handle;
    h->listener = listener;
}

static void callback(bdsc_asr_encoder_t *handle, bdsc_audio_t *audio) {
    if (handle->listener == NULL || audio == NULL) {
        return;
    }
    handle->listener->on_data(handle->listener->custom, audio);
}

int bdsc_asr_encoder_encode(bdsc_asr_encoder_h handle, bdsc_audio_t *audio) {
    if (handle == NULL || audio == NULL) {
        bdsc_logd(TAG, "invalid param! handle=%p, audio=%p", handle, audio);
        return -1;
    }
    bdsc_asr_encoder_t *h = (bdsc_asr_encoder_t *)handle;
    if (h->opus_cache == NULL) {
        h->opus_cache = bdsc_audio_create(((1280 / 8) + 8) * BDSC_PCM_COUNT);
    }
    int ret = bdsc_opus_encode(h->encoder, audio, h->opus_cache);
    if (ret != 0) {
        bdsc_logd(TAG, "encode failed! ret=%d", ret);
    }
    if (bdsc_audio_is_full(h->opus_cache)) {
        callback(h, h->opus_cache);
        bdsc_audio_destroy(h->opus_cache);
        h->opus_cache = NULL;
    }
    return 0;
}

void bdsc_asr_encoder_destroy(bdsc_asr_encoder_h handle) {
    if (!handle) {
        return;
    }
    bdsc_asr_encoder_t *h = (bdsc_asr_encoder_t *)handle;
    if (h->opus_cache) {
        bdsc_audio_destroy(h->opus_cache);
        h->opus_cache = NULL;
    }
    bdsc_opus_destroy(h->encoder);
    h->encoder = NULL;
    bdsc_free(h);
}
