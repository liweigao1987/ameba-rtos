/***********************************************************
 * @author        liweigao
 * @date          2024-11-11
 * @brief         player wrapper
 ***********************************************************/

#ifndef _BDS_PLAYER_H_
#define _BDS_PLAYER_H_

#include "bds_main_ctx.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* bds_player_h;

bds_player_h bds_player_create(bds_main_ctx_h ctx);

void bds_player_destroy(bds_player_h handle);

int bds_player_load_cfg(bds_player_h handle);

int bds_player_wp_play(bds_player_h handle);

int bds_player_direct_play(bds_player_h handle, char* code);

#ifdef __cplusplus
}
#endif

#endif /* _BDS_PLAYER_H_ */
