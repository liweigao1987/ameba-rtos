/***********************************************************
 * @author        liweigao
 * @date          2024-11-08
 * @brief         baidu speech
 ***********************************************************/

#ifndef _BDS_SPEECH_H_
#define _BDS_SPEECH_H_

#include "bds_main_ctx.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* bds_speech_h;

bds_speech_h bds_speech_create(bds_main_ctx_h ctx);

void bds_speech_destroy(bds_speech_h handle);

#ifdef __cplusplus
}
#endif

#endif /* _BDS_SPEECH_H_ */
