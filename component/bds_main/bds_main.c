#include "bds_main.h"

#include "bds_client_log.h"
#include "bds_macro.h"
#include "bds_task.h"
#include "bds_main_ctx.h"

#define TAG "bds_main"

static bds_task_h s_main_task = NULL;
static bds_main_ctx_h s_main_ctx  = NULL;

static void main_run() {
    bdsc_logw(TAG, "main_run +");
    s_main_ctx = bds_main_ctx_create();
    bdsc_logw(TAG, "main_run -");
    vTaskDelete(NULL);
}

void bds_main() {
    printf("bds_main +\n");
    s_main_task = bds_task_create(main_run, "bds_main", 40 * 1024, NULL, BDS_THREAD_PRIORITY_DEFAULT);
    printf("bds_main -\n");
}
