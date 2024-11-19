#include "bds_bdvs.h"

#include <string.h>

#include "bds_client_log.h"
#include "cJSON.h"

#define TAG "bdvs"

static const char* s_test_pam =
"{\"authorization\":{\"ak\":\"0c8b952ee224\",\"pk\":\"aux_pk\",\"fc\":\"aux_fc\",\"bdvsid\":\"9db29c1b9f90910d267d96f6f2ff3a36\",\"signature\":\"592cfd2e679601efd691fb92a85d9e7a\",\"time_stamp\":\"28860896\",\"token_alpha\":\"d4411102-0d3c-4424-a4d3-893373b08712\",\"sdkVersion\":\"0.0.0\",\"dspVersion\":\"0.0.0\",\"methods\":[\"ASR\",\"NLP\",\"TTS\"]},\"bdvs-device-id\":\"aux_fcaux_pk0c8b952ee224\",\"timestamp\":28860896,\"bdvs-version\":\"2.0.0\",\"event\":{\"payload\":{\"query\":null,\"sn\":null,\"idx\":-1,\"asrState\":null},\"header\":{\"messageId\":\"17316538104160\",\"name\":\"TextInput\",\"namespace\":\"bdvs.capability.text_input\",\"dialogRequestId\":\"17316538104160\"}},\"contexts\":[{\"header\":{\"namespace\":\"bdvs.capability.extensions\",\"name\":\"RequestParam\"},\"payload\":{\"extension\":\"{\\\"playerStatus\\\":{\\\"state\\\":\\\"\\\",\\\"domain\\\":\\\"\\\",\\\"timestamp\\\":1731653810416,\\\"PlayProgress\\\":0,\\\"TotalLength\\\":0},\\\"mapStatus\\\":{\\\"state\\\":\\\"Yes\\\"},\\\"ttsParam\\\":{\\\"spd\\\":5,\\\"pit\\\":5,\\\"vol\\\":5,\\\"pronounce\\\":\\\"20100xdsh4gypv\\\",\\\"rate\\\":16,\\\"reverb\\\":0,\\\"audio_ctrl\\\":\\\"{\\\"sampling_rate\\\":16000}\\\"}}\"}}],\"extensions-param\":{\"vp_info\":{\"vp_group_id\":\"\",\"vp_judge_type\":0}}}";

/* static const char* s_test_pam = */
/*     "{\"authorization\":{\"ak\":\"0c8b952ee224\",\"pk\":\"aux_pk\",\"fc\":\"aux_fc\",\"bdvsid\":\"9db29c1b9f90910d267d96f6f2ff3a36\",\"signature\":\"592cfd2e679601efd691fb92a85d9e7a\",\"time_stamp\":\"28860896\",\"token_alpha\":\"d4411102-0d3c-4424-a4d3-893373b08712\",\"sdkVersion\":\"0.0.0\",\"dspVersion\":\"0.0.0\",\"methods\":[\"ASR\",\"NLP\",\"TTS\"]},\"bdvs-device-id\":\"aux_fcaux_pk0c8b952ee224\",\"timestamp\":28860896,\"bdvs-version\":\"2.0.0\",\"event\":{\"payload\":{\"query\":null,\"sn\":null,\"idx\":-1,\"asrState\":null},\"header\":{\"messageId\":\"17316538104160\",\"name\":\"TextInput\",\"namespace\":\"bdvs.capability.text_input\",\"dialogRequestId\":\"17316538104160\"}},\"contexts\":[{\"header\":{\"namespace\":\"bdvs.capability.extensions\",\"name\":\"RequestParam\"},\"payload\":{\"extension\":\"{\\\"playerStatus\\\":{\\\"state\\\":\\\"\\\",\\\"domain\\\":\\\"\\\",\\\"timestamp\\\":1731653810416,\\\"PlayProgress\\\":0,\\\"TotalLength\\\":0},\\\"mapStatus\\\":{\\\"state\\\":\\\"Yes\\\"},\\\"ttsParam\\\":{\\\"spd\\\":5,\\\"pit\\\":5,\\\"vol\\\":5,\\\"pronounce\\\":\\\"20100xdsh4gypv\\\",\\\"rate\\\":16,\\\"reverb\\\":0}}\"}}],\"extensions-param\":{\"vp_info\":{\"vp_group_id\":\"\",\"vp_judge_type\":0}}}"; */

char* bds_bdvs_get_pam() {
    int len = strlen(s_test_pam);
    bdsc_logw(TAG, "pam len=%d", len);
    return s_test_pam;
}
