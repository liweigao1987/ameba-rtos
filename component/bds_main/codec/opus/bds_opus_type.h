/***********************************************************
 * @author        liweigao
 * @date          2024-11-18
 * @brief         opus type
 ***********************************************************/

#ifndef _BDS_OPUS_TYPE_H_
#define _BDS_OPUS_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define DEC_SAMPLE_RATE 16000
#define DEC_CHANNEL     1
#define PCM_20MS_SAMPLE 320
#define PCM_20MS_BYTES  (PCM_20MS_SAMPLE * 2)
#define OPUS_20MS_BYTES 160
#define OPUS_TTS_BAG    (8 + OPUS_20MS_BYTES)

#ifdef __cplusplus
}
#endif

#endif /* _BDS_OPUS_TYPE_H_ */
