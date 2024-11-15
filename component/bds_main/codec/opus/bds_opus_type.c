#include "bds_opus_type.h"

#define TAG "opus_type"

bdsc_audio_t* bdsc_audio_create(uint16_t buffer_length) {
    bdsc_audio_t* handle  = bdsc_malloc(sizeof(bdsc_audio_t) + buffer_length);
    handle->buffer_length = buffer_length;
    handle->flag          = 0;
    handle->offset        = 0;
    return handle;
}

int32_t bdsc_audio_append(bdsc_audio_t* audio, uint8_t* src, uint16_t src_length) {
    if (audio == NULL || src == NULL || src_length == 0) {
        bdsc_logd(TAG, "invalid param! audio=%p, src=%p, length=%d", audio, src, src_length);
        return -1;
    }
    uint16_t dst_offset = audio->offset + src_length;
    if (dst_offset > audio->buffer_length) {
        bdsc_logd(TAG, "audio length too long! dst_offset=%d, buffer_length=%d", dst_offset, audio->buffer_length);
        return -2;
    }
    bdsc_memcpy(audio->buffer + audio->offset, src, src_length);
    audio->offset = dst_offset;
    return 0;
}

bool bdsc_audio_is_full(bdsc_audio_t* audio) {
    if (!audio) {
        return false;
    }
    if (audio->offset >= audio->buffer_length && audio->buffer_length > 0) {
        return true;
    } else {
        return false;
    }
}

void bdsc_audio_destroy(bdsc_audio_t* audio) {
    if (audio) {
        bdsc_free(audio);
    }
}

void int_to_char(uint32_t i, uint8_t ch[4]) {
    ch[0] = i >> 24;
    ch[1] = (i >> 16) & 0xFF;
    ch[2] = (i >> 8) & 0xFF;
    ch[3] = i & 0xFF;
}

int char_to_int(uint8_t ch[4]) {
    int ret =
        (ch[0] << 24 & 0xff000000) | (ch[1] << 16 & 0x00ff0000) | (ch[2] << 8 & 0x0000ff00) | (ch[3] & 0x000000ff);
    return ret;
}
