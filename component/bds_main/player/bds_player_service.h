/***********************************************************
 * @author        liweigao
 * @date          2024-11-11
 * @brief         player service
 ***********************************************************/

#ifndef _BDS_PLAYER_SERVICE_H_
#define _BDS_PLAYER_SERVICE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* bds_player_service_h;

bds_player_service_h bds_player_service_create();

void bds_player_service_destroy(bds_player_service_h handle);

int bds_ps_put_audio(bds_player_service_h handle, uint8_t* audio, int len);

int bds_ps_passthrough_audio(bds_player_service_h handle, uint8_t* audio, int len);

#ifdef __cplusplus
}
#endif

#endif /* _BDS_PLAYER_SERVICE_H_ */
