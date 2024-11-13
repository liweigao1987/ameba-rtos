/***********************************************************
 * @author        liweigao
 * @date          2024-11-11
 * @brief         player service
 ***********************************************************/

#ifndef _BDS_PLAYER_SERVICE_H_
#define _BDS_PLAYER_SERVICE_H_

#include "bds_audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* bds_player_service_h;

bds_player_service_h bds_player_service_create();

void bds_player_service_destroy(bds_player_service_h handle);

int bds_ps_put_audio(bds_player_service_h handle, bds_audio_bag_t* audio);

int bds_ps_passthrough_audio(bds_player_service_h handle, bds_audio_bag_t* audio);

#ifdef __cplusplus
}
#endif

#endif /* _BDS_PLAYER_SERVICE_H_ */
