/***********************************************************
 * @author        liweigao
 * @date          2024-01-30
 * @brief         opus decoder
 ***********************************************************/

#ifndef _BDSC_ASR_DECODER_H_
#define _BDSC_ASR_DECODER_H_

#include "bds_opus_type.h"

typedef void* bdsc_asr_decoder_h;
typedef struct {
    void* custom;
    int (*on_data)(void* custom, bdsc_audio_t* audio);
} bdsc_asr_decoder_listener_t;

bdsc_asr_decoder_h bdsc_asr_decoder_create();

void bdsc_asr_decoder_destroy(bdsc_asr_decoder_h handle);

void bdsc_asr_decoder_set_listener(bdsc_asr_decoder_h handle, bdsc_asr_decoder_listener_t* listener);

int bdsc_asr_decoder_decode(bdsc_asr_decoder_h handle, bdsc_audio_t* audio);

#endif /* _BDSC_ASR_DECODER_H_ */
