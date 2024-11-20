#include "bds_player.h"

#include <stdio.h>

#include "bds_audio_type.h"
#include "bds_client_log.h"
#include "bds_client_memory.h"
#include "bds_file.h"
#include "bds_macro.h"
#include "bds_opus_dec.h"
#include "bds_opus_type.h"
#include "bds_player_service.h"
#include "bds_player_type.h"
#include "bdsc_executor.h"
#include "bds_main_ctx.h"

#define TAG "player"

typedef struct {
    bds_main_ctx_h       ctx;
    bdsc_executor_h      executor;
    bds_player_service_h pservice;
    bds_file_h           file;
    bds_opus_dec_h       opus_dec;
} bds_player_t;

bds_player_h bds_player_create(bds_main_ctx_h ctx) {
    bds_player_t* h                 = bdsc_malloc(sizeof(bds_player_t));
    h->ctx                          = ctx;
    bdsc_executor_param_t exe_param = {
        .name       = "player_exe",
        .capacity   = 3,
        .stack_size = 20 * 1024,
        .priority   = BDS_THREAD_PRIORITY_HIGH,
    };
    h->executor = bdsc_executor_create(&exe_param);
    h->pservice = bds_player_service_create();
    h->file     = bds_file_get_instance();
    h->opus_dec = bds_opus_dec_create();
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
    if (h->opus_dec) {
        bds_opus_dec_destroy(h->opus_dec);
        h->opus_dec = NULL;
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
    int     file_len = bds_file_get_len(path);
    FILE*   fp       = bds_file_open(path, "r");
    uint8_t bag[PCM_BAG_BYTES];
    int     quot = file_len / PCM_BAG_BYTES;
    int     rema = file_len % PCM_BAG_BYTES;
    bdsc_logw(TAG, "quot=%d, rema=%d", quot, rema);
    int ret = 0;
    for (int i = 0; i < quot; i++) {
        ret = bds_file_read(bag, PCM_BAG_BYTES, fp);
        if (ret != PCM_BAG_BYTES) {
            bdsc_loge(TAG, "fread failed! %d:%d", ret, PCM_BAG_BYTES);
        }
        bds_ps_put_audio(h->pservice, bag, PCM_BAG_BYTES);
    }
    if (rema > 0) {
        memset(bag, 0, PCM_BAG_BYTES);
        ret = bds_file_read(bag, rema, fp);
        if (ret != rema) {
            bdsc_loge(TAG, "fread rema failed! %d:%d", ret, rema);
        }
        bds_ps_put_audio(h->pservice, bag, rema);
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

static void decode_tts(bds_player_t* h, bds_tts_frame_t* frame) {
    int header_format = char_to_int(frame->audio);
    bdsc_logw(TAG, "header_format=%08x", header_format);
    int     opus_bag_bytes = tts_frame_get_audio_length(frame) - 4;
    char*   bag_addr       = frame->audio + 4;
    uint8_t bag[PCM_20MS_BYTES];
    int     dec_len = 0;
    int     quot    = opus_bag_bytes / OPUS_TTS_BAG;
    bdsc_logw(TAG, "audio len=%d, quot=%d", opus_bag_bytes, quot);
    for (int i = 0; i < quot; i++) {
        bds_opus_dec_proc(h->opus_dec, bag, &dec_len, bag_addr, OPUS_TTS_BAG);
        if (dec_len != PCM_20MS_BYTES) {
            bdsc_loge(TAG, "dec error! %d:%d", dec_len, PCM_20MS_BYTES);
        }
        bds_ps_put_audio(h->pservice, bag, dec_len);
        bag_addr += OPUS_TTS_BAG;
    }
}

static void online_play_run(bds_player_t* h, bds_session_id_t* id) {
    bds_session_manager_h sm = bds_mc_get_session_manager(h->ctx);
    bds_tts_frame_t*      tts_frame;
    int                   ret = 0;
    while (1) {
        tts_frame = NULL;
        ret       = bds_sm_take_online_audio(sm, id, &tts_frame);
        if (ret != 0) {
            bdsc_logw(TAG, "take tts failed! ret=%d", ret);
            break;
        }
        if (!tts_frame || tts_frame->header_length_offset == 0) {
            bdsc_loge(TAG, "no tts data! f=%p", tts_frame);
            break;
        }
        decode_tts(h, tts_frame);
        bds_tts_frame_destroy(tts_frame);
    }
}

int bds_player_online_play(bds_player_h handle, bds_session_id_t* id) {
    if (!handle || !id) {
        bdsc_loge(TAG, "invalid params! h=%p, id=%p", handle, id);
        return -1;
    }
    bds_player_t*     h     = handle;
    bds_session_id_t* param = bds_session_id_create2(id);
    bdsc_executor_submit2_easy(h->executor, online_play_run, h, param, bds_session_id_destroy);
    return 0;
}
