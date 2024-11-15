#include "bds_session_manager.h"

#include "bds_client_log.h"
#include "bds_client_memory.h"
#include "bds_session_type.h"
#include "list.h"
#include "bds_session.h"

#define TAG "session_manager"

typedef struct {
    List_t*           list;
    bds_session_id_t* active_id;
} bds_session_manager_t;

bds_session_manager_h bds_session_manager_create() {
    bds_session_manager_t* h = bdsc_malloc(sizeof(bds_session_manager_t));
    h->list                  = bdsc_malloc(sizeof(List_t));
    vListInitialise(h->list);
    return h;
}

void bds_session_manager_destroy(bds_session_manager_h handle) {
    if (!handle) {
        return;
    }
    bds_session_manager_t* h = handle;
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
    bdsc_free(handle);
}

int bds_sm_create_session(bds_session_manager_h handle, bds_session_param_t* param) {
    bds_session_manager_t* h       = handle;
    bds_session_h          session = bds_session_create(param);
    ListItem_t*            item    = bdsc_malloc(sizeof(ListItem_t));
    vListInitialiseItem(item);
    listSET_LIST_ITEM_OWNER(item, session);
    vListInsertEnd(h->list, item);
    return 0;
}

void bds_sm_destroy_session(bds_session_manager_h handle, bds_session_id_t* id) {
    bds_session_manager_t* h = handle;
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
    //unlock
    bdsc_logw(TAG, "remove session");
}

void bds_sm_put_online_audio(bds_session_manager_h handle, bdsc_event_data_t* data) {
}

bds_session_id_t* bds_session_id_create(char* sn) {
    bds_session_id_t* id = bdsc_malloc(sizeof(bds_session_id_t));
    memcpy(id->sn, sn, SN_LENGTH);
    return id;
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
