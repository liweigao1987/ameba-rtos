/*
 * bdsc_opus.h
 *
 *  Created on: 2020Äê7ÔÂ28ÈÕ
 *      Author: liweigao
 */

#ifndef CODEC_OPUS_BDSC_OPUS_H_
#define CODEC_OPUS_BDSC_OPUS_H_

#include "bds_opus_type.h"

typedef void *bdsc_opus_h;

bdsc_opus_h bdsc_opus_create();

int bdsc_opus_encode(bdsc_opus_h handle, bdsc_audio_t *src, bdsc_audio_t *dst);

void bdsc_opus_destroy(bdsc_opus_h handle);

#endif /* CODEC_OPUS_BDSC_OPUS_H_ */
