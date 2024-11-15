/***********************************************************
 * @author        liweigao
 * @date          2024-11-15
 * @brief         session type
 ***********************************************************/

#ifndef _BDS_SESSION_TYPE_H_
#define _BDS_SESSION_TYPE_H_

#include "bds_client_context.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char sn[SN_LENGTH];
} bds_session_id_t;

typedef struct {
    int temp;
} bds_session_param_t;

#ifdef __cplusplus
}
#endif

#endif /* _BDS_SESSION_TYPE_H_ */
