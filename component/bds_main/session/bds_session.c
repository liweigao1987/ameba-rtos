#include "bds_session.h"

#include "bds_client_log.h"
#include "bds_client_memory.h"
#include "bds_session_type.h"

#define TAG "session"

typedef struct {
    bds_session_param_t param;
    bds_session_id_t    id;
} bds_session_t;

bds_session_h bds_session_create(bds_session_param_t* param, bds_session_id_t* id) {
    bds_session_t* h = bdsc_malloc(sizeof(bds_session_t));
    memcpy(&h->param, param, sizeof(bds_session_param_t));
    memcpy(&h->id, id, sizeof(bds_session_id_t));
    return h;
}

void bds_session_destroy(bds_session_h handle) {
    if (!handle) {
        return;
    }

    bdsc_free(handle);
}

bds_session_id_t* bds_session_get_id(bds_session_h handle) {
    if (!handle) {
        bdsc_loge(TAG, "invalid params! h=%p", handle);
        return NULL;
    }
    bds_session_t* h = handle;
    return &h->id;
}

void bds_session_put_online_audio(bds_session_h handle, bdsc_event_data_t* data) {
}
