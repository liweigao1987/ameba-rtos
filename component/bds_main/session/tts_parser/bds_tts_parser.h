/*
 * bds_tts_parser.h
 *
 *  Created on: Apr 8, 2020
 *      Author: liweigao
 */

#ifndef LIGHT_BDSPEECH_SDK_MAIN_BUILD_WP_ASR_COMPONENTS_BDS_LIGHT_SDK_TTS_PARSER_BDS_TTS_PARSER_H_
#define LIGHT_BDSPEECH_SDK_MAIN_BUILD_WP_ASR_COMPONENTS_BDS_LIGHT_SDK_TTS_PARSER_BDS_TTS_PARSER_H_

#include "bds_client_event.h"
#include "bds_tts_frame.h"
#include "bds_client_log.h"
#include "semphr.h"

#define TAG         "tts_parser"

typedef struct {
    void *custom;
    void (*on_tts_f)(bds_tts_frame_t *frame, void *custom);
} bds_tts_parser_listener_t;

typedef struct {
    void (*decode)(void *machine, bdsc_event_data_t *data, int offset);
} bds_tp_state_t;

typedef struct {
    bds_tp_state_t base_state;
} bds_tp_header_length_state_t;

typedef struct {
    bds_tp_state_t base_state;
} bds_tp_header_state_t;

typedef struct {
    bds_tp_state_t base_state;
} bds_tp_audio_length_state_t;

typedef struct {
    bds_tp_state_t base_state;
} bds_tp_audio_state_t;

typedef struct {
    bds_tts_parser_listener_t *listener;
    bds_tp_state_t *state;
    bds_tp_header_length_state_t *header_length_state;
    bds_tp_header_state_t *header_state;
    bds_tp_audio_length_state_t *audio_length_state;
    bds_tp_audio_state_t *audio_state;
    bds_tts_frame_t *frame;
    SemaphoreHandle_t lock_call;
} bds_tts_parser_t;

bds_tp_header_length_state_t* bds_tp_header_length_state_create();
bds_tp_header_state_t* bds_tp_header_state_create();
bds_tp_audio_length_state_t* bds_tp_audio_length_state_create();
bds_tp_audio_state_t* bds_tp_audio_state_create();

void bds_tp_header_length_state_destroy(bds_tp_header_length_state_t *state);
void bds_tp_header_state_destroy(bds_tp_header_state_t *state);
void bds_tp_audio_length_state_destroy(bds_tp_audio_length_state_t *state);
void bds_tp_audio_state_destroy(bds_tp_audio_state_t *state);

bds_tts_parser_t* bds_tts_parser_create();

void bds_set_tts_parser_listener(bds_tts_parser_t *handle, bds_tts_parser_listener_t *listener);

void bds_tts_parser_decode(bds_tts_parser_t *handle, bdsc_event_data_t *data, int offset);

void bds_tts_parser_destroy(bds_tts_parser_t *handle);

void bds_set_tts_parser_state(bds_tts_parser_t *handle, bds_tp_state_t *state);

void bds_tts_parser_on_frame(bds_tts_parser_t *handle);

#endif /* LIGHT_BDSPEECH_SDK_MAIN_BUILD_WP_ASR_COMPONENTS_BDS_LIGHT_SDK_TTS_PARSER_BDS_TTS_PARSER_H_ */
