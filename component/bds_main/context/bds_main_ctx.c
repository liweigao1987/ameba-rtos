#include "bds_main_ctx.h"

#include <stdlib.h>

#include "bds_client_log.h"
#include "bds_client_memory.h"
#include "bds_speech.h"
#include "bdsc_executor.h"
#include "bds_player.h"

#define TAG "ctx"

typedef struct {
    bdsc_executor_h executor;
    bds_speech_h    speech;
    bds_player_h player;
} bds_main_ctx_t;

bds_main_ctx_h bds_main_ctx_create() {
    bds_main_ctx_t* h = bdsc_malloc(sizeof(bds_main_ctx_t));
    h->executor       = bdsc_executor_create("main_exe", 100);
    h->speech         = bds_speech_create(h);
    h->player = bds_player_create();
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
    if (h->executor) {
        bdsc_executor_destroy(h->executor);
        h->executor = NULL;
    }
    bdsc_free(handle);
}

static void wp_trigger_run(bds_main_ctx_t* h, bdsc_event_wakeup_t* event) {
    bdsc_logw(TAG, "executor wp!");
    bds_player_wp_play(h->player);

}

void bds_mc_submit_wp(bds_main_ctx_h handle, bdsc_event_wakeup_t* event) {
    bds_main_ctx_t*      h     = handle;
    bdsc_event_wakeup_t* param = bdsc_malloc(sizeof(bdsc_event_wakeup_t));
    memcpy(param, event, sizeof(bdsc_event_wakeup_t));
    bdsc_executor_submit2_easy(h->executor, wp_trigger_run, h, param, bdsc_free);
}
