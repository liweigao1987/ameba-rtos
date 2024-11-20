/***********************************************************
 * @author        liweigao
 * @date          2024-11-08
 * @brief         baidu speech
 ***********************************************************/

#ifndef _BDS_SPEECH_H_
#define _BDS_SPEECH_H_

#include "bds_session_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* bds_speech_h;

bds_speech_h bds_speech_create(void* ctx);

void bds_speech_destroy(bds_speech_h handle);

void bds_speech_start_link(bds_speech_h handle);

void bds_speech_stop_link(bds_speech_h handle);

void bds_speech_start_wp(bds_speech_h handle);

void bds_speech_stop_wp(bds_speech_h handle);

void bds_speech_start_asr(bds_speech_h handle, int back_time, bds_session_id_t* id);

void bds_speech_cancel_asr(bds_speech_h handle);

#ifdef __cplusplus
}
#endif

#endif /* _BDS_SPEECH_H_ */
