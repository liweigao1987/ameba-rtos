#include "bds_player.h"

#include <stdio.h>

#include "bds_audio_type.h"
#include "bds_client_log.h"
#include "bds_client_memory.h"
#include "bds_player_service.h"
#include "bdsc_executor.h"
#include "cJSON.h"
#include "vfs.h"
#include "bds_macro.h"

#define TAG            "player"
#define AUDIO_DIR      "/audio"
#define AUDIO_CFG_PATH AUDIO_DIR "/audio.cfg"
#define KEY_ZAI_NE     "zai_ne"

typedef struct {
    bds_main_ctx_h       ctx;
    cJSON*               cfg_root;
    FILE*                wp_audio;
    int                  wp_audio_size;
    bdsc_executor_h      executor;
    bds_player_service_h pservice;
} bds_player_t;

bds_player_h bds_player_create(bds_main_ctx_h ctx) {
    bds_player_t* h                 = bdsc_malloc(sizeof(bds_player_t));
    h->ctx                          = ctx;
    bdsc_executor_param_t exe_param = {
        .name       = "player_exe",
        .capacity   = 3,
        .stack_size = 10 * 1024,
        .priority   = BDS_THREAD_PRIORITY_DEFAULT,
    };
    h->executor = bdsc_executor_create(&exe_param);
    h->pservice = bds_player_service_create();
    return h;
}

void bds_player_destroy(bds_player_h handle) {
    if (!handle) {
        return;
    }
    bds_player_t* h = handle;
    if (h->cfg_root) {
        cJSON_Delete(h->cfg_root);
        h->cfg_root = NULL;
    }
    if (h->wp_audio) {
        fclose(h->wp_audio);
        h->wp_audio = NULL;
    }
    if (h->pservice) {
        bds_player_service_destroy(h->pservice);
        h->pservice = NULL;
    }
    if (h->executor) {
        bdsc_executor_destroy(h->executor);
        h->executor = NULL;
    }
    bdsc_free(handle);
}

int bds_player_load_cfg(bds_player_h handle) {
    bds_player_t* h          = handle;
    char*         lfs_prefix = find_vfs_tag(VFS_REGION_1);
    char          path[128]  = {0};
    DiagSnPrintf(path, sizeof(path), "%s:%s", lfs_prefix, AUDIO_CFG_PATH);
    bdsc_logw(TAG, "path=%s", path);
    FILE* fp = fopen(path, "r");
    fseek(fp, 0, SEEK_END);
    int f_len = ftell(fp);
    bdsc_logw(TAG, "len=%d", f_len);
    fseek(fp, 0, SEEK_SET);
    char* f_data = bdsc_malloc(f_len + 1);
    fread(f_data, 1, f_len, fp);
    fclose(fp);
    h->cfg_root = cJSON_Parse(f_data);
    cJSON* wp_j = cJSON_GetObjectItem(h->cfg_root, "wp");
    if (!wp_j) {
        bdsc_loge(TAG, "wp json null!");
        return -10;
    }
    cJSON* wp_audio_j = cJSON_GetObjectItem(wp_j, KEY_ZAI_NE);
    if (!wp_audio_j) {
        bdsc_loge(TAG, "wp audio null!");
        return -11;
    }
    char* wp_audio = wp_audio_j->valuestring;
    bdsc_logw(TAG, "wp_audio=%s", wp_audio);
    memset(path, 0, sizeof(path));
    DiagSnPrintf(path, sizeof(path), "%s:%s", lfs_prefix, wp_audio);
    h->wp_audio = fopen(path, "r");
    fseek(h->wp_audio, 0, SEEK_END);
    h->wp_audio_size = ftell(h->wp_audio);
    bdsc_logw(TAG, "wp audio size=%d", h->wp_audio_size);
    bdsc_free(f_data);
    return 0;
}

static void wp_play_run(bds_player_t* h, void* param) {
    bdsc_logw(TAG, "wp play file=%p", h->wp_audio);
    fseek(h->wp_audio, 0, SEEK_SET);
    bds_audio_bag_t audio;
    int             quot = h->wp_audio_size / PCM_BAG_BYTES;
    for (int i = 0; i < quot; i++) {
        fread(audio.audio, 1, PCM_BAG_BYTES, h->wp_audio);
        bds_ps_put_audio(h->pservice, &audio);
    }
}

int bds_player_wp_play(bds_player_h handle) {
    bds_player_t* h = handle;
    bdsc_executor_submit2_easy(h->executor, wp_play_run, h, NULL, NULL);
    return 0;
}
