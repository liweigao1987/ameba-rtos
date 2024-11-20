/***********************************************************
 * @author        liweigao
 * @date          2024-11-08
 * @brief         net manager
 ***********************************************************/

#ifndef _BDS_NET_MANAGER_H_
#define _BDS_NET_MANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void* bds_net_manager_h;

bds_net_manager_h bds_net_manager_create(void* ctx);

void bds_net_manager_destroy(bds_net_manager_h handle);

void bds_nm_check_wifi_status(bds_net_manager_h handle);

#ifdef __cplusplus
}
#endif

#endif /* _BDS_NET_MANAGER_H_ */
