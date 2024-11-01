#ifndef _EXAMPLE_BDSPEECH_H_
#define _EXAMPLE_BDSPEECH_H_

#include "ameba_soc.h"
#include "basic_types.h"
#include "platform_stdlib.h"

extern void* test_create_sdk();
extern void  test_config_sdk(void* handle);
extern void  test_start_sdk(void* handle);
extern void  test_stop_sdk(void* handle);
extern void  test_destory_sdk(void* handle);
extern void  test_start_link(void* handle);
extern void  test_stop_link(void* handle);
extern void  test_start_wakeup(void* handle);
extern void  test_stop_wakeup(void* handle);
extern void  test_start_asr(void* handle, int back_time);
extern void  test_start_event(void* handle);
extern void  test_cpu_monitor();
extern void  test_socket(char* ip , char* port);

u32 test_bdspeech(u16 argc, u8* argv[]);
u32 test_start_asr_cmd(u16 argc, u8* argv[]);
u32 test_cpu_monitor_cmd(u16 argc, u8* argv[]);
u32 test_socket_cmd(u16 argc, u8* argv[]);
u32 test_env_cmd(u16 argc, u8* argv[]);
u32 test_nbsock_cmd(u16 argc, u8* argv[]);

#endif /* _EXAMPLE_BDSPEECH_H_ */
