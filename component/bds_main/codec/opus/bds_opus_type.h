/***********************************************************
 * @author        liweigao
 * @date          2024-01-29
 * @brief         opus type
 ***********************************************************/

#ifndef _BDS_OPUS_TYPE_H_
#define _BDS_OPUS_TYPE_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define CHANNEL               2
#define SAMPLE_RATE           16000
#define PRECISION             2
#define FRAME_MS              20
#define PCM_1MS_BYTES         (SAMPLE_RATE / 1000 * PRECISION * CHANNEL)
#define PCM_20MS_SHORTS       (PCM_1MS_BYTES * FRAME_MS / PRECISION)
#define PCM_20MS_BYTES        (PCM_1MS_BYTES * FRAME_MS)
#define PCM_20MS_SAMPLES      (PCM_20MS_BYTES / PRECISION / CHANNEL)
#define OPUS_20MS_BYTES       (PCM_20MS_BYTES / 8 + 8)
#define OPUS_100MS_BYTES      (OPUS_20MS_BYTES * 5)
#define bdsc_malloc           malloc
#define bdsc_free             free
#define bdsc_memcpy           memcpy
#define BDSLOG_FORMAT(format) " [bdsc-%s:%s:%d] " format "\n"

#define bdsc_logd(tag, format, ...)                                            \
    do {                                                                       \
        printf(BDSLOG_FORMAT(format), tag, __func__, __LINE__, ##__VA_ARGS__); \
    } while (0)

typedef struct {
    int32_t  flag;
    uint16_t offset;
    uint16_t buffer_length;
    uint8_t  buffer[];
} bdsc_audio_t;

bdsc_audio_t* bdsc_audio_create(uint16_t buffer_length);

int32_t bdsc_audio_append(bdsc_audio_t* audio, uint8_t* src, uint16_t src_length);

bool bdsc_audio_is_full(bdsc_audio_t* audio);

void bdsc_audio_destroy(bdsc_audio_t* audio);

void int_to_char(uint32_t i, uint8_t ch[4]);

int char_to_int(uint8_t ch[4]);

#endif /* _BDS_OPUS_TYPE_H_ */
