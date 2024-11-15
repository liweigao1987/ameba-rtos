/***********************************************************
 * @author        liweigao
 * @date          2024-11-15
 * @brief         session manager
 ***********************************************************/

#ifndef _BDS_SESSION_MANAGER_H_
#define _BDS_SESSION_MANAGER_H_

#include "bds_session_type.h"
#include "bds_client_event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* bds_session_manager_h;

bds_session_manager_h bds_session_manager_create();

void bds_session_manager_destroy(bds_session_manager_h handle);

int bds_sm_create_session(bds_session_manager_h handle, bds_session_param_t* param);

void bds_sm_destroy_session(bds_session_manager_h handle, bds_session_id_t* id);

void bds_sm_put_online_audio(bds_session_manager_h handle, bdsc_event_data_t* data);

bds_session_id_t* bds_session_id_create(char* sn);

void bds_session_id_destroy(bds_session_id_t* id);

bool bds_session_is_same(bds_session_id_t* id1, bds_session_id_t* id2);

#ifdef __cplusplus
}
#endif

#endif /* _BDS_SESSION_MANAGER_H_ */
