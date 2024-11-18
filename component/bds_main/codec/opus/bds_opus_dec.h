/***********************************************************
 * @author        liweigao
 * @date          2024-11-18
 * @brief         opus decoder
 ***********************************************************/

#ifndef _BDS_OPUS_DEC_H_
#define _BDS_OPUS_DEC_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* bds_opus_dec_h;

bds_opus_dec_h bds_opus_dec_create();

void bds_opus_dec_destroy(bds_opus_dec_h handle);

int bds_opus_dec_proc(bds_opus_dec_h handle, uint8_t* dst, int* dst_len, uint8_t* src, int src_len);

#ifdef __cplusplus
}
#endif

#endif /* _BDS_OPUS_DEC_H_ */
