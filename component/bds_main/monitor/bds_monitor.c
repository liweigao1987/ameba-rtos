#include "bds_monitor.h"

#include "bds_client_log.h"
#include "bds_client_memory.h"
#include "bds_macro.h"
#include "bds_task.h"

#define TAG               "monitor"
#define STATS_TICKS       pdMS_TO_TICKS(1000)
#define ARRAY_SIZE_OFFSET 5  // Increase this if print_real_time_stats returns -11
#define MONI_SWITCH       0

typedef struct {
    bds_task_h monitor_task;
} bds_monitor_t;

static int print_real_time_stats(TickType_t xTicksToWait) {
    TaskStatus_t *start_array = NULL, *end_array = NULL;
    UBaseType_t   start_array_size, end_array_size;
    uint32_t      start_run_time, end_run_time;
    int           ret;

    // Allocate array to store current task states
    start_array_size = uxTaskGetNumberOfTasks() + ARRAY_SIZE_OFFSET;
    start_array      = bdsc_malloc(sizeof(TaskStatus_t) * start_array_size);
    if (start_array == NULL) {
        ret = -10;
        goto exit;
    }
    // Get current task states
    start_array_size = uxTaskGetSystemState(start_array, start_array_size, &start_run_time);
    if (start_array_size == 0) {
        ret = -11;
        goto exit;
    }

    vTaskDelay(xTicksToWait);

    // Allocate array to store tasks states post delay
    end_array_size = uxTaskGetNumberOfTasks() + ARRAY_SIZE_OFFSET;
    end_array      = bdsc_malloc(sizeof(TaskStatus_t) * end_array_size);
    if (end_array == NULL) {
        ret = -10;
        goto exit;
    }
    // Get post delay task states
    end_array_size = uxTaskGetSystemState(end_array, end_array_size, &end_run_time);
    if (end_array_size == 0) {
        ret = -11;
        goto exit;
    }

    // Calculate total_elapsed_time in units of run time stats clock period.
    uint32_t total_elapsed_time = (end_run_time - start_run_time);
    if (total_elapsed_time == 0) {
        ret = -12;
        goto exit;
    }

    bdsc_logi(TAG, "|     Task     | Prio | Perc | stackWM ");
    // Match each task in start_array to those in the end_array
    for (int i = 0; i < start_array_size; i++) {
        int k = -1;
        for (int j = 0; j < end_array_size; j++) {
            if (start_array[i].xHandle == end_array[j].xHandle) {
                k = j;
                // Mark that task have been matched by overwriting their handles
                start_array[i].xHandle = NULL;
                end_array[j].xHandle   = NULL;
                break;
            }
        }
        // Check if matching task found
        if (k >= 0) {
            uint32_t task_elapsed_time = end_array[k].ulRunTimeCounter - start_array[i].ulRunTimeCounter;
            uint32_t percentage_time   = (task_elapsed_time * 100UL) / (total_elapsed_time * 1);
            bdsc_logi(TAG, "| %12s | %2d   | %2d%%  | %6d ", start_array[i].pcTaskName,
                      start_array[i].uxCurrentPriority, percentage_time,
                      start_array[i].usStackHighWaterMark * sizeof(StackType_t));
        }
    }

    // Print unmatched tasks
    for (int i = 0; i < start_array_size; i++) {
        if (start_array[i].xHandle != NULL) {
            bdsc_logi(TAG, "| %s | Deleted", start_array[i].pcTaskName);
        }
    }
    for (int i = 0; i < end_array_size; i++) {
        if (end_array[i].xHandle != NULL) {
            bdsc_logi(TAG, "| %s | Created", end_array[i].pcTaskName);
        }
    }
    ret = 0;

exit:  // Common return path
    bdsc_free(start_array);
    bdsc_free(end_array);
    return ret;
}

static void stats_mem() {
    uint32_t free_bytes = rtos_mem_get_free_heap_size();
    bdsc_logi(TAG, "------>heap_left:%u", free_bytes);
}

static void monitor_run(bds_monitor_t* h) {
    while (1) {
        bdsc_logi(TAG, "\n\n----------------------------------------");
        stats_mem();
        if (print_real_time_stats(STATS_TICKS) == 0) {
            bdsc_logi(TAG, "----------------------------------------");
        } else {
            bdsc_logi(TAG, "Error getting real time stats");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    vTaskDelete(NULL);
}

bds_monitor_h bds_monitor_create() {
    bds_monitor_t* h = bdsc_malloc(sizeof(bds_monitor_t));
#if (MONI_SWITCH)
    h->monitor_task = bds_task_create(monitor_run, "bds_monitor", 1024, h, BDS_THREAD_PRIORITY_MAX);
#endif
    return h;
}

void bds_monitor_destroy(bds_monitor_h handle) {
    if (!handle) {
        return;
    }
    bds_monitor_t* h = handle;
    if (h->monitor_task) {
        bds_task_destroy(h->monitor_task);
        h->monitor_task = NULL;
    }
    bdsc_free(handle);
}
