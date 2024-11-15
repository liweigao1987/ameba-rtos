/*
 * bds_tp_audio_length_state.c
 *
 *  Created on: Apr 8, 2020
 *      Author: liweigao
 */

#include "bds_tts_parser.h"

static void decode(bds_tts_parser_t *machine, bdsc_event_data_t *data, int offset) {
    if (machine == NULL || data == NULL) {
        return;
    }
    if (data->buffer_length == 0) {
        bdsc_loge(TAG, "buffer_length is 0!");
        return;
    }
    char *start_addr = (char*)data->buffer + offset;
    int left = data->buffer_length - offset;
    if (left <= 0) {
        bdsc_logi(TAG, "no left data!");
        return;
    }
    if (machine->frame == NULL) {
        machine->frame = bds_tts_frame_create();
    }
    int audio_length_lack = tts_frame_audio_length_lack(machine->frame);
    if (audio_length_lack > 0) {
        if (left >= audio_length_lack) {
            tts_frame_clone_audio_length(machine->frame, start_addr, audio_length_lack);
            offset += audio_length_lack;
            bds_set_tts_parser_state(machine, &machine->audio_state->base_state);
            bds_tts_parser_decode(machine, data, offset);
        } else {
            tts_frame_clone_audio_length(machine->frame, start_addr, left);
            offset += left;
        }
    } else {
        bds_set_tts_parser_state(machine, &machine->audio_state->base_state);
        bds_tts_parser_decode(machine, data, offset);
    }
}

bds_tp_audio_length_state_t* bds_tp_audio_length_state_create() {
    bds_tp_audio_length_state_t *state = bdsc_malloc(sizeof(bds_tp_audio_length_state_t));
    state->base_state.decode = decode;
    return state;
}

void bds_tp_audio_length_state_destroy(bds_tp_audio_length_state_t *state) {
    if (state == NULL) {
        return;
    }
    bdsc_free(state);
}
