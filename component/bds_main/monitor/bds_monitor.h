/***********************************************************
 * @author        liweigao
 * @date          2024-11-20
 * @brief         system monitor
 ***********************************************************/

#ifndef _BDS_MONITOR_H_
#define _BDS_MONITOR_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void* bds_monitor_h;

bds_monitor_h bds_monitor_create();

void bds_monitor_destroy(bds_monitor_h handle);

#ifdef __cplusplus
}
#endif

#endif /* _BDS_MONITOR_H_ */
