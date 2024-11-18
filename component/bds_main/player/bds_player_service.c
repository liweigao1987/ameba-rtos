#include "bds_player_service.h"

#include "ameba_soc.h"
#include "audio/audio_control.h"
#include "audio/audio_equalizer.h"
#include "audio/audio_service.h"
#include "audio/audio_track.h"
#include "bds_client_log.h"
#include "bds_client_memory.h"
#include "bds_macro.h"
#include "bds_player_type.h"
#include "bds_ringbuffer.h"
#include "bds_task.h"
#include "common/audio_errnos.h"

#define TAG "player_service"

typedef struct {
    bds_ringbuffer_h     audio_rb;
    bds_task_h           ps_task;
    struct RTAudioTrack* audio_track;
    int                  mbb;
} bds_player_service_t;

static void init_audio_track(bds_player_service_t* h) {
    RTAudioService_Init();
    h->audio_track = RTAudioTrack_Create();
    h->mbb =
        RTAudioTrack_GetMinBufferBytes(h->audio_track, RTAUDIO_CATEGORY_MEDIA, AUDIO_SR, AUDIO_FORMAT, AUDIO_CH) * 4;
    bdsc_logw(TAG, "minbuffersize=%d", h->mbb);
    RTAudioTrackConfig cfg;
    cfg.category_type = RTAUDIO_CATEGORY_MEDIA;
    cfg.sample_rate   = AUDIO_SR;
    cfg.format        = AUDIO_FORMAT;
    cfg.channel_count = AUDIO_CH;
    cfg.buffer_bytes  = h->mbb;
    RTAudioTrack_Init(h->audio_track, &cfg, RTAUDIO_OUTPUT_FLAG_NONE);
    /* RTAudioTrack_SetVolume(h->audio_track, 1.0, 1.0); */  // use passthrough mode
    RTAudioControl_SetHardwareVolume(1.0, 1.0);
    /* RTAudioTrack_SetStartThresholdBytes(h->audio_track, h->mbb); */
    /* uint32_t track_start_threshold = RTAudioTrack_GetStartThresholdBytes(h->audio_track); */
    /* bdsc_logw(TAG, "start threshold=%u", track_start_threshold); */
}
static void ps_run(bds_player_service_t* h) {
    bdsc_logw(TAG, "ps_run +");
    init_audio_track(h);
    int ret = RTAudioTrack_Start(h->audio_track);
    if (ret != AUDIO_OK) {
        bdsc_loge(TAG, "audio track start failed! ret=%d", ret);
        vTaskDelete(NULL);
        return;
    }
    uint8_t audio[PCM_BAG_BYTES];
    int     need_bytes = sizeof(audio);
    while (1) {
        ret = bds_ringbuffer_take(h->audio_rb, audio, need_bytes);
        if (ret != need_bytes) {
            bdsc_loge(TAG, "take audiorb failed! %d!=%d", ret, need_bytes);
        }
        ret = RTAudioTrack_Write(h->audio_track, audio, need_bytes, true);
        if (ret != need_bytes) {
            bdsc_loge(TAG, "audiotrack write failed! ret=%d", ret);
        }
    }
    RTAudioTrack_Pause(h->audio_track);
    RTAudioTrack_Flush(h->audio_track);
    RTAudioTrack_Stop(h->audio_track);
    RTAudioTrack_Destroy(h->audio_track);
    bdsc_logw(TAG, "ps_run -");
    vTaskDelete(NULL);
}

bds_player_service_h bds_player_service_create() {
    bds_player_service_t* h = bdsc_malloc(sizeof(bds_player_service_t));
    h->audio_rb             = bds_ringbuffer_create(AUDIO_RB_SIZE);
    h->ps_task              = bds_task_create(ps_run, "bds_ps", 10 * 1024, h, BDS_THREAD_PRIORITY_HIGH);
    return h;
}

void bds_player_service_destroy(bds_player_service_h handle) {
    if (!handle) {
        return;
    }
    bds_player_service_t* h = handle;
    if (h->audio_rb) {
        bds_ringbuffer_destroy(h->audio_rb);
        h->audio_rb = NULL;
    }
    bdsc_free(handle);
}

int bds_ps_put_audio(bds_player_service_h handle, uint8_t* audio, int len) {
    if (!handle || !audio) {
        bdsc_loge(TAG, "invalid params! h=%p, a=%p", handle, audio);
        return -1;
    }
    bds_player_service_t* h          = handle;
    int                   need_bytes = len;
    int                   ret        = bds_ringbuffer_put_timeout(h->audio_rb, audio, need_bytes, portMAX_DELAY);
    if (ret != 0) {
        bdsc_loge(TAG, "player service put failed! ret=%d", ret);
    }
    return ret;
}

int bds_ps_passthrough_audio(bds_player_service_h handle, uint8_t* audio, int len) {
    bds_player_service_t* h          = handle;
    int                   need_bytes = len;
    int                   ret        = RTAudioTrack_Write(h->audio_track, audio, need_bytes, true);
    if (ret != need_bytes) {
        bdsc_loge(TAG, "audiotrack write failed! ret=%d", ret);
    }
    return ret;
}
