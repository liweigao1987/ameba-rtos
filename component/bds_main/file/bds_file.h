/***********************************************************
 * @author        liweigao
 * @date          2024-11-13
 * @brief         file wrap
 ***********************************************************/

#ifndef _BDS_FILE_H_
#define _BDS_FILE_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* bds_file_h;

bds_file_h bds_file_get_instance();

void bds_file_destroy(bds_file_h handle);

int bds_file_load_cfg(bds_file_h handle);

FILE* bds_file_open(char* path, char* mode);

void bds_file_close(FILE* fp);

int bds_file_read(uint8_t* buffer, uint32_t bytes, FILE* fp);

int bds_file_get_len(char* path);

char* bds_file_get_wp_audio_path();

int bds_file_get_wp_audio_len();

char* bds_file_get_direct_audio_path(char* code);

#ifdef __cplusplus
}
#endif

#endif /* _BDS_FILE_H_ */
