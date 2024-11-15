/*
 * bdsc_asr_encoder.h
 *
 *  Created on: 2020Äê8ÔÂ4ÈÕ
 *      Author: liweigao
 */

#ifndef CODEC_OPUS_BDSC_ASR_ENCODER_H_
#define CODEC_OPUS_BDSC_ASR_ENCODER_H_

#include "bds_opus_type.h"

typedef void* bdsc_asr_encoder_h;

typedef struct {
    void* custom;
    int (*on_data)(void* custom, bdsc_audio_t* audio);
} bdsc_asr_encoder_listener_t;

bdsc_asr_encoder_h bdsc_asr_encoder_create();

void bdsc_asr_encoder_set_listener(bdsc_asr_encoder_h handle, bdsc_asr_encoder_listener_t* listener);

int bdsc_asr_encoder_encode(bdsc_asr_encoder_h handle, bdsc_audio_t* audio);

void bdsc_asr_encoder_destroy(bdsc_asr_encoder_h handle);

#endif /* CODEC_OPUS_BDSC_ASR_ENCODER_H_ */
