#include "bds_opus_dec.h"

#include <stdlib.h>

#include "bds_client_log.h"
#include "bds_client_memory.h"
#include "bds_format.h"
#include "bds_opus_type.h"
#include "opus_multistream.h"

#define TAG "opus_dec"

typedef struct {
    OpusMSDecoder* decoder;
} bds_opus_dec_t;

bds_opus_dec_h bds_opus_dec_create() {
    bds_opus_dec_t* h               = bdsc_malloc(sizeof(bds_opus_dec_t));
    int             streams         = 1;
    int             coupled_streams = 0;
    unsigned char   mapping[2]      = {0, 0};
    int             error           = 0;
    h->decoder =
        opus_multistream_decoder_create(DEC_SAMPLE_RATE, DEC_CHANNEL, streams, coupled_streams, mapping, &error);
    if (error != OPUS_OK) {
        bdsc_logd(TAG, "decoder create failed! error=%d", error);
        bdsc_free(h);
        return NULL;
    }
    bdsc_logd(TAG, "decoder create error=%d", error);
    return h;
}

void bds_opus_dec_destroy(bds_opus_dec_h handle) {
    if (!handle) {
        return;
    }
    bds_opus_dec_t* h = handle;
    if (h->decoder) {
        opus_multistream_decoder_destroy(h->decoder);
        h->decoder = NULL;
    }
    bdsc_free(handle);
}

int bds_opus_dec_proc(bds_opus_dec_h handle, uint8_t* dst, int* dst_len, uint8_t* src, int src_len) {
    if (!handle || !dst || !dst_len || !src) {
        bdsc_loge(TAG, "invalid params! h=%p, dst=%p, dst_len=%p, src=%p", handle, dst, dst_len, src);
        return -1;
    }
    if (src_len != OPUS_TTS_BAG) {
        bdsc_loge(TAG, "src_len=%d invalid!", src_len);
        return -10;
    }
    bds_opus_dec_t* h      = handle;
    int             length = char_to_int(src);
    int             range  = char_to_int(src + 4);
    /* bdsc_logd(TAG, "len=%d, src=%p", length, src); */
    int sample_ret = opus_multistream_decode(h->decoder, src + 8, length, dst, PCM_20MS_SAMPLE, 0);
    if (sample_ret != PCM_20MS_SAMPLE) {
        bdsc_logd(TAG, "decode failed! ret=%d", sample_ret);
        return -11;
    }
    *dst_len = PCM_20MS_BYTES;
    return 0;
}
