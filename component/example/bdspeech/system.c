#include "FreeRTOS.h"
#include "task.h"

#define TAG               "test"
#define STATS_TICKS       pdMS_TO_TICKS(1000)
#define ARRAY_SIZE_OFFSET 5  // Increase this if print_real_time_stats returns -11

static int print_real_time_stats(TickType_t xTicksToWait) {
    TaskStatus_t *start_array = NULL, *end_array = NULL;
    UBaseType_t   start_array_size, end_array_size;
    uint32_t      start_run_time, end_run_time;
    int     ret;

    // Allocate array to store current task states
    start_array_size = uxTaskGetNumberOfTasks() + ARRAY_SIZE_OFFSET;
    start_array      = malloc(sizeof(TaskStatus_t) * start_array_size);
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
    end_array      = malloc(sizeof(TaskStatus_t) * end_array_size);
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

    printf("|     Task     | Prio | Perc | stackWM \n");
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
            printf("| %12s | %2d   | %2d%%  | %6d \n", start_array[i].pcTaskName, start_array[i].uxCurrentPriority,
                   percentage_time, start_array[i].usStackHighWaterMark * sizeof(StackType_t));
        }
    }

    // Print unmatched tasks
    for (int i = 0; i < start_array_size; i++) {
        if (start_array[i].xHandle != NULL) {
            printf("| %s | Deleted\n", start_array[i].pcTaskName);
        }
    }
    for (int i = 0; i < end_array_size; i++) {
        if (end_array[i].xHandle != NULL) {
            printf("| %s | Created\n", end_array[i].pcTaskName);
        }
    }
    ret = 0;

exit:  // Common return path
    free(start_array);
    free(end_array);
    return ret;
}

static void stats_mem() {
    uint32_t free_bytes = rtos_mem_get_free_heap_size();
    printf("------>heap_left:%u\n", free_bytes);
}

static void stats_task(void* arg) {
    // Print real time stats periodically
    while (1) {
        printf("\n\n----------------------------------------\n");
        stats_mem();
        if (print_real_time_stats(STATS_TICKS) == 0) {
            printf("----------------------------------------\n");
        } else {
            printf("Error getting real time stats\n");
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static TaskHandle_t s_cpu_task = NULL;

void test_cpu_monitor() {
    if (!s_cpu_task) {
        xTaskCreate(stats_task, "bds_cpu", 1024, NULL, 10, &s_cpu_task);
    }
}
