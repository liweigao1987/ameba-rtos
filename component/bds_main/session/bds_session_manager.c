#include "bds_session_manager.h"

#include "bds_client_log.h"
#include "bds_client_memory.h"
#include "bds_main_ctx.h"
#include "bds_semaphore_mutex.h"
#include "bds_session.h"
#include "bds_session_type.h"
#include "list.h"

#define TAG "session_manager"

typedef struct {
    bds_main_ctx_h ctx;
    List_t*        list;
    bds_session_h  active_session;
    bdsc_mutex_h   lock;
} bds_session_manager_t;

bds_session_manager_h bds_session_manager_create(bds_main_ctx_h ctx) {
    bds_session_manager_t* h = bdsc_malloc(sizeof(bds_session_manager_t));
    h->ctx                   = ctx;
    h->list                  = bdsc_malloc(sizeof(List_t));
    vListInitialise(h->list);
    h->lock = bdsc_mutex_create("sm_lock");
    return h;
}

void bds_session_manager_destroy(bds_session_manager_h handle) {
    if (!handle) {
        return;
    }
    bds_session_manager_t* h = handle;
    bdsc_mutex_lock(h->lock);
    bds_session_h session = NULL;
    if (!listLIST_IS_EMPTY(h->list)) {
        ListItem_t* head    = listGET_HEAD_ENTRY(h->list);
        ListItem_t* end     = listGET_END_MARKER(h->list);
        ListItem_t* current = NULL;
        for (current = head; current != end; current = listGET_NEXT(current)) {
            session = listGET_LIST_ITEM_OWNER(current);
            if (session) {
                bds_session_destroy(session);
                uxListRemove(current);
                bdsc_free(current);
            }
        }
    }
    bdsc_free(h->list);
    bdsc_mutex_unlock(h->lock);
    bdsc_mutex_destroy(h->lock);
    h->lock = NULL;
    bdsc_free(handle);
}

static void clone_session_id(bds_session_id_t* id, bds_session_h session) {
    bds_session_id_clone(id, bds_session_get_id(session));
}

int bds_sm_active_session_id(bds_session_manager_h handle, bds_session_id_t* id) {
    bds_session_manager_t* h = handle;
    bdsc_mutex_lock(h->lock);
    if (!h->active_session) {
        bdsc_logw(TAG, "no active session!");
        bdsc_mutex_unlock(h->lock);
        return -10;
    }
    clone_session_id(id, h->active_session);
    bdsc_mutex_unlock(h->lock);
    return 0;
}

static bds_session_h get_session(bds_session_manager_t* h, bds_session_id_t* id) {
    bds_session_h session = NULL;
    if (listLIST_IS_EMPTY(h->list)) {
        bdsc_loge(TAG, "list is empty!");
        goto EXIT;
    }
    if (h->active_session) {
        if (bds_session_is_same(bds_session_get_id(h->active_session), id)) {
            session = h->active_session;
            goto EXIT;
        }
    }

    ListItem_t* head    = listGET_HEAD_ENTRY(h->list);
    ListItem_t* end     = listGET_END_MARKER(h->list);
    ListItem_t* current = NULL;
    for (current = head; current != end; current = listGET_NEXT(current)) {
        session = listGET_LIST_ITEM_OWNER(current);
        if (session) {
            if (bds_session_is_same(bds_session_get_id(session), id)) {
                goto EXIT;
            } else {
                session = NULL;
            }
        }
    }

EXIT:
    return session;
}

int bds_sm_create_session(bds_session_manager_h handle, bds_session_param_t* param) {
    bds_session_manager_t* h = handle;
    bdsc_mutex_lock(h->lock);
    bds_session_id_t id = {0};
    bds_generate_uuid(id.sn);
    bdsc_logw(TAG, "new_session sn=%s", id.sn);
    bds_session_h new_session = bds_session_create(h->ctx, param, &id);
    ListItem_t*   item        = bdsc_malloc(sizeof(ListItem_t));
    vListInitialiseItem(item);
    listSET_LIST_ITEM_OWNER(item, new_session);
    vListInsertEnd(h->list, item);
    bds_session_h old_session = h->active_session;
    h->active_session         = new_session;
    if (old_session) {
        bds_session_id_t old_id = {0};
        clone_session_id(&old_id, old_session);
        bds_sm_destroy_session(h, &old_id);
    }
    bdsc_mutex_unlock(h->lock);
    return 0;
}

void bds_sm_destroy_session(bds_session_manager_h handle, bds_session_id_t* id) {
    bds_session_manager_t* h = handle;
    bdsc_mutex_lock(h->lock);
    bds_session_h session = NULL;
    if (listLIST_IS_EMPTY(h->list)) {
        bdsc_loge(TAG, "list is empty!");
        goto EXIT;
    }
    ListItem_t* head    = listGET_HEAD_ENTRY(h->list);
    ListItem_t* end     = listGET_END_MARKER(h->list);
    ListItem_t* current = NULL;
    for (current = head; current != end; current = listGET_NEXT(current)) {
        session = listGET_LIST_ITEM_OWNER(current);
        if (session) {
            if (bds_session_is_same(bds_session_get_id(session), id)) {
                bds_session_destroy(session);
                uxListRemove(current);
                bdsc_free(current);
                goto EXIT;
            } else {
                session = NULL;
            }
        }
    }
EXIT:
    bdsc_mutex_unlock(h->lock);
    bdsc_logw(TAG, "remove session");
}

void bds_sm_put_online_audio(bds_session_manager_h handle, bdsc_event_data_t* data) {
    bds_session_manager_t* h = handle;
    bdsc_mutex_lock(h->lock);
    bds_session_id_t id = {0};
    bds_session_id_build(&id, data->sn);
    bds_session_h session = get_session(h, &id);
    if (!session) {
        bdsc_loge(TAG, "no session! sn=%s", id.sn);
        bdsc_mutex_unlock(h->lock);
        return;
    }
    bds_session_put_online_audio(session, data);
    bdsc_mutex_unlock(h->lock);
}

int bds_sm_take_online_audio(bds_session_manager_h handle, bds_session_id_t* id, bds_tts_frame_t** data) {
    bds_session_manager_t* h = handle;
    bdsc_mutex_lock(h->lock);
    bds_session_h session = get_session(h, id);
    if (!session) {
        bdsc_loge(TAG, "no session! sn=%s", id->sn);
        *data = NULL;
        bdsc_mutex_unlock(h->lock);
        return -10;
    }
    int ret = bds_session_take_online_audio(session, data);
    bdsc_mutex_unlock(h->lock);
    return ret;
}

int bds_sm_start_asr(bds_session_manager_h handle, bds_session_id_t* id) {
    bds_session_manager_t* h = handle;
    bdsc_mutex_lock(h->lock);
    bds_session_h session = get_session(h, id);
    if (!session) {
        bdsc_loge(TAG, "no session! sn=%s", id->sn);
        bdsc_mutex_unlock(h->lock);
        return -10;
    }
    int ret = bds_session_start_asr(session);
    bdsc_mutex_unlock(h->lock);
    return ret;
}

int bds_sm_direct_trigger(bds_session_manager_h handle, bds_session_id_t* id, bdsc_event_direct_t* event) {
    bds_session_manager_t* h = handle;
    bdsc_mutex_lock(h->lock);
    bds_session_h session = get_session(h, id);
    if (!session) {
        bdsc_loge(TAG, "no session! sn=%s", id->sn);
        bdsc_mutex_unlock(h->lock);
        return -10;
    }
    int ret = bds_session_direct_trigger(session, event);
    bdsc_mutex_unlock(h->lock);
    return ret;
}

bds_session_id_t* bds_session_id_create(char* sn) {
    bds_session_id_t* id = bdsc_malloc(sizeof(bds_session_id_t));
    memcpy(id->sn, sn, SN_LENGTH);
    return id;
}

bds_session_id_t* bds_session_id_create2(bds_session_id_t* id) {
    bds_session_id_t* dst = bdsc_malloc(sizeof(bds_session_id_t));
    memcpy(dst, id, sizeof(bds_session_id_t));
    return dst;
}

void bds_session_id_destroy(bds_session_id_t* id) {
    bdsc_free(id);
}

bool bds_session_is_same(bds_session_id_t* id1, bds_session_id_t* id2) {
    if (!id1 || !id2) {
        bdsc_loge(TAG, "invalid params! id1=%p, id2=%p", id1, id2);
        return false;
    }
    if (strcmp(id1->sn, id2->sn) == 0) {
        return true;
    } else {
        return false;
    }
}

void bds_session_id_clone(bds_session_id_t* dst, bds_session_id_t* src) {
    if (!dst || !src) {
        bdsc_loge(TAG, "invalid params! d=%p, s=%p", dst, src);
        return;
    }
    memcpy(dst, src, sizeof(bds_session_id_t));
}

void bds_session_id_build(bds_session_id_t* dst, char* sn) {
    if (!dst || !sn) {
        bdsc_loge(TAG, "invalid params! d=%p, s=%p", dst, sn);
        return;
    }
    memcpy(dst->sn, sn, SN_LENGTH);
}
