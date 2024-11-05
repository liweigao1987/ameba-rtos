#define LOG_TAG "bds_demo"

#include "example_bdspeech.h"

#include "FreeRTOS.h"
#include "task.h"

void* g_speech = NULL;

u32 test_bdspeech(u16 argc, u8* argv[]) {
    printf("argc=%d, argv=%p\n", argc, argv);
    g_speech = test_create_sdk();
    test_config_sdk(g_speech);
    test_start_sdk(g_speech);
    test_start_link(g_speech);
    test_start_wakeup(g_speech);
    return 0;
}

u32 test_start_asr_cmd(u16 argc, u8* argv[]) {
    test_start_asr(g_speech, 0);
    return 0;
}

u32 test_cpu_monitor_cmd(u16 argc, u8* argv[]) {
    test_cpu_monitor();
    return 0;
}

u32 test_socket_cmd(u16 argc, u8* argv[]) {
    test_socket(argv[0], argv[1]);
    return 0;
}

u32 test_env_cmd(u16 argc, u8* argv[]) {
    printf("portTICK_RATE_MS=%d\n", portTICK_RATE_MS);
    uint32_t tick_before = xTaskGetTickCount();
    vTaskDelay(1000 / portTICK_RATE_MS);
    uint32_t tick_after = xTaskGetTickCount();
    printf("sleep 1s, tick=%d\n", tick_after - tick_before);
    return 0;
}

u32 test_nbsock_cmd(u16 argc, u8* argv[]) {
    example_nonblock_connect();
}
