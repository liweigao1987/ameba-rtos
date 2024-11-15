#include "bdsc_asr_decoder.h"

#include <stdint.h>

#include "opus/opus_multistream.h"

#define TAG "decoder"

typedef struct {
    OpusMSDecoder*               decoder;
    bdsc_asr_decoder_listener_t* listener;
} bdsc_asr_decoder_t;

bdsc_asr_decoder_h bdsc_asr_decoder_create() {
    bdsc_asr_decoder_t* h               = bdsc_malloc(sizeof(bdsc_asr_decoder_t));
    int                 streams         = 1;
    int                 coupled_streams = 0;
    unsigned char       mapping[2]      = {0, 0};
    int                 error           = 0;
    h->decoder = opus_multistream_decoder_create(SAMPLE_RATE, CHANNEL, streams, coupled_streams, mapping, &error);
    if (error != OPUS_OK) {
        bdsc_logd(TAG, "decoder create failed! error=%d", error);
        bdsc_free(h);
        return NULL;
    }
    bdsc_logd(TAG, "decoder create error=%d", error);
    return h;
}

void bdsc_asr_decoder_destroy(bdsc_asr_decoder_h handle) {
    bdsc_asr_decoder_t* h = handle;
    opus_multistream_decoder_destroy(h->decoder);
    h->decoder = NULL;
    bdsc_free(h);
}

void bdsc_asr_decoder_set_listener(bdsc_asr_decoder_h handle, bdsc_asr_decoder_listener_t* listener) {
    bdsc_asr_decoder_t* h = handle;
    h->listener           = listener;
}

static void callback(bdsc_asr_decoder_t* h, bdsc_audio_t* pcm) {
    if (!h->listener) {
        bdsc_logd(TAG, "listener null");
        return;
    }
    h->listener->on_data(h->listener->custom, pcm);
}

int bdsc_asr_decoder_decode(bdsc_asr_decoder_h handle, bdsc_audio_t* audio) {
    bdsc_asr_decoder_t* h                           = handle;
    opus_int16          pcm_buffer[PCM_20MS_SHORTS] = {0};
    unsigned char*      src                         = audio->buffer;
    int                 length                      = char_to_int(src);
    int                 range                       = char_to_int(src + 4);
    bdsc_logd(TAG, "len=%d, range=%08x", length, range);
    /* opus_multistream_decoder_ctl(h->decoder, range); */
    int samples_ret = opus_multistream_decode(h->decoder, src + 8, length, pcm_buffer, PCM_20MS_SAMPLES, 0);
    if (samples_ret != PCM_20MS_SAMPLES) {
        bdsc_logd(TAG, "decode failed! ret=%d", samples_ret);
        return -2;
    }
    bdsc_audio_t* pcm_t = bdsc_audio_create(PCM_20MS_BYTES);
    bdsc_audio_append(pcm_t, pcm_buffer, PCM_20MS_BYTES);
    callback(h, pcm_t);
    bdsc_audio_destroy(pcm_t);
    return 0;
}
