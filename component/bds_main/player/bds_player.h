/***********************************************************
 * @author        liweigao
 * @date          2024-11-11
 * @brief         player wrapper
 ***********************************************************/

#ifndef _BDS_PLAYER_H_
#define _BDS_PLAYER_H_

#include "bds_main_ctx.h"
#include "bds_session_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* bds_player_h;

bds_player_h bds_player_create(bds_main_ctx_h ctx);

void bds_player_destroy(bds_player_h handle);

int bds_player_load_cfg(bds_player_h handle);

int bds_player_wp_play(bds_player_h handle);

int bds_player_direct_play(bds_player_h handle, char* code);

int bds_player_online_play(bds_player_h handle, bds_session_id_t* id);

#ifdef __cplusplus
}
#endif

#endif /* _BDS_PLAYER_H_ */
