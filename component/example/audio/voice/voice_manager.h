#ifndef __EXAMPLE_VOICE_VOICE_MANAGER_H__
#define __EXAMPLE_VOICE_VOICE_MANAGER_H__


#include "ring_buffer.h"

#define VOICE_DEBUG 0
#if VOICE_DEBUG
#define LOGV(fmt, ...)  printf("VOICE : " fmt "\n", ##__VA_ARGS__)
#else
#define LOGV(fmt, ...)  do { } while(0)
#endif
#define LOGD(fmt, ...)  printf("VOICE : " fmt "\n", ##__VA_ARGS__)
#define LOGI(fmt, ...)  printf("VOICE : " fmt "\n", ##__VA_ARGS__)
#define LOGE(fmt, ...)  printf("VOICE : error - " fmt "\n", ##__VA_ARGS__)

#define RES_DATA_ADDR								0x08E04020
#define RECORD_SIZE									768
#define DEFAULT_STACK_SIZE							4096
#define RING_BUFFER_SIZE							4096
#define VOICE_DATA_CHANNEL0							0
#define VOICE_DATA_CHANNEL1							1

#define VOIDCE_KEY_WORD_ID_1						1
#define VOIDCE_KEY_WORD_ID_2						2

#define VOIDCE_KEY_WORD_VOLUME_UP1					40
#define VOIDCE_KEY_WORD_VOLUME_UP2					87

#define VOIDCE_KEY_WORD_VOLUME_DOWN1				41
#define VOIDCE_KEY_WORD_VOLUME_DOWN2				89

#define HOST_CMD_TYPE_SET_WK_INFO               0x5a000000
#define HOST_CMD_TYPE_SET_LOW_PWR               0x5a000001
#define HOST_CMD_TYPE_SET_NORMAL_PWR            0x5a000002
#define HOST_CMD_TYPE_SET_HOST_HEART_FREEZE_MAX 0x5a000003
#define HOST_CMD_TYPE_SET_DISP_STR              0x5a000004
#define HOST_CMD_TYPE_SET_SPI_OUT_BUF_ORDER     0x5a000005
#define HOST_CMD_TYPE_SET_TDM_OUT_BUF_ORDER     0x5a000006
#define HOST_CMD_TYPE_SET_AUDIO_LANE_CFG        0x5a000007
#define HOST_CMD_TYPE_SEND_LIPS_MOVE_PROB       0x5a000008
#define HOST_CMD_TYPE_SET_DEVICE_STATE          0x5a000009
#define HOST_CMD_TYPE_SET_WAKEUP_SHOUCI         0x5a00000a
#define HOST_CMD_TYPE_SET_WK_MULTI_MODE         0x5a00000b
#define HOST_CMD_TYPE_SET_WK_WORD_ON            0x5a00000c
#define HOST_CMD_TYPE_SET_WK_WORD_OFF           0x5a00000d=
#define HOST_CMD_TYPE_DBG_SIG                   0x5a5adbdb

#ifdef ARM_CORE_CM4
#define VOICE_RPC_MODE (BLOCK_MODE | (RPC_NP_DSP << 1) | (RPC_DSP_NP << 4))
#elif defined(RSICV_CORE_KR4)
#define VOICE_RPC_MODE (BLOCK_MODE | (RPC_AP_DSP << 1) | (RPC_DSP_AP << 4))
#endif

int32_t Init(void);
int32_t Start(void);
int32_t Pause(void);
int32_t Stop(void);
int32_t Release(void);

#endif //__EXAMPLE_VOICE_VOICE_MANAGER_H__