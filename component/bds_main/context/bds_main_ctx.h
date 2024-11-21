/***********************************************************
 * @author        liweigao
 * @date          2024-11-08
 * @brief         app context
 ***********************************************************/

#ifndef _BDS_MAIN_CTX_H_
#define _BDS_MAIN_CTX_H_

#include "bds_client_event.h"
#include "bds_net_manager.h"
#include "bds_player.h"
#include "bds_session_manager.h"
#include "bds_speech.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* bds_main_ctx_h;

bds_main_ctx_h bds_main_ctx_create();

void bds_main_ctx_destroy(bds_main_ctx_h handle);

bds_session_manager_h bds_mc_get_session_manager(bds_main_ctx_h handle);

bds_net_manager_h bds_mc_get_net_manager(bds_main_ctx_h handle);

bds_player_h bds_mc_get_player(bds_main_ctx_h handle);

bds_speech_h bds_mc_get_speech(bds_main_ctx_h handle);

void bds_mc_submit_wp(bds_main_ctx_h handle, bdsc_event_wakeup_t* event);

void bds_mc_submit_direct(bds_main_ctx_h handle, bdsc_event_direct_t* event);

void bds_mc_submit_wifi_connected(bds_main_ctx_h handle);

void bds_mc_submit_online_play(bds_main_ctx_h handle, bdsc_event_process_t* event);

void bds_mc_play_saopin(bds_main_ctx_h handle);

#ifdef __cplusplus
}
#endif

#endif /* _BDS_MAIN_CTX_H_ */
