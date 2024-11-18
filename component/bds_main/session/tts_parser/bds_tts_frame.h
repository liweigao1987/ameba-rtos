/*
 * bds_tts_frame.h
 *
 *  Created on: Apr 8, 2020
 *      Author: liweigao
 */

#ifndef LIGHT_BDSPEECH_SDK_COMPONENTS_BDS_LIGHT_SDK_TTS_PARSER_BDS_TTS_FRAME_H_
#define LIGHT_BDSPEECH_SDK_COMPONENTS_BDS_LIGHT_SDK_TTS_PARSER_BDS_TTS_FRAME_H_

#include "bds_client_memory.h"
#include "bds_client_log.h"
#include "bds_endian.h"

#define TAG                             "tts frame"

#define TTS_FRAME_HEADER_LENGTH_BYTES   2
#define TTS_FRAME_AUDIO_LENGTH_BYTES    4

typedef struct {
    char header_length[TTS_FRAME_HEADER_LENGTH_BYTES];
    int header_length_offset;
    char *header;
    int header_offset;
    char audio_length[TTS_FRAME_AUDIO_LENGTH_BYTES];
    int audio_length_offset;
    char *audio;
    int audio_offset;
} bds_tts_frame_t;

static bds_tts_frame_t* bds_tts_frame_create() {
    bds_tts_frame_t *frame = bdsc_malloc(sizeof(bds_tts_frame_t));
    frame->header_length_offset = 0;
    frame->header = NULL;
    frame->header_offset = 0;
    frame->audio_length_offset = 0;
    frame->audio = NULL;
    frame->audio_offset = 0;
    return frame;
}

static void bds_tts_frame_destroy(bds_tts_frame_t *frame) {
    if (frame) {
        if (frame->header) {
            bdsc_free(frame->header);
            frame->header = NULL;
        }
        if (frame->audio) {
            bdsc_free(frame->audio);
            frame->audio = NULL;
        }
        bdsc_free(frame);
    }
}

static short tts_frame_get_header_length(bds_tts_frame_t *frame) {
    return le16toh(*((short*)frame->header_length));
}

static int tts_frame_get_audio_length(bds_tts_frame_t *frame) {
    return le32toh(*((int*)frame->audio_length));
}

static int tts_frame_header_length_lack(bds_tts_frame_t *frame) {
    return TTS_FRAME_HEADER_LENGTH_BYTES - frame->header_length_offset;
}

static int tts_frame_header_lack(bds_tts_frame_t *frame) {
    return tts_frame_get_header_length(frame) - frame->header_offset;
}

static int tts_frame_audio_length_lack(bds_tts_frame_t *frame) {
    return TTS_FRAME_AUDIO_LENGTH_BYTES - frame->audio_length_offset;
}

static int tts_frame_audio_lack(bds_tts_frame_t *frame) {
    return tts_frame_get_audio_length(frame) - frame->audio_offset;
}

static int tts_frame_clone(char *dst, int offset, char *src, int length) {
    memcpy(dst + offset, src, length);
    offset += length;
    return offset;
}

static void tts_frame_clone_header_length(bds_tts_frame_t *frame, char *src, int length) {
    frame->header_length_offset = tts_frame_clone(frame->header_length, frame->header_length_offset, src, length);
}

static void tts_frame_clone_header(bds_tts_frame_t *frame, char *src, int length) {
    if (frame->header == NULL) {
        frame->header = bdsc_malloc(tts_frame_get_header_length(frame));
    }
    frame->header_offset = tts_frame_clone(frame->header, frame->header_offset, src, length);
}

static void tts_frame_clone_audio_length(bds_tts_frame_t *frame, char *src, int length) {
    frame->audio_length_offset = tts_frame_clone(frame->audio_length, frame->audio_length_offset, src, length);
}

static void tts_frame_clone_audio(bds_tts_frame_t *frame, char *src, int length) {
    if (frame->audio == NULL) {
        frame->audio = bdsc_malloc(tts_frame_get_audio_length(frame));
    }
    frame->audio_offset = tts_frame_clone(frame->audio, frame->audio_offset, src, length);
}

#endif /* LIGHT_BDSPEECH_SDK_COMPONENTS_BDS_LIGHT_SDK_TTS_PARSER_BDS_TTS_FRAME_H_ */
