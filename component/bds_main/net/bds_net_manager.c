#include "bds_net_manager.h"

#include "bds_client_log.h"
#include "bds_client_memory.h"
#include "bds_task.h"
#include "bdsc_executor.h"
#include "lwip/netif.h"
#include "lwip_netconf.h"
#include "rtw_wifi_defs.h"
#include "wifi_conf.h"
#include "bds_macro.h"

#define TAG "net_manager"

typedef struct {
    bds_main_ctx_h  ctx;
    bdsc_executor_h executor;
} bds_net_manager_t;

bds_net_manager_h bds_net_manager_create(bds_main_ctx_h ctx) {
    bds_net_manager_t* h            = bdsc_malloc(sizeof(bds_net_manager_t));
    h->ctx                          = ctx;
    bdsc_executor_param_t exe_param = {
        .name       = "net_exe",
        .capacity   = 3,
        .stack_size = 10 * 1024,
        .priority   = BDS_THREAD_PRIORITY_DEFAULT,
    };
    h->executor = bdsc_executor_create(&exe_param);
    return h;
}

void bds_net_manager_destroy(bds_net_manager_h handle) {
    if (!handle) {
        return;
    }
    bds_net_manager_t* h = handle;
    if (h->executor) {
        bdsc_executor_destroy(h->executor);
        h->executor = NULL;
    }
    bdsc_free(handle);
}

static void check_wifi_status_run(bds_net_manager_t* h, void* param) {
    while (!((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32*)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
        bdsc_logw(TAG, "wait wifi connection ...");
        bds_delay(2000);
    }
    bds_mc_submit_wifi_connected(h->ctx);
}

void bds_nm_check_wifi_status(bds_net_manager_h handle) {
    bds_net_manager_t* h = handle;
    bdsc_executor_submit2_easy(h->executor, check_wifi_status_run, h, NULL, NULL);
}
