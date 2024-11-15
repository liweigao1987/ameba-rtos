#include "bdsc_opus.h"

#include "opus_multistream.h"

#define TAG "opus"

typedef struct {
    OpusMSEncoder* encoder;
    int sample_rate;
    int channel;
    int application;
} bdsc_opus_t;

bdsc_opus_h bdsc_opus_create() {
    bdsc_opus_t* handle = bdsc_malloc(sizeof(bdsc_opus_t));
    handle->sample_rate = 16000;
    handle->channel = 2;
    int streams = 1;
    int coupled_streams = handle->channel - streams;
    unsigned char mapping[256] = {0, 1};
    int error = 0;
    handle->encoder = opus_multistream_surround_encoder_create(
        handle->sample_rate, handle->channel, 1, &streams, &coupled_streams, mapping, OPUS_APPLICATION_AUDIO, &error);
    bdsc_logd(TAG, "opus ms create error=%d", error);
    opus_multistream_encoder_ctl(handle->encoder, OPUS_SET_BITRATE(32000 * 2));
    opus_multistream_encoder_ctl(handle->encoder, OPUS_SET_BANDWIDTH(OPUS_AUTO));
    opus_multistream_encoder_ctl(handle->encoder, OPUS_SET_VBR(0));
    opus_multistream_encoder_ctl(handle->encoder, OPUS_SET_VBR_CONSTRAINT(0));
    opus_multistream_encoder_ctl(handle->encoder, OPUS_SET_INBAND_FEC(0));
    opus_multistream_encoder_ctl(handle->encoder, OPUS_SET_FORCE_CHANNELS(OPUS_AUTO));
    opus_multistream_encoder_ctl(handle->encoder, OPUS_SET_DTX(0));
    opus_multistream_encoder_ctl(handle->encoder, OPUS_SET_PACKET_LOSS_PERC(0));
    opus_multistream_encoder_ctl(handle->encoder, OPUS_SET_COMPLEXITY(10));
    opus_multistream_encoder_ctl(handle->encoder, OPUS_SET_LSB_DEPTH(16));
    opus_multistream_encoder_ctl(handle->encoder, OPUS_SET_EXPERT_FRAME_DURATION(OPUS_FRAMESIZE_20_MS));
    return handle;
}

int bdsc_opus_encode(bdsc_opus_h handle, bdsc_audio_t* src, bdsc_audio_t* dst) {
    if (src == NULL || dst == NULL) {
        bdsc_logd(TAG, "invalid param! src=%p, dst=%p", src, dst);
        return -1;
    }
    bdsc_opus_t* h = (bdsc_opus_t*)handle;
    opus_int16* src_16 = src->buffer;
    int frame_size = src->offset / (2 * h->channel);
    int opus_bytes_20ms = 160;
    unsigned char* opus_buffer = bdsc_malloc(opus_bytes_20ms);
    /* bdsc_logi(TAG, "frame_size=%d, left=%d", frame_size, bdsc_audio_get_left_length(dst)); */
    /* bdsc_logv(TAG, "test encode + t=%u", bdsc_get_tick_count()); */
    /* bdsc_logd(TAG, "3-- %02x, %02x...%02x, %02x", src->buffer[0] & 0xff, src->buffer[1] & 0xff, */
    /*           src->buffer[src->offset - 2] & 0xff, src->buffer[src->offset - 1] & 0xff); */

    opus_int32 opus_bytes = opus_multistream_encode(h->encoder, src_16, frame_size, opus_buffer, opus_bytes_20ms);
    if (opus_bytes != opus_bytes_20ms) {
        bdsc_logd(TAG, "opus encoded bytes not 160!!! bytes=%d", opus_bytes);
    }
    /* bdsc_logv(TAG, "test encode - t=%u, bytes=%d", bdsc_get_tick_count(), opus_bytes); */
    if (opus_bytes < 0) {
        bdsc_logd(TAG, "encode failed! opus_bytes=%d", opus_bytes);
        bdsc_free(opus_buffer);
        return -2;
    }
    uint8_t ret_char[4];
    int_to_char(opus_bytes, ret_char);
    bdsc_audio_append(dst, ret_char, 4);
    opus_uint32 range = 0;
    opus_multistream_encoder_ctl(h->encoder, OPUS_GET_FINAL_RANGE(&range));
    int_to_char(range, ret_char);
    bdsc_audio_append(dst, ret_char, 4);
    bdsc_audio_append(dst, opus_buffer, opus_bytes);
    bdsc_free(opus_buffer);
    /* bdsc_logd(TAG, "4-- %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x", dst->buffer[0], dst->buffer[1],
     */
    /*           dst->buffer[2], dst->buffer[3], dst->buffer[4], dst->buffer[5], dst->buffer[6], dst->buffer[7], */
    /*           dst->buffer[8], dst->buffer[9]); */
    return 0;
}

void bdsc_opus_destroy(bdsc_opus_h handle) {
    if (!handle) {
        return;
    }
    bdsc_opus_t* h = (bdsc_opus_t*)handle;
    opus_multistream_encoder_destroy(h->encoder);
    h->encoder = NULL;
    bdsc_free(handle);
}
