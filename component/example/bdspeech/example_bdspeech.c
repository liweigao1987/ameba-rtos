#define LOG_TAG "bds_demo"

#include "example_bdspeech.h"

extern void* test_create_sdk();
extern void  test_config_sdk(void* handle);
extern void  test_start_sdk(void* handle);

void* g_speech = NULL;

u32 test_bdspeech(u16 argc, u8* argv[]) {
    printf("argc=%d, argv=%p\n", argc, argv);
    g_speech = test_create_sdk();
    test_config_sdk(g_speech);
    test_start_sdk(g_speech);
    return 0;
}
