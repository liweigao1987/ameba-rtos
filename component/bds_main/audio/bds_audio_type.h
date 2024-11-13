/***********************************************************
 * @author        liweigao
 * @date          2024-11-11
 * @brief         audio type
 ***********************************************************/

#ifndef _BDS_AUDIO_TYPE_H_
#define _BDS_AUDIO_TYPE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_SR      16000
#define AUDIO_FORMAT  RTAUDIO_FORMAT_PCM_16_BIT
#define AUDIO_CH      1
#define PCM_BAG_BYTES 1024

typedef struct {
    uint8_t audio[PCM_BAG_BYTES];
} bds_audio_bag_t;

#ifdef __cplusplus
}
#endif

#endif /* _BDS_AUDIO_TYPE_H_ */
