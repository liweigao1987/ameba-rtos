#include "bds_main_ctx.h"

#include <stdlib.h>

#include "bds_client_log.h"
#include "bds_client_memory.h"
#include "bds_macro.h"
#include "bdsc_executor.h"

#define TAG "ctx"

typedef struct {
    bdsc_executor_h       executor;
    bds_speech_h          speech;
    bds_player_h          player;
    bds_net_manager_h     net_manager;
    bds_session_manager_h session_manager;
} bds_main_ctx_t;

bds_main_ctx_h bds_main_ctx_create() {
    bds_main_ctx_t*       h         = bdsc_malloc(sizeof(bds_main_ctx_t));
    bdsc_executor_param_t exe_param = {
        .name       = "main_exe",
        .capacity   = 100,
        .stack_size = 10 * 1024,
        .priority   = BDS_THREAD_PRIORITY_MAX,
    };
    h->executor = bdsc_executor_create(&exe_param);
    h->speech   = bds_speech_create(h);
    h->player   = bds_player_create(h);
    bds_player_load_cfg(h->player);
    h->net_manager = bds_net_manager_create(h);
    bds_nm_check_wifi_status(h->net_manager);
    h->session_manager = bds_session_manager_create(h);
    return h;
}

void bds_main_ctx_destroy(bds_main_ctx_h handle) {
    if (!handle) {
        return;
    }
    bds_main_ctx_t* h = handle;
    if (h->speech) {
        bds_speech_destroy(h->speech);
        h->speech = NULL;
    }
    if (h->player) {
        bds_player_destroy(h->player);
        h->player = NULL;
    }
    if (h->session_manager) {
        bds_session_manager_destroy(h->session_manager);
        h->session_manager = NULL;
    }
    if (h->net_manager) {
        bds_net_manager_destroy(h->net_manager);
        h->net_manager = NULL;
    }
    if (h->executor) {
        bdsc_executor_destroy(h->executor);
        h->executor = NULL;
    }
    bdsc_free(handle);
}

bds_session_manager_h bds_mc_get_session_manager(bds_main_ctx_h handle) {
    if (!handle) {
        return NULL;
    }
    bds_main_ctx_t* h = handle;
    return h->session_manager;
}

bds_net_manager_h bds_mc_get_net_manager(bds_main_ctx_h handle) {
    if (!handle) {
        return NULL;
    }
    bds_main_ctx_t* h = handle;
    return h->net_manager;
}

bds_player_h bds_mc_get_player(bds_main_ctx_h handle) {
    if (!handle) {
        return NULL;
    }
    bds_main_ctx_t* h = handle;
    return h->player;
}

bds_speech_h bds_mc_get_speech(bds_main_ctx_h handle) {
    if (!handle) {
        return NULL;
    }
    bds_main_ctx_t* h = handle;
    return h->speech;
}

static void wp_trigger_run(bds_main_ctx_t* h, bdsc_event_wakeup_t* event) {
    bdsc_logw(TAG, "executor wp");
    bds_session_param_t param = {0};
    param.create_tick         = bdsc_get_tick_count();
    bds_sm_create_session(h->session_manager, &param);
    bds_player_wp_play(h->player);
    bds_session_id_t id  = {0};
    int              ret = bds_sm_active_session_id(h->session_manager, &id);
    if (ret != 0) {
        bdsc_loge(TAG, "no id! ret=%d", ret);
        // todo: need free session create
        return;
    }
    ret = bds_sm_start_asr(h->session_manager, &id);
    bdsc_logw(TAG, "wp_trigger ret=%d", ret);
}

void bds_mc_submit_wp(bds_main_ctx_h handle, bdsc_event_wakeup_t* event) {
    bds_main_ctx_t*      h     = handle;
    bdsc_event_wakeup_t* param = bdsc_malloc(sizeof(bdsc_event_wakeup_t));
    memcpy(param, event, sizeof(bdsc_event_wakeup_t));
    bdsc_executor_submit2_easy(h->executor, wp_trigger_run, h, param, bdsc_free);
}

static void direct_trigger_run(bds_main_ctx_t* h, bdsc_event_direct_t* event) {
    bdsc_logw(TAG, "executor direct=%s", event->keywords);
    bds_session_id_t id  = {0};
    int              ret = bds_sm_active_session_id(h->session_manager, &id);
    if (ret != 0) {
        bdsc_loge(TAG, "no id! ret=%d", ret);
        // todo: need free session create
        return;
    }
    ret = bds_sm_direct_trigger(h->session_manager, &id, event);
    bdsc_logw(TAG, "direct_trigger ret=%d", ret);
}

void bds_mc_submit_direct(bds_main_ctx_h handle, bdsc_event_direct_t* event) {
    bds_main_ctx_t*      h     = handle;
    bdsc_event_direct_t* param = bdsc_malloc(sizeof(bdsc_event_direct_t));
    memcpy(param, event, sizeof(bdsc_event_direct_t));
    bdsc_executor_submit2_easy(h->executor, direct_trigger_run, h, param, bdsc_free);
}

static void wifi_connected_run(bds_main_ctx_t* h, void* param) {
    bds_speech_start_link(h->speech);
}

void bds_mc_submit_wifi_connected(bds_main_ctx_h handle) {
    bds_main_ctx_t* h = handle;
    bdsc_executor_submit2_easy(h->executor, wifi_connected_run, h, NULL, NULL);
}

static void online_play_run(bds_main_ctx_t* h, bds_session_id_t* id) {
    bds_player_online_play(h->player, id);
}

void bds_mc_submit_online_play(bds_main_ctx_h handle, bdsc_event_process_t* event) {
    bds_main_ctx_t*   h     = handle;
    bds_session_id_t* param = bds_session_id_create(event->sn);
    bdsc_executor_submit2_easy(h->executor, online_play_run, h, param, bds_session_id_destroy);
}
