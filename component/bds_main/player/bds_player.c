#include "bds_player.h"

#include <stdio.h>

#include "bds_audio_type.h"
#include "bds_client_log.h"
#include "bds_client_memory.h"
#include "bds_file.h"
#include "bds_macro.h"
#include "bds_player_service.h"
#include "bdsc_executor.h"

#define TAG "player"

typedef struct {
    bds_main_ctx_h       ctx;
    bdsc_executor_h      executor;
    bds_player_service_h pservice;
    bds_file_h           file;
} bds_player_t;

bds_player_h bds_player_create(bds_main_ctx_h ctx) {
    bds_player_t* h                 = bdsc_malloc(sizeof(bds_player_t));
    h->ctx                          = ctx;
    bdsc_executor_param_t exe_param = {
        .name       = "player_exe",
        .capacity   = 3,
        .stack_size = 10 * 1024,
        .priority   = BDS_THREAD_PRIORITY_HIGH,
    };
    h->executor = bdsc_executor_create(&exe_param);
    h->pservice = bds_player_service_create();
    h->file     = bds_file_get_instance();
    return h;
}

void bds_player_destroy(bds_player_h handle) {
    if (!handle) {
        return;
    }
    bds_player_t* h = handle;
    if (h->pservice) {
        bds_player_service_destroy(h->pservice);
        h->pservice = NULL;
    }
    if (h->file) {
        bds_file_destroy(h->file);
        h->file = NULL;
    }
    if (h->executor) {
        bdsc_executor_destroy(h->executor);
        h->executor = NULL;
    }
    bdsc_free(handle);
}

int bds_player_load_cfg(bds_player_h handle) {
    bds_player_t* h = handle;
    return bds_file_load_cfg(h->file);
}

static void file_play(bds_player_t* h, char* path) {
    int             file_len = bds_file_get_len(path);
    FILE*           fp       = bds_file_open(path, "r");
    bds_audio_bag_t bag;
    int             quot = file_len / PCM_BAG_BYTES;
    int             rema = file_len % PCM_BAG_BYTES;
    bdsc_logw(TAG, "quot=%d, rema=%d", quot, rema);
    int ret = 0;
    for (int i = 0; i < quot; i++) {
        ret = bds_file_read(bag.audio, PCM_BAG_BYTES, fp);
        if (ret != PCM_BAG_BYTES) {
            bdsc_loge(TAG, "fread failed! %d:%d", ret, PCM_BAG_BYTES);
        }
        bds_ps_put_audio(h->pservice, &bag);
    }
    if (rema > 0) {
        memset(bag.audio, 0, PCM_BAG_BYTES);
        ret = bds_file_read(bag.audio, rema, fp);
        if (ret != rema) {
            bdsc_loge(TAG, "fread rema failed! %d:%d", ret, rema);
        }
        bds_ps_put_audio(h->pservice, &bag);
    }
    bds_file_close(fp);
}

static void wp_play_run(bds_player_t* h, void* param) {
    file_play(h, bds_file_get_wp_audio_path());
}

int bds_player_wp_play(bds_player_h handle) {
    bds_player_t* h = handle;
    bdsc_executor_submit2_easy(h->executor, wp_play_run, h, NULL, NULL);
    return 0;
}

static void direct_play_run(bds_player_t* h, char* code) {
    char* path = bds_file_get_direct_audio_path(code);
    if (!path) {
        bdsc_loge(TAG, "direct code no audio! code=%s", code);
        return;
    }
    file_play(h, path);
}

int bds_player_direct_play(bds_player_h handle, char* code) {
    if (!handle || !code) {
        bdsc_loge(TAG, "invalid params! h=%p, code=%p", handle, code);
        return -1;
    }
    bds_player_t* h     = handle;
    int           len   = strlen(code) + 1;
    char*         param = bdsc_malloc(len);
    memcpy(param, code, len);
    bdsc_executor_submit2_easy(h->executor, direct_play_run, h, param, bdsc_free);
    return 0;
}
