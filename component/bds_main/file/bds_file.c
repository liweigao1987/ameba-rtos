#include "bds_file.h"

#include "bds_client_log.h"
#include "bds_client_memory.h"
#include "bds_file_type.h"
#include "cJSON.h"
#include "vfs.h"

#define TAG "file"

typedef struct {
    char*  lfs_prefix;
    cJSON* cfg_root;
    cJSON* cfg_direct;
    char*  wp_audio_path;
    int    wp_audio_size;
} bds_file_t;

static bds_file_t* s_file = NULL;

static bds_file_h bds_file_create() {
    bds_file_t* h = bdsc_malloc(sizeof(bds_file_t));
    h->lfs_prefix = find_vfs_tag(VFS_REGION_1);
    return h;
}

bds_file_h bds_file_get_instance() {
    if (!s_file) {
        s_file = bds_file_create();
    }
    return s_file;
}

void bds_file_destroy(bds_file_h handle) {
    if (!handle) {
        return;
    }
    bds_file_t* h = handle;
    if (h->cfg_root) {
        cJSON_Delete(h->cfg_root);
        h->cfg_root = NULL;
    }
    bdsc_free(handle);
}

static void get_lfs_path(bds_file_t* h, char* dst, int dst_len, char* src) {
    DiagSnPrintf(dst, dst_len, "%s:%s", h->lfs_prefix, src);
    /* bdsc_logw(TAG, "src=%s, dst=%s", src, dst); */
}

int bds_file_load_cfg(bds_file_h handle) {
    bds_file_t* h        = handle;
    char*       cfg_path = AUDIO_CFG_PATH;
    int         f_len    = bds_file_get_len(cfg_path);
    char*       f_data   = bdsc_malloc(f_len + 1);
    FILE*       fp       = bds_file_open(cfg_path, "r");
    bds_file_read(f_data, f_len, fp);
    bds_file_close(fp);
    h->cfg_root = cJSON_Parse(f_data);
    bdsc_free(f_data);
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
    h->wp_audio_path = wp_audio_j->valuestring;
    bdsc_logw(TAG, "wp_audio_path=%s", h->wp_audio_path);
    h->wp_audio_size = bds_file_get_len(h->wp_audio_path);
    h->cfg_direct    = cJSON_GetObjectItem(h->cfg_root, "direct");
    return 0;
}

FILE* bds_file_open(char* path, char* mode) {
    bds_file_t* h = s_file;
    if (!h) {
        bdsc_loge(TAG, "bds_file uncreate!");
        return NULL;
    }
    char lfs_path[128] = {0};
    get_lfs_path(h, lfs_path, sizeof(lfs_path), path);
    return fopen(lfs_path, mode);
}

void bds_file_close(FILE* fp) {
    if (!fp) {
        return;
    }
    fclose(fp);
}

int bds_file_read(uint8_t* buffer, uint32_t bytes, FILE* fp) {
    if (!buffer || !fp) {
        bdsc_loge(TAG, "invalid params! b=%p, fp=%p", buffer, fp);
        return -1;
    }
    return fread(buffer, 1, bytes, fp);
}

int bds_file_get_len(char* path) {
    FILE* fp = bds_file_open(path, "r");
    fseek(fp, 0, SEEK_END);
    int f_len = ftell(fp);
    bdsc_logw(TAG, "file=%s, len=%d", path, f_len);
    /* fseek(fp, 0, SEEK_SET); */
    bds_file_close(fp);
    return f_len;
}

char* bds_file_get_wp_audio_path() {
    bds_file_t* h = s_file;
    if (!h) {
        return NULL;
    }
    return h->wp_audio_path;
}

int bds_file_get_wp_audio_len() {
    bds_file_t* h = s_file;
    if (!h) {
        return -1;
    }
    return h->wp_audio_size;
}

char* bds_file_get_direct_audio_path(char* code) {
    bds_file_t* h = s_file;
    if (!h || !h->cfg_direct) {
        return NULL;
    }
    cJSON* direct_audio_j = cJSON_GetObjectItem(h->cfg_direct, code);
    if (!direct_audio_j) {
        bdsc_loge(TAG, "direct audio null! code=%s", code);
        return NULL;
    }
    return direct_audio_j->valuestring;
}
