/***********************************************************
 * @author        liweigao
 * @date          2024-11-15
 * @brief         session manager
 ***********************************************************/

#ifndef _BDS_SESSION_MANAGER_H_
#define _BDS_SESSION_MANAGER_H_

#include "bds_client_event.h"
#include "bds_session_type.h"
#include "tts_parser/bds_tts_frame.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* bds_session_manager_h;

bds_session_manager_h bds_session_manager_create(void* ctx);

void bds_session_manager_destroy(bds_session_manager_h handle);

int bds_sm_active_session_id(bds_session_manager_h handle, bds_session_id_t* id);

bds_session_id_t* bds_session_id_create(char* sn);

bds_session_id_t* bds_session_id_create2(bds_session_id_t* id);

void bds_session_id_destroy(bds_session_id_t* id);

bool bds_session_is_same(bds_session_id_t* id1, bds_session_id_t* id2);

void bds_session_id_clone(bds_session_id_t* dst, bds_session_id_t* src);

void bds_session_id_build(bds_session_id_t* dst, char* sn);

int bds_sm_create_session(bds_session_manager_h handle, bds_session_param_t* param);

void bds_sm_destroy_session(bds_session_manager_h handle, bds_session_id_t* id);

void bds_sm_put_online_audio(bds_session_manager_h handle, bdsc_event_data_t* data);

int bds_sm_take_online_audio(bds_session_manager_h handle, bds_session_id_t* id, bds_tts_frame_t** data);

int bds_sm_start_asr(bds_session_manager_h handle, bds_session_id_t* id);

int bds_sm_direct_trigger(bds_session_manager_h handle, bds_session_id_t* id, bdsc_event_direct_t* event);

int bds_sm_online_play(bds_session_manager_h handle, bds_session_id_t* id);

int bds_sm_active_start_asr(bds_session_manager_h handle);

int bds_sm_active_direct_trigger(bds_session_manager_h handle, bdsc_event_direct_t* event);

#ifdef __cplusplus
}
#endif

#endif /* _BDS_SESSION_MANAGER_H_ */
