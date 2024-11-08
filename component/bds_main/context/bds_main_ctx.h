/***********************************************************
 * @author        liweigao
 * @date          2024-11-08
 * @brief         app context
 ***********************************************************/

#ifndef _BDS_MAIN_CTX_H_
#define _BDS_MAIN_CTX_H_

#include "bds_client_event.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* bds_main_ctx_h;

bds_main_ctx_h bds_main_ctx_create();

void bds_main_ctx_destroy(bds_main_ctx_h handle);

void bds_mc_submit_wp(bds_main_ctx_h handle, bdsc_event_wakeup_t* event);

#ifdef __cplusplus
}
#endif

#endif /* _BDS_MAIN_CTX_H_ */
