/***********************************************************
 * @author        liweigao
 * @date          2024-11-11
 * @brief         player wrapper
 ***********************************************************/

#ifndef _BDS_PLAYER_H_
#define _BDS_PLAYER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void* bds_player_h;

bds_player_h bds_player_create();

void bds_player_destroy(bds_player_h handle);

    int bds_player_load_cfg(bds_player_h handle);

int bds_player_wp_play(bds_player_h handle);

#ifdef __cplusplus
}
#endif

#endif /* _BDS_PLAYER_H_ */
