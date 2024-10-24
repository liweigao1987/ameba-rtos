#include "stdio.h"
#include "string.h"
#include "cJSON.h"
#include "bds_client_context.h"
#include "bds_common_utility.h"

char g_pam[] =
    "{ \
	\"value\": { \
		\"clientContext\": [{ \
			\"header\": { \
				\"namespace\": \"ai.dueros.device_interface.voice_output\", \
				\"name\": \"SpeechState\" \
			}, \
			\"payload\": { \
				\"token\": \"\", \
				\"offsetInMilliseconds\": 0, \
				\"playerActivity\": \"FINISHED\" \
			} \
		}, { \
			\"header\": { \
				\"namespace\": \"ai.dueros.device_interface.speaker_controller\", \
				\"name\": \"Volume\" \
			}, \
			\"payload\": { \
				\"volume\": 15, \
				\"muted\": false \
			} \
		}, { \
			\"header\": { \
				\"namespace\": \"ai.dueros.device_interface.audio_player\", \
				\"name\": \"PlaybackState\" \
			}, \
			\"payload\": { \
				\"token\": \"\", \
				\"offsetInMilliseconds\": 0, \
				\"playerActivity\": \"FINISHED\" \
			} \
		}, { \
			\"header\": { \
				\"namespace\": \"ai.dueros.device_interface.extensions.local_audio_player\", \
				\"name\": \"PlaybackState\" \
			}, \
			\"payload\": { \
				\"status\": \"FOREGROUND\" \
			} \
		}, { \
			\"header\": { \
				\"namespace\": \"ai.dueros.device_interface.alerts\", \
				\"name\": \"AlertsState\" \
			}, \
			\"payload\": { \
				\"allAlerts\": [], \
				\"activeAlerts\": [] \
			} \
		}], \
		\"event\": { \
			\"header\": { \
				\"namespace\": \"ai.dueros.device_interface.voice_input\", \
				\"name\": \"ListenStarted\", \
				\"messageId\": \"1bf300000001541d196bdb00002be600000004\", \
				\"dialogRequestId\": \"1bf300000001542d38c5cc00002be600000005\" \
			}, \
			\"payload\": { \
				\"format\": \"AUDIO_L16_RATE_16000_CHANNELS_1\" \
			} \
		} \
	}, \
	\"dueros-device-id\": \"6bbe894db80cbfd2b3463e17e8614853\", \
	\"StandbyDeviceId\": \"\", \
	\"user-agent\": \"test\", \
	\"Authorization\": \"Bearer 24.1f67b669b582b4fb43925a6819a58eac.2592000.1671275534.282335-16030937\", \
	\"from\": \"dumi\", \
	\"LinkVersion\": 2 \
}";

const char *MSG_ID = "1bf300000001541d196bdb00002be600000004";
const char *DIA_ID = "1bf300000001542d38c5cc00002be600000005";

const int generate_pam(char* pam_prama)
{
    memcpy(pam_prama, g_pam, strlen(g_pam) + 1);
    char *target = NULL;
    char sn[SN_LENGTH];

    target = strstr(pam_prama, MSG_ID);
    if (!target) {
        printf("Failed to find msg id\n");
        return NULL;
    }

    bds_generate_uuid(sn);
    memcpy(target, sn, strlen(sn));

    target = strstr(pam_prama, DIA_ID);
    if (!target) {
        printf("Failed to find dialog id\n");
        return NULL;
    }

    bds_generate_uuid(sn);
    memcpy(target, sn, strlen(sn));
    // printf("pam:%s", pam_prama);
    return 0;
}

char* get_dcs_pam() {
    cJSON *pam_json = cJSON_CreateObject();
    assert(pam_json != NULL);
    if (pam_json == NULL) {
        /* ESP_LOGE("pam", "pam_json memory error"); */
        return NULL;
    }
    cJSON_AddStringToObject(pam_json, "dueros-device-id", "7bbe894db80cbfd2b3463e17e8614853");
    cJSON_AddStringToObject(pam_json, "StandbyDeviceId", "");
    cJSON_AddStringToObject(pam_json, "user-agent", "test");
    cJSON_AddStringToObject(pam_json, "Authorization",
                            "Bearer 24.1f67b669b582b4fb43925a6819a58eac.2592000.1671275534.282335-16030937");
    cJSON_AddStringToObject(pam_json, "from", "dumi");
    cJSON_AddNumberToObject(pam_json, "LinkVersion", 2);

    char *pam_json_json_string = cJSON_PrintUnformatted(pam_json);
    assert(pam_json_json_string != NULL);

    if (NULL != pam_json) {
        cJSON_Delete(pam_json);
        pam_json = NULL;
    }
    return pam_json_json_string;
}

char* get_wifi_mac() {
    return "02:42:47:dc:03:55";
}
