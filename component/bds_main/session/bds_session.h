/***********************************************************
 * @author        liweigao
 * @date          2024-11-15
 * @brief         session
 ***********************************************************/

#ifndef _BDS_SESSION_H_
#define _BDS_SESSION_H_

#include "bds_client_event.h"
#include "bds_session_type.h"
#include "bds_tts_frame.h"
#include "bds_main_ctx.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* bds_session_h;

bds_session_h bds_session_create(bds_main_ctx_h ctx, bds_session_param_t* param, bds_session_id_t* id);

void bds_session_destroy(bds_session_h handle);

bds_session_id_t* bds_session_get_id(bds_session_h handle);

void bds_session_put_online_audio(bds_session_h handle, bdsc_event_data_t* data);

int bds_session_take_online_audio(bds_session_h handle, bds_tts_frame_t** data);

int bds_session_start_asr(bds_session_h handle);

int bds_session_direct_trigger(bds_session_h handle, bdsc_event_direct_t* event);

#ifdef __cplusplus
}
#endif

#endif /* _BDS_SESSION_H_ */
